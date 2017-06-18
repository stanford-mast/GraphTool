/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReader.cpp
 *   Implementation of common functionality for reading and interpreting graph
 *   files of various formats.
 *****************************************************************************/

#include "Graph.h"
#include "GraphReader.h"
#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <silo.h>
#include <spindle.h>


namespace GraphTool
{
    // -------- TYPE DEFINITIONS ------------------------------------------- //

    /// Provides all information needed to specify a graph read operation.
    template <typename TEdgeData> struct SGraphReadSpec
    {
        FILE* file;                                                         ///< File handle.
        Graph<TEdgeData>* graph;                                            ///< Graph object to be filled.
        GraphReader<TEdgeData>* reader;                                     ///< Graph reader object.
        SEdge<TEdgeData>* bufs[2];                                          ///< Edge data buffers.
        TEdgeCount counts[2];                                               ///< Edge data buffer counts.
        size_t* refreshDegreeBuf;                                           ///< Buffer for refreshing degree information.
        bool readSuccessfulSoFar;                                           ///< Indicates the continued success of the read operation.
    };


    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "GraphReader.h" for documentation.

    template <typename TEdgeData> GraphReader<TEdgeData>::GraphReader(void)
    {
        // Nothing to do here.
    }

    // --------

    template <typename TEdgeData> GraphReader<TEdgeData>::~GraphReader(void)
    {
        // Nothing to do here.
    }


    // -------- CLASS METHODS ---------------------------------------------- //
    // See "GraphReader.h" for documentation.

    template <typename TEdgeData> void GraphReader<TEdgeData>::EdgeConsumer(void* arg)
    {
        const uint32_t localThreadID = spindleGetLocalThreadID();
        const uint32_t localThreadCount = spindleGetLocalThreadCount();
        
        SGraphReadSpec<TEdgeData>* readSpec = (SGraphReadSpec<TEdgeData>*)arg;
        uint32_t currentIndex = 0;

        // Iteratively consume edges that the edge producer loads into the edge buffers.
        while (true)
        {
            // Wait for the buffer to be filled with edges.
            spindleBarrierGlobal();

            // Check for termination.
            if (0 == readSpec->counts[currentIndex])
                break;

            // Check for I/O errors.
            if (false == readSpec->readSuccessfulSoFar)
                return;

            // Read the buffer into the graph.
            // Use different parallelization strategies based on the number of threads created.
            switch (localThreadCount)
            {
            case 1:
                for (TEdgeCount i = 0; i < readSpec->counts[currentIndex]; ++i)
                {
                    readSpec->graph->InsertEdgeByDestination(readSpec->bufs[currentIndex][i]);
                    readSpec->graph->InsertEdgeBySource(readSpec->bufs[currentIndex][i]);
                }
                break;

            case 2:
                for (TEdgeCount i = 0; i < readSpec->counts[currentIndex]; ++i)
                {
                    switch (localThreadID)
                    {
                    case 0:
                        readSpec->graph->InsertEdgeByDestination(readSpec->bufs[currentIndex][i]);
                        break;

                    case 1:
                        readSpec->graph->InsertEdgeBySource(readSpec->bufs[currentIndex][i]);
                        break;

                    default:
                        break;
                    }
                }
                break;

            default:
                for (TEdgeCount i = 0; i < readSpec->counts[currentIndex]; ++i)
                {
                    if (localThreadID == (readSpec->bufs[currentIndex][i].destinationVertex % localThreadCount))
                        readSpec->graph->FastInsertEdgeByDestination(readSpec->bufs[currentIndex][i]);
                    
                    if (localThreadID == (readSpec->bufs[currentIndex][i].sourceVertex % localThreadCount))
                        readSpec->graph->FastInsertEdgeBySource(readSpec->bufs[currentIndex][i]);
                }
                break;
            }

            // Switch to the other buffer to consume in parallel with edge production.
            currentIndex = (~currentIndex) & 1;
        }
        
        // The fast insertion methods did not update the degree and vector counts, so do that here.
        // These were only used with higher numbers of threads, so this is only needed in that case.
        if (localThreadCount > 2)
        {
            if (0 == localThreadID)
                readSpec->refreshDegreeBuf = new size_t[localThreadCount << 1];
            
            spindleBarrierLocal();
            readSpec->graph->ParallelRefreshDegreeInfo(readSpec->refreshDegreeBuf);
            spindleBarrierLocal();
            
            if (0 == localThreadID)
                delete[] readSpec->refreshDegreeBuf;
        }
    }

    // --------

    template <typename TEdgeData> void GraphReader<TEdgeData>::EdgeProducer(void* arg)
    {
        SGraphReadSpec<TEdgeData>* readSpec = (SGraphReadSpec<TEdgeData>*)arg;
        uint32_t currentBufferIndex = 0;

        while (true)
        {
            // Fill the buffer with edges.
            readSpec->counts[currentBufferIndex] = readSpec->reader->ReadEdgesToBuffer(readSpec->file, readSpec->bufs[currentBufferIndex], (kGraphReadBufferSize / sizeof(SEdge<void>)));

            // Check for any I/O errors.
            if (ferror(readSpec->file))
                readSpec->readSuccessfulSoFar = false;

            // Synchronize with consumers.
            spindleBarrierGlobal();

            // Check for termination or I/O errors detected previously.
            if (0 == readSpec->counts[currentBufferIndex] || false == readSpec->readSuccessfulSoFar)
                break;

            // Switch to the other buffer to read from file during a consumption operation.
            currentBufferIndex = (~currentBufferIndex) & 1;
        }
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "GraphReader.h" for documentation.

    template <typename TEdgeData> bool GraphReader<TEdgeData>::ReadGraphFromFile(const char* const filename, Graph<TEdgeData>& graph)
    {
        // First, open the file.
        FILE* graphfile = this->OpenAndInitializeGraphFileForRead(filename);
        if (NULL == graphfile)
            return false;
        
        graph.SetNumVertices(numVerticesInFile);
        
        
        // Allocate some buffers for read data.
        SEdge<TEdgeData>* bufs[] = { (SEdge<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]), (SEdge<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]) };

        // Define the graph read task.
        SGraphReadSpec<TEdgeData> readSpec;

        readSpec.file = graphfile;
        readSpec.graph = &graph;
        readSpec.reader = this;
        readSpec.bufs[0] = bufs[0];
        readSpec.bufs[1] = bufs[1];
        readSpec.counts[0] = 0;
        readSpec.counts[1] = 0;
        readSpec.refreshDegreeBuf = NULL;
        readSpec.readSuccessfulSoFar = true;

        // Define the parallelization strategy.
        SSpindleTaskSpec taskSpec[2];

        taskSpec[0].func = &EdgeProducer;
        taskSpec[0].arg = (void*)&readSpec;
        taskSpec[0].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
        taskSpec[0].numThreads = 1;
        taskSpec[0].smtPolicy = SpindleSMTPolicyPreferLogical;

        taskSpec[1].func = &EdgeConsumer;
        taskSpec[1].arg = (void*)&readSpec;
        taskSpec[1].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
        taskSpec[1].numThreads = 0;
        taskSpec[1].smtPolicy = SpindleSMTPolicyPreferLogical;

        // Launch the graph read task.
        spindleThreadsSpawn(taskSpec, sizeof(taskSpec) / sizeof(taskSpec[0]), true);

        // Clean up.
        fclose(graphfile);
        delete[] bufs[0];
        delete[] bufs[1];

        return readSpec.readSuccessfulSoFar;
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class GraphReader<void>;
    template class GraphReader<uint64_t>;
    template class GraphReader<double>;
}
