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
        // -------- INSTANCE VARIABLES --------------------------------------------- //
        
        /// Destination-grouped edge data structure.
        /// Maps from a destination vertex ID to a set of vertices from which in-edges exist.
        EdgeIndex<TEdgeData> edgesByDestination;
        
        /// Source-grouped vertex data structure.
        /// Maps from a source vertex ID to a set of vertices to which out-edges exist.
        EdgeIndex<TEdgeData> edgesBySource;
        
        
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

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


        // -------- INSTANCE METHODS ----------------------------------------------- //
        
        /// Reads edges from a buffer and adds them to the graph.
        /// Specific format is as specified by the subclass that performs the import operation.
        /// This is a convenience method that calls both #AddInEdges and #AddOutEdges in sequence.
        /// @param [in] edges Buffer containing edges to read.
        /// @param [in] count Number of edges contained within the buffer, size determined by the implementation.
        void AddEdges(SEdgeBufferData<TEdgeData>* edges, size_t count)
        {
            this->AddInEdges(edges, count);
            this->AddOutEdges(edges, count);
        }
        
        /// Reads edges from a buffer and adds them to the part of the graph that stores in-edges.
        /// Both this method and #AddOutEdges should be called on the same buffer to ensure data structure consistency.
        /// This method is exposed directly to facilitate parallel buffer consumption for both in-edges and out-edges.
        /// Otherwise, use #AddEdges instead.
        /// @param [in] edges Buffer containing edges to read.
        /// @param [in] count Number of edges contained within the buffer, size determined by the implementation.
        void AddInEdges(SEdgeBufferData<TEdgeData>* edges, size_t count)
        {
            for (uint64_t i = 0; i < count; ++i)
            {
                EdgeIndex<TEdgeData>::TEdge edgeInfo;
                edgeInfo.FillFromSourceEdgeBuffer(edges[i]);
                this->edgesByDestination.InsertEdge(edges[i].destinationVertex, edgeInfo);
            }
        }
        
        /// Reads edges from a buffer and adds them to the part of the graph that stores out-edges.
        /// Both this method and #AddInEdges should be called on the same buffer to ensure data structure consistency.
        /// This method is exposed directly to facilitate parallel buffer consumption for both in-edges and out-edges.
        /// Otherwise, use #AddEdges instead.
        /// @param [in] edges Buffer containing edges to read.
        /// @param [in] count Number of edges contained within the buffer, size determined by the implementation.
        void AddOutEdges(SEdgeBufferData<TEdgeData>* edges, size_t count)
        {
            for (uint64_t i = 0; i < count; ++i)
            {
                EdgeIndex<TEdgeData>::TEdge edgeInfo;
                edgeInfo.FillFromDestinationEdgeBuffer(edges[i]);
                this->edgesBySource.InsertEdge(edges[i].sourceVertex, edgeInfo);
            }
        }
        
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
    };
}
