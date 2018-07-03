/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphWriter.cpp
 *   Implementation of common functionality for producing graph files of
 *   various formats.
 *****************************************************************************/

#include "EdgeList.h"
#include "VertexIndex.h"
#include "Graph.h"
#include "GraphWriter.h"
#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <silo.h>
#include <spindle.h>


namespace GraphTool
{
    // -------- TYPE DEFINITIONS ------------------------------------------- //

    /// Provides all information needed to specify a graph write operation.
    template <typename TEdgeData> struct SGraphWriteSpec
    {
        FILE* file;                                                         ///< File handle.
        const Graph<TEdgeData>* graph;                                      ///< Graph object being exported.
        GraphWriter<TEdgeData>* writer;                                     ///< Graph write object.
        SEdge<TEdgeData>* bufs[2];                                          ///< Edge data buffers.
        TEdgeCount counts[2];                                               ///< Edge data buffer counts.
        bool groupedByDestination;                                          ///< Indicates that graph edges should be grouped by destination instead of by source.
        bool writeSuccessfulSoFar;                                          ///< Indicates the continued success of the write operation.
    };


    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> GraphWriter<TEdgeData>::GraphWriter(void)
    {
        // Nothing to do here.
    }

    // --------

    template <typename TEdgeData> GraphWriter<TEdgeData>::~GraphWriter(void)
    {
        // Nothing to do here.
    }


    // -------- CLASS METHODS ---------------------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> void GraphWriter<TEdgeData>::EdgeConsumer(void* arg)
    {
        SGraphWriteSpec<TEdgeData>* writeSpec = (SGraphWriteSpec<TEdgeData>*)arg;
        uint32_t currentBufferIndex = 0;

        // Iteratively consume edges that the edge producer loads into the edge buffers.
        while (true)
        {
            // Wait for the buffer to be filled with edges.
            spindleBarrierGlobal();

            // Check for termination or I/O errors detected previously.
            if (0 == writeSpec->counts[currentBufferIndex] || false == writeSpec->writeSuccessfulSoFar)
                break;

            // Consume the buffer and write edges to the file.
            writeSpec->writer->WriteEdgesToFile(writeSpec->file, *writeSpec->graph, writeSpec->bufs[currentBufferIndex], writeSpec->counts[currentBufferIndex], writeSpec->groupedByDestination);

            // Check for any I/O errors.
            if (ferror(writeSpec->file))
                writeSpec->writeSuccessfulSoFar = false;

            // Switch to the other buffer to consume in parallel with edge production.
            currentBufferIndex = (~currentBufferIndex) & 1;
        }
    }

    // --------

    template <typename TEdgeData> void GraphWriter<TEdgeData>::EdgeProducer(void* arg)
    {
        SGraphWriteSpec<TEdgeData>* writeSpec = (SGraphWriteSpec<TEdgeData>*)arg;
        uint32_t currentBufferIndex = 0;
        
        const size_t writeBufferCount = (kGraphWriteBufferSize / sizeof(SEdge<void>));
        const VertexIndex<TEdgeData>& vertexIndex = (writeSpec->groupedByDestination ? writeSpec->graph->VertexIndexDestination() : writeSpec->graph->VertexIndexSource());
        
        TVertexID topLevelVertex = 0;
        
        while ((topLevelVertex < writeSpec->graph->GetNumVertices()) && (NULL == vertexIndex[topLevelVertex]))
            topLevelVertex += 1;
        
        auto edgeIter = vertexIndex[topLevelVertex]->BeginIterator();
        
        while (true)
        {
            // Fill the buffer with edges.
            TEdgeCount edgeIdx = 0;
        
            while ((edgeIdx < writeBufferCount) && (topLevelVertex < writeSpec->graph->GetNumVertices()))
            {
                // Write out the edge.
                vertexIndex[topLevelVertex]->FillEdge(edgeIter, writeSpec->bufs[currentBufferIndex][edgeIdx], topLevelVertex, writeSpec->groupedByDestination);
                edgeIdx += 1;

                // Advance to the next edge.
                ++edgeIter;

                if (vertexIndex[topLevelVertex]->EndIterator() == edgeIter)
                {
                    topLevelVertex += 1;

                    while ((topLevelVertex < writeSpec->graph->GetNumVertices()) && (NULL == vertexIndex[topLevelVertex]))
                        topLevelVertex += 1;
                    
                    if (topLevelVertex < writeSpec->graph->GetNumVertices())
                        edgeIter = vertexIndex[topLevelVertex]->BeginIterator();
                }
            }
            
            writeSpec->counts[currentBufferIndex] = edgeIdx;
        
            // Synchronize with the consumer.
            spindleBarrierGlobal();
        
            // Check for termination or I/O errors.
            if (0 == writeSpec->counts[currentBufferIndex] || false == writeSpec->writeSuccessfulSoFar)
                break;
        
            // Switch to the other buffer to read from file during a consumption operation.
            currentBufferIndex = (~currentBufferIndex) & 1;
        }
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> bool GraphWriter<TEdgeData>::WriteGraphToFile(const char* const filename, const Graph<TEdgeData>& graph, const bool groupedByDestination)
    {
        // First, open the file.
        FILE* graphfile = this->OpenAndInitializeGraphFileForWrite(filename, graph, groupedByDestination);
        if (NULL == graphfile)
            return false;

        // Allocate some buffers for read data.
        SEdge<TEdgeData>* bufs[] = { (SEdge<TEdgeData>*)(new uint8_t[kGraphWriteBufferSize]), (SEdge<TEdgeData>*)(new uint8_t[kGraphWriteBufferSize]) };

        // Define the graph write task.
        SGraphWriteSpec<TEdgeData> writeSpec;

        writeSpec.file = graphfile;
        writeSpec.graph = &graph;
        writeSpec.writer = this;
        writeSpec.bufs[0] = bufs[0];
        writeSpec.bufs[1] = bufs[1];
        writeSpec.counts[0] = 0;
        writeSpec.counts[1] = 0;
        writeSpec.groupedByDestination = groupedByDestination;
        writeSpec.writeSuccessfulSoFar = true;

        // Define the parallelization strategy.
        SSpindleTaskSpec taskSpec[2];

        taskSpec[0].func = &EdgeProducer;
        taskSpec[0].arg = (void*)&writeSpec;
        taskSpec[0].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
        taskSpec[0].numThreads = 1;
        taskSpec[0].smtPolicy = SpindleSMTPolicyPreferPhysical;

        taskSpec[1].func = &EdgeConsumer;
        taskSpec[1].arg = (void*)&writeSpec;
        taskSpec[1].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
        taskSpec[1].numThreads = 1;
        taskSpec[1].smtPolicy = SpindleSMTPolicyPreferPhysical;

        // Launch the graph write task.
        spindleThreadsSpawn(taskSpec, sizeof(taskSpec) / sizeof(taskSpec[0]), true);

        // Clean up.
        fclose(graphfile);
        delete[] bufs[0];
        delete[] bufs[1];

        return writeSpec.writeSuccessfulSoFar;
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class GraphWriter<void>;
    template class GraphWriter<uint64_t>;
    template class GraphWriter<double>;
}
