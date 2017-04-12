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

#include "Edge.hpp"
#include "Types.h"

#include <map>
#include <list>


namespace GraphTool
{
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
        typedef Edge<TEdgeData> TEdge;
        
        /// Alias for the container type to use to store edges within each index bucket.
        typedef std::list<TEdge> TEdgeList;

        /// Enables read-only access to all edges in this index, forward direction only.
        /// Accumulates and exposes data from multiple sources, so not a standard iterator type.
        class ConstIterator
        {
        private:
            // -------- INSTANCE VARIABLES --------------------------------- //

            /// Reference to the associated edge index.
            const std::map<TVertexID, TEdgeList>* vertexIndex;
            
            /// Top-level iterator, iterates through the vertex index.
            typename std::map<TVertexID, TEdgeList>::const_iterator indexIterator;
            
            /// Second-level iterator, iterates through the edges of each vertex in the index.
            typename TEdgeList::const_iterator vertexIterator;
            
            
        public:
            // -------- CONSTRUCTION AND DESTRUCTION ----------------------- //
            
            /// Default constructor.
            ConstIterator() : vertexIndex(), indexIterator(), vertexIterator()
            {
                // Nothing to do here.
            }
            
        private:
            /// Initialization constructor. Can only be invoked by a class-level factory method.
            ConstIterator(const std::map<TVertexID, TEdgeList>* vertexIndex, typename std::map<TVertexID, TEdgeList>::const_iterator& indexIterator, typename TEdgeList::const_iterator& vertexIterator) : vertexIndex(vertexIndex), indexIterator(indexIterator), vertexIterator(vertexIterator)
            {
                // Nothing to do here.
            }
            
            
        public:
            // -------- CLASS METHODS -------------------------------------- //
            
            /// Creates and returns an iterator to the beginning of the specified index.
            /// @param [in] vertexIndex Pointer to the vertex index of interest.
            /// @returns Read-only iterator to the beginning of the index.
            static ConstIterator ConstIteratorBegin(const std::map<TVertexID, TEdgeList>* vertexIndex)
            {
                auto indexIterator = vertexIndex->cbegin();
                auto vertexIterator = indexIterator->second.cbegin();

                return ConstIterator(vertexIndex, indexIterator, vertexIterator);
            }
            
            /// Creates and returns an iterator to the past-the-end element of the specified index.
            /// @param [in] vertexIndex Pointer to the vertex index of interest.
            /// @returns Read-only iterator to the past-the-end element of the index.
            static ConstIterator ConstIteratorEnd(const std::map<TVertexID, TEdgeList>* vertexIndex)
            {
                auto indexIterator = vertexIndex->cend();
                auto vertexIterator = (--(vertexIndex->cend()))->second.cend();

                return ConstIterator(vertexIndex, indexIterator, vertexIterator);
            }
            
            
            // -------- OPERATORS ------------------------------------------ //
            
            /// Compares this instance with another for equality.
            /// @param [in] other Reference to the other instance.
            /// @return `true` if this instance is equal to the other instance, `false` otherwise.
            inline bool operator==(const ConstIterator& other)
            {
                return ((this->indexIterator == other.indexIterator) && (this->vertexIterator == other.vertexIterator));
            }

            /// Compares this instance with another for inequality.
            /// @param [in] other Reference to the other instance.
            /// @return `true` if this instance is different from the other instance, `false` otherwise.
            inline bool operator!=(const ConstIterator& other)
            {
                return ((this->indexIterator != other.indexIterator) || (this->vertexIterator != other.vertexIterator));
            }

            /// Prefix-increments this instance by advancing it to the next position.
            /// @return This instance after it has been advanced.
            inline ConstIterator& operator++(void)
            {
                ++(this->vertexIterator);

                if (this->indexIterator->second.cend() == this->vertexIterator)
                {
                    ++(this->indexIterator);
                    
                    if (vertexIndex->cend() != this->indexIterator)
                        this->vertexIterator = this->indexIterator->second.cbegin();
                }

                return *this;
            }

            /// Postfix-increments this instance by advancing it to the next position.
            /// @return This instance before it has been advanced.
            inline ConstIterator operator++(int)
            {
                ConstIterator beforeIncrement = *this;
                ++(*this);
                return beforeIncrement;
            }
            
            
            // -------- INSTANCE METHODS ----------------------------------- //

            /// Retrieves and returns the vertex identifier of one end of the edge, namely the indexed vertex, at this iterator's current position.
            /// @return Identifier of the indexed vertex.
            inline TVertexID GetIndexedVertexID(void)
            {
                return this->indexIterator->first;
            }
            
