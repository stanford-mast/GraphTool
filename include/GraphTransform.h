/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphTransform.h
 *   Declaration of the common functionality for objects capable of applying
 *   transformations to graphs.
 *****************************************************************************/

#pragma once

#include "IGraphTransform.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    class Graph;

    
    /// Base class for all object types that are used to apply transformations to graphs.
    /// Some common functionality is implemented directly in the base class.
    class GraphTransform : public IGraphTransform
    {   
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor.
        GraphTransform(void);
        
        /// Destructor.
        virtual ~GraphTransform(void);
        
        
    private:
        // -------- CLASS METHODS ------------------------------------------ //

        /// Spindle entry point for parallelizable graph transformations.
        /// @param [in] Pointer to an SGraphTransformSpec object used for internal book-keeping.
        static void ParallelTransformFunc(void* arg);
        
        
        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Specifies if the particular transformation can be parallelized using Spindle.
        /// This is a transformation-specific decision, and the default implementation returns `true`.
        /// @return `true` if the transformation operation can be parallelized, `false` otherwise.
        virtual bool CanParallelizeTransformation(void);
        
        
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //
        
        /// Performs the actual transformation operation.
        /// If it can be parallelized, this method is invoked by multiple threads in a Spindle parallel region.
        /// Otherwise it is invoked by only a single thread.
        virtual EGraphResult TransformGraph(Graph& graph) = 0;
        
        
    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "IGraphTransform.h" for documentation.
        
        virtual EGraphResult ApplyTransformation(Graph& graph);
    };
}
