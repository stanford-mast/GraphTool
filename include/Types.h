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

    /// Holds edge data, such as a weight, using multiple possible representations.
    union UEdgeData
    {
        uint64_t u;                                                         ///< Weight as an unsigned 64-bit integer.
        double d;                                                           ///< Weight as a double-precision floating-point value.
        
        inline UEdgeData& operator=(const uint64_t nu)
        {
            u = nu;
            return *this;
        }
        
        inline UEdgeData& operator=(const double nd)
        {
            d = nd;
            return *this;
        }
        
        inline operator uint64_t(void) const
        {
            return u;
        }
        
        inline operator double(void) const
        {
            return d;
        }
    };
    
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

    /// Represents an individual edge within an index.
    /// Contains both the other end of the edge and the edge data, such as a weight.
    struct SIndexedEdge
    {
        TVertexID otherVertex;                                          ///< Vertex identifier for the other end of the edge.
        UEdgeData edgeData;                                             ///< Edge data, such as a weight.
    };
}
