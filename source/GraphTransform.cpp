/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphTransform.cpp
 *   Implementation of common functionality for transforming graphs.
 *****************************************************************************/

#include "Graph.h"
#include "GraphTransform.h"
#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <silo.h>
#include <spindle.h>
#include <topo.h>


namespace GraphTool
{
    // -------- TYPE DEFINITIONS ------------------------------------------- //

    /// Provides all information needed to specify a graph transformation operation.
    struct SGraphTransformSpec
    {
        Graph* graph;                                                       ///< Graph object to be transformed.
        GraphTransform* transform;                                          ///< Graph transformation object.
        EGraphResult* transformResult;                                      ///< Buffer containing an array of transformation results.
    };


    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "GraphTransform.h" for documentation.

    GraphTransform::GraphTransform(void)
    {
        // Nothing to do here.
    }

    // --------

    GraphTransform::~GraphTransform(void)
    {
        // Nothing to do here.
    }


    // -------- CLASS METHODS ---------------------------------------------- //
    // See "GraphTransform.h" for documentation.

    void GraphTransform::ParallelTransformFunc(void* arg)
    {
        SGraphTransformSpec* const transformSpec = (SGraphTransformSpec*)arg;
        transformSpec->transformResult[spindleGetLocalThreadID()] = transformSpec->transform->TransformGraph(*(transformSpec->graph));
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "GraphTransform.h" for documentation.
    
    bool GraphTransform::CanParallelizeTransformation(void)
    {
        return true;
    }
    

    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphTransform.h" for documentation.

    EGraphResult GraphTransform::ApplyTransformation(Graph& graph)
    {
        if (CanParallelizeTransformation())
        {
            const uint32_t numaNode = (uint32_t)siloGetNUMANodeForVirtualAddress((void*)&graph);
            const uint32_t numThreads = topoGetNUMANodeLogicalCoreCount(numaNode);
            
            if ((numaNode > topoGetSystemNUMANodeCount()) || (0 == numThreads))
                return EGraphResult::GraphResultErrorUnknown;
            
            // Define the graph transformation task.
            SGraphTransformSpec transformSpec;
            transformSpec.graph = &graph;
            transformSpec.transform = this;
            transformSpec.transformResult = new EGraphResult[numThreads];
            
            if (NULL == transformSpec.transformResult)
                return EGraphResult::GraphResultErrorNoMemory;
            
            for (uint32_t i = 0; i < numThreads; ++i)
                transformSpec.transformResult[i] = EGraphResult::GraphResultSuccess;
            
            // Define the parallelization strategy.
            SSpindleTaskSpec taskSpec;
            taskSpec.func = &ParallelTransformFunc;
            taskSpec.arg = (void*)&transformSpec;
            taskSpec.numaNode = numaNode;
            taskSpec.numThreads = numThreads;
            taskSpec.smtPolicy = SpindleSMTPolicyPreferLogical;
            
            // Launch the graph transformation task.
            const uint32_t spawnResult = spindleThreadsSpawn(&taskSpec, 1, true);
            
            // Collect the results.
            EGraphResult result = EGraphResult::GraphResultSuccess;
            
            for (uint32_t i = 0; i < numThreads; ++i)
            {
                if (EGraphResult::GraphResultSuccess != transformSpec.transformResult[i])
                {
                    result = transformSpec.transformResult[i];
                    break;
                }
            }
            
            // Clean up.
            delete[] transformSpec.transformResult;
            
            if (0 != spawnResult)
                return EGraphResult::GraphResultErrorUnknown;
            else
                return result;
        }
        else
        {
            return TransformGraph(graph);
        }
    }
}
