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
        if (IsInitialized())
            return;

        // Allocate memory.
        uint64_t* tempCountsByDestination = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        uint64_t* tempIndexByDestination = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        VectorSparseElement<TEdgeData>* tempVectorsByDestination = (VectorSparseElement<TEdgeData>*)malloc(sizeof(VectorSparseElement<TEdgeData>) * numVectorsByDestination);

        uint64_t* tempCountsBySource = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        uint64_t* tempIndexBySource = (uint64_t*)malloc(sizeof(uint64_t) * numVertices);
        VectorSparseElement<TEdgeData>* tempVectorsBySource = (VectorSparseElement<TEdgeData>*)malloc(sizeof(VectorSparseElement<TEdgeData>) * numVectorsBySource);

        if ((NULL == tempCountsByDestination) || (NULL == tempIndexByDestination) || (NULL == tempVectorsByDestination) || (NULL == tempCountsBySource) || (NULL == tempIndexBySource) || (NULL == tempVectorsBySource))
        {
            Deinitialize();
            return;
        }
        
        // Initialize the vertex indices.
        // Default is to assume vertices are not present.
        // They will be marked present if they show up in the graph.
        for (uint64_t i = 0; i < numVertices; ++i)
        {
            tempCountsByDestination[i] = 0ull;
            tempIndexByDestination[i] = kVertexIndexVertexNotPresent;
            
            tempCountsBySource[i] = 0ull;
            tempIndexBySource[i] = kVertexIndexVertexNotPresent;
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
                    tempCountsByDestination[i] = graph.VertexIndexDestination()[i]->GetNumVectors();
                    tempIndexByDestination[i] = previousTotalVectorIndices;
                    previousTotalVectorIndices += tempCountsByDestination[i];
                    lastValidVertexSeen = i;
                }
            }

            for (uint64_t i = (lastValidVertexSeen + 1); i < numVertices; ++i)
            {
                tempIndexByDestination[i] = kVertexIndexVertexPastEnd;
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
                    tempCountsBySource[i] = graph.VertexIndexSource()[i]->GetNumVectors();
                    tempIndexBySource[i] = previousTotalVectorIndices;
                    previousTotalVectorIndices += tempCountsBySource[i];
                    lastValidVertexSeen = i;
                }
            }

            for (uint64_t i = (lastValidVertexSeen + 1); i < numVertices; ++i)
            {
                tempIndexBySource[i] = kVertexIndexVertexPastEnd;
            }
        }

        // Create the Vector-Sparse edge lists.
        // For each top-level vertex, create a Vector-Sparse element by grouping up to 4 edges into it.

        {
            // Destination-grouped edge list.
            uint64_t nextVectorIndex = 0ull;

            for (uint64_t i = 0; i < numVertices; ++i)
            {
                if (NULL != graph.VertexIndexDestination()[i])
                {
                    uint8_t edgeCount = 0;
                    SIndexedEdge<TEdgeData> edges[4];
                    
                    for (auto it = graph.VertexIndexDestination()[i]->BeginIterator(); it != graph.VertexIndexDestination()[i]->EndIterator(); ++it)
                    {
                        if (4 == edgeCount)
                        {
                            tempVectorsByDestination[nextVectorIndex].FillFromIndexedEdges(i, edges, 4);
                            nextVectorIndex += 1;
                            edgeCount = 0;
                        }
                        
                        edges[edgeCount] = *it;
                        edgeCount += 1;
                    }
                    
                    tempVectorsByDestination[nextVectorIndex].FillFromIndexedEdges(i, edges, edgeCount);
                    nextVectorIndex += 1;
                }
            }
        }

        {
            // Source-grouped edge list.
            uint64_t nextVectorIndex = 0ull;

            for (uint64_t i = 0; i < numVertices; ++i)
            {
                if (NULL != graph.VertexIndexSource()[i])
                {
                    uint8_t edgeCount = 0;
                    SIndexedEdge<TEdgeData> edges[4];

                    for (auto it = graph.VertexIndexSource()[i]->BeginIterator(); it != graph.VertexIndexSource()[i]->EndIterator(); ++it)
                    {
                        if (4 == edgeCount)
                        {
                            tempVectorsBySource[nextVectorIndex].FillFromIndexedEdges(i, edges, 4);
                            nextVectorIndex += 1;
                            edgeCount = 0;
                        }

                        edges[edgeCount] = *it;
                        edgeCount += 1;
                    }

                    tempVectorsBySource[nextVectorIndex].FillFromIndexedEdges(i, edges, edgeCount);
                    nextVectorIndex += 1;
                }
            }
        }

        // Save allocated memory buffer pointers back to this object.
        // At this point they become read-only buffers.
        countsByDestination = tempCountsByDestination;
        indexByDestination = tempIndexByDestination;
        vectorsByDestination = tempVectorsByDestination;
        countsBySource = tempCountsBySource;
        indexBySource = tempIndexBySource;
        vectorsBySource = tempVectorsBySource;

        // This graph is now initialized.
        isInitialized = true;
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
