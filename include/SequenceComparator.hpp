/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file SequenceComparator.hpp
 *   Implementation of a function object that compares edges based on their
 *   sequence numbers, thus respecting the order from an input file.
 *****************************************************************************/

#pragma once

#include "Comparator.hpp"
#include "Edge.hpp"


namespace GraphTool
{
    /// Compares edges based on their sequence numbers, returning a result based on a less-than comparison.
    /// Sorting order is ascending based on the sequence number of each supplied edge.
    /// Preserves input file ordering of edges in a graph.
    /// This is a stateless comparison.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct SequenceAscendingComparator : public StatelessComparator<TEdgeData>
    {
        // -------- ABSTRACT OPERATORS ------------------------------------- //
        // See "Comparator.hpp" for documentation.
       
        virtual bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second) const
        {
            return (first.seq < second.seq);
        }
    };

    /// Compares edges based on their sequence numbers, returning a result based on a greater-than comparison.
    /// Sorting order is descending based on the sequence number of each supplied edge.
    /// Reverses input file ordering of edges in a graph.
    /// This is a stateless comparison.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct SequenceDescendingComparator : public StatelessComparator<TEdgeData>
    {
        // -------- ABSTRACT OPERATORS ------------------------------------- //
        // See "Comparator.hpp" for documentation.

        virtual bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second) const
        {
            return (first.seq > second.seq);
        }
    };
}
