/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Graph.h
 *   Declaration of the common interface to an object that represents a graph.
 *****************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>


namespace GraphTool
{
    /// Specifies the type to use for identifying vertices.
    typedef uint64_t TVertexID;

    /// Specifies the type to use for counting vertices.
    typedef uint64_t TVertexCount;

    /// Specifies the type to use for counting edges.
    typedef uint64_t TEdgeCount;
    
    /// Base class for all object types that are used to represent a graph.
    /// Because graph implementations differ significantly, common functionality is limited.
    class Graph
    {
    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Default constructor.
        Graph(void);

        /// Default destructor.
        virtual ~Graph(void);


        // -------- ABSTRACT INSTANCE METHODS -------------------------------------- //
        
        /// Reads edges from a buffer and adds them to the part of the graph that stores in-edges.
        /// Both this method and #AddOutEdges should be called on the same buffer to ensure data structure consistency.
        /// This method is exposed directly to facilitate parallel buffer consumption for both in-edges and out-edges.
        /// Otherwise, use #AddEdges instead.
        /// @param [in] buffer Buffer containing edges to read.
        /// @param [in] count Number of edges contained within the buffer, size determined by the implementation.
        virtual void AddInEdges(void* buffer, size_t count) = 0;
        
        /// Reads edges from a buffer and adds them to the part of the graph that stores out-edges.
        /// Both this method and #AddInEdges should be called on the same buffer to ensure data structure consistency.
        /// This method is exposed directly to facilitate parallel buffer consumption for both in-edges and out-edges.
        /// Otherwise, use #AddEdges instead.
        /// @param [in] buffer Buffer containing edges to read.
        /// @param [in] count Number of edges contained within the buffer, size determined by the implementation.
        virtual void AddOutEdges(void* buffer, size_t count) = 0;
        
        /// Writes the in-edges for the specified vertex to the specified buffer.
        /// Behavior is controlled by specified options, which are subclass-defined.
        /// Buffer is assumed to be large enough to hold all of the edges for the specified vertex, subject to the specified options.
        /// @param [in] buffer Target buffer.
        /// @param [in] vertex Vertex of interest.
        /// @param [in] options Options that control export behavior.
        /// @return Number of edges written to the buffer. May be 0 if the vertex has no edges or if there is a problem with the options.
        virtual TEdgeCount ExportVertexInEdges(void* buffer, TVertexID vertex, uint64_t options) = 0;
        
        /// Writes the out-edges for the specified vertex to the specified buffer.
        /// Behavior is controlled by specified options, which are subclass-defined.
        /// Buffer is assumed to be large enough to hold all of the edges for the specified vertex, subject to the specified options.
        /// @param [in] buffer Target buffer.
        /// @param [in] vertex Vertex of interest.
        /// @param [in] options Options that control export behavior.
        /// @return Number of edges written to the buffer. May be 0 if the vertex has no edges or if there is a problem with the options.
        virtual TEdgeCount ExportVertexOutEdges(void* buffer, TVertexID vertex, uint64_t options) = 0;
        
        /// Determines the size, in bytes, occupied by an edge to be exported based on the specified options.
        /// @param [in] options Options that control export behavior.
        /// @return Size occupied by each edge to be exported under the specified options. 0 indicates a problem with the options or, less likely, an absence of edges.
        virtual size_t GetExportEdgeSizeWithOptions(uint64_t options) = 0;
        
        /// Retrieves and returns the in-degree of the vertex with the largest in-degree.
        /// @return Highest in-degree in the graph.
        virtual TVertexCount GetMaximumIndegree(void) = 0;
        
        /// Retrieves and returns the out-degree of the vertex with the largest out-degree.
        /// @return Highest out-degree in the graph.
        virtual TVertexCount GetMaximumOutdegree(void) = 0;
        
        /// Retrieves and returns the number of edges in the graph.
        /// @return Number of edges in the graph.
        virtual TEdgeCount GetNumEdges(void) = 0;

        /// Retrieves and returns the number of vertices in the graph.
        /// @return Number of vertices in the graph.
        virtual TVertexCount GetNumVertices(void) = 0;
        
        /// Retrieves and returns the in-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return In-degree of the specified vertex. 0 is returned if the vertex has no in-edges or does not exist.
        virtual TVertexCount GetVertexIndegree(TVertexID vertex) = 0;
        
        /// Retrieves and returns the out-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return Out-degree of the specified vertex. 0 is returned if the vertex has no out-edges or does not exist.
        virtual TVertexCount GetVertexOutdegree(TVertexID vertex) = 0;
        
        /// Removes an edge from the graph.
        /// @param [in] fromVertex Identifies the source vertex of the edge.
        /// @param [in] toVertex Identifies the destination vertex of the edge.
        virtual void RemoveEdge(TVertexID fromVertex, TVertexID toVertex) = 0;
        
        /// Removes a vertex from the graph, including all edges that include it.
        /// @param [in] vertex Identifier of the vertex to remove.
        virtual void RemoveVertex(TVertexID vertex) = 0;

        
        // -------- INSTANCE METHODS ----------------------------------------------- //

        /// Reads edges from a buffer and adds them to the graph.
        /// Specific format is as specified by the subclass that performs the import operation.
        /// This is a convenience method that calls both #AddInEdges and #AddOutEdges in sequence.
        /// @param [in] buffer Buffer containing edges to read.
        /// @param [in] count Number of edges contained within the buffer, size determined by the implementation.
        void AddEdges(void* buffer, size_t count);

        /// Computes the recommended buffer size (in bytes) for exporting in-edges, based on the specified options.
        /// Buffer size is computed by multiplying the size per edge by the highest in-degree in the graph.
        /// @param options Options that control export behavior.
        /// @return Recommended in-edge export buffer size, in bytes. 0 indicates a problem with the options or, less likely, an absence of edges.
        size_t GetExportRecommendedInEdgeBufferSizeWithOptions(uint64_t options);

        /// Computes the recommended buffer size (in bytes) for exporting out-edges, based on the specified options.
        /// Buffer size is computed by multiplying the size per edge by the highest out-degree in the graph.
        /// @param options Options that control export behavior.
        /// @return Recommended out-edge export buffer size, in bytes. 0 indicates a problem with the options.
        size_t GetExportRecommendedOutEdgeBufferSizeWithOptions(uint64_t options);
    };
}
