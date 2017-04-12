/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file BinaryEdgeListWriter.hpp
 *   Implementation of an object capable of writing graphs to files in
 *   binary edge list format.
 *****************************************************************************/

#pragma once

#include "Edge.hpp"
#include "GraphWriter.hpp"
#include "Types.h"

#include <cstdio>
#include <cstdlib>
#include <string>


namespace GraphTool
{
    /// Writer object class for graphs represented in text edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class BinaryEdgeListWriter : public GraphWriter<TEdgeData>
    {
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphWriter.hpp" for documentation.

        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename, Graph<TEdgeData>& graph, bool groupedByDestination)
        {
            // This class writes files in binary mode.
            FILE* graphfile = fopen(filename.c_str(), "wb");

            if (NULL != graphfile)
            {
                // Write out the number of vertices and edges in the graph.
                TVertexCount metadata[2] = { graph.GetNumVertices(), graph.GetNumEdges() };
                fwrite((void*)metadata, sizeof(metadata[0]), sizeof(metadata) / sizeof(metadata[0]), graphfile);
            }

            return graphfile;
        }

        // --------

        virtual void WriteEdgesToFile(FILE* graphfile, const Graph<TEdgeData>& graph, const SEdgeBufferData<TEdgeData>* buf, size_t count, bool groupedByDestination)
        {
            fwrite((void*)buf, sizeof(const SEdgeBufferData<TEdgeData>), count, graphfile);
        }
    };

    /// Convenience alias for writing graphs that do not use edge data or weights.
    typedef BinaryEdgeListWriter<void> UnweightedBinaryEdgeListWriter;
}
