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
#include "VectorSparseElement.h"

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
    public:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Value in the vertex index indicating that a vertex is not present.
        static const uint64_t kVertexIndexVertexNotPresent = 0x7fffffffffffffffull;
        
        /// Value in the vertex index indicating that a vertex is not present because it is past the end of the edge list.
        static const uint64_t kVertexIndexVertexPastEnd = 0xffffffffffffffffull;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Specifies if this graph has been successfully initialized.
        bool isInitialized;
        
        /// Number of edges in the graph.
        const TEdgeCount numEdges;
        
        /// Number of vertices in the graph.
        const TVertexCount numVertices;
        
        /// Number of vectors per vertex in the destination-grouped Vector-Sparse edge list.
        uint64_t* countsByDestination;
        
        /// Destination-grouped vertex index into the Vector-Sparse edge list.
        uint64_t* indexByDestination;
        
        /// Destination-grouped Vector-Sparse edge list.
        VectorSparseElement<TEdgeData>* vectorsByDestination;
        
        /// Number of vectors in the destination-grouped Vector-Sparse edge list.
        const uint64_t numVectorsByDestination;
        
        /// Number of vectors per vertex in the source-grouped Vector-Sparse edge list.
        uint64_t* countsBySource;
        
        /// Source-grouped vertex index into the Vector-Sparse edge list.
        uint64_t* indexBySource;
        
        /// Source-grouped Vector-Sparse edge list.
        VectorSparseElement<TEdgeData>* vectorsBySource;
        
        /// Number of vectors in the source-grouped Vector-Sparse edge list.
        const uint64_t numVectorsBySource;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Initialization constructor.
        /// Turns a mutable graph into an immutable graph.
        /// @param [in] graph Mutable graph to read.
        VectorSparseGraph(const Graph<TEdgeData>& graph);
        
        /// Destructor.        
        /// Destroys all created edge lists.
        ~VectorSparseGraph(void);

    
    private:
        // -------- HELPERS ------------------------------------------------ //

        /// Deinitializes this object and frees all allocated memory.
        /// Resets fields to their uninitialized states.
        void Deinitialize(void);
        
        /// Initializes the Vector-Sparse data structures of this graph, given a mutable graph as a data source.
        /// @param [in] graph Mutable graph to read.
        void InitializeFromMutableGraph(const Graph<TEdgeData>& graph);
        
        
    public:
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Specifies if initialization has succeeded on this object.
        /// If this method returns `false` then initialization has failed and this object should be destroyed.
        bool IsInitialized(void);
    };
}
