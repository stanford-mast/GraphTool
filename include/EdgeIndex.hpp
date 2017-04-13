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

#include "Comparator.hpp"
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

        /// Alias for the container type used to implement the top-level vertex index.
        typedef std::map<TVertexID, TEdgeList> TVertexIndex;
        
        /// Enables read-only access to the indexed vertices in this index, forward direction only.
        /// Not a standard iterator type.
        class ConstVertexIterator
        {
            // -------- FRIENDSHIPS -------------------------------------------- //

            friend class ConstEdgeIterator;
            
            
        private:
            // -------- INSTANCE VARIABLES --------------------------------- //

            /// Reference to the associated edge index.
            const TVertexIndex* vertexIndex;

            /// Iterates through the vertex index.
            typename TVertexIndex::const_iterator indexIterator;


        public:
            // -------- CONSTRUCTION AND DESTRUCTION ----------------------- //

            /// Default constructor.
            ConstVertexIterator() : vertexIndex(NULL), indexIterator()
            {
                // Nothing to do here.
            }

        private:
            /// Initialization constructor. Can only be invoked by a class-level factory method.
            ConstVertexIterator(const TVertexIndex* vertexIndex, typename TVertexIndex::const_iterator& indexIterator) : vertexIndex(vertexIndex), indexIterator(indexIterator)
            {
                // Nothing to do here.
            }


        public:
            // -------- CLASS METHODS -------------------------------------- //
            
            /// Creates and returns an iterator to the specified vertex in this edge index, if it exists.
            /// @return Read-only iterator at the specified position, or an invalid iterator if the vertex is not present in the specified edge index.
            static ConstVertexIterator ConstVertexIteratorAt(const TVertexIndex* vertexIndex, const TVertexID vertexID)
            {
                if (0 != vertexIndex->count(vertexID))
                    return ConstVertexIterator(vertexIndex, vertexIndex->find(vertexID));
                else
                    return ConstVertexIterator();
            }
            
            /// Creates and returns an iterator to the beginning of the specified index.
            /// @param [in] vertexIndex Pointer to the vertex index of interest.
            /// @return Read-only iterator to the beginning of the index.
            static ConstVertexIterator ConstVertexIteratorBegin(const TVertexIndex* vertexIndex)
            {
                return ConstVertexIterator(vertexIndex, vertexIndex->cbegin());
            }

            /// Creates and returns an iterator to the past-the-end element of the specified index.
            /// @param [in] vertexIndex Pointer to the vertex index of interest.
            /// @return Read-only iterator to the past-the-end element of the index.
            static ConstVertexIterator ConstVertexIteratorEnd(const TVertexIndex* vertexIndex)
            {
                return ConstVertexIterator(vertexIndex, vertexIndex->cend());
            }
            
            
            // -------- OPERATORS ------------------------------------------ //

            /// Compares this instance with another for equality.
            /// @param [in] other Reference to the other instance.
            /// @return `true` if this instance is equal to the other instance, `false` otherwise.
            inline bool operator==(const ConstVertexIterator& other) const
            {
                return (this->indexIterator == other.indexIterator);
            }

            /// Compares this instance with another for inequality.
            /// @param [in] other Reference to the other instance.
            /// @return `true` if this instance is different from the other instance, `false` otherwise.
            inline bool operator!=(const ConstVertexIterator& other) const
            {
                return (this->indexIterator != other.indexIterator);
            }

            /// Prefix-increments this instance by advancing it to the next position.
            /// @return This instance after it has been advanced.
            ConstVertexIterator& operator++(void)
            {
                ++(this->indexIterator);
                return *this;
            }

            /// Postfix-increments this instance by advancing it to the next position.
            /// @return This instance before it has been advanced.
            ConstVertexIterator operator++(int)
            {
                ConstVertexIterator beforeIncrement = *this;
                ++(*this);
                return beforeIncrement;
            }

            /// Advances this iterator by the specified number of positions.
            /// Saturates at the end of the associated edge index (i.e. will not advance past its past-the-end iterator).
            /// @param [in] numPositions Number of positions by which to advance this iteartor.
            ConstVertexIterator& operator+=(const TVertexCount numPositions)
            {
                ConstVertexIterator endIter = ConstVertexIteratorEnd(this->vertexIndex);

                for (TVertexCount i = 0; i < numPositions && *this != endIter; ++i)
                    ++(*this);

                return *this;
            }

            /// Advances a copy of this iterator by the specified number of positions.
            /// Saturates at the end of the associated edge index (i.e. will not advance past its past-the-end iterator).
            /// @param [in] numPositions Number of positions by which to advance this iterator.
            ConstVertexIterator operator+(const TEdgeCount numPositions) const
            {
                ConstVertexIterator result = *this;
                result += numPositions;
                return result;
            }


            // -------- INSTANCE METHODS ----------------------------------- //

            /// Determines if this iterator is associated with the specified edge index.
            /// @param [in] edgeIndex Edge index to check.
            /// @return `true` if this iterator belongs to the specified edge index, `false` otherwise.
            inline bool BelongsToEdgeIndex(const EdgeIndex<TEdgeData>& edgeIndex) const
            {
                return (&edgeIndex.vertexIndex == this->vertexIndex);
            }

            /// Specifies if this iterator is correctly initialized with internal state that can be used to retrieve graph information.
            /// Does not determine validity across graph manipulation operations.
            /// @return `true` if this iterator is valid, `false` otherwise.
            inline bool IsValid(void) const
            {
                return (NULL != this->vertexIndex);
            }
            
            /// Retrieves and returns the vertex identifier of the vertex in the index to which this iterator is currently pointing.
            /// @return Identifier of the indexed vertex.
            inline TVertexID GetVertexID(void) const
            {
                return this->indexIterator->first;
            }
        };
        
        /// Enables read-only access to all edges in this index, forward direction only.
        /// Accumulates and exposes data from multiple sources, so not a standard iterator type.
        class ConstEdgeIterator
        {
        private:
            // -------- INSTANCE VARIABLES --------------------------------- //

            /// Reference to the associated edge index.
            const TVertexIndex* vertexIndex;
            
            /// Top-level iterator, iterates through the vertex index.
            typename TVertexIndex::const_iterator indexIterator;
            
            /// Second-level iterator, iterates through the edges of each vertex in the index.
            typename TEdgeList::const_iterator vertexIterator;
            
            
        public:
            // -------- CONSTRUCTION AND DESTRUCTION ----------------------- //
            
            /// Default constructor.
            ConstEdgeIterator() : vertexIndex(NULL), indexIterator(), vertexIterator()
            {
                // Nothing to do here.
            }
            
        private:
            /// Initialization constructor. Can only be invoked by a class-level factory method.
            ConstEdgeIterator(const TVertexIndex* vertexIndex, typename TVertexIndex::const_iterator& indexIterator, typename TEdgeList::const_iterator& vertexIterator) : vertexIndex(vertexIndex), indexIterator(indexIterator), vertexIterator(vertexIterator)
            {
                // Nothing to do here.
            }
            
            
        public:
            // -------- CLASS METHODS -------------------------------------- //
            
            /// Creates and returns an iterator to the position that corresponds to the beginning of the edges at the specified vertex.
            /// @param [in] vertexIndex Pointer to the vertex index of interest.
            /// @return Read-only iterator to the beginning of the edges for the corresponding vertex, or an invalid iterator if the vertex iterator supplied does not belong to the edge index.
            static ConstEdgeIterator ConstEdgeIteratorAt(const EdgeIndex<TEdgeData>& edgeIndex, const ConstVertexIterator& vertexIterator)
            {
                if (vertexIterator.BelongsToEdgeIndex(edgeIndex))
                {
                    auto indexIterator = vertexIterator.indexIterator;
                    auto vertexIterator = indexIterator->second.cbegin();

                    return ConstEdgeIterator(&edgeIndex.vertexIndex, indexIterator, vertexIterator);
                }
                else
                    return ConstEdgeIterator();
            }
            
            /// Creates and returns an iterator to the beginning of the specified index.
            /// @param [in] vertexIndex Pointer to the vertex index of interest.
            /// @return Read-only iterator to the beginning of the index.
            static ConstEdgeIterator ConstEdgeIteratorBegin(const TVertexIndex* vertexIndex)
            {
                auto indexIterator = vertexIndex->cbegin();
                auto vertexIterator = indexIterator->second.cbegin();

                return ConstEdgeIterator(vertexIndex, indexIterator, vertexIterator);
            }
            
            /// Creates and returns an iterator to the past-the-end element of the specified index.
            /// @param [in] vertexIndex Pointer to the vertex index of interest.
            /// @return Read-only iterator to the past-the-end element of the index.
            static ConstEdgeIterator ConstEdgeIteratorEnd(const TVertexIndex* vertexIndex)
            {
                auto indexIterator = vertexIndex->cend();
                auto vertexIterator = (--(vertexIndex->cend()))->second.cend();

                return ConstEdgeIterator(vertexIndex, indexIterator, vertexIterator);
            }
            
            
            // -------- OPERATORS ------------------------------------------ //
            
            /// Compares this instance with another for equality.
            /// @param [in] other Reference to the other instance.
            /// @return `true` if this instance is equal to the other instance, `false` otherwise.
            inline bool operator==(const ConstEdgeIterator& other) const
            {
                return ((this->indexIterator == other.indexIterator) && (this->vertexIterator == other.vertexIterator));
            }

            /// Compares this instance with another for inequality.
            /// @param [in] other Reference to the other instance.
            /// @return `true` if this instance is different from the other instance, `false` otherwise.
            inline bool operator!=(const ConstEdgeIterator& other) const
            {
                return ((this->indexIterator != other.indexIterator) || (this->vertexIterator != other.vertexIterator));
            }
            
            /// Prefix-increments this instance by advancing it to the next position.
            /// @return This instance after it has been advanced.
            ConstEdgeIterator& operator++(void)
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
            ConstEdgeIterator operator++(int)
            {
                ConstEdgeIterator beforeIncrement = *this;
                ++(*this);
                return beforeIncrement;
            }

            /// Advances this iterator by the specified number of positions.
            /// Saturates at the end of the associated edge index (i.e. will not advance past its past-the-end iterator).
            /// @param [in] numPositions Number of positions by which to advance this iteartor.
            ConstEdgeIterator& operator+=(const TEdgeCount numPositions)
            {
                ConstEdgeIterator endIter = ConstEdgeIteratorEnd(this->vertexIndex);

                for (TEdgeCount i = 0; i < numPositions && *this != endIter; ++i)
                    ++(*this);

                return *this;
            }

            /// Advances a copy of this iterator by the specified number of positions.
            /// Saturates at the end of the associated edge index (i.e. will not advance past its past-the-end iterator).
            /// @param [in] numPositions Number of positions by which to advance this iterator.
            ConstEdgeIterator operator+(const TEdgeCount numPositions) const
            {
                ConstEdgeIterator result = *this;
                result += numPositions;
                return result;
            }
            
            
            // -------- INSTANCE METHODS ----------------------------------- //
            
            /// Determines if this iterator is associated with the specified edge index.
            /// @param [in] edgeIndex Edge index to check.
            /// @return `true` if this iterator belongs to the specified edge index, `false` otherwise.
            inline bool BelongsToEdgeIndex(const EdgeIndex<TEdgeData>& edgeIndex) const
            {
                return (&edgeIndex.vertexIndex == this->vertexIndex);
            }
            
            /// Specifies if this iterator is correctly initialized with internal state that can be used to retrieve graph information.
            /// Does not determine validity across graph manipulation operations.
            /// @return `true` if this iterator is valid, `false` otherwise.
            inline bool IsValid(void) const
            {
                return (NULL != this->vertexIndex);
            }
            
            /// Retrieves and returns the vertex identifier of one end of the edge, namely the indexed vertex, at this iterator's current position.
            /// @return Identifier of the indexed vertex.
            inline TVertexID GetIndexedVertexID(void) const
            {
                return this->indexIterator->first;
            }
            
            /// Retrieves and returns the vertex identifier of one end of the edge, namely the non-indexed vertex, at this iterator's current position.
            /// @return Identifier of the indexed vertex.
            inline TVertexID GetOtherVertexID(void) const
            {
                return this->vertexIterator->vertex;
            }
            
            /// Retrieves and returns the vertex identifier of the second end of the edge, namely the other vertex and any edge data, at this iterator's current position.
            /// @return Information on the other end of the edge.
            inline const TEdge& GetOtherEdgeInfo(void) const
            {
                return *(this->vertexIterator);
            }
        };
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Main data structure, indexed by one end of each edge.
        /// Maps from a vertex identifier to a list of edges connected to it.
        TVertexIndex vertexIndex;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        EdgeIndex(void)
        {
            // Nothing to do here.
        }
        
        
        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Creates and returns an iterator to the beginning of the edges corresponding to the specified vertex iterator.
        /// @return Read-only iterator at the specified position, or an invalid iterator if the vertex iterator supplied does not belong to this edge index.
        inline ConstEdgeIterator ConstEdgeIteratorAt(ConstVertexIterator& vertexIterator) const
        {
            return ConstEdgeIterator::ConstEdgeIteratorAt(*this, vertexIterator);
        }
        
        /// Creates and returns an iterator to the beginning of this edge index.
        /// @return Read-only iterator to the beginning of the index.
        inline ConstEdgeIterator ConstEdgeIteratorBegin(void) const
        {
            return ConstEdgeIterator::ConstEdgeIteratorBegin(&(this->vertexIndex));
        }

        /// Creates and returns an iterator to the past-the-end element of this edge index.
        /// @return Read-only iterator to the end of the index.
        inline ConstEdgeIterator ConstEdgeIteratorEnd(void) const
        {
            return ConstEdgeIterator::ConstEdgeIteratorEnd(&(this->vertexIndex));
        }

        /// Creates and returns an iterator to the specified vertex in this edge index, if it exists.
        /// @return Read-only iterator at the specified position, or an invalid iterator if the vertex is not present in this edge index.
        inline ConstVertexIterator ConstVertexIteratorAt(const TVertexID vertexID) const
        {
            return ConstVertexIterator::ConstVertexIteratorAt(&(this->vertexIndex), vertexID);
        }
        
        /// Creates and returns an iterator to the beginning vertex in this edge index.
        /// @return Read-only iterator to the beginning of the index.
        inline ConstVertexIterator ConstVertexIteratorBegin(void) const
        {
            return ConstVertexIterator::ConstVertexIteratorBegin(&(this->vertexIndex));
        }

        /// Creates and returns an iterator to the past-the-end vertex in this edge index.
        /// @return Read-only iterator to the end of the index.
        inline ConstVertexIterator ConstVertexIteratorEnd(void) const
        {
            return ConstVertexIterator::ConstVertexIteratorEnd(&(this->vertexIndex));
        }

        /// Retrieves and returns information about the vertex of highest degree.
        /// @param [out] highestDegreeVertex Vertex identifier of the vertex with the highest degree.
        /// @param [out] highestDegree Degree of the vertex with the highest degree.
        void GetHighestDegreeInfo(TVertexID& highestDegreeVertex, TEdgeCount& highestDegree) const
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
        bool GetIndexLowerBound(TVertexID& lowestIndexedVertex) const
        {
            if (vertexIndex.empty())
                return false;
            
            lowestIndexedVertex = vertexIndex.cbegin()->first;
            return true;
        }
        
        /// Retrieves and returns the vertex identifier of the highest-identified vertex in the index.
        /// @param [out] highestIndexedVertex Vertex identifier of the highest-identified vertex in the index.
        /// @return `false` if the index is empty and thus no information is available, `true` otherwise.
        bool GetIndexUpperBound(TVertexID& highestIndexedVertex) const
        {
            if (vertexIndex.empty())
                return false;
            
            highestIndexedVertex = vertexIndex.crbegin()->first;
            return true;
        }
        
        /// Retrieves and returns the total number of edges in this data structure.
        /// @return Total number of edges.
        TEdgeCount GetNumEdges(void) const
        {
            TEdgeCount numEdges = 0;

            for (auto it = vertexIndex.cbegin(); it != vertexIndex.cend(); ++it)
                numEdges += it->second.size();

            return numEdges;
        }

        /// Retrieves and returns the number of vertices in the index.
        /// @return Total number of indexed vertices.
        TVertexCount GetNumIndexedVertices(void) const
        {
            return vertexIndex.size();
        }
        
        /// Retrieves and returns the degree of the specified vertex.
        /// This is determined by querying the number of vertices in the edge list of the specified vertex in the index.
        /// @param [in] indexedVertex Identifier of the vertex of interest.
        /// @return Degree of the specified vertex, which could be 0.
        TEdgeCount GetVertexDegree(TVertexID indexedVertex) const
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
        /// @param [in] other Edge index whose edges are to be merged into this one.
        /// @param [in] comparator Instance of the predicate, which must be stateless.
        void MergeEdges(EdgeIndex<TEdgeData>& other, StatelessComparator<TEdgeData>& comparator)
        {
            WrappedComparator<TEdgeData> compare(comparator);
            
            for (auto it = other.vertexIndex.begin(); it != other.vertexIndex.end(); ++it)
                vertexIndex[it->first].merge(it->second, compare);

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

    /// Convenience alias for edge indices that do not use edge data or weights.
    typedef EdgeIndex<void> UnweightedEdgeIndex;
}
