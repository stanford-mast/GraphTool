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

#include "Edge.hpp"
#include "Types.h"

#include <map>


namespace GraphTool
{
    /// Holds edges in a way optimized for fast insertion; useful for ingress.
    /// Represents graph topology data and can hold edge data, such as weights, as well.
    /// This indexed data structure represents unidirectional edges but does not specify the direction.
    /// Direction information depends on the usage semantics and is governed by the code that instantiates objects of this type.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class DynamicEdgeList
    {
    private:
        // -------- TYPE DEFINITIONS --------------------------------------- //

        /// Specifies the information to be held with each edge.
        /// Contains both the edge bit-mask and a pointer to edge data.
        /// See EdgeList documentation for more information.
        struct SDynamicEdgeInfo
        {
            uint64_t edges;                                                 ///< Edge presence bit-mask.
            TEdgeData* data;                                                ///< Pointer to array of edge data.
        };

    public:
        /// Alias for the iterator type used by this class.
        typedef typename std::map<uint64_t, SDynamicEdgeInfo>::const_iterator EdgeIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds all edge information.
        /// Key is the edge block identifier, value is the edge information structure.
        std::map<uint64_t, SDynamicEdgeInfo> edgeList;

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

        /// Inserts the specified edge into this data structure.
        /// @param [in] edge Edge to insert.
        void InsertEdge(const Edge<TEdgeData>& edge);

        /// Inserts the specified edge into this data structure, using the destination as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferDestination(const SEdgeBufferData<TEdgeData>& edge);

        /// Inserts the specified edge into this data structure, using the source as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferSource(const SEdgeBufferData<TEdgeData>& edge);

        /// Removes the specified edge from this data structure.
        /// @param [in] otherVertex Vertex at the other end of the edge to remove.
        void RemoveEdge(const TVertexID otherVertex);
    };

    /// Holds edges in a way optimized for fast insertion, specialized for unweighted graphs.
    template <> class DynamicEdgeList<void>
    {
    private:
        // -------- TYPE DEFINITIONS --------------------------------------- //

        /// Specifies the information to be held within each edge block.
        /// Contains the edge bit-mask.
        /// See EdgeList documentation for more information.
        struct SDynamicEdgeInfo
        {
            uint64_t edges;                                                 ///< Edge presence bit-mask.
        };
        
    public:
        /// Alias for the iterator type used by this class.
        typedef typename std::map<uint64_t, SDynamicEdgeInfo>::const_iterator EdgeIterator;
        
        
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds all edge information.
        /// Key is the edge block identifier, value is the edge information structure.
        std::map<uint64_t, SDynamicEdgeInfo> edgeList;

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

        /// Inserts the specified edge into this data structure.
        /// @param [in] edge Edge to insert.
        void InsertEdge(const Edge<void>& edge);

        /// Inserts the specified edge into this data structure, using the destination as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferDestination(const SEdgeBufferData<void>& edge);

        /// Inserts the specified edge into this data structure, using the source as its data source.
        /// @param [in] edge Edge to insert.
        void InsertEdgeBufferSource(const SEdgeBufferData<void>& edge);

        /// Removes the specified edge from this data structure.
        /// @param [in] otherVertex Vertex at the other end of the edge to remove.
        void RemoveEdge(const TVertexID otherVertex);
    };
}
