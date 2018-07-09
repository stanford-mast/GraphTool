/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file NullEdgeDataTransform.cpp
 *   Implementation of an edge data value generator for null values.
 *****************************************************************************/

#include "NullEdgeDataTransform.h"
#include "Types.h"


namespace GraphTool
{
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "EdgeDataTransform.h" for documentation.
    
    template <typename TEdgeData> TEdgeData NullEdgeDataTransform<TEdgeData>::GenerateEdgeData(const TVertexID sourceVertex, const TVertexID destinationVertex, const TEdgeData oldEdgeData)
    {
        return (TEdgeData)0;
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class NullEdgeDataTransform<uint64_t>;
    template class NullEdgeDataTransform<double>;
}
