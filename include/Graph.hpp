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
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Destination-grouped edge data structure.
        /// Maps from a destination vertex ID to a set of vertices from which in-edges exist.
        EdgeIndex<TEdgeData> edgesByDestination;
        
        /// Source-grouped vertex data structure.
        /// Maps from a source vertex ID to a set of vertices to which out-edges exist.
        EdgeIndex<TEdgeData> edgesBySource;
        
        
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        Graph(void) : edgesByDestination(), edgesBySource()
        {
            // Nothing to do here.
        }

        /// Default destructor.
        virtual ~Graph(void)
        {
            // Nothing to do here.
        }


        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Retrieves and returns the number of edges in the graph.
        /// @return Number of edges in the graph.
        TEdgeCount GetNumEdges(void)
        {
            return this->edgesBySource.GetNumEdges();
        }

        /// Retrieves and returns the number of vertices in the graph.
        /// @return Number of vertices in the graph.
        TVertexCount GetNumVertices(void)
        {
            // TODO
            return 0;
        }
        
        /// Retrieves and returns the in-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return In-degree of the specified vertex. 0 is returned if the vertex has no in-edges or does not exist.
        TEdgeCount GetVertexIndegree(TVertexID vertex)
        {
            return this->edgesByDestination.GetVertexDegree(vertex);
        }
        
        /// Retrieves and returns the out-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return Out-degree of the specified vertex. 0 is returned if the vertex has no out-edges or does not exist.
        TEdgeCount GetVertexOutdegree(TVertexID vertex)
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
}
