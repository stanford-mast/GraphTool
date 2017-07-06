/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file VectorSparseElement.h
 *   Declaration of the functionality required to represent and manipulate
 *   individual vectors in the Vector-Sparse format.
 *****************************************************************************/

#pragma once

#include "PlatformFunctions.h"
#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <immintrin.h>


namespace GraphTool
{
    /// Represents an individual edge vector, containing graph topology information as well as edge data.
    template <typename TEdgeData> struct VectorSparseElement
    {
        /// Graph topology information.
        __m256i topology;

        /// Edge data, represented as a packed 4-element vector.
        __m256i data;

        /// Fills the contents of this Vector-Sparse element from the supplied edge information structure.
        /// @param [in] sharedVertex Identifier of the top-level vertex common to all edge information structures.
        /// @param [in] edges Array of four edge information structures.
        /// @param [in] count Number of valid elements in the array, which should be in the range 1 to 4.
        inline void FillFromIndexedEdges(const TVertexID sharedVertex, const SIndexedEdge<TEdgeData>* edges, const uint8_t count)
        {
            // Compose the shared vertex ID encoding by splitting it into pieces.
            const uint64_t sharedVertexPieces[4] = {
                (sharedVertex & 0x0000000000007fffull) >> 0,    // bits 14:0
                (sharedVertex & 0x000000003fff8000ull) >> 15,   // bits 29:15
                (sharedVertex & 0x00001fffc0000000ull) >> 30,   // bits 44:30
                (sharedVertex & 0x0000e00000000000ull) >> 45    // bits 47:45
            };

            // Fill in the topology information.
            topology = _mm256_set_epi64x(
                ((count > 3 ? (1ull << 63) : 0ull) | (sharedVertexPieces[3] << 48) | edges[3].otherVertex),
                ((count > 2 ? (1ull << 63) : 0ull) | (sharedVertexPieces[2] << 48) | edges[2].otherVertex),
                ((count > 1 ? (1ull << 63) : 0ull) | (sharedVertexPieces[1] << 48) | edges[1].otherVertex),
                ((count > 0 ? (1ull << 63) : 0ull) | (sharedVertexPieces[0] << 48) | edges[0].otherVertex)
            );

            // Fill in the edge data.
            data = _mm256_set_epi64x(
                (count > 3 ? (*(reinterpret_cast<const uint64_t*>(&edges[3].edgeData))) : 0ull),
                (count > 2 ? (*(reinterpret_cast<const uint64_t*>(&edges[2].edgeData))) : 0ull),
                (count > 1 ? (*(reinterpret_cast<const uint64_t*>(&edges[1].edgeData))) : 0ull),
                (count > 0 ? (*(reinterpret_cast<const uint64_t*>(&edges[0].edgeData))) : 0ull)
            );
        }
        
        /// Extracts the shared vertex ID from the topology data.
        /// @return Shared vertex ID.
        inline TVertexID GetSharedVertexID(void) const
        {
            return (
                ((_mm256_extract_epi64(topology, 0) & 0x7fff000000000000ull) >> 48) |
                ((_mm256_extract_epi64(topology, 1) & 0x7fff000000000000ull) >> 33) |
                ((_mm256_extract_epi64(topology, 2) & 0x7fff000000000000ull) >> 18) |
                ((_mm256_extract_epi64(topology, 3) & 0x0007000000000000ull) >> 3)
            );
        }

        /// Extracts the 12-bit variable field.
        /// @return Value of the 12-bit variable field.
        inline uint16_t GetVariableField(void) const
        {
            return (uint16_t)((_mm256_extract_epi64(topology, 3) & 0x7ff8000000000000ull) >> 51);
        }

        /// Sets the value of the 12-bit variable field.
        /// @param [in] value Value to set.
        inline void SetVariableField(const uint16_t value)
        {
            _mm256_insert_epi64(topology, (_mm256_extract_epi64(topology, 3) & 0x8007ffffffffffffull) | ((((uint64_t)value) & 0x0000000000000fffull) << 51), 3);
        }
    };

    /// Represents an individual edge vector, containing only graph topology information and specialized for unweighted graphs.
    template <> struct VectorSparseElement<void>
    {
        /// Graph topology information.
        __m256i topology;
        
        /// Fills the contents of this Vector-Sparse element from the supplied edge information structure.
        /// @param [in] sharedVertex Identifier of the top-level vertex common to all edge information structures.
        /// @param [in] edgeInfo Array of four edge information structures.
        /// @param [in] count Number of valid elements in the array, which should be in the range 1 to 4.
        inline void FillFromIndexedEdges(const TVertexID sharedVertex, const SIndexedEdge<void>* edges, const uint8_t count)
        {
            // Compose the shared vertex ID encoding by splitting it into pieces.
            const uint64_t sharedVertexPieces[4] = {
                (sharedVertex & 0x0000000000007fffull) >> 0,    // bits 14:0
                (sharedVertex & 0x000000003fff8000ull) >> 15,   // bits 29:15
                (sharedVertex & 0x00001fffc0000000ull) >> 30,   // bits 44:30
                (sharedVertex & 0x0000e00000000000ull) >> 45    // bits 47:45
            };

            // Fill in the topology information.
            topology = _mm256_set_epi64x(
                ((count > 3 ? (1ull << 63) : 0ull) | (sharedVertexPieces[3] << 48) | edges[3].otherVertex),
                ((count > 2 ? (1ull << 63) : 0ull) | (sharedVertexPieces[2] << 48) | edges[2].otherVertex),
                ((count > 1 ? (1ull << 63) : 0ull) | (sharedVertexPieces[1] << 48) | edges[1].otherVertex),
                ((count > 0 ? (1ull << 63) : 0ull) | (sharedVertexPieces[0] << 48) | edges[0].otherVertex)
            );
        }
        
        /// Extracts the shared vertex ID from the topology data.
        /// @return Shared vertex ID.
        inline TVertexID GetSharedVertexID(void) const
        {
            return (
                ((_mm256_extract_epi64(topology, 0) & 0x7fff000000000000ull) >> 48) |
                ((_mm256_extract_epi64(topology, 1) & 0x7fff000000000000ull) >> 33) |
                ((_mm256_extract_epi64(topology, 2) & 0x7fff000000000000ull) >> 18) |
                ((_mm256_extract_epi64(topology, 3) & 0x0007000000000000ull) >> 3)
            );
        }

        /// Extracts the 12-bit variable field.
        /// @return Value of the 12-bit variable field.
        inline uint16_t GetVariableField(void) const
        {
            return (uint16_t)((_mm256_extract_epi64(topology, 3) & 0x7ff8000000000000ull) >> 51);
        }

        /// Sets the value of the 12-bit variable field.
        /// @param [in] value Value to set.
        inline void SetVariableField(const uint16_t value)
        {
            _mm256_insert_epi64(topology, (_mm256_extract_epi64(topology, 3) & 0x8007ffffffffffffull) | ((((uint64_t)value) & 0x0000000000000fffull) << 51), 3);
        }
    };
}
