/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Graph.cpp
 *   Implementation of the top-level data structure used to represent a
 *   mutable graph in a format similar to Compressed-Sparse.
 *****************************************************************************/

#include "Graph.h"
#include "Types.h"
#include "VertexIndex.h"

#include <cstddef>
#include <cstdint>


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "Graph.h" for documentation.

    Graph::Graph(void) : edgeDataType(EEdgeDataType::EdgeDataTypeVoid), edgesByDestination(), edgesBySource()
    {
        // Nothing to do here.
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "Graph.h" for documentation.

    void Graph::RemoveVertex(const TVertexID vertex)
    {
        if (vertex >= GetNumVertices())
            return;
        
        // Remove all instances of the vertex as a source from the destination-grouped vertex index.
        if (NULL != edgesBySource[vertex])
        {
            for (auto destIt = edgesBySource[vertex]->BeginIterator(); destIt != edgesBySource[vertex]->EndIterator(); ++destIt)
                edgesByDestination.RemoveEdge(destIt->otherVertex, vertex);
        }

        // Remove all instances of the vertex as a destination from the source-grouped vertex index.
        if (NULL != edgesByDestination[vertex])
        {
            for (auto sourceIt = edgesByDestination[vertex]->BeginIterator(); sourceIt != edgesByDestination[vertex]->EndIterator(); ++sourceIt)
                edgesBySource.RemoveEdge(sourceIt->otherVertex, vertex);
        }

        // Remove the top-level vertices.
        edgesByDestination.RemoveVertex(vertex);
        edgesBySource.RemoveVertex(vertex);
    }
    
    // --------
    
    template <> void Graph::SetEdgeDataType<void>(void)
    {
        edgeDataType = EEdgeDataType::EdgeDataTypeVoid;
    }
    
    // --------
    
    template <> void Graph::SetEdgeDataType<uint64_t>(void)
    {
        edgeDataType = EEdgeDataType::EdgeDataTypeInteger;
    }
    
    // --------
    
    template <> void Graph::SetEdgeDataType<double>(void)
    {
        edgeDataType = EEdgeDataType::EdgeDataTypeFloatingPoint;
    }
}
