/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file VectorSparseGraph.cpp
 *   Implementation of the top-level data structure used to represent an
 *   immutable graph in Vector-Sparse format.
 *****************************************************************************/

#include "Graph.h"
#include "Types.h"
#include "VectorSparseElement.h"
#include "VectorSparseGraph.h"

#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <malloc.h>


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "VectorSparseGraph.h" for documentation.
    
    template <typename TEdgeData> VectorSparseGraph<TEdgeData>::VectorSparseGraph(const Graph<TEdgeData>& graph) : isInitialized(false), numEdges(graph.GetNumEdges()), numVertices(graph.GetNumVertices()), numVectorsByDestination(graph.GetNumVectorsDestination()), numVectorsBySource(graph.GetNumVectorsSource())
    {
        countsByDestination = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        indexByDestination = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        vectorsByDestination = (VectorSparseElement<TEdgeData>*)malloc(sizeof(VectorSparseElement<TEdgeData>) * numVectorsByDestination);
        
        countsBySource = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        indexBySource = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        vectorsBySource = (VectorSparseElement<TEdgeData>*)malloc(sizeof(VectorSparseElement<TEdgeData>) * numVectorsBySource);

        if ((NULL == countsByDestination) || (NULL == indexByDestination) || (NULL == vectorsByDestination) || (NULL == countsBySource) || (NULL == indexBySource) || (NULL == vectorsBySource))
            Deinitialize();
        else
            InitializeFromMutableGraph(graph);
    }
    
    // --------
    
    template <typename TEdgeData> VectorSparseGraph<TEdgeData>::~VectorSparseGraph(void)
    {
        Deinitialize();
    }


    // -------- HELPERS ---------------------------------------------------- //
    // See "VectorSparseGraph.h" for documentation.

    template <typename TEdgeData> void VectorSparseGraph<TEdgeData>::Deinitialize(void)
    {
        if (NULL != countsByDestination)
        {
            free((void*)countsByDestination);
            countsByDestination = NULL;
        }
        
        if (NULL != indexByDestination)
        {
            free((void*)indexByDestination);
            indexByDestination = NULL;
        }

        if (NULL != vectorsByDestination)
        {
            free((void*)vectorsByDestination);
            vectorsByDestination = NULL;
        }

        if (NULL != countsBySource)
        {
            free((void*)countsBySource);
            countsBySource = NULL;
        }
        
        if (NULL != indexBySource)
        {
            free((void*)indexBySource);
            indexBySource = NULL;
        }

        if (NULL != vectorsBySource)
        {
            free((void*)vectorsBySource);
            vectorsBySource = NULL;
        }

        isInitialized = false;
    }

    // --------

    template <typename TEdgeData> void VectorSparseGraph<TEdgeData>::InitializeFromMutableGraph(const Graph<TEdgeData>& graph)
    {
        // Initialize the vertex indices.
        // Default is to assume vertices are not present.
        // They will be marked present if they show up in the graph.
        for (uint64_t i = 0; i < numVertices; ++i)
        {
            countsByDestination[i] = 0ull;
            indexByDestination[i] = kVertexIndexVertexNotPresent;
            
            countsBySource[i] = 0ull;
            indexBySource[i] = kVertexIndexVertexNotPresent;
        }

        // Calculate the correct values for each position in the vertex index.
        // If a vertex is present, the value should be the total number of vectors that come before it.
        // Otherwise, it should be marked as missing.

        {
            // Destination-grouped index.
            uint64_t previousTotalVectorIndices = 0ull;
            uint64_t lastValidVertexSeen = numVertices - 1;

            for (uint64_t i = 0; i < numVertices; ++i)
            {
                if (NULL != graph.VertexIndexDestination()[i])
                {
                    countsByDestination[i] = graph.VertexIndexDestination()[i]->GetNumVectors();
                    indexByDestination[i] = previousTotalVectorIndices;
                    previousTotalVectorIndices += countsByDestination[i];
                    lastValidVertexSeen = i;
                }
            }

            for (uint64_t i = (lastValidVertexSeen + 1); i < numVertices; ++i)
            {
                indexByDestination[i] = kVertexIndexVertexPastEnd;
            }
        }
        
        {
            // Source-grouped index.
            uint64_t previousTotalVectorIndices = 0ull;
            uint64_t lastValidVertexSeen = numVertices - 1;

            for (uint64_t i = 0; i < numVertices; ++i)
            {
                if (NULL != graph.VertexIndexSource()[i])
                {
                    countsBySource[i] = graph.VertexIndexSource()[i]->GetNumVectors();
                    indexBySource[i] = previousTotalVectorIndices;
                    previousTotalVectorIndices += countsBySource[i];
                    lastValidVertexSeen = i;
                }
            }

            for (uint64_t i = (lastValidVertexSeen + 1); i < numVertices; ++i)
            {
                indexBySource[i] = kVertexIndexVertexPastEnd;
            }
        }


    }

    
    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "VectorSparseGraph.h" for documentation.

    template <typename TEdgeData> bool VectorSparseGraph<TEdgeData>::IsInitialized(void)
    {
        return isInitialized;
    }

    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class VectorSparseGraph<void>;
    template class VectorSparseGraph<uint64_t>;
    template class VectorSparseGraph<double>;
}
