/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file DynamicVertexIndex.h
 *   Declaration of a container for indexing top-level vertices, optimized
 *   for fast insertion.
 *****************************************************************************/

#pragma once

#include "DynamicEdgeList.h"
#include "Types.h"

#include <map>


namespace GraphTool
{
    /// Indexes top-level vertices in a way optimized for fast insertion; useful for ingress.
    /// Whether the index is by source or destination is not specified by this data structure but rather is inferred based on how it is used.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class DynamicVertexIndex
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //

        /// Alias for the iterator type used by this class.
        typedef typename std::map<TVertexID, DynamicEdgeList<TEdgeData>>::const_iterator VertexIterator;


    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds all vertex and edge information.
        /// Key is the vertex identifier, value is the corresponding edge list for the vertex.
        std::map<TVertexID, DynamicEdgeList<TEdgeData>> vertexIndex;
        
        /// Holds the total number of edges present in this data structure.
        TEdgeCount numEdges;
        
        /// Holds the total number of Vector-Sparse vectors required to represent the edges in this data structure.
        size_t numVectors;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        DynamicVertexIndex(void);


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

        /// Returns the degree of a specific vertex.
        /// @return Degree of the specified vertex.
        inline TEdgeCount GetDegree(TVertexID vertex) const
        {
            if (0 != vertexIndex.count(vertex))
                return vertexIndex.at(vertex).GetDegree();
            else
                return 0;
        }
        
        /// Returns the identifier of the highest vertex in the index.
        /// @return Highest vertex identifier in the index, or 0 (which may also be a valid return value) if the index is empty.
        inline TVertexID GetMaximumVertexID(void) const
        {
            TVertexID maxVertexID = 0;

            if (0 != vertexIndex.size())
            {
                auto it = --vertexIndex.cend();
                maxVertexID = it->first;
            }

            return maxVertexID;
        }
        
        /// Returns the total number of edges in the index.
        /// @return Total number of edges.
        inline TEdgeCount GetNumEdges(void) const
        {
            return numEdges;
        }
        
        /// Returns the number of Vector-Sparse vectors required to represent the edges in this data structure.
        /// @return Number of vectors.
        inline size_t GetNumVectors(void) const
        {
            return numVectors;
        }
        
        /// Returns the number of top-level vertices in the index.
        /// @return Number of indexed vertices.
        inline TVertexCount GetNumVertices(void) const
        {
            return vertexIndex.size();
        }

        /// Inserts the specified edge into this data structure, using the destination as the top-level vertex.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferIndexedByDestination(const SEdge<TEdgeData>& edge);

        /// Inserts the specified edge into this data structure, using the source as the top-level vertex.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferIndexedBySource(const SEdge<TEdgeData>& edge);

        /// Removes the specified edge from this data structure.
        /// @param [in] indexedVertex Top-level vertex to which the edge corresponds.
        /// @param [in] otherVertex Vertex at the other end of the edge to remove.
        void RemoveEdge(const TVertexID indexedVertex, const TVertexID otherVertex);

        /// Removes the specified top-level vertex from this data structure.
        /// Does not remove any other references to the vertex within individual edge lists, just removes it from the index.
        /// @param [in] indexedVertex Top-level vertex to remove.
        void RemoveVertex(const TVertexID indexedVertex);
    };
}
