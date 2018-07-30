/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Matrix32Writer.h
 *   Declaration of a graph writer for binary matrix files.
 *   Edges and vertices are represented using 32-bit integers.
 *****************************************************************************/

#pragma once

#include "GraphWriter.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    /// Writer object class for graphs represented in binary matrix format with 32-bit integers for all values.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class Matrix32Writer : public GraphWriter<TEdgeData>
    {
        // -------- CLASS METHODS ------------------------------------------ //

        // Formats the edge weight from the specified edge as a 32-bit unsigned integer.
        static uint32_t EdgeWeightFromEdge(const SEdge<TEdgeData>& edge);

        
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphWriter.h" for documentation.

        virtual FILE* OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination);
        virtual void WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination, const unsigned int currentPass);
    };
}
