/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReader.hpp
 *   Declaration of the common interface to objects capable of interpreting
 *   graph files of various formats, templated by edge data type.
 *****************************************************************************/

#pragma once

#include "Edge.hpp"
#include "EdgeIndex.hpp"
#include "Graph.hpp"
#include "SequenceComparator.hpp"
#include "Types.h"

#include <cstddef>
#include <cstdio>
#include <silo.h>
#include <spindle.h>
#include <string>


namespace GraphTool
{
    /// Base class for all object types that are used to interpret graph files of various formats.
    /// Objects in this hierarchy load graph data from a file and produce one of the graph objects.
    /// Some common functionality is implemented directly in the base class.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class GraphReader
    {   
    private:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Specifies the size in bytes of each read buffer to use when reading data from the file.
        /// Two buffers are created, 64MB each by default.
        static const size_t kGraphReadBufferSize = (64ull * 1024ull * 1024ull);
        
        
        // -------- TYPE DEFINITIONS --------------------------------------- //

        /// Provides all information needed to specify a graph read operation.
        struct SGraphReadSpec
        {
            FILE* file;                                                     ///< File handle.
            Graph<TEdgeData>* graph;                                        ///< Graph object to be filled.
            EdgeIndex<TEdgeData>* edgeIndicesByDestination;                 ///< Array of destination-grouped edge indices. One is created per thread.
            EdgeIndex<TEdgeData>* edgeIndicesBySource;                      ///< Array of source-grouped edge indices. One is created per thread.
            GraphReader<TEdgeData>* reader;                                 ///< Graph reader object.
            SEdgeBufferData<TEdgeData>* bufs[2];                            ///< Edge data buffers.
            TEdgeCount counts[2];                                           ///< Edge data buffer counts.
            bool readSuccessfulSoFar;                                       ///< Indicates the continued success of the read operation.
        };
        
        
        // -------- CLASS METHODS ------------------------------------------ //

        /// Controls the consumption of edges from a buffer to a graph object, for use as a Spindle task function.
        /// @param [in] arg Pointer to an instance of #SGraphReadSpec that defines the graph read operation.
        static void EdgeConsumer(void* arg)
        {
            SGraphReadSpec* readSpec = (SGraphReadSpec*)arg;
            uint32_t currentIndex = 0;
            uint64_t totalEdgesRead = 0ull;

            // Allocate each thread's edge index and initialize the edge counter. Only one thread needs to do this.
            if (0 == spindleGetLocalThreadID())
            {
                readSpec->edgeIndicesByDestination = new EdgeIndex<TEdgeData>[spindleGetLocalThreadCount()];
                readSpec->edgeIndicesBySource = new EdgeIndex<TEdgeData>[spindleGetLocalThreadCount()];
            }

            spindleBarrierLocal();

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
                for (TEdgeCount i = spindleGetLocalThreadID(); i < readSpec->counts[currentIndex]; i += spindleGetLocalThreadCount())
                {
                    // Add each edge assigned to this thread to the edge index.
                    Edge<TEdgeData> edgeInfo;
                    
                    edgeInfo.FillFromDestinationEdgeBuffer(readSpec->bufs[currentIndex][i]);
                    edgeInfo.seq = totalEdgesRead + i;
                    readSpec->edgeIndicesBySource[spindleGetLocalThreadID()].InsertEdge(readSpec->bufs[currentIndex][i].sourceVertex, edgeInfo);

                    edgeInfo.vertex = readSpec->bufs[currentIndex][i].sourceVertex;
                    edgeInfo.seq = totalEdgesRead + i;
                    readSpec->edgeIndicesByDestination[spindleGetLocalThreadID()].InsertEdge(readSpec->bufs[currentIndex][i].destinationVertex, edgeInfo);
                }

                // Update the total number of edges read so far.
                totalEdgesRead += readSpec->counts[currentIndex];

                // Switch to the other buffer to consume in parallel with edge production.
                currentIndex = (~currentIndex) & 1;
            }
            
            // Merge all edge indices together into a single final edge index.
            // This will be done using a parallel tree merge pattern.
            // The value of this variable is an indicator of the current level of the tree.
            // Each iteration it is multiplied by two to indicate the number of consecutive edge indices that end up being merged together.
            // At the end, all edges are merged into the graph, and within each indexed vertex edges will appear in the same order as they appeared in the input file.
            currentIndex = 2;

            while (currentIndex < (spindleGetLocalThreadCount() << 1))
            {
                const uint32_t selectionMask = currentIndex - 1;
                const uint32_t mergeWithOffset = (currentIndex >> 1);
                SequenceAscendingComparator<TEdgeData> comparator;

                spindleBarrierLocal();

                // Only some threads are active during the current iteration.
                // This formula is equivalent to checking if the local thread ID is divisible by the current tree level index.
                // Additionally parallelize across source and destination vertex indices.
                // File ordering is preserved using the sequence number field of each edge.
                // Since each individual index is already sorted by sequence number just by virtue of reading the file in order, merging by sequence number produces the correct order.
                switch (spindleGetLocalThreadID() & selectionMask)
                {
                    case 0:
                        if ((spindleGetLocalThreadID() + mergeWithOffset) < spindleGetLocalThreadCount())
                            readSpec->edgeIndicesByDestination[spindleGetLocalThreadID()].MergeEdges(readSpec->edgeIndicesByDestination[spindleGetLocalThreadID() + mergeWithOffset], comparator);
                        break;
                        
                    case 1:
                        if ((spindleGetLocalThreadID() - 1 + mergeWithOffset) < spindleGetLocalThreadCount())
                            readSpec->edgeIndicesBySource[spindleGetLocalThreadID() - 1].MergeEdges(readSpec->edgeIndicesBySource[spindleGetLocalThreadID() - 1 + mergeWithOffset], comparator);
                        break;
                        
                    default:
                        break;
                }
                
                currentIndex <<= 1;
            }

            spindleBarrierLocal();

            // Merging complete. Set the graph to have the merged edges.
            if (0 == spindleGetLocalThreadID())
                readSpec->graph->SetEdgeIndices(readSpec->edgeIndicesByDestination[0], readSpec->edgeIndicesBySource[0]);
        }
        
