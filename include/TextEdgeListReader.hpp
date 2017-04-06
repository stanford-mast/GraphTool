/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextEdgeListReader.hpp
 *   Implementation of an object capable of reading from graphs represented
 *   in text edge list format.
 *****************************************************************************/

#pragma once

#include "GraphReader.hpp"
#include "Types.h"

#include <cstdlib>
#include <cctype>


namespace GraphTool
{
    /// Reader object class for graphs represented in text edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class TextEdgeListReader : public GraphReader<TEdgeData>
    {
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Parses edge data from the given string.
        /// Actual expected string format is determined by the type of data of interest, which would need to be specified by subclasses.
        /// @param [in] edgeDataString String from which to parse edge data.
        /// @param [out] edgeData Edge data element to be filled if parsing succeeds.
        /// @return `true` if parsing was successful, `false` otherwise.
        virtual bool ParseEdgeData(const char* edgeDataString, TEdgeData& edgeData) = 0;
        
        
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphReader.hpp" for documentation.

        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename)
        {
            // This class reads files in text mode.
            FILE* graphfile = fopen(filename.c_str(), "r");

            if (NULL != graphfile)
            {
                char linebuf[1024];
                
                // Skip over the two values that indicate the number of edges and vertices in a graph.
                fgets(linebuf, sizeof(linebuf) / sizeof(linebuf[0]), graphfile);
                fgets(linebuf, sizeof(linebuf) / sizeof(linebuf[0]), graphfile);
            }

            return graphfile;
        }

        // --------

        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, SEdgeBufferData<TEdgeData>* buf, size_t size)
        {
            const size_t edgeSize = sizeof(SEdgeBufferData<TEdgeData>);
            const size_t edgeCount = (size / edgeSize);
            TEdgeCount numEdgesRead = 0;
            
            while ((numEdgesRead < edgeCount) && !(feof(graphfile)) && !(ferror(graphfile)))
            {
                char linebuf[1024];
                char* currpos = &linebuf[0];
                TVertexID sourceVertex;
                TVertexID destinationVertex;
                TEdgeData edgeData;

                // Read the line into the line buffer and verify that it is a valid edge.
                fgets(currpos, sizeof(linebuf) / sizeof(linebuf[0]), graphfile);
                if (!isdigit((int)*currpos)) continue;

                // Extract out the source vertex ID, then skip over whitespace and verify that another number is present.
                sourceVertex = strtoull(currpos, &currpos, 10);
                while (isspace((int)*currpos)) currpos += 1;
                if (!isdigit((int)*currpos)) continue;

                // Extract out the destination vertex ID, then skip over whitespace.
                destinationVertex = strtoull(currpos, &currpos, 10);
                while (isspace((int)*currpos)) currpos += 1;

                // Parse the edge data out of the input file.
                if (true != this->ParseEdgeData(currpos, edgeData)) continue;

                // Write the edge to the buffer and increment the number of edges that have been read.
                buf[numEdgesRead].sourceVertex = sourceVertex;
                buf[numEdgesRead].destinationVertex = destinationVertex;
                buf[numEdgesRead].edgeData = edgeData;
                numEdgesRead += 1;
            }

            return numEdgesRead;
        }
    };

    /// Reader object class for graphs represented in text edge list format, specialized for unweighted graphs.
    template <> class TextEdgeListReader<void> : public GraphReader<void>
    {
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphReader.hpp" for documentation.

        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename)
        {
            // This class reads files in text mode.
            FILE* graphfile = fopen(filename.c_str(), "r");

            if (NULL != graphfile)
            {
                char linebuf[1024];

                // Skip over the two values that indicate the number of edges and vertices in a graph.
                fgets(linebuf, sizeof(linebuf) / sizeof(linebuf[0]), graphfile);
                fgets(linebuf, sizeof(linebuf) / sizeof(linebuf[0]), graphfile);
            }

            return graphfile;
        }

        // --------

        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, SEdgeBufferData<void>* buf, size_t count)
        {
            TEdgeCount numEdgesRead = 0;

            while (numEdgesRead < count)
            {
                char linebuf[1024];
                char* currpos = &linebuf[0];
                TVertexID sourceVertex;
                TVertexID destinationVertex;

                // Read the line into the line buffer and verify that it is a valid edge.
                fgets(currpos, sizeof(linebuf) / sizeof(linebuf[0]), graphfile);
                if (feof(graphfile) || ferror(graphfile)) break;
                if (!isdigit((int)*currpos)) continue;

                // Extract out the source vertex ID, then skip over whitespace and verify that another number is present.
                sourceVertex = strtoull(currpos, &currpos, 10);
                while (isspace((int)*currpos)) currpos += 1;
                if (!isdigit((int)*currpos)) continue;

                // Extract out the destination vertex ID.
                destinationVertex = strtoull(currpos, &currpos, 10);
                
                // Write the edge to the buffer and increment the number of edges that have been read.
                buf[numEdgesRead].sourceVertex = sourceVertex;
                buf[numEdgesRead].destinationVertex = destinationVertex;
                numEdgesRead += 1;
            }

            return numEdgesRead;
        }
    };
}
