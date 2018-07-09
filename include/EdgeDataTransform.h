/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file EdgeDataTransform.h
 *   Declaration of an abstract graph transformation that generates values for
 *   edge data elements.
 *****************************************************************************/

#pragma once

#include "GraphTransform.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    class Graph;

    
    /// Abstract transformation class for generating edge data values.
    /// Schedules parallel work but leaves actual value generation to subclasses.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class EdgeDataTransform : public GraphTransform
    {   
    public:
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //
        
        /// Generates edge data given the source and destination vertex identifiers as well as the old edge data value.
        /// @param [in] sourceVertex Source vertex identifier.
        /// @param [in] destinationVertex Destination vertex identifier.
        /// @param [in] oldEdgeData Edge data value for the specified edge prior to the transformation.
        virtual TEdgeData GenerateEdgeData(const TVertexID sourceVertex, const TVertexID destinationVertex, const TEdgeData oldEdgeData) = 0;
        
        
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphTransform.h" for documentation.
        
        virtual EGraphResult TransformGraph(Graph& graph);
    };
}
