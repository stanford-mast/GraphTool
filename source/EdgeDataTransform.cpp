/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file EdgeDataTransform.cpp
 *   Implementation of common functionality for transformations that generate
 *   edge data values.
 *****************************************************************************/

#include "EdgeList.h"
#include "EdgeDataTransform.h"
#include "Graph.h"
#include "Types.h"
#include "VertexIndex.h"

#include <cstddef>
#include <cstdint>
#include <parutil.h>
#include <spindle.h>


namespace GraphTool
{
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphTransform.h" for documentation.
    
    template <typename TEdgeData> EGraphResult EdgeDataTransform<TEdgeData>::TransformGraph(Graph& graph)
    {
        // Dynamically schedule work on the basis of vertices.
        void* scheduler = NULL;
        const TVertexID numUnits = (TVertexID)graph.GetNumVertices();
        const TVertexID firstUnit = (TVertexID)parutilSchedulerDynamicInit(numUnits, &scheduler);
        
        if (NULL == scheduler)
            return EGraphResult::GraphResultErrorUnknown;
        
        // Each unit of work involves setting edge weight values for both the source-grouped and destination-grouped edge lists.
        for (TVertexID vertex = firstUnit; vertex < numUnits; vertex = (TVertexID)parutilSchedulerDynamicGetWork(scheduler))
        {
            if (0 != graph.GetVertexIndegree(vertex))
            {
                const Graph::WritableVertexIterator vertexIterator = graph.VertexIteratorDestinationAtWritable(vertex);
                
                for (auto edgeIterator = (*vertexIterator)->BeginIteratorWritable(); edgeIterator != (*vertexIterator)->EndIteratorWritable(); ++edgeIterator)
                {
                    edgeIterator->edgeData = GenerateEdgeData(edgeIterator->otherVertex, vertex, edgeIterator->edgeData);
                }
            }
            
            if (0 != graph.GetVertexOutdegree(vertex))
            {
                const Graph::WritableVertexIterator vertexIterator = graph.VertexIteratorSourceAtWritable(vertex);
                
                for (auto edgeIterator = (*vertexIterator)->BeginIteratorWritable(); edgeIterator != (*vertexIterator)->EndIteratorWritable(); ++edgeIterator)
                {
                    edgeIterator->edgeData = GenerateEdgeData(vertex, edgeIterator->otherVertex, edgeIterator->edgeData);
                }
            }
        }
        
        parutilSchedulerDynamicExit(scheduler);
        
        if (0 == spindleGetLocalThreadID())
            graph.SetEdgeDataType<TEdgeData>();
        
        return EGraphResult::GraphResultSuccess;
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class EdgeDataTransform<uint64_t>;
    template class EdgeDataTransform<double>;
}
