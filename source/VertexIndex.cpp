/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file VertexIndex.cpp
 *   Implementation of a container for indexing top-level vertices, optimized
 *   for easy modification and traversal.
 *****************************************************************************/

#include "VertexIndex.h"
#include "Types.h"

#include <spindle.h>


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "VertexIndex.h" for documentation.

    VertexIndex::VertexIndex(void) : vertexIndex(), numEdges(0), numVerticesPresent(0), numVectors(0)
    {
        // Nothing to do here.
    }

    // --------

    VertexIndex::~VertexIndex(void)
    {
        for (auto it = vertexIndex.begin(); it != vertexIndex.end(); ++it)
        {
            if (NULL != *it)
            {
                delete *it;
                *it = NULL;
            }
        }
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "VertexIndex.h" for documentation.

    template <typename TEdgeData> void VertexIndex::InsertEdgeIndexedByDestination(const SEdge<TEdgeData>& edge)
    {
        if (edge.destinationVertex >= vertexIndex.size())
            vertexIndex.resize(1 + edge.destinationVertex);
        
        if (NULL == vertexIndex[edge.destinationVertex])
        {
            numVerticesPresent += 1;
            vertexIndex[edge.destinationVertex] = new EdgeList();
        }
        
        const TEdgeCount oldDegree = vertexIndex[edge.destinationVertex]->GetDegree();
        const uint64_t oldVectors = vertexIndex[edge.destinationVertex]->GetNumVectors();

        vertexIndex[edge.destinationVertex]->InsertEdgeUsingSource(edge);

        if (oldDegree < vertexIndex[edge.destinationVertex]->GetDegree())
            numEdges += 1;
        
        if (oldVectors < vertexIndex[edge.destinationVertex]->GetNumVectors())
            numVectors += 1;
    }

    // --------

    template <typename TEdgeData> void VertexIndex::InsertEdgeIndexedBySource(const SEdge<TEdgeData>& edge)
    {
        if (edge.sourceVertex >= vertexIndex.size())
            vertexIndex.resize(1 + edge.sourceVertex);

        if (NULL == vertexIndex[edge.sourceVertex])
        {
            numVerticesPresent += 1;
            vertexIndex[edge.sourceVertex] = new EdgeList();
        }
        
        const TEdgeCount oldDegree = vertexIndex[edge.sourceVertex]->GetDegree();
        const uint64_t oldVectors = vertexIndex[edge.sourceVertex]->GetNumVectors();

        vertexIndex[edge.sourceVertex]->InsertEdgeUsingDestination(edge);

        if (oldDegree < vertexIndex[edge.sourceVertex]->GetDegree())
            numEdges += 1;
        
        if (oldVectors < vertexIndex[edge.sourceVertex]->GetNumVectors())
            numVectors += 1;
    }

    // --------

    void VertexIndex::ParallelRefreshMetadata(uint64_t* buf)
    {
        const uint32_t localThreadID = spindleGetLocalThreadID();
        const uint32_t localThreadCount = spindleGetLocalThreadCount();
        
        const size_t indexDegree = localThreadID;
        const size_t indexVector = localThreadCount + indexDegree;
        const size_t indexVerticesPresent = (localThreadCount << 1) + indexDegree;
        
        spindleBarrierLocal();
        
        buf[indexDegree] = 0;
        buf[indexVector] = 0;
        buf[indexVerticesPresent] = 0;
        
        for (size_t i = localThreadID; i < vertexIndex.size(); i += localThreadCount)
        {
            if (NULL != vertexIndex[i])
            {
                buf[indexDegree] += vertexIndex[i]->GetDegree();
                buf[indexVector] += vertexIndex[i]->GetNumVectors();
                buf[indexVerticesPresent] += 1;
            }
        }
        
        spindleBarrierLocal();
        
        if (0 == localThreadID)
        {
            numEdges = 0;
            numVectors = 0;
            numVerticesPresent = 0;
            
            for (size_t i = 0; i < localThreadCount; ++i)
            {
                numEdges += buf[i];
                numVectors += buf[i + localThreadCount];
                numVerticesPresent += buf[i + (localThreadCount << 1)];
            }
        }
    }
    
    // --------
    
    void VertexIndex::RemoveEdge(const TVertexID indexedVertex, const TVertexID otherVertex)
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
                numVerticesPresent -= 1;
            }
        }
    }

    // --------

    void VertexIndex::RemoveVertex(const TVertexID indexedVertex)
    {
        if (NULL != vertexIndex[indexedVertex])
        {
            numEdges -= vertexIndex[indexedVertex]->GetDegree();
            numVectors -= vertexIndex[indexedVertex]->GetNumVectors();
            
            delete vertexIndex[indexedVertex];
            vertexIndex[indexedVertex] = NULL;
            numVerticesPresent -= 1;
        }
    }

    // --------

    void VertexIndex::SetNumVertices(const TVertexCount numVertices)
    {
        if (numVertices < vertexIndex.size())
        {
            for (size_t i = numVertices; i < vertexIndex.size(); ++i)
            {
                if (NULL != vertexIndex[i])
                {
                    delete vertexIndex[i];
                    vertexIndex[i] = NULL;
                    numVerticesPresent -= 1;
                }
            }
        }
        
        vertexIndex.resize(numVertices, NULL);
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template void VertexIndex::InsertEdgeIndexedByDestination(const SEdge<void>& edge);
    template void VertexIndex::InsertEdgeIndexedByDestination(const SEdge<uint64_t>& edge);
    template void VertexIndex::InsertEdgeIndexedByDestination(const SEdge<double>& edge);
    
    template void VertexIndex::InsertEdgeIndexedBySource(const SEdge<void>& edge);
    template void VertexIndex::InsertEdgeIndexedBySource(const SEdge<uint64_t>& edge);
    template void VertexIndex::InsertEdgeIndexedBySource(const SEdge<double>& edge);
}
