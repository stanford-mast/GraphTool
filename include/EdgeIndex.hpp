/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file EdgeIndex.hpp
 *   Implementation of a templated data structure that holds edges in a form
 *   indexed by the vertex at one end.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <map>
#include <list>


namespace GraphTool
{
    /// Represents an individual edge within an edge index data structure.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct SEdge
    {
        // -------- INSTANCE VARIABLES --------------------------------------------- //

        /// Identifier of one end of the edge. The other end is inferred from position within the index.
        TVertexID vertex;

        /// Data associated with the edge, such as a weight.
        TEdgeData edgeData;


        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Default constructor.
        SEdge(void) : vertex(), edgeData()
        {
            // Nothing to do here.
        }
        
        
        // -------- OPERATORS ---------------------------------------------- //

        /// Compares this instance with another.
        /// Equality is only determined by vertex, not by edge data.
        /// @param [in] other Reference to the other instance.
        /// @return `true` if this instance is equal to the other instance, `false` otherwise.
        inline bool operator==(const SEdge& other)
        {
            return (this->vertex == other.vertex);
        }
        
        
        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Fills information in this structure from edge buffer data, using the destination vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromDestinationEdgeBuffer(SEdgeBufferData<TEdgeData>& edgeBuffer)
        {
            this->vertex = edgeBuffer.destinationVertex;
            this->edgeData = edgeBuffer.edgeData;
        }
        
        /// Fills information in this structure from edge buffer data, using the source vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromSourceEdgeBuffer(SEdgeBufferData<TEdgeData>& edgeBuffer)
        {
            this->vertex = edgeBuffer.sourceVertex;
            this->edgeData = edgeBuffer.edgeData;
        }
    };

    /// Represents an individual edge within an edge index data structure, specialized for unweighted graphs.
    template <> struct SEdge<void>
    {
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Identifier of one end of the edge. The other end is inferred from position within the index.
        TVertexID vertex;


        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor.
        SEdge(void) : vertex()
        {
            // Nothing to do here.
        }
        
        /// Enables implicit conversion to this type from a vertex identifier.
        SEdge(TVertexID vertex) : vertex(vertex)
        {
            // Nothing to do here.
        }
        
        
        // -------- OPERATORS ---------------------------------------------- //
        
        /// Compares this instance with another.
        /// Equality is only determined by vertex, not by edge data.
        /// @param [in] other Reference to the other instance.
        /// @return `true` if this instance is equal to the other instance, `false` otherwise.
        inline bool operator==(const SEdge& other)
        {
            return (this->vertex == other.vertex);
        }
        
                
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Fills information in this structure from edge buffer data, using the destination vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromDestinationEdgeBuffer(SEdgeBufferData<void>& edgeBuffer)
        {
            this->vertex = edgeBuffer.destinationVertex;
        }

        /// Fills information in this structure from edge buffer data, using the source vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromSourceEdgeBuffer(SEdgeBufferData<void>& edgeBuffer)
        {
            this->vertex = edgeBuffer.sourceVertex;
        }
    };
    
    /// Holds edges within an indexed data structure.
    /// Represents graph topology data and can hold edge data, such as weights, as well.
    /// This indexed data structure represents unidirectional edges but does not specify the direction.
    /// Direction information depends on the usage semantics and is governed by the code that instantiates objects of this type.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class EdgeIndex
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Alias for the templated type of edge data to store.
        typedef SEdge<TEdgeData> TEdge;
        
        /// Alias for the container type to use to store edges within each index bucket.
        typedef std::list<TEdge> TEdgeList;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Main data structure, indexed by one end of each edge.
        /// Maps from a vertex identifier to a list of edges connected to it.
        std::map <TVertexID, TEdgeList> vertexIndex;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        EdgeIndex(void)
        {
            // Nothing to do here.
        }
        
        /// Default destructor.
        virtual ~EdgeIndex()
        {
            // Nothing to do here.
        }


        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Inserts a single edge into the indexed data structure.
        /// @param [in] indexedVertex Identifier of the vertex in the index with which to associate the edge being inserted.
        /// @param [in] edgeInfo Individual edge information, including both the other end of the edge and edge data.
        void InsertEdge(TVertexID indexedVertex, TEdge& edgeInfo)
        {
            vertexIndex[indexedVertex].push_back(edgeInfo);
        }
        
