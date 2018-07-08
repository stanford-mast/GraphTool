/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Graph.h
 *   Declaration of the top-level data structure used to represent a mutable
 *   graph in a format similar to Compressed-Sparse.
 *****************************************************************************/

#pragma once

#include "Types.h"
#include "VertexIndex.h"

#include <cstddef>
#include <cstdint>


namespace GraphTool
{
    /// Represents a graph.
    /// Internal format is like Compressed-Sparse and is optimized for mutability.
    /// Holds topology information and per-edge data, such as weights.
    class Graph
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Alias for read-only iterators over indexed vertices.
        typedef typename VertexIndex::VertexIterator VertexIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Edge data type indicator.
        /// Set by objects that modify the graph to indicate the type of data held at each edge.
        EEdgeDataType edgeDataType;
        
        /// Destination-grouped edge data structure.
        /// Maps from a destination vertex ID to a set of vertices from which in-edges exist.
        VertexIndex edgesByDestination;
        
        /// Source-grouped vertex data structure.
        /// Maps from a source vertex ID to a set of vertices to which out-edges exist.
        VertexIndex edgesBySource;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        Graph(void);


        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Performs a simple and fast insertion of the specified edge into the destination-grouped data structure.
        /// Does not update any internal counters for vectors or edges, nor does it automatically maintain consistency.
        /// Can be invoked from multiple threads, so long as each thread updates a different top-level vertex.
        /// Intended to be invoked during ingress or during large batch updates.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> inline void FastInsertEdgeByDestination(const SEdge<TEdgeData>& edge)
        {
            edgesByDestination.FastInsertEdgeIndexedByDestination(edge);
        }
        
        /// Performs a simple and fast insertion of the specified edge into the source-grouped data structure.
        /// Does not update any internal counters for vectors or edges, nor does it automatically maintain consistency.
        /// Can be invoked from multiple threads, so long as each thread updates a different top-level vertex.
        /// Intended to be invoked during ingress or during large batch updates.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> inline void FastInsertEdgeBySource(const SEdge<TEdgeData>& edge)
        {
            edgesBySource.FastInsertEdgeIndexedBySource(edge);
        }
        
        /// Retrieves and returns the type of data held at each edge.
        /// @return Edge data type.
        inline EEdgeDataType GetEdgeDataType(void) const
        {
            return edgeDataType;
        }
        
        /// Retrieves and returns the number of edges in the graph.
        /// @return Number of edges in the graph.
        inline TEdgeCount GetNumEdges(void) const
        {
            return edgesBySource.GetNumEdges();
        }

        /// Retrieves and returns the number of vertices in the graph.
        /// @return Number of vertices in the graph.
        inline TVertexCount GetNumVertices(void) const
        {
            return edgesBySource.GetNumVertices();
        }

        /// Retrieves and returns the number of vertices having non-zero indegree.
        /// @return Number of vertices with in-edges.
        inline TVertexCount GetNumVerticesPresentDestination(void) const
        {
            return edgesByDestination.GetNumVerticesPresent();
        }

        /// Retrieves and returns the number of vertices having non-zero outdegree.
        /// @return Number of vertices with out-edges.
        inline TVertexCount GetNumVerticesPresentSource(void) const
        {
            return edgesBySource.GetNumVerticesPresent();
        }
        
        /// Retrieves and returns the number of Vector-Sparse vectors required to represent the destination-grouped edges.
        /// @return Number of vectors required.
        inline uint64_t GetNumVectorsDestination(void) const
        {
            return edgesByDestination.GetNumVectors();
        }
        
        /// Retrieves and returns the number of Vector-Sparse vectors required to represent the source-grouped edges.
        /// @return Number of vectors required.
        inline uint64_t GetNumVectorsSource(void) const
        {
            return edgesBySource.GetNumVectors();
        }
        
        /// Retrieves and returns the in-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return In-degree of the specified vertex. 0 is returned if the vertex has no in-edges or does not exist.
        inline TEdgeCount GetVertexIndegree(TVertexID vertex) const
        {
            return edgesByDestination.GetDegree(vertex);
        }
        
        /// Retrieves and returns the out-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return Out-degree of the specified vertex. 0 is returned if the vertex has no out-edges or does not exist.
        inline TEdgeCount GetVertexOutdegree(TVertexID vertex) const
        {
            return edgesBySource.GetDegree(vertex);
        }
        
        /// Inserts an edge into the graph.
        /// Intended to perform minor updates to the graph after ingress.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> inline void InsertEdge(const SEdge<TEdgeData>& edge)
        {
            InsertEdgeByDestination(edge);
            InsertEdgeBySource(edge);
        }
        