            /// Retrieves and returns the vertex identifier of one end of the edge, namely the non-indexed vertex, at this iterator's current position.
            /// @return Identifier of the indexed vertex.
            inline TVertexID GetOtherVertexID(void)
            {
                return this->vertexIterator->vertex;
            }
            
            /// Retrieves and returns the vertex identifier of the second end of the edge, namely the other vertex and any edge data, at this iterator's current position.
            /// @return Information on the other end of the edge.
            inline const TEdge& GetOtherEdgeInfo(void)
            {
                return *(this->vertexIterator);
            }
        };
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Main data structure, indexed by one end of each edge.
        /// Maps from a vertex identifier to a list of edges connected to it.
        std::map<TVertexID, TEdgeList> vertexIndex;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        EdgeIndex(void)
        {
            // Nothing to do here.
        }
        
        
        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Creates and returns an iterator to the beginning of this edge index.
        /// @returns Read-only iterator to the beginning of the index.
        inline ConstIterator ConstIteratorBegin(void)
        {
            return ConstIterator::ConstIteratorBegin(&(this->vertexIndex));
        }

        /// Creates and returns an iterator to the past-the-end element of this edge index.
        /// @returns Read-only iterator to the end of the index.
        inline ConstIterator ConstIteratorEnd(void)
        {
            return ConstIterator::ConstIteratorEnd(&(this->vertexIndex));
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

        /// Retrieves and returns the vertex identifier of the lowest-identified vertex in the index.
        /// @param [out] lowestIndexedVertex Vertex identifier of the lowest-identified vertex in the index.
        /// @return `false` if the index is empty and thus no information is available, `true` otherwise.
        bool GetIndexLowerBound(TVertexID& lowestIndexedVertex)
        {
            if (vertexIndex.empty())
                return false;
            
            lowestIndexedVertex = vertexIndex.cbegin()->first;
            return true;
        }
        
        /// Retrieves and returns the vertex identifier of the highest-identified vertex in the index.
        /// @param [out] highestIndexedVertex Vertex identifier of the highest-identified vertex in the index.
        /// @return `false` if the index is empty and thus no information is available, `true` otherwise.
        bool GetIndexUpperBound(TVertexID& highestIndexedVertex)
        {
            if (vertexIndex.empty())
                return false;
            
            highestIndexedVertex = vertexIndex.crbegin()->first;
            return true;
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
        
        /// Inserts a single edge into the indexed data structure.
        /// @param [in] indexedVertex Identifier of the vertex in the index with which to associate the edge being inserted.
        /// @param [in] edgeInfo Individual edge information, including both the other end of the edge and edge data.
        void InsertEdge(TVertexID indexedVertex, TEdge& edgeInfo)
        {
            vertexIndex[indexedVertex].push_back(edgeInfo);
        }
        
        /// Specifies if this edge index is empty (i.e. contains no edges).
        /// @return `true` if empty, `false` otherwise.
        bool IsEmpty(void)
        {
            return vertexIndex.empty();
        }

        /// Merges the contents of the other index with this one by moving all the edges from the other index into this one, maintaining an existing sorted order.
        /// Expects both indices to be in some sorted order already, according to the predicate passed in.
        /// When completed, the other edge index is empty.
        /// @tparam TCompare Functon object or function pointer that implements the predicate.
        /// @param [in] other Edge index whose edges are to be merged into this one.
        /// @param [in] comparator Instance of the predicate.
        template <typename TCompare> void MergeEdges(EdgeIndex<TEdgeData>& other, TCompare comparator)
        {
            for (auto it = other.vertexIndex.begin(); it != other.vertexIndex.end(); ++it)
                vertexIndex[it->first].merge(it->second, comparator);

            other.vertexIndex.clear();
        }

        /// Merges the contents of the other index with this one, using sequence number as the comparison predicate.
        /// @param [in] other Edge index whose edges are to be merged into this one.
        inline void MergeEdgesBySeq(EdgeIndex<TEdgeData>& other)
        {
            MergeEdges(other, EdgeCompareLessBySeq<TEdgeData>());
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
        
        /// Splices the contents of the other index onto this one by moving all edges from the other index to this one, without regard for order.
        /// Does not attempt to sort, and does not expect either edge index to have been sorted prior to this operation.
        /// When completed, the other edge index is empty.
        /// @param [in] other Edge index whose edges are to be merged into this one.
        void SpliceEdges(EdgeIndex<TEdgeData>& other)
        {
            for (auto it = other.vertexIndex.begin(); it != other.vertexIndex.end(); ++it)
                vertexIndex[it->first].splice(vertexIndex[it->first].end(), it->second);
            
            other.vertexIndex.clear();
        }
    };
}
