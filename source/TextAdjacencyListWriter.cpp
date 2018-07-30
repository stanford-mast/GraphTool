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
    // -------- LOCALS ----------------------------------------------------- //
    
    // File header for unweighted graphs.
    static const char unweightedHeader[] = "AdjacencyGraph";
    
    // File header for weighted graphs.
    static const char weightedHeader[] = "WeightedAdjacencyGraph";
    
    
    // -------- CLASS VARIABLES -------------------------------------------- //
    // See "TextAdjacencyListWriter.h" for documentation.
    
    template <typename TEdgeData> const char* const TextAdjacencyListWriter<TEdgeData>::outputFileHeader = weightedHeader;
    template <> const char* const TextAdjacencyListWriter<void>::outputFileHeader = unweightedHeader;
    
    
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> unsigned int TextAdjacencyListWriter<TEdgeData>::NumberOfPassesRequired(void)
    {
        return 2;
    }
    
    // --------
    
    template <> unsigned int TextAdjacencyListWriter<void>::NumberOfPassesRequired(void)
    {
        return 1;
    }
    
    // --------
    
    template <typename TEdgeData> FILE* TextAdjacencyListWriter<TEdgeData>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // This class writes files in text mode.
        FILE* graphfile = fopen(filename, "w");

        if (NULL != graphfile)
        {
            // Write out the number of vertices and edges in the graph.
            fprintf(graphfile, "%s\n%llu\n%llu\n", outputFileHeader, (long long unsigned int)graph.GetNumVertices(), (long long unsigned int)graph.GetNumEdges());
            
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

    // --------

    template <typename TEdgeData> void TextAdjacencyListWriter<TEdgeData>::WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination, const unsigned int currentPass)
    {
        switch (currentPass)
        {
        case 0:
            // Write out each edge's topology information.
            for (size_t i = 0; i < count; ++i)
            {
                // Write the source or destination vertex, depending on the grouping.
                if (groupedByDestination)
                    fprintf(graphfile, "%llu\n", (long long unsigned int)buf[i].sourceVertex);
                else
                    fprintf(graphfile, "%llu\n", (long long unsigned int)buf[i].destinationVertex);
            }
            break;
            
        case 1:
            // Write out each edge's data.
            for (size_t i = 0; i < count; ++i)
            {
                fprintf(graphfile, "%llu\n", (long long unsigned int)buf[i].edgeData);
            }
            break;
            
        default:
            break;
        }
    }
    
    // --------
    
    template <> void TextAdjacencyListWriter<void>::WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<void>* buf, const size_t count, const bool groupedByDestination, const unsigned int currentPass)
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
