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

#include <spindle.h>


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

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::InsertEdgeIndexedByDestination(const SEdge<TEdgeData>& edge)
    {
        if (NULL == vertexIndex[edge.destinationVertex])
            vertexIndex[edge.destinationVertex] = new DynamicEdgeList<TEdgeData>();
        
        const TEdgeCount oldDegree = vertexIndex[edge.destinationVertex]->GetDegree();
        const size_t oldVectors = vertexIndex[edge.destinationVertex]->GetNumVectors();

        vertexIndex[edge.destinationVertex]->InsertEdgeUsingSource(edge);

        if (oldDegree < vertexIndex[edge.destinationVertex]->GetDegree())
            numEdges += 1;
        
        if (oldVectors < vertexIndex[edge.destinationVertex]->GetNumVectors())
            numVectors += 1;
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::InsertEdgeIndexedBySource(const SEdge<TEdgeData>& edge)
    {
        if (NULL == vertexIndex[edge.sourceVertex])
            vertexIndex[edge.sourceVertex] = new DynamicEdgeList<TEdgeData>();
        
        const TEdgeCount oldDegree = vertexIndex[edge.sourceVertex]->GetDegree();
        const size_t oldVectors = vertexIndex[edge.sourceVertex]->GetNumVectors();

        vertexIndex[edge.sourceVertex]->InsertEdgeUsingDestination(edge);

        if (oldDegree < vertexIndex[edge.sourceVertex]->GetDegree())
            numEdges += 1;
        
        if (oldVectors < vertexIndex[edge.sourceVertex]->GetNumVectors())
            numVectors += 1;
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::ParallelRefreshDegreeInfo(size_t* buf)
    {
        const uint32_t localThreadID = spindleGetLocalThreadID();
        const uint32_t localThreadCount = spindleGetLocalThreadCount();
        
        const size_t indexDegree = localThreadID;
        const size_t indexVector = localThreadCount + indexDegree;
        
        spindleBarrierLocal();
        
        buf[indexDegree] = 0;
        buf[indexVector] = 0;
        
        for (size_t i = localThreadID; i < vertexIndex.size(); i += localThreadCount)
        {
            if (NULL != vertexIndex[i])
            {
                buf[indexDegree] += vertexIndex[i]->GetDegree();
                buf[indexVector] += vertexIndex[i]->GetNumVectors();
            }
        }
        
        spindleBarrierLocal();
        
        if (0 == localThreadID)
        {
            numEdges = 0;
            numVectors = 0;
            
            for (size_t i = 0; i < localThreadCount; ++i)
            {
                numEdges += buf[i];
                numVectors += buf[i + localThreadCount];
            }
        }
    }
    
    // --------
    
    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::RemoveEdge(const TVertexID indexedVertex, const TVertexID otherVertex)
    {
        if (NULL != vertexIndex[indexedVertex])
        {
            const TEdgeCount oldDegree = vertexIndex[indexedVertex]->GetDegree();
            const size_t oldVectors = vertexIndex[indexedVertex]->GetNumVectors();

            vertexIndex[indexedVertex]->RemoveEdge(otherVertex);

            if (oldDegree > vertexIndex[indexedVertex]->GetDegree())
                numEdges -= 1;
            
            if (oldVectors > vertexIndex[indexedVertex]->GetNumVectors())
                numVectors -= 1;

            if (0 == vertexIndex[indexedVertex]->GetDegree())
            {
                delete vertexIndex[indexedVertex];
                vertexIndex[indexedVertex] = NULL;
            }
        }
    }

    // --------

    template <typename TEdgeData> void DynamicVertexIndex<TEdgeData>::RemoveVertex(const TVertexID indexedVertex)
    {
        if (NULL != vertexIndex[indexedVertex])
        {
            numEdges -= vertexIndex[indexedVertex]->GetDegree();
            numVectors -= vertexIndex[indexedVertex]->GetNumVectors();
            
            delete vertexIndex[indexedVertex];
            vertexIndex[indexedVertex] = NULL;
        }
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class DynamicVertexIndex<void>;
    template class DynamicVertexIndex<uint64_t>;
    template class DynamicVertexIndex<double>;
}
