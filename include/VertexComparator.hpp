/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file VertexComparator.hpp
 *   Implementation of a function object that compares edges based on their
 *   vertex identifiers, thus sorting by vertex.
 *****************************************************************************/

#pragma once

#include "Comparator.hpp"
#include "Edge.hpp"


namespace GraphTool
{
    /// Compares edges based on their vertex identifiers, returning a result based on a less-than comparison.
    /// Resulting order is sorted ascending by vertex ID.
    /// This is a stateless comparison.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct VertexAscendingComparator : public StatelessComparator<TEdgeData>
    {
        // -------- ABSTRACT OPERATORS ------------------------------------- //
        // See "Comparator.hpp" for documentation.

        virtual bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second)
        {
            return (first.vertex < second.vertex);
        }
    };

    /// Compares edges based on their vertex identifiers, returning a result based on a greater-than comparison.
    /// Resulting order is sorted descending by vertex ID.
    /// This is a stateless comparison.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct VertexDescendingComparator : public StatelessComparator<TEdgeData>
    {
        // -------- ABSTRACT OPERATORS ------------------------------------- //
        // See "Comparator.hpp" for documentation.

        virtual bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second)
        {
            return (first.vertex > second.vertex);
        }
    };
}
