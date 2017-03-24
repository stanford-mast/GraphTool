/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReader.h
 *   Declaration of the common interface to objects capable of interpreting
 *   graph files of various formats.
 *****************************************************************************/

#pragma once

#include <Graph.h>

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>


namespace GraphTool
{
    /// Base class for all object types that are used to interpret graph files of various formats.
    /// Objects in this hierarchy load graph data from a file and produce one of the graph objects.
    /// Some common functionality is implemented directly in the base class.
    class GraphReader
    {   
    public:
        // -------- CONSTANTS ------------------------------------------------------ //
        
        /// Specifies the size in bytes of each read buffer to use when reading data from the file.
        /// Two buffers are created, 1GB each by default.
        static const size_t kGraphReadBufferSize = (1024ull * 1024ull * 1024ull);
        
        
    private:
        // -------- TYPE DEFINITIONS ----------------------------------------------- //

        /// Provides all information needed to specify a graph read operation.
        struct SGraphReadSpec
        {
            FILE* file;
            Graph* graph;
            GraphReader* reader;
            void* bufs[2];
            TEdgeCount counts[2];
        };
        
        
        // -------- CLASS METHODS -------------------------------------------------- //

        /// Controls the consumption of edges from a buffer to a graph object, for use as a Spindle task function.
        /// Should be called by either one or two threads.
        /// @param [in] arg Pointer to an instance of #SGraphReadSpec that defines the graph read operation.
        static void EdgeConsumer(void* arg);
        
        /// Controls the production of edges from a graph file to a buffer, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of #SGraphReadSpec that defines the graph read operation.
        static void EdgeProducer(void* arg);
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Default constructor.
        GraphReader();

        /// Default destructor.
        virtual ~GraphReader();
        
        
    protected:
        // -------- ABSTRACT INSTANCE METHODS -------------------------------------- //

        /// Allocates a graph object of the correct type.
        /// Subclasses determine which type of graph object is correct for the file format being read and the specified options.
        /// @return Graph object of the correct type.
        virtual Graph* AllocateGraphObject(void) = 0;
        
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
        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, void* buf, size_t size) = 0;
        

    public:
        // -------- INSTANCE METHODS ----------------------------------------------- //
        
        /// Reads a graph from the specified file.
        /// @param [in] filename File name of the file to be read.
        /// @return Graph object, or NULL in the event of an error.
        Graph* ReadGraphFromFile(const std::string& filename);
    };
}
