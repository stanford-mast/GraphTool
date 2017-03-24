/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file UnweightedGraph.cpp
 *   Implementation of functionality needed to represent graphs without edge
 *   weights.
 *****************************************************************************/

#include "UnweightedGraph.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>

using namespace GraphTool;


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "UnweightedGraph.h" for documentation.

UnweightedGraph::UnweightedGraph(void) : Graph(), verticesByDestination(), verticesBySource()
{
    // Nothing to do here.
}


// -------- CONCRETE INSTANCE METHODS -------------------------------------- //
// See "Graph.h" for documentation.

void UnweightedGraph::AddInEdges(void* buffer, size_t count)
{
    UnweightedGraph::SEdge* edges = (UnweightedGraph::SEdge*)buffer;

    for (uint64_t i = 0; i < count; ++i)
        this->verticesByDestination[edges[i].destinationVertex].insert(edges[i].sourceVertex);
}

// --------

void UnweightedGraph::AddOutEdges(void* buffer, size_t count)
{
    UnweightedGraph::SEdge* edges = (UnweightedGraph::SEdge*)buffer;

    for (uint64_t i = 0; i < count; ++i)
        this->verticesBySource[edges[i].sourceVertex].insert(edges[i].destinationVertex);
}

// --------

TEdgeCount UnweightedGraph::ExportVertexInEdges(void* buffer, TVertexID vertex, uint64_t options)
{
    TEdgeCount numEdgesWritten = 0;

    if (0 < this->verticesByDestination.count(vertex))
    {
        std::set<TVertexID>& inedges = this->verticesByDestination.at(vertex);
        TVertexID* ptr = (TVertexID*)buffer;

        for (auto it = inedges.cbegin(); it != inedges.cend(); ++it)
        {
            numEdgesWritten += 1;

            // Write out the source vertex ID, unless it is to be skipped.
            // Once done, advance the buffer pointer to the next element.
            if (!(options & UnweightedGraph::kExportOptionsExcludeSource))
            {
                *ptr = *it;
                ptr = &ptr[1];
            }

            // Same as above for the destination vertex ID.
            if (!(options & UnweightedGraph::kExportOptionsExcludeDestination))
            {
                *ptr = vertex;
                ptr = &ptr[1];
            }
        }
    }
    
    return numEdgesWritten;
}

// --------

TEdgeCount UnweightedGraph::ExportVertexOutEdges(void* buffer, TVertexID vertex, uint64_t options)
{
    TEdgeCount numEdgesWritten = 0;

    if (0 < this->verticesBySource.count(vertex))
    {
        std::set<TVertexID>& outedges = this->verticesBySource.at(vertex);
        TVertexID* ptr = (TVertexID*)buffer;

        for (auto it = outedges.cbegin(); it != outedges.cend(); ++it)
        {
            numEdgesWritten += 1;

            // Write out the source vertex ID, unless it is to be skipped.
            // Once done, advance the buffer pointer to the next element.
            if (!(options & UnweightedGraph::kExportOptionsExcludeSource))
            {
                *ptr = vertex;
                ptr = &ptr[1];
            }

            // Same as above for the destination vertex ID.
            if (!(options & UnweightedGraph::kExportOptionsExcludeDestination))
            {
                *ptr = *it;
                ptr = &ptr[1];
            }
        }
    }

    return numEdgesWritten;
}

// --------

size_t UnweightedGraph::GetExportEdgeSizeWithOptions(uint64_t options)
{
    size_t sourceMultiplier = ((options & UnweightedGraph::kExportOptionsExcludeSource) ? 0 : 1);
    size_t destinationMultiplier = ((options & UnweightedGraph::kExportOptionsExcludeDestination) ? 0 : 1);
    
    return ((sourceMultiplier * sizeof(TVertexID)) + (destinationMultiplier * sizeof(TVertexID)));
}

// --------

TVertexCount UnweightedGraph::GetMaximumIndegree(void)
{
    TVertexCount indegreeMax = 0;
    
    for (auto it = verticesByDestination.cbegin(); it != verticesByDestination.cend(); ++it)
    {
        TVertexCount indegreeCurr = it->second.size();

        if (indegreeCurr > indegreeMax)
            indegreeMax = indegreeCurr;
    }
    
    return indegreeMax;
}

// --------

TVertexCount UnweightedGraph::GetMaximumOutdegree(void)
{
    TVertexCount outdegreeMax = 0;

    for (auto it = verticesBySource.cbegin(); it != verticesBySource.cend(); ++it)
    {
        TVertexCount outdegreeCurr = it->second.size();

        if (outdegreeCurr > outdegreeMax)
            outdegreeMax = outdegreeCurr;
    }

    return outdegreeMax;
}

// --------

TEdgeCount UnweightedGraph::GetNumEdges(void)
{
    TEdgeCount numEdges = 0;

    for (auto it = verticesByDestination.cbegin(); it != verticesByDestination.cend(); ++it)
        numEdges += it->second.size();

    return numEdges;
}

// --------

TVertexCount UnweightedGraph::GetNumVertices(void)
{
    return this->verticesByDestination.size();
}

TVertexCount UnweightedGraph::GetVertexIndegree(TVertexID vertex)
{
    TVertexCount indegree = 0;
    
    if (0 < this->verticesByDestination.count(vertex))
        indegree = this->verticesByDestination.at(vertex).size();
    
    return indegree;
}

// --------

TVertexCount UnweightedGraph::GetVertexOutdegree(TVertexID vertex)
{
    TVertexCount outdegree = 0;

    if (0 < this->verticesBySource.count(vertex))
        outdegree = this->verticesBySource.at(vertex).size();

    return outdegree;
}

// --------

void UnweightedGraph::RemoveEdge(TVertexID fromVertex, TVertexID toVertex)
{
    // Remove the edge from the source vertex data structure.
    if (0 < this->verticesBySource.count(fromVertex))
    {
        this->verticesBySource.at(fromVertex).erase(toVertex);

        // If the source vertex now has no out-edges, remove it from the index as well.
        if (0 == this->verticesBySource.at(fromVertex).size())
            this->verticesBySource.erase(fromVertex);
    }

    // Remove the edge from the destination vertex data structure.
    if (0 < this->verticesByDestination.count(toVertex))
    {
        this->verticesByDestination.at(toVertex).erase(fromVertex);

        // If the destination vertex now has no in-edges, remove it from the index as well.
        if (0 == this->verticesByDestination.at(toVertex).size())
            this->verticesByDestination.erase(toVertex);
    }
}

// --------

void UnweightedGraph::RemoveVertex(TVertexID vertex)
{
    // Remove the vertex from the source vertex data structure.
    this->verticesBySource.erase(vertex);
    for (auto it = this->verticesBySource.begin(); it != this->verticesBySource.end(); ++it)
        it->second.erase(vertex);
    
    // Remove the vertex from the destination vertex data structure.
    this->verticesByDestination.erase(vertex);
    for (auto it = this->verticesByDestination.begin(); it != this->verticesByDestination.end(); ++it)
        it->second.erase(vertex);
}
