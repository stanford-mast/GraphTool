/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Graph.hpp
 *   Implementation of a templated data structure used to represent a graph.
 *****************************************************************************/

#pragma once

#include "EdgeIndex.hpp"
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

        /// Convenience alias for read-only iterators over edges.
        typedef typename EdgeIndex<TEdgeData>::ConstEdgeIterator ConstEdgeIterator;

        /// Convenience alias for read-only iterators over indexed verties.
        typedef typename EdgeIndex<TEdgeData>::ConstVertexIterator ConstVertexIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Destination-grouped edge data structure.
        /// Maps from a destination vertex ID to a set of vertices from which in-edges exist.
        EdgeIndex<TEdgeData> edgesByDestination;
        
        /// Source-grouped vertex data structure.
        /// Maps from a source vertex ID to a set of vertices to which out-edges exist.
        EdgeIndex<TEdgeData> edgesBySource;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        Graph(void) : edgesByDestination(), edgesBySource()
        {
            // Nothing to do here.
        }


        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Obtains a read-only iterator to the beginning of the in-edges edges that correspond to the supplied destination vertex iterator.
        /// @return Iterator, as described, or an invalid iterator if the supplied vertex iterator does not belong to the destination edge index.
        inline ConstEdgeIterator ConstEdgeIteratorDestinationAt(ConstVertexIterator& vertexIterator) const
        {
            return edgesByDestination.ConstEdgeIteratorAt(vertexIterator);
        }
        
        /// Obtains a read-only iterator to the beginning of this graph's edges, grouped by destination vertex.
        /// @return Iterator, as described.
        inline ConstEdgeIterator ConstEdgeIteratorDestinationBegin(void) const
        {
            return edgesByDestination.ConstEdgeIteratorBegin();
        }

        /// Obtains a read-only iterator to the end of this graph's edges, grouped by destination vertex.
        /// @return Iterator, as described.
        inline ConstEdgeIterator ConstEdgeIteratorDestinationEnd(void) const
        {
            return edgesByDestination.ConstEdgeIteratorEnd();
        }

        /// Obtains a read-only iterator to the beginning of the out-edges edges that correspond to the supplied source vertex iterator.
        /// @return Iterator, as described, or an invalid iterator if the supplied vertex iterator does not belong to the destination edge index.
        inline ConstEdgeIterator ConstEdgeIteratorSourceAt(ConstVertexIterator& vertexIterator) const
        {
            return edgesBySource.ConstEdgeIteratorAt(vertexIterator);
        }
        
        /// Obtains a read-only iterator to the beginning of this graph's edges, grouped by source vertex.
        /// @return Iterator, as described.
        inline ConstEdgeIterator ConstEdgeIteratorSourceBegin(void) const
        {
            return edgesBySource.ConstEdgeIteratorBegin();
        }
        
        /// Obtains a read-only iterator to the end of this graph's edges, grouped by source vertex.
        /// @return Iterator, as described.
        inline ConstEdgeIterator ConstEdgeIteratorSourceEnd(void) const
        {
            return edgesBySource.ConstEdgeIteratorEnd();
        }

        /// Obtains a read-only iterator to this graph's destination vertices starting with the specified vertex.
        /// @return Iterator, as described, or an invalid iterator if the supplied vertex is not present in the graph.
        inline ConstVertexIterator ConstVertexIteratorDestinationAt(const TVertexID vertexID) const
        {
            return edgesByDestination.ConstVertexIteratorAt(vertexID);
        }
        
        /// Obtains a read-only iterator to the beginning of this graph's destination vertices.
        /// @return Iterator, as described.
        inline ConstVertexIterator ConstVertexIteratorDestinationBegin(void) const
        {
            return edgesByDestination.ConstVertexIteratorBegin();
        }

        /// Obtains a read-only iterator to the end of this graph's destination vertices.
        /// @return Iterator, as described.
        inline ConstVertexIterator ConstVertexIteratorDestinationEnd(void) const
        {
            return edgesByDestination.ConstVertexIteratorEnd();
        }

        /// Obtains a read-only iterator to this graph's source vertices starting with the specified vertex.
        /// @return Iterator, as described, or an invalid iterator if the supplied vertex is not present in the graph.
        inline ConstVertexIterator ConstVertexIteratorSourceAt(const TVertexID vertexID) const
        {
            return edgesBySource.ConstVertexIteratorAt(vertexID);
        }
        
        /// Obtains a read-only iterator to the beginning of this graph's source vertices.
        /// @return Iterator, as described.
        inline ConstVertexIterator ConstVertexIteratorSourceBegin(void) const
        {
            return edgesBySource.ConstVertexIteratorBegin();
        }

        /// Obtains a read-only iterator to the end of this graph's source vertices.
        /// @return Iterator, as described.
        inline ConstVertexIterator ConstVertexIteratorSourceEnd(void) const
        {
            return edgesBySource.ConstVertexIteratorEnd();
        }
        
        /// Retrieves and returns the number of edges in the graph.
        /// @return Number of edges in the graph.
        inline TEdgeCount GetNumEdges(void) const
        {
            return this->edgesBySource.GetNumEdges();
        }

        /// Retrieves and returns the number of vertices in the graph.
        /// @return Number of vertices in the graph.
        TVertexCount GetNumVertices(void) const
        {
            TVertexCount numDestinationVertices = 0;
            TVertexCount numSourceVertices = 0;

            bool destinationHasVertices = this->edgesByDestination.GetIndexUpperBound(numDestinationVertices);
            bool sourceHasVertices = this->edgesBySource.GetIndexUpperBound(numSourceVertices);

            numDestinationVertices = (destinationHasVertices ? numDestinationVertices + 1 : 0);
            numSourceVertices = (sourceHasVertices ? numSourceVertices + 1 : 0);
            
            return (numDestinationVertices > numSourceVertices ? numDestinationVertices : numSourceVertices);
        }
        
        /// Retrieves and returns the in-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return In-degree of the specified vertex. 0 is returned if the vertex has no in-edges or does not exist.
        inline TEdgeCount GetVertexIndegree(TVertexID vertex) const
        {
            return this->edgesByDestination.GetVertexDegree(vertex);
        }
        
        /// Retrieves and returns the out-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return Out-degree of the specified vertex. 0 is returned if the vertex has no out-edges or does not exist.
        inline TEdgeCount GetVertexOutdegree(TVertexID vertex) const
        {
            return this->edgesBySource.GetVertexDegree(vertex);
        }
        
        /// Removes an edge from the graph.
        /// @param [in] fromVertex Identifies the source vertex of the edge.
        /// @param [in] toVertex Identifies the destination vertex of the edge.
        void RemoveEdge(TVertexID fromVertex, TVertexID toVertex)
        {
            this->edgesByDestination.RemoveEdge(toVertex, fromVertex);
            this->edgesBySource.RemoveEdge(fromVertex, toVertex);
        }
        
        /// Removes a vertex from the graph, including all edges that include it.
        /// @param [in] vertex Identifier of the vertex to remove.
        void RemoveVertex(TVertexID vertex)
        {
            this->edgesByDestination.RemoveVertex(vertex);
            this->edgesBySource.RemoveVertex(vertex);
        }

        /// Replaces the existing graph edge indices with those provided.
        /// Implemented internally as a swap, so the provided edge indices will be empty when done.
        /// It is up to the caller to generate proper and consistent data structures, as this method does no checking.
        /// @param [in,out] replacementEdgesByDestination Edge data structure containing new edges to set, indexed by destination vertex.
        /// @param [in,out] replacementEdgesBySource Edge data structure containing new edges to set, indexed by source vertex.
        void SetEdgeIndices(EdgeIndex<TEdgeData>& replacementEdgesByDestination, EdgeIndex<TEdgeData>& replacementEdgesBySource)
        {
            edgesByDestination.SetEdges(replacementEdgesByDestination);
            edgesBySource.SetEdges(replacementEdgesBySource);
        }
    };

    /// Convenience alias for graphs that do not use edge data or weights.
    typedef Graph<void> UnweightedGraph;
}
