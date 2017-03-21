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

using namespace GraphTool;


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Built-in comparator for producing an ascending ordering of vertices.
/// @param [in] first First vertex to compare.
/// @param [in] second Second vertex to compare.
/// @return `true` if `first < second`, `false` otherwise.
static bool CompareVerticesAscending(TVertexID first, TVertexID second)
{
    return (first < second);
}

/// Built-in comparator for producing a descending ordering of vertices.
/// @param [in] first First vertex to compare.
/// @param [in] second Second vertex to compare.
/// @return `true` if `first > second`, `false` otherwise.
static bool CompareVerticesDescending(TVertexID first, TVertexID second)
{
    return (first > second);
}


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "Graph.h" for documentation.

Graph::Graph() {
    // Nothing special to do here.
}

// --------

Graph::~Graph() {
    // Nothing special to do here.
}


// -------- INSTANCE METHODS ----------------------------------------------- //
// See "Graph.h" for documentation.

void Graph::SortVerticesAscending(void)
{
    this->SortVertices(&CompareVerticesAscending);
}

// --------

void Graph::SortVerticesDescending(void)
{
    this->SortVertices(&CompareVerticesDescending);
}
