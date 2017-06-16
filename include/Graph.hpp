/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Graph.hpp
 *   Implementation of the data structure used to represent a graph.
 *****************************************************************************/

#pragma once

#include "DynamicEdgeList.h"
#include "DynamicVertexIndex.h"
#include "Types.h"

#include <cstddef>
#include <cstdint>


namespace GraphTool
{
    /// Represents a graph.
    /// Holds topology information and per-edge data, such as weights.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class Graph
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Alias for read-only iterators over indexed vertices.
        typedef typename DynamicVertexIndex<TEdgeData>::VertexIterator VertexIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Destination-grouped edge data structure.
        /// Maps from a destination vertex ID to a set of vertices from which in-edges exist.
        DynamicVertexIndex<TEdgeData> edgesByDestination;
        
        /// Source-grouped vertex data structure.
        /// Maps from a source vertex ID to a set of vertices to which out-edges exist.
        DynamicVertexIndex<TEdgeData> edgesBySource;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        Graph(void) : edgesByDestination(), edgesBySource()
        {
            // Nothing to do here.
        }


        // -------- INSTANCE METHODS --------------------------------------- //
        
        inline void FastInsertEdgeByDestination(const SEdge<TEdgeData>& edge)
        {
            edgesByDestination.FastInsertEdgeIndexedByDestination(edge);
        }
        
        inline void FastInsertEdgeBySource(const SEdge<TEdgeData>& edge)
        {
            edgesBySource.FastInsertEdgeIndexedBySource(edge);
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
        
        /// Retrieves and returns the number of Vector-Sparse vectors required to represent the destination-grouped edges.
        /// @return Number of vectors required.
        inline size_t GetNumVectorsDestination(void) const
        {
            return edgesByDestination.GetNumVectors();
        }
        
        /// Retrieves and returns the number of Vector-Sparse vectors required to represent the source-grouped edges.
        /// @return Number of vectors required.
        inline size_t GetNumVectorsSource(void) const
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
        
        /// Inserts an edge into the destination-grouped representation using the specified edge buffer.
        /// @param [in] edge Edge to insert.
        inline void InsertEdgeByDestination(const SEdge<TEdgeData>& edge)
        {
            edgesByDestination.InsertEdgeIndexedByDestination(edge);
        }
        
        /// Inserts an edge into the source-grouped representation using the specified edge buffer.
        /// @param [in] edge Edge to insert.
        inline void InsertEdgeBySource(const SEdge<TEdgeData>& edge)
        {
            edgesBySource.InsertEdgeIndexedBySource(edge);
        }
        
        /// Refreshes the counts of edges and vectors in this data structure.
        /// Intended to be called from within a Spindle parallelized region.
        /// Required after invoking fast insertion methods, which do not update any counts.
        /// @param [in] buf Temporary array allocated with two locations per thread.
        inline void ParallelRefreshDegreeInfo(size_t* buf)
        {
            edgesByDestination.ParallelRefreshDegreeInfo(buf);
            edgesBySource.ParallelRefreshDegreeInfo(buf);
        }
        
        /// Removes an edge from the graph.
        /// @param [in] fromVertex Identifies the source vertex of the edge.
        /// @param [in] toVertex Identifies the destination vertex of the edge.
        inline void RemoveEdge(TVertexID fromVertex, TVertexID toVertex)
        {
            edgesByDestination.RemoveEdge(toVertex, fromVertex);
            edgesBySource.RemoveEdge(fromVertex, toVertex);
        }
        
        /// Removes a vertex from the graph, including all edges that include it.
        /// @param [in] vertex Identifier of the vertex to remove.
        inline void RemoveVertex(TVertexID vertex)
        {
            // TODO: Implement this method.
        }
        
        /// Sets the number of vertices in the graph.
        /// This operation causes a change in the underlying graph data structures.
        /// @param [in] numVertices Number of vertices.
        inline void SetNumVertices(const TVertexCount numVertices)
        {
            edgesByDestination.SetNumVertices(numVertices);
            edgesBySource.SetNumVertices(numVertices);
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
