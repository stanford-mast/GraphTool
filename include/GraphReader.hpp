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
        // -------- CONSTANTS ------------------------------------------------------ //
        
        /// Specifies the size in bytes of each read buffer to use when reading data from the file.
        /// Two buffers are created, 64MB each by default.
        static const size_t kGraphReadBufferSize = (64ull * 1024ull * 1024ull);
        
        
    private:
        // -------- TYPE DEFINITIONS ----------------------------------------------- //

        /// Provides all information needed to specify a graph read operation.
        struct SGraphReadSpec
        {
            FILE* file;
            EdgeIndex<TEdgeData>* edgeIndices;
            GraphReader<TEdgeData>* reader;
            SEdgeBufferData<TEdgeData>* bufs[2];
            TEdgeCount counts[2];
        };
        
        
        // -------- CLASS METHODS -------------------------------------------------- //

        /// Controls the consumption of edges from a buffer to a graph object, for use as a Spindle task function.
        /// Should be called by either one or two threads.
        /// @param [in] arg Pointer to an instance of #SGraphReadSpec that defines the graph read operation.
        static void EdgeConsumer(void* arg)
        {
            SGraphReadSpec* readSpec = (SGraphReadSpec*)arg;
            uint8_t currentBufferIndex = 0ull;

            while (true)
            {
                // Wait for the buffer to be filled with edges.
                spindleBarrierGlobal();

                // Check for termination.
                if (0 == readSpec->counts[currentBufferIndex])
                    break;

                // Read the buffer into the graph.
                for (TEdgeCount i = spindleGetLocalThreadID(); i < readSpec->counts[currentBufferIndex]; i += spindleGetLocalThreadCount())
                {
                    // TODO: Consume the edge at the specified index.
                }

                // Switch to the other buffer to consume in parallel with edge production.
                currentBufferIndex = (~currentBufferIndex) & 1ull;
            }
        }
        
        /// Controls the production of edges from a graph file to a buffer, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of #SGraphReadSpec that defines the graph read operation.
        static void EdgeProducer(void* arg)
        {
            SGraphReadSpec* readSpec = (SGraphReadSpec*)arg;
            uint8_t currentBufferIndex = 0ull;

            while (true)
            {
                // Fill the buffer with edges.
                readSpec->counts[currentBufferIndex] = readSpec->reader->ReadEdgesToBuffer(readSpec->file, readSpec->bufs[currentBufferIndex], kGraphReadBufferSize);

                // Synchronize with consumers.
                spindleBarrierGlobal();

                // Check for termination.
                if (0 == readSpec->counts[currentBufferIndex])
                    break;

                // Switch to the other buffer to read from file during a consumption operation.
                currentBufferIndex = (~currentBufferIndex) & 1ull;
            }
        }
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

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
        
        
    protected:
        // -------- ABSTRACT INSTANCE METHODS -------------------------------------- //
        
        /// Opens and performs any initial file reading tasks required to prepare the graph file for reading of edges.
        /// @param [in] filename File name of the file to be opened for reading.
        /// @return File handle for the opened file.
        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename) = 0;
        
        /// Reads the next set of edges from the specified graph file into the specified buffer.
        /// Size is supplied in bytes, and the subclass should use this to determine how many edges to read.
        /// @param [in] graphfile File handle for the open graph file.
        /// @param [in] buf Buffer to which to read edge data.
        /// @param [in] size Size of the buffer, in bytes.
        /// @return Number of edges read to the buffer.
        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, SEdgeBufferData<TEdgeData>* buf, size_t size) = 0;
        

    public:
        // -------- INSTANCE METHODS ----------------------------------------------- //
        
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
            void* bufs[] = { (void*)(new uint8_t[kGraphReadBufferSize]), (void*)(new uint8_t[kGraphReadBufferSize]) };

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