        /// Inserts an edge into the destination-grouped representation using the specified edge buffer.
        /// Does not automatically maintain consistency.
        /// Intended to be invoked during ingress or during large batch updates.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> inline void InsertEdgeByDestination(const SEdge<TEdgeData>& edge)
        {
            edgesByDestination.InsertEdgeIndexedByDestination(edge);
        }
        
        /// Inserts an edge into the source-grouped representation using the specified edge buffer.
        /// Does not automatically maintain consistency.
        /// Intended to be invoked during ingress or during large batch updates.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> inline void InsertEdgeBySource(const SEdge<TEdgeData>& edge)
        {
            edgesBySource.InsertEdgeIndexedBySource(edge);
        }
        
        /// Refreshes graph metadata, such as degree information.
        /// Intended to be called from within a Spindle parallelized region.
        /// Required after invoking fast insertion methods, which do not update any counts.
        /// @param [in] buf Temporary array allocated with two locations per thread.
        inline void ParallelRefreshMetadata(uint64_t* buf)
        {
            edgesByDestination.ParallelRefreshMetadata(buf);
            edgesBySource.ParallelRefreshMetadata(buf);
        }
        
        /// Removes an edge from the graph.
        /// @param [in] fromVertex Identifies the source vertex of the edge.
        /// @param [in] toVertex Identifies the destination vertex of the edge.
        inline void RemoveEdge(const TVertexID fromVertex, const TVertexID toVertex)
        {
            edgesByDestination.RemoveEdge(toVertex, fromVertex);
            edgesBySource.RemoveEdge(fromVertex, toVertex);
        }
        
        /// Removes a vertex from the graph, including all edges that include it.
        /// @param [in] vertex Identifier of the vertex to remove.
        void RemoveVertex(const TVertexID vertex);
        
        /// Sets the edge data type based on the template parameter.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        template <typename TEdgeData> void SetEdgeDataType(void);
        
        /// Sets the number of vertices in the graph.
        /// This operation causes a change in the underlying graph data structures.
        /// @param [in] numVertices Number of vertices.
        inline void SetNumVertices(const TVertexCount numVertices)
        {
            edgesByDestination.SetNumVertices(numVertices);
            edgesBySource.SetNumVertices(numVertices);
        }
        
        /// Enables direct random read-only access to the destination-grouped vertex index.
        /// @return Read-only reference to destination-grouped vertex index.
        inline const VertexIndex& VertexIndexDestination(void) const
        {
            return (const VertexIndex&)edgesByDestination;
        }

        /// Enables direct random read-only access to the source-grouped vertex index.
        /// @return Read-only reference to source-grouped vertex index.
        inline const VertexIndex& VertexIndexSource(void) const
        {
            return (const VertexIndex&)edgesBySource;
        }
        
        /// Obtains a read-only random-access iterator to the specified vertex within the destination-grouped vertex index.
        /// @param [in] vertex Position within the vertex index.
        /// @return Read-only iterator to the specified vertex within the index.
        inline VertexIterator VertexIteratorDestinationAt(TVertexID vertex) const
        {
            return edgesByDestination.BeginIterator() + vertex;
        }
        
        /// Obtains a read-only random-access iterator to the beginning of the destination-grouped vertex index.
        /// @return Read-only iterator to the beginning of the vertex index.
        inline VertexIterator VertexIteratorDestinationBegin(void) const
        {
            return edgesByDestination.BeginIterator();
        }
        
        /// Obtains a read-only random-access iterator to the past-the-end element of the destination-grouped vertex index.
        /// @return Read-only iterator to the past-the-end element of the vertex index.
        inline VertexIterator VertexIteratorDestinationEnd(void) const
        {
            return edgesByDestination.EndIterator();
        }
        
        /// Obtains a read-only random-access iterator to the specified vertex within the source-grouped vertex index.
        /// @param [in] vertex Position within the vertex index.
        /// @return Read-only iterator to the specified vertex within the index.
        inline VertexIterator VertexIteratorSourceAt(TVertexID vertex) const
        {
            return edgesBySource.BeginIterator() + vertex;
        }

        /// Obtains a read-only random-access iterator to the beginning of the source-grouped vertex index.
        /// @return Read-only iterator to the beginning of the vertex index.
        inline VertexIterator VertexIteratorSourceBegin(void) const
        {
            return edgesBySource.BeginIterator();
        }

        /// Obtains a read-only random-access iterator to the past-the-end element of the source-grouped vertex index.
        /// @return Read-only iterator to the past-the-end element of the vertex index.
        inline VertexIterator VertexIteratorSourceEnd(void) const
        {
            return edgesBySource.EndIterator();
        }
    };
}
