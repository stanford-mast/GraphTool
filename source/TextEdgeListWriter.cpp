/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextEdgeListWriter.cpp
 *   Implementation of a graph writer for text edge list files.
 *****************************************************************************/

#include "Graph.h"
#include "GraphWriter.h"
#include "TextEdgeListWriter.h"
#include "Types.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


namespace GraphTool
{
    // -------- CLASS METHODS ---------------------------------------------- //
    // See "TextEdgeListWriter.h" for documentation.

    template <> int TextEdgeListWriter<void>::StringFromEdgeData(const SEdge<void>& edgeBuf, char* const edgeDataString, const size_t edgeDataStringCount)
    {
        return 0;
    }

    // --------

    template <> int TextEdgeListWriter<uint64_t>::StringFromEdgeData(const SEdge<uint64_t>& edgeBuf, char* const edgeDataString, const size_t edgeDataStringCount)
    {
        return snprintf(edgeDataString, edgeDataStringCount, "%llu", (long long unsigned int)edgeBuf.edgeData);
    }

    // --------

    template <> int TextEdgeListWriter<double>::StringFromEdgeData(const SEdge<double>& edgeBuf, char* const edgeDataString, const size_t edgeDataStringCount)
    {
        return snprintf(edgeDataString, edgeDataStringCount, "%.10lf", edgeBuf.edgeData);
    }


    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> FILE* TextEdgeListWriter<TEdgeData>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // This class writes files in text mode.
        FILE* graphfile = fopen(filename, "w");

        if (NULL != graphfile)
        {
            // Write out the number of vertices and edges in the graph.
            fprintf(graphfile, "%llu\n%llu\n", (long long unsigned int)graph.GetNumVertices(), (long long unsigned int)graph.GetNumEdges());
        }

        return graphfile;
    }

    // --------

    template <typename TEdgeData> void TextEdgeListWriter<TEdgeData>::WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination)
    {
        // Write out each edge.
        for (size_t i = 0; i < count; ++i)
        {
            // First, write the source and destination vertices.
            fprintf(graphfile, "%llu %llu", (long long unsigned int)buf[i].sourceVertex, (long long unsigned int)buf[i].destinationVertex);
        
            // Next, see if edge data are available for writing.
            char edgeDataString[128];
            const size_t edgeDataStringLength = StringFromEdgeData(buf[i], edgeDataString, sizeof(edgeDataString) / sizeof(edgeDataString[0]));
        
            if (0 < edgeDataStringLength)
                fprintf(graphfile, " %s", edgeDataString);
        
            // End the line.
            fprintf(graphfile, "\n");
        }
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class TextEdgeListWriter<void>;
    template class TextEdgeListWriter<uint64_t>;
    template class TextEdgeListWriter<double>;
}
