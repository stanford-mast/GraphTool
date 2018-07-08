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

    /// Enumerates all supported data types for edge data (i.e. edge weights).
    enum EEdgeDataType : int64_t
    {
        EdgeDataTypeVoid,                                                   ///< No edge data (i.e. an unweighted graph).
        EdgeDataTypeInteger,                                                ///< Edge data are integral. Currently this means 64-bit unsigned.
        EdgeDataTypeFloatingPoint                                           ///< Edge data are floating-point. Currently this means double-precision.
    };
    
    /// Holds edge data, such as a weight, using multiple possible representations.
    union UEdgeData
    {
        /// Edge data as an unsigned 64-bit integer.
        uint64_t u;
        
        /// Edge data as a double-precision floating-point value.
        double d;
        
        /// Allows direct assignment from a primitive type.
        /// Facilitates use of this type in templated operations.
        /// @param [in] nu New value to set, expressed as an unsigned 64-bit integer.
        /// @return Reference to this instance to support assignment chaining.
        inline UEdgeData& operator=(const uint64_t nu)
        {
            u = nu;
            return *this;
        }
        
        /// Allows direct assignment from a primitive type.
        /// Facilitates use of this type in templated operations.
        /// @param [in] nu New value to set, expressed as double-precision floating-point value.
        /// @return Reference to this instance to support assignment chaining.
        inline UEdgeData& operator=(const double nd)
        {
            d = nd;
            return *this;
        }
        
        /// Allows direct value extraction to a primitive type.
        /// Facilitates the use of this type in templated operations.
        /// @return Edge data as an unsigned 64-bit integer.
        inline operator uint64_t(void) const
        {
            return u;
        }
        
        /// Allows direct value extraction to a primitive type.
        /// Facilitates the use of this type in templated operations.
        /// @return Edge data as a double-precision floating-point value.
        inline operator double(void) const
        {
            return d;
        }
        
        /// Invalidates the value held in this instance, effectively removing the edge data from the associated edge.
        inline void Invalidate(void)
        {
            u = UINT64_MAX;
        }
        
        /// Specifies if the value held is a valid edge data value.
        /// If not, the edge associated with this instance has no data stored.
        /// @return `true` if so, `false` otherwise.
        inline bool IsValid(void) const
        {
            return (u != UINT64_MAX);
        }
    };

    /// Represents an individual edge within an index.
    /// Contains both the other end of the edge and the edge data, such as a weight.
    struct SIndexedEdge
    {
        TVertexID otherVertex;                                              ///< Vertex identifier for the other end of the edge.
        UEdgeData edgeData;                                                 ///< Edge data, such as a weight.
    };
    
    /// Represents an individual edge within a buffer.
    /// Fully specifies both source and destination, along with any edge data.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct SEdge
    {
        TVertexID sourceVertex;                                             ///< Identifier of the source vertex.
        TVertexID destinationVertex;                                        ///< Identifier of the destination vertex.
        TEdgeData edgeData;                                                 ///< Edge data, such as a weight.
    };

    /// Represents an individual edge within a buffer, specialized for unweighted edges.
    /// Fully specifies both source and destination.
    template <> struct SEdge<void>
    {
        TVertexID sourceVertex;                                             ///< Identifier of the source vertex.
        TVertexID destinationVertex;                                        ///< Identifier of the destination vertex.
    };
}
