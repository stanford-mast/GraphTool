/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextEdgeListWriter.hpp
 *   Implementation of an object capable of writing graphs to files in
 *   text edge list format.
 *****************************************************************************/

#pragma once

#include "GraphWriter.hpp"
#include "Types.h"

#include <cstdio>
#include <cstdlib>


namespace GraphTool
{
    /// Writer object class for graphs represented in text edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class TextEdgeListWriter : public GraphWriter<TEdgeData>
    {
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Creates a string from the edge data in the specified edge.
        /// @param [in] edgeBuf Edge buffer of interest.
        /// @param [out] edgeDataString String representation of the edge data.
        virtual void StringFromEdgeData(const SEdgeBufferData<TEdgeData>& edgeBuf, char* edgeDataString, size_t edgeDataStringCount) = 0;


        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphWriter.hpp" for documentation.

        virtual FILE* OpenAndInitializeGraphFileForWrite(const char* filename, Graph<TEdgeData>& graph, bool groupedByDestination)
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

        virtual void WriteEdgesToFile(FILE* graphfile, const Graph<TEdgeData>& graph, const SEdgeBufferData<TEdgeData>* buf, size_t count, bool groupedByDestination)
        {
            // Select an edge grouping based on the passed parameter.
            typename Graph<TEdgeData>::EdgeIterator iter;
            typename Graph<TEdgeData>::EdgeIterator end;

            // Write out each edge.
            for (size_t i = 0; i < count; ++i)
            {
                // First, write the source and destination vertices.
                fprintf(graphfile, "%llu %llu", (long long unsigned int)buf[i].sourceVertex, (long long unsigned int)buf[i].destinationVertex);

                // Next, see if edge data are available for writing.
                char edgeDataString[128];
                StringFromEdgeData(buf[i], edgeDataString, sizeof(edgeDataString) / sizeof(edgeDataString[0]));

                if ('\0' != edgeDataString[0])
                    fprintf(graphfile, " %s", edgeDataString);

                // End the line.
                fprintf(graphfile, "\n");
            }
        }
    };

    /// Convenience implementation for writing graphs that do not use edge data or weights.
    class UnweightedTextEdgeListWriter : public TextEdgeListWriter<void>
    {
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See above for documentation.
        
        virtual void StringFromEdgeData(const SEdgeBufferData<void>& edgeBuf, char* edgeDataString, size_t edgeDataStringCount)
        {
            if (0 < edgeDataStringCount)
                edgeDataString[0] = '\0';
        }
    };
}
