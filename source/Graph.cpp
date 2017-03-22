/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Graph.cpp
 *   Implementation of common graph representation functionality.
 *****************************************************************************/

#include "Graph.h"

#include <cstddef>
#include <cstdint>

using namespace GraphTool;


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "Graph.h" for documentation.

Graph::Graph() {
    // Nothing to do here.
}

// --------

Graph::~Graph() {
    // Nothing to do here.
}


// -------- INSTANCE METHODS ----------------------------------------------- //
// See "Graph.h" for documentation.

void Graph::AddEdges(void* buffer, size_t count)
{
    this->AddInEdges(buffer, count);
    this->AddOutEdges(buffer, count);
}

// --------

size_t Graph::GetExportRecommendedInEdgeBufferSizeWithOptions(uint64_t options)
{
    return (this->GetMaximumIndegree() * this->GetExportEdgeSizeWithOptions(options));
}

// --------

size_t Graph::GetExportRecommendedOutEdgeBufferSizeWithOptions(uint64_t options)
{
    return (this->GetMaximumOutdegree() * this->GetExportEdgeSizeWithOptions(options));
}
