/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file EdgeList.cpp
 *   Implementation of a container for holding edges that correspond to a
 *   single top-level vertex, optimized for easy modification.
 *****************************************************************************/

#include "EdgeList.h"
#include "Types.h"


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "EdgeList.h" for documentation.
    
    template <typename TEdgeData> EdgeList<TEdgeData>::EdgeList(void) : edgeList(), degree(0), numVectors(0)
    {
        // Nothing to do here.
    }
    
    
    // -------- HELPERS ---------------------------------------------------- //
    // See "EdgeList.h" for documentation.

    template <typename TEdgeData> void EdgeList<TEdgeData>::FillEdgeInfoFromEdge(SIndexedEdge<TEdgeData>& edgeInfo, const SEdge<TEdgeData>& edge, bool useDestinationVertex)
    {
        edgeInfo.otherVertex = (useDestinationVertex ? edge.destinationVertex : edge.sourceVertex);
        edgeInfo.edgeData = edge.edgeData;
    }

    // --------

    template <> void EdgeList<void>::FillEdgeInfoFromEdge(SIndexedEdge<void>& edgeInfo, const SEdge<void>& edge, bool useDestinationVertex)
    {
        edgeInfo.otherVertex = (useDestinationVertex ? edge.destinationVertex : edge.sourceVertex);
    }

    
    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "EdgeList.h" for documentation.
    
    template <typename TEdgeData> void EdgeList<TEdgeData>::FillEdge(EdgeIterator& position, SEdge<TEdgeData>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const
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

    template <> void EdgeList<void>::FillEdge(EdgeIterator& position, SEdge<void>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const
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
    
    template <typename TEdgeData> void EdgeList<TEdgeData>::InsertEdgeUsingDestination(const SEdge<TEdgeData>& edge)
    {
        edgeList.emplace_back();
        FillEdgeInfoFromEdge(edgeList.back(), edge, true);
        
        degree += 1;
        
        if (1 == (degree & 3))
            numVectors += 1;
    }
    
    // --------
    
    template <typename TEdgeData> void EdgeList<TEdgeData>::InsertEdgeUsingSource(const SEdge<TEdgeData>& edge)
    {
        edgeList.emplace_back();
        FillEdgeInfoFromEdge(edgeList.back(), edge, false);

        degree += 1;

        if (1 == (degree & 3))
            numVectors += 1;
    }
    
    // --------
    
    template <typename TEdgeData> void EdgeList<TEdgeData>::RemoveEdge(const TVertexID otherVertex)
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
    
    template class EdgeList<void>;
    template class EdgeList<uint64_t>;
    template class EdgeList<double>;
}
