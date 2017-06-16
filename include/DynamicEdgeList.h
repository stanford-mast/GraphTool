/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file DynamicEdgeList.h
 *   Declaration of a container for holding edges that correspond to a single
 *   top-level vertex, optimized for fast insertion.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <forward_list>
#include <map>


namespace GraphTool
{
    /// Specifies the information to be held for each edge.
    /// Contains both the other end of the edge and the edge data (i.e. weight).
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct SEdgeInfo
    {
        TVertexID otherVertex;                                          ///< Vertex identifier for the other end of the edge.
        TEdgeData edgeData;                                             ///< Edge data, such as a weight.
    };

    /// Specifies the information to be held for each edge.
    /// This version is specialized for unweighted graphs and only contains other end of the edge.
    template <> struct SEdgeInfo<void>
    {
        TVertexID otherVertex;                                          ///< Vertex identifier for the other end of the edge.
    };
    
    /// Holds edges in a way optimized for fast insertion; useful for ingress.
    /// Represents graph topology data and can hold edge data, such as weights, as well.
    /// This indexed data structure represents unidirectional edges but does not specify the direction.
    /// Direction information depends on the usage semantics and is governed by the code that instantiates objects of this type.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class DynamicEdgeList
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Alias for the iterator type used by this class.
        typedef typename std::forward_list<SEdgeInfo<TEdgeData>>::const_iterator EdgeIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds all edge information.
        /// Key is the edge block identifier, value is the edge information structure.
        std::forward_list<SEdgeInfo<TEdgeData>> edgeList;

        /// Holds the total number of edges present in this data structure.
        TEdgeCount degree;
        
        /// Holds the total number of Vector-Sparse vectors needed to represent the edges in this data structure.
        size_t numVectors;


    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        DynamicEdgeList(void);


    private:
        // -------- HELPERS ------------------------------------------------ //

        /// Fills in an edge information structure with edge information.
        /// @param [out] edgeInfo Edge information structure to fill.
        /// @param [in] edge Edge to use as the data source.
        /// @param [in] useDestinationVertex Specifies that the destination vertex, rather than the source vertex, in the edge should be used as the other vertex identifier.
        void FillEdgeInfoFromEdge(SEdgeInfo<TEdgeData>& edgeInfo, const SEdge<TEdgeData>& edge, bool useDestinationVertex);

        
    public:
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Returns a read-only iterator for the beginning of the edge list.
        /// @return Read-only iterator to the beginning of the edge list.
        inline EdgeIterator BeginIterator(void) const
        {
            return edgeList.cbegin();
        }

        /// Returns a read-only iterator for the end of the edge list.
        /// @return Read-only iterator for the end of the edge list.
        inline EdgeIterator EndIterator(void) const
        {
            return edgeList.cend();
        }

        /// Returns the total number of edges in this data structure (i.e. the degree of the top-level vertex it represents).
        /// @return Total number of edges, which could be 0.
        inline TEdgeCount GetDegree(void) const
        {
            return degree;
        }
        
        /// Returns the number of Vector-Sparse vectors needed to represent the edges in this data structure.
        /// @return Total number of vectors, which could be 0.
        inline size_t GetNumVectors(void) const
        {
            return numVectors;
        }

        /// Inserts the specified edge into this data structure, using the destination as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeUsingDestination(const SEdge<TEdgeData>& edge);

        /// Inserts the specified edge into this data structure, using the source as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeUsingSource(const SEdge<TEdgeData>& edge);

        /// Removes the specified edge from this data structure.
        /// @param [in] otherVertex Vertex at the other end of the edge to remove.
        void RemoveEdge(const TVertexID otherVertex);
    };
}
