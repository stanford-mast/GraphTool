/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file DynamicEdgeList.cpp
 *   Implementation of a container for holding edges that correspond to a
 *   single top-level vertex, optimized for fast insertion.
 *****************************************************************************/

#include "DynamicEdgeList.h"
#include "Types.h"


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "DynamicEdgeList.h" for documentation.
    
    template <typename TEdgeData> DynamicEdgeList<TEdgeData>::DynamicEdgeList(void) : edgeList(), degree(0), numVectors(0)
    {
        // Nothing to do here.
    }
    
    
    // -------- HELPERS ---------------------------------------------------- //
    // See "DynamicEdgeList.h" for documentation.

    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::FillEdgeInfoFromEdge(SEdgeInfo<TEdgeData>& edgeInfo, const SEdge<TEdgeData>& edge, bool useDestinationVertex)
    {
        edgeInfo.otherVertex = (useDestinationVertex ? edge.destinationVertex : edge.sourceVertex);
        edgeInfo.edgeData = edge.edgeData;
    }

    // --------

    template <> void DynamicEdgeList<void>::FillEdgeInfoFromEdge(SEdgeInfo<void>& edgeInfo, const SEdge<void>& edge, bool useDestinationVertex)
    {
        edgeInfo.otherVertex = (useDestinationVertex ? edge.destinationVertex : edge.sourceVertex);
    }

    
    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "DynamicEdgeList.h" for documentation.
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::FillEdge(EdgeIterator& position, SEdge<TEdgeData>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const
    {
        if (topLevelIsDestination)
        {
            edge.destinationVertex = topLevelVertex;
            edge.sourceVertex = position->otherVertex;
        }
        else
        {
            edge.destinationVertex = position->otherVertex;
            edge.sourceVertex = topLevelVertex;
        }

        edge.edgeData = position->edgeData;
    }

    // --------

    template <> void DynamicEdgeList<void>::FillEdge(EdgeIterator& position, SEdge<void>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const
    {
        if (topLevelIsDestination)
        {
            edge.destinationVertex = topLevelVertex;
            edge.sourceVertex = position->otherVertex;
        }
        else
        {
            edge.destinationVertex = position->otherVertex;
            edge.sourceVertex = topLevelVertex;
        }
    }

    // --------
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::InsertEdgeUsingDestination(const SEdge<TEdgeData>& edge)
    {
        edgeList.emplace_back();
        FillEdgeInfoFromEdge(edgeList.back(), edge, true);
        
        degree += 1;
        
        if (1 == (degree & 3))
            numVectors += 1;
    }
    
    // --------
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::InsertEdgeUsingSource(const SEdge<TEdgeData>& edge)
    {
        edgeList.emplace_back();
        FillEdgeInfoFromEdge(edgeList.back(), edge, false);

        degree += 1;

        if (1 == (degree & 3))
            numVectors += 1;
    }
    
    // --------
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::RemoveEdge(const TVertexID otherVertex)
    {
        for (auto it = edgeList.cbegin(); it != edgeList.cend(); ++it)
        {
            if (otherVertex == it->otherVertex)
            {
                auto prevIt = it;
                --prevIt;
                
                edgeList.erase(it);
                it = prevIt;
            }
        }
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //
    
    template class DynamicEdgeList<void>;
    template class DynamicEdgeList<uint64_t>;
    template class DynamicEdgeList<double>;
}