        /// Accesses edges for the specified indexed vertex by providing iterators to the start and end of the corresponding edge list.
        /// @param [in] indexedVertex Identifier of the vertex in the index for which edges should be accessed.
        /// @param [out] begin Iterator to fill with the beginning iterator of the edge list.
        /// @param [out] end Iterator to fill with the end iterator of the edge list.
        /// @return `true` if iterators were filled, `false` otherwise (for example, if the degree of the vertex is 0, then no iterators would be filled).
        bool GetEdgesForVertex(TVertexID indexedVertex, typename TEdgeList::const_iterator& begin, typename TEdgeList::const_iterator& end)
        {
            bool gotEdges = false;
            
            if (0 != vertexIndex.count(indexedVertex))
            {
                gotEdges = true;

                begin = vertexIndex.at(indexedVertex).cbegin();
                end = vertexIndex.at(indexedVertex).cend();
            }
            
            return gotEdges;
        }

        /// Retrieves and returns information about the vertex of highest degree.
        /// @param [out] highestDegreeVertex Vertex identifier of the vertex with the highest degree.
        /// @param [out] highestDegree Degree of the vertex with the highest degree.
        void GetHighestDegreeInfo(TVertexID& highestDegreeVertex, TEdgeCount& highestDegree)
        {
            highestDegreeVertex = 0;
            highestDegree = 0;
            
            for (auto it = vertexIndex.cbegin(); it != vertexIndex.cend(); ++it)
            {
                if (it->second.size() > highestDegree)
                {
                    highestDegreeVertex = it->first;
                    highestDegree = it->second.size();
                }
            }
        }

        /// Retrieves and returns the total number of edges in this data structure.
        /// @return Total number of edges.
        TEdgeCount GetNumEdges(void)
        {
            TEdgeCount numEdges = 0;

            for (auto it = vertexIndex.cbegin(); it != vertexIndex.cend(); ++it)
                numEdges += it->second.size();

            return numEdges;
        }

        /// Retrieves and returns the number of vertices in the index.
        /// @return Total number of indexed vertices.
        TVertexCount GetNumIndexedVertices(void)
        {
            return vertexIndex.size();
        }
        
        /// Retrieves and returns the degree of the specified vertex.
        /// This is determined by querying the number of vertices in the edge list of the specified vertex in the index.
        /// @param [in] indexedVertex Identifier of the vertex of interest.
        /// @return Degree of the specified vertex, which could be 0.
        TEdgeCount GetVertexDegree(TVertexID indexedVertex)
        {
            TVertexCount vertexDegree = 0;

            if (0 != vertexIndex.count(indexedVertex))
                vertexDegree = vertexIndex.at(indexedVertex).size();

            return vertexDegree;
        }
        
        /// Merges this index with the other index by inserting all edges from the other index into this one.
        /// When completed, the other edge index is empty.
        /// @param [in] other Edge index whose edges are to be merged into this one.
        void MergeEdges(EdgeIndex<TEdgeData>& other)
        {
            for (auto it = other.vertexIndex.begin(); it != other.vertexIndex.end(); ++it)
                vertexIndex[it->first].splice(vertexIndex[it->first].end(), it->second);

            other.vertexIndex.clear();
        }

        /// Removes a single edge from the indexed data structure.
        /// @param [in] indexedVertex Identifier of the vertex in the index with which to associate the edge being removed.
        /// @param [in] otherVertex Identifier of the vertex on the other end of the edge.
        void RemoveEdge(TVertexID indexedVertex, TVertexID otherVertex)
        {
            if (0 != vertexIndex.count(indexedVertex))
            {
                vertexIndex.at(indexedVertex).remove(otherVertex);

                if (0 == vertexIndex[indexedVertex].size())
                    vertexIndex.erase(indexedVertex);
            }
        }

        /// Removes all edges containing the specified vertex from the indexed data structure.
        /// @param [in] vertex Vertex to remove.
        void RemoveVertex(TVertexID vertex)
        {
            vertexIndex.erase(vertex);

            for (auto it = vertexIndex.begin(); it != vertexIndex.end(); ++it)
                it->second.remove(vertex);
        }

        /// Replaces all edges contained in the current data structure with those contained in the supplied replacement.
        /// Internally implemented as a swap, so the replacement data structure will be cleared during this process.
        /// @param [in,out] replacementEdges Replacement edge data structure.
        void SetEdges(EdgeIndex<TEdgeData>& replacementEdges)
        {
            vertexIndex.clear();
            std::swap(vertexIndex, replacementEdges.vertexIndex);
        }
    };
}
