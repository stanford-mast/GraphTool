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
#include "VectorSparseGraph.h"

#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <malloc.h>


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "VectorSparseGraph.h" for documentation.
    
    template <typename TEdgeData> VectorSparseGraph<TEdgeData>::VectorSparseGraph(const Graph<TEdgeData>& graph) : numEdges(graph.GetNumEdges()), numVertices(graph.GetNumVertices()), numVectorsByDestination(graph.GetNumVectorsDestination()), numVectorsBySource(graph.GetNumVectorsSource())
    {
        indexByDestination = NULL;
        vectorsByDestination = NULL;
        
        indexBySource = NULL;
        vectorsBySource = NULL;
    }
    
    // --------
    
    template <typename TEdgeData> VectorSparseGraph<TEdgeData>::~VectorSparseGraph(void)
    {
        if (NULL != indexByDestination)
            free((void*)indexByDestination);
        
        if (NULL != vectorsByDestination)
            free((void*)vectorsByDestination);
        
        if (NULL != indexBySource)
            free((void*)indexBySource);
        
        if (NULL != vectorsBySource)
            free((void*)vectorsBySource);    
    }
}
