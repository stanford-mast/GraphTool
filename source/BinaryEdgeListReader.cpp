/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file BinaryEdgeListReader.cpp
 *   Implementation of a graph reader that can interpret graphs stored as
 *   binary edge list files.
 *****************************************************************************/

#include "BinaryEdgeListReader.h"
#include "Graph.h"
#include "UnweightedGraph.h"

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>

using namespace GraphTool;


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "BinaryEdgeListReader.h" for documentation.

BinaryEdgeListReader::BinaryEdgeListReader() : GraphReader()
{
    // Nothing to do here.
}

// -------- CONCRETE INSTANCE METHODS -------------------------------------- //
// See "GraphReader.h" for documentation.

Graph* BinaryEdgeListReader::AllocateGraphObject(void)
{
    // This class reads to an unweighted graph.
    return new UnweightedGraph();
}

// --------

FILE* BinaryEdgeListReader::OpenAndInitializeGraphFile(const std::string& filename)
{
    // This class reads files in binary mode.
    FILE* graphfile = fopen(filename.c_str(), "rb");

    if (NULL != graphfile)
    {
        // Skip over the two values that indicate the number of edges and vertices in a graph.
        fseek(graphfile, 16, SEEK_SET);
    }

    return graphfile;
}

// --------

TEdgeCount BinaryEdgeListReader::ReadEdgesToBuffer(FILE* graphfile, void* buf, size_t size)
{
    const size_t edgeSize = sizeof(UnweightedGraph::SEdge);
    const size_t edgeCount = (size / sizeof(UnweightedGraph::SEdge));
    
    return (TEdgeCount)fread(buf, edgeSize, edgeCount, graphfile);
}
