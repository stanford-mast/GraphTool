/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextAdjacencyListWriter.cpp
 *   Implementation of a graph writer for text adjacency list files.
 *****************************************************************************/

#include "Graph.h"
#include "GraphWriter.h"
#include "TextAdjacencyListWriter.h"
#include "Types.h"
#include "VertexIndex.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


namespace GraphTool
{
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> FILE* TextAdjacencyListWriter<TEdgeData>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // This class writes files in text mode.
        FILE* graphfile = fopen(filename, "w");

        if (NULL != graphfile)
        {
            // Write out the number of vertices and edges in the graph.
            fprintf(graphfile, "AdjacencyGraph\n%llu\n%llu\n", (long long unsigned int)graph.GetNumVertices(), (long long unsigned int)graph.GetNumEdges());
            
            // Write out the vertex index.
            const VertexIndex& vertexIndex = (groupedByDestination ? graph.VertexIndexDestination() : graph.VertexIndexSource());
            const TVertexCount numVertices = vertexIndex.GetNumVertices();
            TVertexCount currentDegree = 0;
            
            for (TVertexCount i = 0; i < numVertices; ++i)
            {
                fprintf(graphfile, "%llu\n", (long long unsigned int)currentDegree);
                currentDegree += vertexIndex.GetDegree(i);
            }
        }

        return graphfile;
    }
    
    template <> FILE* TextAdjacencyListWriter<uint64_t>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // This format does not currently support weighted graphs.
        return NULL;
    }
    
    template <> FILE* TextAdjacencyListWriter<double>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // This format does not currently support weighted graphs.
        return NULL;
    }

    // --------

    template <typename TEdgeData> void TextAdjacencyListWriter<TEdgeData>::WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination)
    {
        // Write out each edge.
        for (size_t i = 0; i < count; ++i)
        {
            // Write the source or destination vertex, depending on the grouping.
            if (groupedByDestination)
                fprintf(graphfile, "%llu\n", (long long unsigned int)buf[i].sourceVertex);
            else
                fprintf(graphfile, "%llu\n", (long long unsigned int)buf[i].destinationVertex);
        }
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class TextAdjacencyListWriter<void>;
    template class TextAdjacencyListWriter<uint64_t>;
    template class TextAdjacencyListWriter<double>;
}
