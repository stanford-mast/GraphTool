/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file NullEdgeDataTransform.h
 *   Declaration of an edge data generator for null values.
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
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class NullEdgeDataTransform : public EdgeDataTransform<TEdgeData>
    {   
    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "EdgeDataTransform.h" for documentation.
        
        virtual TEdgeData GenerateEdgeData(const TVertexID sourceVertex, const TVertexID destinationVertex, const TEdgeData oldEdgeData);
    };
}
