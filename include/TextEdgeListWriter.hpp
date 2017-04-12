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

#include "Edge.hpp"
#include "GraphWriter.hpp"
#include "Types.h"

#include <cstdio>
#include <cstdlib>
#include <string>


namespace GraphTool
{
    /// Reader object class for graphs represented in text edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class TextEdgeListWriter : public GraphWriter<TEdgeData>
    {
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Creates a string from the edge data in the specified edge.
        /// @param [in] edge Edge of interest.
        /// @param [out] edgeDataString String representation of the edge data.
        virtual void StringFromEdgeData(const Edge<TEdgeData>& edge, std::string& edgeDataString) = 0;


        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphWriter.hpp" for documentation.

        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename, bool groupedByDestination)
        {
            // This class writes files in text mode.
            return fopen(filename.c_str(), "w");            
        }

        // --------

        virtual bool WriteGraphContentToFile(FILE* graphfile, Graph<TEdgeData>& graph, bool groupedByDestination)
        {
            // Write out the number of vertices and edges in the graph.
            fprintf(graphfile, "%llu\n%llu\n", (long long unsigned int)graph.GetNumVertices(), (long long unsigned int)graph.GetNumEdges());
            if (ferror(graphfile)) return false;

            // Select an edge grouping based on the passed parameter.
            Graph<TEdgeData>::ConstEdgeIterator iter;
            Graph<TEdgeData>::ConstEdgeIterator end;

            if (groupedByDestination)
            {
                iter = graph.ConstIteratorDestinationBegin();
                end = graph.ConstIteratorDestinationEnd();
            }
            else
            {
                iter = graph.ConstIteratorSourceBegin();
                end = graph.ConstIteratorSourceEnd();
            }

            // Write out each edge.
            for (; iter != end; ++iter)
            {
                // First, write the source and destination vertices.
                if (groupedByDestination)
                    fprintf(graphfile, "%llu %llu", (long long unsigned int)iter.GetOtherVertexID(), (long long unsigned int)iter.GetIndexedVertexID());
                else
                    fprintf(graphfile, "%llu %llu", (long long unsigned int)iter.GetIndexedVertexID(), (long long unsigned int)iter.GetOtherVertexID());

                // Next, see if edge data are available for writing.
                std::string edgeDataString;
                StringFromEdgeData(iter.GetOtherEdgeInfo(), edgeDataString);

                if (!edgeDataString.empty())
                    fprintf(graphfile, " %s", edgeDataString.c_str());

                // End the line.
                fprintf(graphfile, "\n");

                // Check for errors.
                if (ferror(graphfile)) return false;
            }

            return true;
        }
    };

    /// Convenience implementation for writing graphs that do not use edge data or weights.
    class UnweightedTextEdgeListWriter : public TextEdgeListWriter<void>
    {
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See above for documentation.
        
        virtual void StringFromEdgeData(const Edge<void>& edge, std::string& edgeDataString)
        {
            edgeDataString.clear();
        }
    };
}
