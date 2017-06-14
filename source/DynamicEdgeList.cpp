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
#include "Edge.hpp"
#include "Types.h"


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "DynamicEdgeList.h" for documentation.
    
    template <typename TEdgeData> DynamicEdgeList<TEdgeData>::DynamicEdgeList(void) : edgeList(), degree(0)
    {
        // Nothing to do here.
    }
    
    
    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "DynamicEdgeList.h" for documentation.
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::InsertEdge(const Edge<TEdgeData>& edge)
    {
        const uint64_t block = ((uint64_t)edge.vertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.vertex) & 31ull);
        const uint32_t mask = (1u << bit);
    
        if (edgeList[block].edges & mask)
            degree += 1;
        
        edgeList[block].edges |= mask;
        
        // TODO: insert the edge data properly
    }
    
    // --------
    
    template <> void DynamicEdgeList<void>::InsertEdge(const Edge<void>& edge)
    {
        const uint64_t block = ((uint64_t)edge.vertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.vertex) & 31ull);
        const uint32_t mask = (1u << bit);
    
        if (edgeList[block].edges & mask)
            degree += 1;
        
        edgeList[block].edges |= mask;
    }
    
    // --------
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::InsertEdgeBufferDestination(const SEdgeBufferData<TEdgeData>& edge)
    {
        const uint64_t block = ((uint64_t)edge.destinationVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.destinationVertex) & 31ull);
        const uint32_t mask = (1u << bit);
    
        if (edgeList[block].edges & mask)
            degree += 1;
        
        edgeList[block].edges |= mask;
    
        // TODO: insert the edge data properly
    }
    
    // --------
    
    template <> void DynamicEdgeList<void>::InsertEdgeBufferDestination(const SEdgeBufferData<void>& edge)
    {
        const uint64_t block = ((uint64_t)edge.destinationVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.destinationVertex) & 31ull);
        const uint32_t mask = (1u << bit);
        
        if (edgeList[block].edges & mask)
            degree += 1;

        edgeList[block].edges |= mask;
    }
    
    // --------
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::InsertEdgeBufferSource(const SEdgeBufferData<TEdgeData>& edge)
    {
        const uint64_t block = ((uint64_t)edge.sourceVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.sourceVertex) & 31ull);
        const uint32_t mask = (1u << bit);

        if (edgeList[block].edges & mask)
            degree += 1;
    
        edgeList[block].edges |= mask;
    
        // TODO: insert the edge data properly
    }
    
    // --------
    
    template <> void DynamicEdgeList<void>::InsertEdgeBufferSource(const SEdgeBufferData<void>& edge)
    {
        const uint64_t block = ((uint64_t)edge.sourceVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.sourceVertex) & 31ull);
        const uint32_t mask = (1u << bit);

        if (edgeList[block].edges & mask)
            degree += 1;
    
        edgeList[block].edges |= mask;
    }
    
    // --------
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::RemoveEdge(const TVertexID otherVertex)
    {
        const uint64_t block = ((uint64_t)otherVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)otherVertex) & 31ull);
        const uint32_t mask = (1u << bit);
    
        if (0 != edgeList.count(block))
        {
            if (edgeList[block].edges & mask)
                degree -= 1;

            edgeList[block].edges &= ~mask;
    
            if (0ull == edgeList[block].edges)
                edgeList.erase(block);
        }
    
        // TODO: remove the edge data properly
    }
    
    // --------
    
    template <> void DynamicEdgeList<void>::RemoveEdge(const TVertexID otherVertex)
    {
        const uint64_t block = ((uint64_t)otherVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)otherVertex) & 31ull);
        const uint32_t mask = (1u << bit);

        if (0 != edgeList.count(block))
        {
            if (edgeList[block].edges & mask)
                degree -= 1;
            
            edgeList[block].edges &= ~mask;
    
            if (0ull == edgeList[block].edges)
                edgeList.erase(block);
        }
    }
    
    
    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //
    
    template class DynamicEdgeList<void>;
    template class DynamicEdgeList<uint64_t>;
    template class DynamicEdgeList<double>;
}
