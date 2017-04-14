/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphWriter.hpp
 *   Declaration of the common interface to objects capable of producing
 *   graph files of various formats.
 *****************************************************************************/

#pragma once

#include "Graph.hpp"
#include "Types.h"

#include <cstddef>
#include <cstdio>
#include <silo.h>
#include <spindle.h>
#include <string>


namespace GraphTool
{
    /// Base class for all object types that are used to produce graph files of various formats.
    /// Objects in this hierarchy consume graph objects and produce files.
    /// Some common functionality is implemented directly in the base class.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class GraphWriter
    {
    private:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Specifies the size in bytes of each write buffer to use when reading data from the file.
        /// Two buffers are created, 64MB each by default.
        static const size_t kGraphWriteBufferSize = (64ull * 1024ull * 1024ull);
        
        
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Provides all information needed to specify a graph write operation.
        struct SGraphWriteSpec
        {
            FILE* file;                                                     ///< File handle.
            Graph<TEdgeData>* graph;                                        ///< Graph object being written.
            typename Graph<TEdgeData>::EdgeIterator edgesBegin;        ///< Iterator to the beginning of the graph's edges.
            typename Graph<TEdgeData>::EdgeIterator edgesEnd;          ///< Iterator to the end of the graph's edges.
            GraphWriter<TEdgeData>* writer;                                 ///< Graph write object.
            SEdgeBufferData<TEdgeData>* bufs[2];                            ///< Edge data buffers.
            TEdgeCount counts[2];                                           ///< Edge data buffer counts.
            bool groupedByDestination;                                      ///< Indicates that graph edges should be grouped by destination instead of by source.
            bool writeSuccessfulSoFar;                                      ///< Indicates the continued success of the write operation.
        };
        
        
        // -------- CLASS METHODS ------------------------------------------ //
        
        /// Controls the consumption of edges from a buffer to a file, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of #SGraphWriteSpec that defines the graph write operation.
        static void EdgeConsumer(void* arg)
        {
            SGraphWriteSpec* writeSpec = (SGraphWriteSpec*)arg;
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

        /// Controls the production of edges from a graph object to a buffer, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of #SGraphWriteSpec that defines the graph write operation.
        static void EdgeProducer(void* arg)
        {
            SGraphWriteSpec* writeSpec = (SGraphWriteSpec*)arg;
            uint32_t currentBufferIndex = 0;

            const size_t writeBufferCount = (kGraphWriteBufferSize / sizeof(SEdgeBufferData<void>));
            auto edgeIter = writeSpec->edgesBegin;

            while (true)
            {
                // Fill the buffer with edges.
                TEdgeCount edgeIdx = 0;

                while ((edgeIdx < writeBufferCount) && !(edgeIter == writeSpec->edgesEnd))
                {
                    if (writeSpec->groupedByDestination)
                        edgeIter.GetOtherEdgeInfo().ExportToSourceEdgeBuffer(edgeIter.GetIndexedVertexID(), writeSpec->bufs[currentBufferIndex][edgeIdx]);
                    else
                        edgeIter.GetOtherEdgeInfo().ExportToDestinationEdgeBuffer(edgeIter.GetIndexedVertexID(), writeSpec->bufs[currentBufferIndex][edgeIdx]);

                    edgeIdx += 1;
                    ++edgeIter;
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

        
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Opens and performs any initial file reading tasks required to prepare the graph file for writing of graph data.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// @param [in] filename File name of the file to be opened for writing.
        /// @param [in] graph Graph to be written.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return File handle for the opened file.
        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename, Graph<TEdgeData>& graph, bool groupedByDestination) = 0;
        
        /// Writes edge data from the specified buffer into the specified file.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// @param [in] graphfile File handle for the open graph file.
        /// @param [in] graph Graph to be written.
        /// @param [in] buf Buffer from which to read edge data.
        /// @param [in] count Number of edges in the buffer.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return `true` if all edges were successfully written, `false` otherwise.
        virtual void WriteEdgesToFile(FILE* graphfile, const Graph<TEdgeData>& graph, const SEdgeBufferData<TEdgeData>* buf, size_t count, bool groupedByDestination) = 0;
        
        
    public:
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Writes a graph to the specified file.
        /// @param [in] filename File name of the file to be written.
        /// @param [in] graph Graph object to be written to the file.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return `true` if the graph was written successfully, `false` otherwise.
        bool WriteGraphToFile(const std::string& filename, Graph<TEdgeData>& graph, bool groupedByDestination = false)
        {
            // First, open the file.
            FILE* graphfile = this->OpenAndInitializeGraphFile(filename, graph, groupedByDestination);
            if (NULL == graphfile)
                return false;
            
            // Allocate some buffers for read data.
            SEdgeBufferData<TEdgeData>* bufs[] = { (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphWriteBufferSize]), (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphWriteBufferSize]) };

            // Define the graph write task.
            SGraphWriteSpec writeSpec;

            writeSpec.file = graphfile;
            writeSpec.edgesBegin = (groupedByDestination ? graph.EdgeIteratorDestinationBegin() : graph.EdgeIteratorSourceBegin());
            writeSpec.edgesEnd = (groupedByDestination ? graph.EdgeIteratorDestinationEnd() : graph.EdgeIteratorSourceEnd());
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
    };
}
