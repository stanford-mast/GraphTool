/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file DynamicVertexIndex.cpp
 *   Implementation of a container for indexing top-level vertices, optimized
 *   for fast insertion.
 *****************************************************************************/

#include "DynamicVertexIndex.h"
#include "Edge.hpp"
#include "Types.h"


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "DynamicVertexIndex.h" for documentation.

    template <typename TEdgeData> DynamicVertexIndex<TEdgeData>::DynamicVertexIndex(void) : vertexIndex(), numEdges(0)
    {
        // Nothing to do here.
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "DynamicVertexIndex.h" for documentation.

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::InsertEdge(const TVertexID indexedVertex, const Edge<TEdgeData>& edge)
    {
        const TEdgeCount oldDegree = vertexIndex[indexedVertex].GetDegree();

        vertexIndex[indexedVertex].InsertEdge(edge);

        if (oldDegree < vertexIndex[indexedVertex].GetDegree())
            numEdges += 1;
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::InsertEdgeBufferIndexedByDestination(const SEdgeBufferData<TEdgeData>& edge)
    {
        const TEdgeCount oldDegree = vertexIndex[edge.destinationVertex].GetDegree();

        vertexIndex[edge.destinationVertex].InsertEdgeBufferSource(edge);

        if (oldDegree < vertexIndex[edge.destinationVertex].GetDegree())
            numEdges += 1;
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::InsertEdgeBufferIndexedBySource(const SEdgeBufferData<TEdgeData>& edge)
    {
        const TEdgeCount oldDegree = vertexIndex[edge.sourceVertex].GetDegree();

        vertexIndex[edge.sourceVertex].InsertEdgeBufferDestination(edge);

        if (oldDegree < vertexIndex[edge.sourceVertex].GetDegree())
            numEdges += 1;
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::RemoveEdge(const TVertexID indexedVertex, const TVertexID otherVertex)
    {
        if (0 != vertexIndex.count(indexedVertex))
        {
            const TEdgeCount oldDegree = vertexIndex[indexedVertex].GetDegree();

            vertexIndex[indexedVertex].RemoveEdge(otherVertex);

            if (oldDegree > vertexIndex[indexedVertex].GetDegree())
                numEdges -= 1;

            if (0 == vertexIndex[indexedVertex].GetDegree())
                vertexIndex.erase(indexedVertex);
        }
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::RemoveVertex(const TVertexID indexedVertex)
    {
        if (0 != vertexIndex.count(indexedVertex))
        {
            numEdges -= vertexIndex[indexedVertex].GetDegree();
            vertexIndex.erase(indexedVertex);
        }

        // TODO: Remove all references to the vertex in other indexed vertices.
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class DynamicVertexIndex<void>;
    template class DynamicVertexIndex<uint64_t>;
    template class DynamicVertexIndex<double>;
}
