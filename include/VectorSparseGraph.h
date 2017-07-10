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
    private:
        // -------- TYPE DEFINITIONS --------------------------------------- //

        /// Describes the layout of part of a Vector-Sparse edge list.
        /// Each record of this type specifies both the starting index and vector count for a single top-level vertex.
        /// In aggregate, these records fully determine the layout of an edge list.
        /// The top-level vertex identifier is not specified but can be obtained by reading it from the edge list itself.
        struct SLayoutInfo
        {
            uint64_t index;                                                 ///< Starting index for vectors belonging to a top-level vertex.
            uint64_t count;                                                 ///< Number of vectors that correspond to the top-level vertex.
        };


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
        
        /// In-degree of each vertex in the graph.
        const TEdgeCount* indegree;

        /// Out-degree of each vertex in the graph.
        const TEdgeCount* outdegree;
        
        /// Number of vectors per vertex in the destination-grouped Vector-Sparse edge list.
        const uint64_t* countsByDestination;
        
        /// Destination-grouped vertex index into the Vector-Sparse edge list.
        const uint64_t* indexByDestination;

        /// Array of SLayoutInfo structures that determines the layout of the destination-grouped edge list.
        const SLayoutInfo* layoutByDestination;

        /// Number of SLayoutInfo structures used to determine the layout of the destination-grouped edge list.
        const TVertexCount layoutCountByDestination;
        
        /// Destination-grouped Vector-Sparse edge list.
        const VectorSparseElement<TEdgeData>* vectorsByDestination;
        
        /// Number of vectors in the destination-grouped Vector-Sparse edge list.
        const uint64_t numVectorsByDestination;
        
        /// Number of vectors per vertex in the source-grouped Vector-Sparse edge list.
        const uint64_t* countsBySource;
        
        /// Source-grouped vertex index into the Vector-Sparse edge list.
        const uint64_t* indexBySource;

        /// Array of SLayoutInfo structures that determines the layout of the source-grouped edge list.
        const SLayoutInfo* layoutBySource;

        /// Number of SLayoutInfo structures used to determine the layout of the source-grouped edge list.
        const TVertexCount layoutCountBySource;
        
        /// Source-grouped Vector-Sparse edge list.
        const VectorSparseElement<TEdgeData>* vectorsBySource;
        
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
        /// If this object is already initialized with graph data structures, erases them and re-initializes to the newly-supplied graph.
        /// @param [in] graph Mutable graph to read.
        void InitializeFromMutableGraph(const Graph<TEdgeData>& graph);
        
        
    public:
        // -------- INSTANCE METHODS --------------------------------------- //
                
        /// Retrieves and returns the number of edges in the graph.
        /// @return Number of edges in the graph.
        inline TEdgeCount GetNumEdges(void) const
        {
            return numEdges;
        }
        
        /// Retrieves and returns the number of vertices in the graph.
        /// @return Number of vertices in the graph.
        inline TVertexCount GetNumVertices(void) const
        {
            return numVertices;
        }
        
        /// Retrieves and returns the number of vertices having non-zero indegree.
        /// @return Number of vertices with in-edges.
        inline TVertexCount GetNumVerticesPresentDestination(void) const
        {
            return layoutCountByDestination;
        }
                
        /// Retrieves and returns the number of vertices having non-zero outdegree.
        /// @return Number of vertices with out-edges.
        inline TVertexCount GetNumVerticesPresentSource(void) const
        {
            return layoutCountBySource;
        }
        
        /// Retrieves and returns the number of Vector-Sparse vectors required to represent the destination-grouped edges.
        /// @return Number of vectors required.
        inline uint64_t GetNumVectorsDestination(void) const
        {
            return numVectorsByDestination;
        }
        
        /// Retrieves and returns the number of Vector-Sparse vectors required to represent the source-grouped edges.
        /// @return Number of vectors required.
        inline uint64_t GetNumVectorsSource(void) const
        {
            return numVectorsBySource;
        }
        
        /// Retrieves and returns the in-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return In-degree of the specified vertex. 0 is returned if the vertex has no in-edges or does not exist.
        inline TEdgeCount GetVertexIndegree(TVertexID vertex) const
        {
            if (vertex < GetNumVertices())
                return indegree[vertex];
            else
                return 0;
        }
        
        /// Retrieves and returns the out-degree of the specified vertex.
        /// @param [in] vertex Identifier of the vertex of interest.
        /// @return Out-degree of the specified vertex. 0 is returned if the vertex has no out-edges or does not exist.
        inline TEdgeCount GetVertexOutdegree(TVertexID vertex) const
        {
            if (vertex < GetNumVertices())
                return outdegree[vertex];
            else
                return 0;
        }
        
        /// Specifies if initialization has succeeded on this object.
        /// If this method returns `false` then initialization has failed and this object should be destroyed.
        bool IsInitialized(void);
    };
}
