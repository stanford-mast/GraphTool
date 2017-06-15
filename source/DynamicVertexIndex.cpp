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
#include "Types.h"


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "DynamicVertexIndex.h" for documentation.

    template <typename TEdgeData> DynamicVertexIndex<TEdgeData>::DynamicVertexIndex(void) : vertexIndex(), numEdges(0), numVectors(0)
    {
        // Nothing to do here.
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "DynamicVertexIndex.h" for documentation.

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::InsertEdgeBufferIndexedByDestination(const SEdge<TEdgeData>& edge)
    {
        const TEdgeCount oldDegree = vertexIndex[edge.destinationVertex].GetDegree();
        const size_t oldVectors = vertexIndex[edge.destinationVertex].GetNumVectors();

        vertexIndex[edge.destinationVertex].InsertEdgeBufferSource(edge);

        if (oldDegree < vertexIndex[edge.destinationVertex].GetDegree())
            numEdges += 1;
        
        if (oldVectors < vertexIndex[edge.destinationVertex].GetNumVectors())
            numVectors += 1;
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::InsertEdgeBufferIndexedBySource(const SEdge<TEdgeData>& edge)
    {
        const TEdgeCount oldDegree = vertexIndex[edge.sourceVertex].GetDegree();
        const size_t oldVectors = vertexIndex[edge.sourceVertex].GetNumVectors();

        vertexIndex[edge.sourceVertex].InsertEdgeBufferDestination(edge);

        if (oldDegree < vertexIndex[edge.sourceVertex].GetDegree())
            numEdges += 1;
        
        if (oldVectors < vertexIndex[edge.sourceVertex].GetNumVectors())
            numVectors += 1;
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::RemoveEdge(const TVertexID indexedVertex, const TVertexID otherVertex)
    {
        if (0 != vertexIndex.count(indexedVertex))
        {
            const TEdgeCount oldDegree = vertexIndex[indexedVertex].GetDegree();
            const size_t oldVectors = vertexIndex[indexedVertex].GetNumVectors();

            vertexIndex[indexedVertex].RemoveEdge(otherVertex);

            if (oldDegree > vertexIndex[indexedVertex].GetDegree())
                numEdges -= 1;
            
            if (oldVectors > vertexIndex[indexedVertex].GetNumVectors())
                numVectors -= 1;

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
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class DynamicVertexIndex<void>;
    template class DynamicVertexIndex<uint64_t>;
    template class DynamicVertexIndex<double>;
}
