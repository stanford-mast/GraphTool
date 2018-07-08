/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file IGraphTransform.h
 *   Declaration of the interface to objects that transform graphs.
 *****************************************************************************/

#pragma once

#include "Types.h"


namespace GraphTool
{
    class Graph;

    
    /// Interface for objects that apply transformations to graphs.
    class IGraphTransform
    {
    public:
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Applies a transformation to the specified graph object.
        /// The exact transformation operation is subclass-specific.
        /// @param [in] graph Graph object to be transformed.
        /// @return Result of the transformation operation.
        virtual EGraphResult ApplyTransformation(Graph& graph) = 0;
    };
}
