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
            const bool hasDestinationVertices = (0 != edgesByDestination.GetNumVertices());
            const bool hasSourceVertices = (0 != edgesBySource.GetNumVertices());

            const TVertexCount numDestinationVertices = (hasDestinationVertices ? 1 + edgesByDestination.GetMaximumVertexID() : 0);
            const TVertexCount numSourceVertices = (hasSourceVertices? 1 + edgesBySource.GetMaximumVertexID() : 0);
            
            return (numDestinationVertices > numSourceVertices ? numDestinationVertices : numSourceVertices);
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
        inline void InsertEdgeByDestination(const SEdgeBufferData<TEdgeData>& edge)
        {
            edgesByDestination.InsertEdgeBufferIndexedByDestination(edge);
        }
        
        /// Inserts an edge into the source-grouped representation using the specified edge buffer.
        /// @param [in] edge Edge to insert.
        inline void InsertEdgeBySource(const SEdgeBufferData<TEdgeData>& edge)
        {
            edgesBySource.InsertEdgeBufferIndexedBySource(edge);
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
            edgesByDestination.RemoveVertex(vertex);
            edgesBySource.RemoveVertex(vertex);
        }

        inline VertexIterator VertexIteratorDestinationBegin(void) const
        {
            return edgesByDestination.BeginIterator();
        }
        
        inline VertexIterator VertexIteratorDestinationEnd(void) const
        {
            return edgesByDestination.EndIterator();
        }
        
        inline VertexIterator VertexIteratorSourceBegin(void) const
        {
            return edgesBySource.BeginIterator();
        }

        inline VertexIterator VertexIteratorSourceEnd(void) const
        {
            return edgesBySource.EndIterator();
        }
    };
}
