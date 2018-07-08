/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReader.h
 *   Declaration of the common functionality for objects capable of
 *   interpreting graph files of various formats.
 *****************************************************************************/

#pragma once

#include "IGraphReader.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    class Graph;

    
    /// Base class for all object types that are used to interpret graph files of various formats.
    /// Objects in this hierarchy load graph data from a file and produce one of the graph objects.
    /// Some common functionality is implemented directly in the base class.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class GraphReader : public IGraphReader
    {   
    private:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Specifies the size in bytes of each read buffer to use when reading data from the file.
        /// Two buffers are created, 64MB each by default.
        static const size_t kGraphReadBufferSize = (64ull * 1024ull * 1024ull);
        
        
    protected:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Specifies the number of vertices expected to be read in from the file.
        TVertexCount numVerticesInFile;
        
        /// Specifies the number of edges expected to be read in from the file.
        TEdgeCount numEdgesInFile;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor.
        GraphReader(void);
        
        /// Destructor.
        virtual ~GraphReader(void);
        
        
    private:
        // -------- CLASS METHODS ------------------------------------------ //

        /// Controls the consumption of edges from a buffer to a graph object, for use as a Spindle task function.
        /// @param [in] arg Pointer to an instance of SGraphReadSpec that defines the graph read operation.
        static void EdgeConsumer(void* arg);
        
        /// Controls the production of edges from a graph file to a buffer, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of SGraphReadSpec that defines the graph read operation.
        static void EdgeProducer(void* arg);
        
        
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //
        
        /// Opens and performs any initial file reading tasks required to prepare the graph file for reading of edges.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// Subclasses must set the numEdgesInFile and numVerticesInFile variables during this invocation.
        /// @param [in] filename File name of the file to be opened for reading.
        /// @return File handle for the opened file.
        virtual FILE* OpenAndInitializeGraphFileForRead(const char* const filename) = 0;
        
        /// Reads the next set of edges from the specified graph file into the specified buffer.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// @param [in] graphfile File handle for the open graph file.
        /// @param [in] buf Buffer to which to read edge data.
        /// @param [in] count Number of edges that the buffer can hold.
        /// @return Number of edges read to the buffer.
        virtual TEdgeCount ReadEdgesToBuffer(FILE* const graphfile, SEdge<TEdgeData>* buf, const size_t count) = 0;
        

    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "IGraphReader.h" for documentation.
        
        virtual bool ReadGraphFromFile(const char* const filename, Graph& graph);
    };
}
