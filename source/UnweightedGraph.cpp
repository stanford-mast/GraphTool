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

#include "Graph.h"
#include "Types.h"
#include "UnweightedGraph.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>

using namespace GraphTool;


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "UnweightedGraph.h" for documentation.

UnweightedGraph::UnweightedGraph(void) : Graph(), edgesByDestination(), edgesBySource()
{
    // Nothing to do here.
}


// -------- CONCRETE INSTANCE METHODS -------------------------------------- //
// See "Graph.h" for documentation.

void UnweightedGraph::AddInEdges(void* buffer, size_t count)
{
    UnweightedGraph::SEdgeBufferData* edges = (UnweightedGraph::SEdgeBufferData*)buffer;
    
    for (uint64_t i = 0; i < count; ++i)
        this->edgesByDestination.InsertEdge(edges[i].destinationVertex, edges[i].sourceVertex);
}

// --------

void UnweightedGraph::AddOutEdges(void* buffer, size_t count)
{
    UnweightedGraph::SEdgeBufferData* edges = (UnweightedGraph::SEdgeBufferData*)buffer;

    for (uint64_t i = 0; i < count; ++i)
        this->edgesBySource.InsertEdge(edges[i].sourceVertex, edges[i].destinationVertex);
}

// --------

TEdgeCount UnweightedGraph::ExportVertexInEdges(void* buffer, TVertexID vertex, uint64_t options)
{
    TEdgeCount numEdgesWritten = 0;

    // TODO
    
    return numEdgesWritten;
}

// --------

TEdgeCount UnweightedGraph::ExportVertexOutEdges(void* buffer, TVertexID vertex, uint64_t options)
{
    TEdgeCount numEdgesWritten = 0;

    // TODO

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

TEdgeCount UnweightedGraph::GetMaximumIndegree(void)
{
    TVertexID highestDegreeVertex = 0;
    TEdgeCount highestDegree = 0;
    
    this->edgesByDestination.GetHighestDegreeInfo(highestDegreeVertex, highestDegree);
    
    return highestDegree;
}

// --------

TEdgeCount UnweightedGraph::GetMaximumOutdegree(void)
{
    TVertexID highestDegreeVertex = 0;
    TEdgeCount highestDegree = 0;

    this->edgesBySource.GetHighestDegreeInfo(highestDegreeVertex, highestDegree);

    return highestDegree;
}

// --------

TEdgeCount UnweightedGraph::GetNumEdges(void)
{
    return this->edgesByDestination.GetNumEdges();
}

// --------

TVertexCount UnweightedGraph::GetNumVertices(void)
{
    const TVertexCount numDestinationVertices = this->edgesByDestination.GetNumIndexedVertices();
    const TVertexCount numSourceVertices = this->edgesBySource.GetNumIndexedVertices();
    const TVertexCount numVertices = ((numDestinationVertices > numSourceVertices) ? numDestinationVertices : numSourceVertices);
    
    return numVertices;
}

TEdgeCount UnweightedGraph::GetVertexIndegree(TVertexID vertex)
{
    return this->edgesByDestination.GetVertexDegree(vertex);
}

// --------

TEdgeCount UnweightedGraph::GetVertexOutdegree(TVertexID vertex)
{
    return this->edgesBySource.GetVertexDegree(vertex);
}

// --------

void UnweightedGraph::RemoveEdge(TVertexID fromVertex, TVertexID toVertex)
{
    this->edgesByDestination.RemoveEdge(toVertex, fromVertex);
    this->edgesBySource.RemoveEdge(fromVertex, toVertex);
}

// --------

void UnweightedGraph::RemoveVertex(TVertexID vertex)
{
    this->edgesByDestination.RemoveVertex(vertex);
    this->edgesBySource.RemoveVertex(vertex);
}
