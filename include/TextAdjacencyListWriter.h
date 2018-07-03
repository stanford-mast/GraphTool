/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextAdjacencyListWriter.h
 *   Declaration of a graph writer for text adjacency list files.
 *****************************************************************************/

#pragma once

#include "GraphWriter.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    /// Writer object class for graphs represented in text adjacency list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class TextAdjacencyListWriter : public GraphWriter<TEdgeData>
    {
    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphWriter.h" for documentation.

        virtual FILE* OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph<TEdgeData>& graph, const bool groupedByDestination);
        virtual void WriteEdgesToFile(FILE* const graphfile, const Graph<TEdgeData>& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination);
    };
}
