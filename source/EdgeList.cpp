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
    
    EdgeList::EdgeList(void) : edgeList(), degree(0), numVectors(0)
    {
        // Nothing to do here.
    }
    
    
    // -------- HELPERS ---------------------------------------------------- //
    // See "EdgeList.h" for documentation.

    template <typename TEdgeData> void EdgeList::FillEdgeInfoFromEdge(SIndexedEdge& edgeInfo, const SEdge<TEdgeData>& edge, bool useDestinationVertex)
    {
        edgeInfo.otherVertex = (useDestinationVertex ? edge.destinationVertex : edge.sourceVertex);
        edgeInfo.edgeData = edge.edgeData;
    }

    // --------

    template <> void EdgeList::FillEdgeInfoFromEdge(SIndexedEdge& edgeInfo, const SEdge<void>& edge, bool useDestinationVertex)
    {
        edgeInfo.otherVertex = (useDestinationVertex ? edge.destinationVertex : edge.sourceVertex);
        edgeInfo.edgeData.Invalidate();
    }

    
    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "EdgeList.h" for documentation.
    
    template <typename TEdgeData> void EdgeList::FillEdge(EdgeIterator& position, SEdge<TEdgeData>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const
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

    template <> void EdgeList::FillEdge(EdgeIterator& position, SEdge<void>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const
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
    
    template <typename TEdgeData> void EdgeList::InsertEdgeUsingDestination(const SEdge<TEdgeData>& edge)
    {
        edgeList.emplace_back();
        FillEdgeInfoFromEdge(edgeList.back(), edge, true);
        
        degree += 1;
        
        if (1 == (degree & 3))
            numVectors += 1;
    }
    
    // --------
    
    template <typename TEdgeData> void EdgeList::InsertEdgeUsingSource(const SEdge<TEdgeData>& edge)
    {
        edgeList.emplace_back();
        FillEdgeInfoFromEdge(edgeList.back(), edge, false);

        degree += 1;

        if (1 == (degree & 3))
            numVectors += 1;
    }
    
    // --------
    
    void EdgeList::RemoveEdge(const TVertexID otherVertex)
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
    
    template void EdgeList::FillEdgeInfoFromEdge(SIndexedEdge& edgeInfo, const SEdge<void>& edge, bool useDestinationVertex);
    template void EdgeList::FillEdgeInfoFromEdge(SIndexedEdge& edgeInfo, const SEdge<uint64_t>& edge, bool useDestinationVertex);
    template void EdgeList::FillEdgeInfoFromEdge(SIndexedEdge& edgeInfo, const SEdge<double>& edge, bool useDestinationVertex);
    
    template void EdgeList::FillEdge(EdgeIterator& position, SEdge<void>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const;
    template void EdgeList::FillEdge(EdgeIterator& position, SEdge<uint64_t>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const;
    template void EdgeList::FillEdge(EdgeIterator& position, SEdge<double>& edge, TVertexID topLevelVertex, bool topLevelIsDestination) const;
    
    template void EdgeList::InsertEdgeUsingDestination(const SEdge<void>& edge);
    template void EdgeList::InsertEdgeUsingDestination(const SEdge<uint64_t>& edge);
    template void EdgeList::InsertEdgeUsingDestination(const SEdge<double>& edge);
    
    template void EdgeList::InsertEdgeUsingSource(const SEdge<void>& edge);
    template void EdgeList::InsertEdgeUsingSource(const SEdge<uint64_t>& edge);
    template void EdgeList::InsertEdgeUsingSource(const SEdge<double>& edge);
}
