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

#include <cstdio>
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
        /// @param [out] edgeDataBuf Edge data buffer into which to place parsed edge data, if parsing succeeds.
        /// @return `true` if parsing was successful, `false` otherwise.
        virtual bool ParseEdgeData(const char* edgeDataString, SEdgeBufferData<TEdgeData>& edgeDataBuf) = 0;
        
        
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
                if (NULL == fgets(linebuf, sizeof(linebuf) / sizeof(linebuf[0]), graphfile))
                {
                    fclose(graphfile);
                    return NULL;
                }
                
                if (NULL == fgets(linebuf, sizeof(linebuf) / sizeof(linebuf[0]), graphfile))
                {
                    fclose(graphfile);
                    return NULL;
                }
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

                // Read the line into the line buffer and verify that it is a valid edge.
                if (NULL == fgets(currpos, sizeof(linebuf) / sizeof(linebuf[0]), graphfile)) break;
                if (!isdigit((int)*currpos)) continue;
                if (feof(graphfile) || ferror(graphfile)) break;

                // Extract out the source vertex ID, then skip over whitespace and verify that another number is present.
                sourceVertex = strtoull(currpos, &currpos, 10);
                while (isspace((int)*currpos)) currpos += 1;
                if (!isdigit((int)*currpos)) continue;
                if (feof(graphfile) || ferror(graphfile)) break;

                // Extract out the destination vertex ID, then skip over whitespace.
                destinationVertex = strtoull(currpos, &currpos, 10);
                while (isspace((int)*currpos)) currpos += 1;

                // Parse the edge data out of the input file.
                if (true != this->ParseEdgeData(currpos, buf[numEdgesRead])) continue;

                // Write the edge to the buffer and increment the number of edges that have been read.
                buf[numEdgesRead].sourceVertex = sourceVertex;
                buf[numEdgesRead].destinationVertex = destinationVertex;
                numEdgesRead += 1;
            }

            return numEdgesRead;
        }
    };

    /// Convenience implementation for reading graphs that do not use edge data or weights.
    class UnweightedTextEdgeListReader : public TextEdgeListReader<void>
    {
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See above for documentation.

        virtual bool ParseEdgeData(const char* edgeDataString, SEdgeBufferData<void>& edgeDataBuf)
        {
            return true;
        }
    };
}
