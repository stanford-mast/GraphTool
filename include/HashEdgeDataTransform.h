/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file HashEdgeDataTransform.h
 *   Declaration of an edge data generator that performs a multiplicative
 *   hash of source and destination vertex identifiers to compute the value.
 *****************************************************************************/

#pragma once

#include "EdgeDataTransform.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    class Graph;

    
    /// Transformation class for applying null values to edge data elements.
    class HashEdgeDataTransform : public EdgeDataTransform<uint64_t>
    {   
    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "EdgeDataTransform.h" for documentation.
        
        virtual uint64_t GenerateEdgeData(const TVertexID sourceVertex, const TVertexID destinationVertex, const uint64_t oldEdgeData);
    };
}
