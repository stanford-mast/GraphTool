/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file VectorSparseGraph.h
 *   Declaration of the top-level data structure used to represent an
 *   immutable graph in Vector-Sparse format.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <immintrin.h>


namespace GraphTool
{
    template <typename TEdgeData> class Graph;
    
    
    /// Represents a graph.
    /// Internal format is Vector-Sparse, optimized for traversal throughput at the expense of mutability.
    /// Holds topology information and per-edge data, such as weights.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class VectorSparseGraph
    {
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Number of edges in the graph.
        const TEdgeCount numEdges;
        
        /// Number of vertices in the graph.
        const TVertexCount numVertices;
        
        /// Destination-grouped vertex index into the Vector-Sparse edge list.
        const size_t* indexByDestination;
        
        /// Destination-grouped Vector-Sparse edge list.
        const __m256i* vectorsByDestination;
        
        /// Number of vectors in the destination-grouped Vector-Sparse edge list.
        const size_t numVectorsByDestination;
        
        /// Source-grouped vertex index into the Vector-Sparse edge list.
        const size_t* indexBySource;
        
        /// Source-grouped Vector-Sparse edge list.
        const __m256i* vectorsBySource;
        
        /// Number of vectors in the source-grouped Vector-Sparse edge list.
        const size_t numVectorsBySource;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Initialization constructor.
        /// Turns a mutable graph into an immutable graph.
        VectorSparseGraph(const Graph<TEdgeData>& graph);
        
        /// Destructor.        
        /// Destroys all created edge lists.
        ~VectorSparseGraph(void);
    };
}
