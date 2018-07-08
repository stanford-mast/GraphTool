/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphWriter.h
 *   Declaration of the common functionality for objects capable of producing
 *   graph files of various formats.
 *****************************************************************************/

#pragma once

#include "IGraphWriter.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    class Graph;

    
    /// Base class for all object types that are used to produce graph files of various formats.
    /// Objects in this hierarchy consume graph objects and produce files.
    /// Some common functionality is implemented directly in the base class.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class GraphWriter : public IGraphWriter
    {
    private:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Specifies the size in bytes of each write buffer to use when reading data from the file.
        /// Two buffers are created, 64MB each by default.
        static const size_t kGraphWriteBufferSize = (64ull * 1024ull * 1024ull);
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor.
        GraphWriter(void);

        /// Destructor.
        virtual ~GraphWriter(void);
        
        
    private:
        // -------- CLASS METHODS ------------------------------------------ //
        
        /// Controls the consumption of edges from a buffer to a file, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of SGraphWriteSpec that defines the graph write operation.
        static void EdgeConsumer(void* arg);

        /// Controls the production of edges from a graph object to a buffer, for use as a Spindle task function.
        /// Should be called by a single thread.
        /// @param [in] arg Pointer to an instance of SGraphWriteSpec that defines the graph write operation.
        static void EdgeProducer(void* arg);

        
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Opens and performs any initial file reading tasks required to prepare the graph file for writing of graph data.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// @param [in] filename File name of the file to be opened for writing.
        /// @param [in] graph Graph to be written.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return File handle for the opened file.
        virtual FILE* OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination) = 0;
        
        /// Writes edge data from the specified buffer into the specified file.
        /// Invoked by only a single thread, so it is safe to modify any needed state without synchronization.
        /// @param [in] graphfile File handle for the open graph file.
        /// @param [in] graph Graph to be written.
        /// @param [in] buf Buffer from which to read edge data.
        /// @param [in] count Number of edges in the buffer.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return `true` if all edges were successfully written, `false` otherwise.
        virtual void WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination) = 0;
        
        
    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "IGraphWriter.h" for documentation.
        
        virtual EGraphResult WriteGraphToFile(const char* const filename, const Graph& graph, const bool groupedByDestination = false);
    };
}
