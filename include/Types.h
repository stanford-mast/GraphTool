/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Types.h
 *   Definitions of common types used throughout this program.
 *****************************************************************************/

#pragma once

#include <cstdint>


namespace GraphTool
{
    /// Specifies the type to use for identifying vertices.
    typedef uint64_t TVertexID;

    /// Specifies the type to use for identifying edges.
    typedef uint64_t TEdgeID;

    /// Specifies the type to use for counting vertices.
    typedef uint64_t TVertexCount;

    /// Specifies the type to use for counting edges.
    typedef uint64_t TEdgeCount;

    /// Represents an individual edge within a buffer.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct SEdge
    {
        TVertexID sourceVertex;                                             ///< Identifier of the source vertex.
        TVertexID destinationVertex;                                        ///< Identifier of the destination vertex.
        TEdgeData edgeData;                                                 ///< Edge data, such as a weight.
    };

    /// Represents an individual edge within a buffer, specialized for unweighted graphs.
    template <> struct SEdge<void>
    {
        TVertexID sourceVertex;                                             ///< Identifier of the source vertex.
        TVertexID destinationVertex;                                        ///< Identifier of the destination vertex.
    };
}
