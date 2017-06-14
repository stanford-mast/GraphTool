/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextEdgeListReader.cpp
 *   Implementation of a graph reader for text edge list files.
 *****************************************************************************/

#include "GraphReader.h"
#include "TextEdgeListReader.h"
#include "Types.h"

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


namespace GraphTool
{
    // -------- CLASS METHODS ---------------------------------------------- //
    // See "TextEdgeListReader.h" for documentation.

    template <> bool TextEdgeListReader<void>::ParseEdgeData(const char* const edgeDataString, SEdgeBufferData<void>& edgeDataBuf)
    {
        return true;
    }

    // --------

    template <> bool TextEdgeListReader<uint64_t>::ParseEdgeData(const char* const edgeDataString, SEdgeBufferData<uint64_t>& edgeDataBuf)
    {
        char* endPos;
        edgeDataBuf.edgeData = strtoull(edgeDataString, &endPos, 0);

        return (edgeDataString != endPos);
    }

    // --------

    template <> bool TextEdgeListReader<double>::ParseEdgeData(const char* const edgeDataString, SEdgeBufferData<double>& edgeDataBuf)
    {
        char* endPos;
        edgeDataBuf.edgeData = strtod(edgeDataString, &endPos);

        return (edgeDataString != endPos);
    }


    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphReader.h" for documentation.

    template <typename TEdgeData> FILE* TextEdgeListReader<TEdgeData>::OpenAndInitializeGraphFileForRead(const char* const filename)
    {
        // This class reads files in text mode.
        FILE* graphfile = fopen(filename, "r");

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

    template <typename TEdgeData> TEdgeCount TextEdgeListReader<TEdgeData>::ReadEdgesToBuffer(FILE* const graphfile, SEdgeBufferData<TEdgeData>* buf, const size_t size)
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


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class TextEdgeListReader<void>;
    template class TextEdgeListReader<uint64_t>;
    template class TextEdgeListReader<double>;
}
