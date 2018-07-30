/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file BinaryEdgeListWriter.cpp
 *   Implementation of a graph writer for binary edge list files.
 *****************************************************************************/

#include "BinaryEdgeListWriter.h"
#include "Graph.h"
#include "Types.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


namespace GraphTool
{
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> FILE* BinaryEdgeListWriter<TEdgeData>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // This class writes files in binary mode.
        FILE* graphfile = fopen(filename, "wb");

        if (NULL != graphfile)
        {
            // Write out the number of vertices and edges in the graph.
            const TVertexCount metadata[2] = { graph.GetNumVertices(), graph.GetNumEdges() };
            fwrite((const void*)metadata, sizeof(metadata[0]), sizeof(metadata) / sizeof(metadata[0]), graphfile);
        }

        return graphfile;
    }

    // --------

    template <typename TEdgeData> void BinaryEdgeListWriter<TEdgeData>::WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination, const unsigned int currentPass)
    {
        fwrite((void*)buf, sizeof(const SEdge<TEdgeData>), count, graphfile);
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class BinaryEdgeListWriter<void>;
    template class BinaryEdgeListWriter<uint64_t>;
    template class BinaryEdgeListWriter<double>;
}
