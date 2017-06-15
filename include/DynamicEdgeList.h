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

#include <map>


namespace GraphTool
{
    /// Specifies the information to be held for each edge.
    /// Contains both the edge bit-mask and a pointer to edge data.
    /// The bit-mask contains bit set to '1' if the corresponding edge is present and '0' otherwise.
    /// The data points to an array of edge data for each '1' bit.
    /// Which vertices are represented per instance is determined by the data structure that uses it, such as by position.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct SDynamicEdgeInfo
    {
        uint32_t edges;                                                 ///< Edge presence bit-mask.
        TEdgeData* data;                                                ///< Pointer to array of edge data.
    };

    /// Specifies the information to be held for each edge.
    /// This version is specialized for unweighted graphs and only contains the edge bit-mask.
    template <> struct SDynamicEdgeInfo<void>
    {
        uint32_t edges;                                                 ///< Edge presence bit-mask.
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
        typedef typename std::map<uint64_t, SDynamicEdgeInfo<TEdgeData>>::const_iterator EdgeIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds all edge information.
        /// Key is the edge block identifier, value is the edge information structure.
        std::map<uint64_t, SDynamicEdgeInfo<TEdgeData>> edgeList;

        /// Holds the total number of edges present in this data structure.
        TEdgeCount degree;


    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        DynamicEdgeList(void);


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

        /// Returns the number of blocks in this data structure.
        /// This is a lower-level measurement related to the underlying data structure implementation.
        inline size_t GetNumBlocks(void) const
        {
            return edgeList.size();
        }

        /// Inserts the specified edge into this data structure, using the destination as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferDestination(const SEdge<TEdgeData>& edge);

        /// Inserts the specified edge into this data structure, using the source as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferSource(const SEdge<TEdgeData>& edge);

        /// Removes the specified edge from this data structure.
        /// @param [in] otherVertex Vertex at the other end of the edge to remove.
        void RemoveEdge(const TVertexID otherVertex);
    };
}
