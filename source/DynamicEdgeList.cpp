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
    
    
    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "DynamicEdgeList.h" for documentation.
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::InsertEdgeBufferDestination(const SEdge<TEdgeData>& edge)
    {
        const uint64_t block = ((uint64_t)edge.destinationVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.destinationVertex) & 31ull);
        const uint32_t mask = (1u << bit);
    
        if (!(edgeList[block].edges & mask))
        {
            degree += 1;
            
            if (1 == (degree & 3))
                numVectors += 1;
        }
        
        edgeList[block].edges |= mask;
    
        // TODO: insert the edge data properly
    }
    
    // --------
    
    template <> void DynamicEdgeList<void>::InsertEdgeBufferDestination(const SEdge<void>& edge)
    {
        const uint64_t block = ((uint64_t)edge.destinationVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.destinationVertex) & 31ull);
        const uint32_t mask = (1u << bit);
        
        if (!(edgeList[block].edges & mask))
        {
            degree += 1;
            
            if (1 == (degree & 3))
                numVectors += 1;
        }

        edgeList[block].edges |= mask;
    }
    
    // --------
    
    template <typename TEdgeData> void DynamicEdgeList<TEdgeData>::InsertEdgeBufferSource(const SEdge<TEdgeData>& edge)
    {
        const uint64_t block = ((uint64_t)edge.sourceVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.sourceVertex) & 31ull);
        const uint32_t mask = (1u << bit);

        if (!(edgeList[block].edges & mask))
        {
            degree += 1;
            
            if (1 == (degree & 3))
                numVectors += 1;
        }
    
        edgeList[block].edges |= mask;
    
        // TODO: insert the edge data properly
    }
    
    // --------
    
    template <> void DynamicEdgeList<void>::InsertEdgeBufferSource(const SEdge<void>& edge)
    {
        const uint64_t block = ((uint64_t)edge.sourceVertex) >> 5ull;
        const uint8_t bit = (uint8_t)(((uint64_t)edge.sourceVertex) & 31ull);
        const uint32_t mask = (1u << bit);

        if (!(edgeList[block].edges & mask))
        {
            degree += 1;
            
            if (1 == (degree & 3))
                numVectors += 1;
        }
    
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
            {
                degree -= 1;
                
                if (0 == (degree & 3))
                    numVectors -= 1;
            }

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
            {
                degree -= 1;
                
                if (0 == (degree & 3))
                    numVectors -= 1;
            }
            
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