        /// Controls the production of edges from a graph file to a buffer, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of #SGraphReadSpec that defines the graph read operation.
        static void EdgeProducer(void* arg)
        {
            SGraphReadSpec* readSpec = (SGraphReadSpec*)arg;
            uint32_t currentBufferIndex = 0;

            while (true)
            {
                // Fill the buffer with edges.
                readSpec->counts[currentBufferIndex] = readSpec->reader->ReadEdgesToBuffer(readSpec->file, readSpec->bufs[currentBufferIndex], (kGraphReadBufferSize / sizeof(SEdgeBufferData<void>)));

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
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        GraphReader(void)
        {
            // Nothing to do here.
        }
        
        
    private:
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //
        
        /// Opens and performs any initial file reading tasks required to prepare the graph file for reading of edges.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// @param [in] filename File name of the file to be opened for reading.
        /// @return File handle for the opened file.
        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename) = 0;
        
        /// Reads the next set of edges from the specified graph file into the specified buffer.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// @param [in] graphfile File handle for the open graph file.
        /// @param [in] buf Buffer to which to read edge data.
        /// @param [in] count Number of edges that the buffer can hold.
        /// @return Number of edges read to the buffer.
        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, SEdgeBufferData<TEdgeData>* buf, size_t count) = 0;
        

    public:
        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Reads a graph from the specified file.
        /// @param [in] filename File name of the file to be read.
        /// @param [out] graph Graph object to be filled. Not modified if an error occurs during reading.
        /// @return Number of edges read, or kGraphReadError in the event of an error.
        TEdgeCount ReadGraphFromFile(const std::string& filename, Graph<TEdgeData>& graph)
        {
            // First, open the file.
            FILE* graphfile = this->OpenAndInitializeGraphFile(filename);
            if (NULL == graphfile)
                return false;

            // Allocate some buffers for read data.
            SEdgeBufferData<TEdgeData>* bufs[] = { (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]), (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]) };

            // Define the graph read task.
            SGraphReadSpec readSpec;

            readSpec.file = graphfile;
            readSpec.graph = &graph;
            readSpec.reader = this;
            readSpec.bufs[0] = bufs[0];
            readSpec.bufs[1] = bufs[1];
            readSpec.counts[0] = 0;
            readSpec.counts[1] = 0;
            readSpec.readSuccessfulSoFar = true;

            // Define the parallelization strategy.
            SSpindleTaskSpec taskSpec[2];

            taskSpec[0].func = &EdgeProducer;
            taskSpec[0].arg = (void*)&readSpec;
            taskSpec[0].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
            taskSpec[0].numThreads = 1;
            taskSpec[0].smtPolicy = SpindleSMTPolicyPreferPhysical;

            taskSpec[1].func = &EdgeConsumer;
            taskSpec[1].arg = (void*)&readSpec;
            taskSpec[1].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
            taskSpec[1].numThreads = 0;
            taskSpec[1].smtPolicy = SpindleSMTPolicyPreferPhysical;

            // Launch the graph read task.
            spindleThreadsSpawn(taskSpec, sizeof(taskSpec) / sizeof(taskSpec[0]), true);

            // Clean up.
            fclose(graphfile);
            delete[] bufs[0];
            delete[] bufs[1];

            return readSpec.readSuccessfulSoFar;
        }
    };
}
