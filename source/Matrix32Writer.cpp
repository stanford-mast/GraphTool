/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Matrix32Writer.cpp
 *   Implementation of a graph writer for binary matrix files.
 *   Edges and vertices are represented using 32-bit integers.
 *****************************************************************************/

#include "Matrix32Writer.h"
#include "Graph.h"
#include "Types.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


namespace GraphTool
{
    // -------- CLASS METHODS ------------------------------------------ //
    // See "Matrix32Writer.h" for documentation.
    
    template <typename TEdgeData> uint32_t Matrix32Writer<TEdgeData>::EdgeWeightFromEdge(const SEdge<TEdgeData>& edge)
    {
        return (uint32_t)edge.edgeData;
    }

    // --------

    template <> uint32_t Matrix32Writer<void>::EdgeWeightFromEdge(const SEdge<void>& edge)
    {
        return 1;
    }


    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> FILE* Matrix32Writer<TEdgeData>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // This class writes files in binary mode.
        FILE* graphfile = fopen(filename, "wb");

        if (NULL != graphfile)
        {
            // Write out the number of rows and columns (vertices) and non-zero elements (edges) in the matrix (graph).
            const uint32_t metadata[3] = { (uint32_t)graph.GetNumVertices(), (uint32_t)graph.GetNumVertices(), (uint32_t)graph.GetNumEdges() };
            fwrite((const void*)metadata, sizeof(metadata[0]), sizeof(metadata) / sizeof(metadata[0]), graphfile);
        }

        return graphfile;
    }

    // --------

    template <typename TEdgeData> void Matrix32Writer<TEdgeData>::WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination, const unsigned int currentPass)
    {
        // Write out each edge.
        // Note that matrix format is 1-based, not 0-based, hence the need to add 1 to all vertex identifiers.
        for (size_t i = 0; i < count; ++i)
        {
            const uint32_t edge[3] = { (uint32_t)buf[i].sourceVertex + 1, (uint32_t)buf[i].destinationVertex + 1, EdgeWeightFromEdge(buf[i]) };
            fwrite((const void*)edge, sizeof(edge[0]), sizeof(edge) / sizeof(edge[0]), graphfile);
        }
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class Matrix32Writer<void>;
    template class Matrix32Writer<uint64_t>;
    template class Matrix32Writer<double>;
}
