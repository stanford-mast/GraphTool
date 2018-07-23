/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file HashEdgeDataTransform.cpp
 *   Implementation of an edge data generator that performs a multiplicative
 *   hash of source and destination vertex identifiers to compute the value.
 *****************************************************************************/

#include "HashEdgeDataTransform.h"
#include "Types.h"


namespace GraphTool
{
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "EdgeDataTransform.h" for documentation.
    
    uint64_t HashEdgeDataTransform::GenerateEdgeData(const TVertexID sourceVertex, const TVertexID destinationVertex, const uint64_t oldEdgeData)
    {
        // This method applies a variation of Knuth's multiplicative hash algorithm to source and destination vertex to determine the edge data value.
        const uint64_t weightBase = (destinationVertex << 2ull) + destinationVertex + sourceVertex;
        const uint64_t weightHashed = weightBase * 2654435761ull;
        const uint64_t weightAdjusted = (weightHashed >> 25ull) & 32767ull;
        const uint64_t weightFinal = weightAdjusted + 1ull;
        
        return weightFinal;
    }
}
