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

#include "EdgeIndex.hpp"
#include "Graph.hpp"
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
    public:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Specifies the size in bytes of each read buffer to use when reading data from the file.
        /// Two buffers are created, 64MB each by default.
        static const size_t kGraphReadBufferSize = (64ull * 1024ull * 1024ull);
        
        
    private:
        // -------- TYPE DEFINITIONS --------------------------------------- //

        /// Provides all information needed to specify a graph read operation.
        struct SGraphReadSpec
        {
            FILE* file;
            Graph<TEdgeData>* graph;
            EdgeIndex<TEdgeData>* edgeIndicesByDestination;
            EdgeIndex<TEdgeData>* edgeIndicesBySource;
            GraphReader<TEdgeData>* reader;
            SEdgeBufferData<TEdgeData>* bufs[2];
            TEdgeCount counts[2];
        };
        
        
        // -------- CLASS METHODS ------------------------------------------ //

        /// Controls the consumption of edges from a buffer to a graph object, for use as a Spindle task function.
        /// Should be called by either one or two threads.
        /// @param [in] arg Pointer to an instance of #SGraphReadSpec that defines the graph read operation.
        static void EdgeConsumer(void* arg)
        {
            SGraphReadSpec* readSpec = (SGraphReadSpec*)arg;
            uint32_t currentIndex = 0;

            // Allocate each thread's edge index. Only one thread needs to do this.
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

                // Read the buffer into the graph.
                for (TEdgeCount i = spindleGetLocalThreadID(); i < readSpec->counts[currentIndex]; i += spindleGetLocalThreadCount())
                {
                    // Add each edge assigned to this thread to the edge index.
                    SEdge<TEdgeData> edgeInfo;
                    
                    edgeInfo.FillFromDestinationEdgeBuffer(readSpec->bufs[currentIndex][i]);
                    readSpec->edgeIndicesBySource[spindleGetLocalThreadID()].InsertEdge(readSpec->bufs[currentIndex][i].sourceVertex, edgeInfo);

                    edgeInfo.vertex = readSpec->bufs[currentIndex][i].sourceVertex;
                    readSpec->edgeIndicesByDestination[spindleGetLocalThreadID()].InsertEdge(readSpec->bufs[currentIndex][i].destinationVertex, edgeInfo);
                }

                // Switch to the other buffer to consume in parallel with edge production.
                currentIndex = (~currentIndex) & 1;
            }
            
            // Merge all edge indices together into a single final edge index.
            // This will be done using a parallel tree merge pattern.
            // The value of this variable is an indicator of the current level of the tree.
            // Each iteration it is multiplied by two to indicate the number of consecutive edge indices that end up being merged together.
            currentIndex = 2;

            while (currentIndex < (spindleGetLocalThreadCount() << 1))
            {
                const uint32_t selectionMask = currentIndex - 1;
                const uint32_t mergeWithOffset = (currentIndex >> 1);
                
                spindleBarrierLocal();

                // Only some threads are active during the current iteration.
                // This formula is equivalent to checking if the local thread ID is divisible by the current tree level index.
                // Additionally parallelize across source and destination vertex indices.
                switch (spindleGetLocalThreadID() & selectionMask)
                {
                case 0:
                    if ((spindleGetLocalThreadID() + mergeWithOffset) < spindleGetLocalThreadCount())
                        readSpec->edgeIndicesByDestination[spindleGetLocalThreadID()].MergeEdges(readSpec->edgeIndicesByDestination[spindleGetLocalThreadID() + mergeWithOffset]);
                    break;
                
                case 1:
                    if ((spindleGetLocalThreadID() -1 + mergeWithOffset) < spindleGetLocalThreadCount())
                        readSpec->edgeIndicesBySource[spindleGetLocalThreadID() - 1].MergeEdges(readSpec->edgeIndicesBySource[spindleGetLocalThreadID() - 1 + mergeWithOffset]);
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
            uint32_t currentBufferIndex = 0ull;

            while (true)
            {
                // Fill the buffer with edges.
                readSpec->counts[currentBufferIndex] = readSpec->reader->ReadEdgesToBuffer(readSpec->file, readSpec->bufs[currentBufferIndex], (kGraphReadBufferSize / sizeof(SEdgeBufferData<void>)));

                // Synchronize with consumers.
                spindleBarrierGlobal();

                // Check for termination.
                if (0 == readSpec->counts[currentBufferIndex])
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

        /// Default destructor.
        virtual ~GraphReader()
        {
            // Nothing to do here.
        }
        
        
    private:
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //
        
        /// Opens and performs any initial file reading tasks required to prepare the graph file for reading of edges.
        /// @param [in] filename File name of the file to be opened for reading.
        /// @return File handle for the opened file.
        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename) = 0;
        
        /// Reads the next set of edges from the specified graph file into the specified buffer.
        /// Size is supplied in bytes, and the subclass should use this to determine how many edges to read.
        /// @param [in] graphfile File handle for the open graph file.
        /// @param [in] buf Buffer to which to read edge data.
        /// @param [in] count Number of edges that the buffer can hold.
        /// @return Number of edges read to the buffer.
        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, SEdgeBufferData<TEdgeData>* buf, size_t count) = 0;
        

    public:
        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Reads a graph from the specified file.
        /// @param [in] filename File name of the file to be read.
        /// @return Graph object, or NULL in the event of an error.
        Graph<TEdgeData>* ReadGraphFromFile(const std::string& filename)
        {
            // First, open the file.
            FILE* graphfile = this->OpenAndInitializeGraphFile(filename);
            if (NULL == graphfile)
                return NULL;

            // Next, allocate the graph object.
            Graph<TEdgeData>* graph = new Graph<TEdgeData>();

            // Allocate some buffers for read data.
            SEdgeBufferData<TEdgeData>* bufs[] = { (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]), (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]) };

            // Define the graph read task.
            SGraphReadSpec readSpec;

            readSpec.file = graphfile;
            readSpec.graph = graph;
            readSpec.reader = this;
            readSpec.bufs[0] = bufs[0];
            readSpec.bufs[1] = bufs[1];
            readSpec.counts[0] = 0;
            readSpec.counts[1] = 0;

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

            return graph;
        }
    };
}
