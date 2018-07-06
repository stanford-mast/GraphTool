/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file VertexIndex.h
 *   Declaration of a container for indexing top-level vertices, optimized
 *   for easy modification and traversal.
 *****************************************************************************/

#pragma once

#include "EdgeList.h"
#include "Types.h"

#include <cstddef>
#include <vector>


namespace GraphTool
{
    /// Indexes top-level vertices in a way optimized for fast insertion; useful for ingress.
    /// Whether the index is by source or destination is not specified by this data structure but rather is inferred based on how it is used.
    class VertexIndex
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Alias for the iterator type used by this class.
        typedef typename std::vector<EdgeList*>::const_iterator VertexIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds all vertex and edge information.
        /// Key is the vertex identifier, value is the corresponding edge list for the vertex.
        std::vector<EdgeList*> vertexIndex;
        
        /// Holds the total number of edges present in this data structure.
        TEdgeCount numEdges;

        /// Holds the number of vertices that have valid edge lists.
        TVertexCount numVerticesPresent;
        
        /// Holds the total number of Vector-Sparse vectors required to represent the edges in this data structure.
        uint64_t numVectors;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        VertexIndex(void);

        /// Destructor.
        /// Destroys all edge lists.
        ~VertexIndex(void);


        // -------- OPERATORS ---------------------------------------------- //
        
        /// Faciliates read-only access to the vertex index.
        inline const EdgeList*& operator[](size_t n) const
        {
            return (const EdgeList*&)vertexIndex[n];
        }
        
        
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Returns a read-only iterator for the beginning of the vertex index.
        /// @return Read-only iterator to the beginning of the vertex index.
        inline VertexIterator BeginIterator(void) const
        {
            return vertexIndex.cbegin();
        }

        /// Returns a read-only iterator for the end of the vertex index.
        /// @return Read-only iterator for the end of the vertex index.
        inline VertexIterator EndIterator(void) const
        {
            return vertexIndex.cend();
        }

        /// Performs a simple and fast insertion of the specified edge into this data structure, using the destination as the top-level vertex.
        /// Does not update any internal counters for vectors or edges.
        /// Can be invoked from multiple threads, so long as each thread updates a different top-level vertex.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> inline void FastInsertEdgeIndexedByDestination(const SEdge<TEdgeData>& edge)
        {
            if (NULL == vertexIndex[edge.destinationVertex])
                vertexIndex[edge.destinationVertex] = new EdgeList();
            
            vertexIndex[edge.destinationVertex]->InsertEdgeUsingSource(edge);
        }
        
        /// Performs a simple and fast insertion of the specified edge into this data structure, using the source as the top-level vertex.
        /// Does not update any internal counters for vectors or edges.
        /// Can be invoked from multiple threads, so long as each thread updates a different top-level vertex.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> inline void FastInsertEdgeIndexedBySource(const SEdge<TEdgeData>& edge)
        {
            if (NULL == vertexIndex[edge.sourceVertex])
                vertexIndex[edge.sourceVertex] = new EdgeList();
            
            vertexIndex[edge.sourceVertex]->InsertEdgeUsingDestination(edge);
        }
        
        /// Returns the degree of a specific vertex.
        /// @return Degree of the specified vertex.
        inline TEdgeCount GetDegree(TVertexID vertex) const
        {
            if (NULL != vertexIndex.at(vertex))
                return vertexIndex.at(vertex)->GetDegree();
            else
                return 0;
        }
        
        /// Returns the total number of edges in the index.
        /// @return Total number of edges.
        inline TEdgeCount GetNumEdges(void) const
        {
            return numEdges;
        }
        
        /// Returns the number of Vector-Sparse vectors required to represent the edges in this data structure.
        /// @return Number of vectors.
        inline uint64_t GetNumVectors(void) const
        {
            return numVectors;
        }
        
        /// Returns the number of top-level vertices in the index.
        /// @return Number of indexed vertices.
        inline TVertexCount GetNumVertices(void) const
        {
            return vertexIndex.size();
        }

        /// Returns the number of top-level vertices in the index having a valid edge list associated with them.
        /// @return Number of indexed vertices with edge lists.
        inline TVertexCount GetNumVerticesPresent(void) const
        {
            return numVerticesPresent;
        }

        /// Inserts the specified edge into this data structure, using the destination as the top-level vertex.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> void InsertEdgeIndexedByDestination(const SEdge<TEdgeData>& edge);

        /// Inserts the specified edge into this data structure, using the source as the top-level vertex.
        /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
        /// @param [in] edge Edge to insert.
        template <typename TEdgeData> void InsertEdgeIndexedBySource(const SEdge<TEdgeData>& edge);

        /// Refreshes metadata, such as degree information.
        /// Intended to be called from within a Spindle parallelized region.
        /// Required after invoking fast insertion methods, which do not update any counts.
        /// @param [in] buf Temporary array allocated with four locations per thread.
        void ParallelRefreshMetadata(uint64_t* buf);
        
        /// Removes the specified edge from this data structure.
        /// @param [in] indexedVertex Top-level vertex to which the edge corresponds.
        /// @param [in] otherVertex Vertex at the other end of the edge to remove.
        void RemoveEdge(const TVertexID indexedVertex, const TVertexID otherVertex);

        /// Removes the specified top-level vertex from this data structure.
        /// Does not remove any other references to the vertex within individual edge lists, just removes it from the index.
        /// @param [in] indexedVertex Top-level vertex to remove.
        void RemoveVertex(const TVertexID indexedVertex);
        
        /// Sets the number of indexed vertices.
        /// Constructs new vertices or destroys existing ones as appropriate.
        /// @param [in] numVertices Number of vertices.
        void SetNumVertices(const TVertexCount numVertices);
    };
}
