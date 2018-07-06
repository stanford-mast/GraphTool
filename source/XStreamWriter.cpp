/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file XStreamWriter.cpp
 *   Implementation of a graph writer for the format used by X-Stream.
 *****************************************************************************/

#include "BinaryEdgeListWriter.h"
#include "Graph.h"
#include "Types.h"
#include "XStreamWriter.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //

    template <typename TEdgeData> XStreamWriter<TEdgeData>::XStreamWriter(void) : graphTypeIdentifier("1")
    {
        // Nothing to do here.
    }

    // --------

    template <> XStreamWriter<void>::XStreamWriter(void) : graphTypeIdentifier("2")
    {
        // Nothing to do here.
    }

    
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphWriter.h" for documentation.

    template <typename TEdgeData> FILE* XStreamWriter<TEdgeData>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // Obtain the filename of the metadata file.
        char metafilename[2048];
        unsigned int nMetafilename = (unsigned int)snprintf(metafilename, sizeof(metafilename) / sizeof(metafilename[0]), "%s.ini", filename);

        if (nMetafilename >= (sizeof(metafilename) / sizeof(metafilename[0])))
            return NULL;

        // Write the metadata file.
        FILE* metafile = fopen(metafilename, "w");

        if (NULL == metafile)
            return NULL;

        fputs("[graph]\n", metafile);
        fprintf(metafile, "type=%s\n", graphTypeIdentifier);
        fprintf(metafile, "name=%s\n", filename);
        fprintf(metafile, "vertices=%llu\n", (long long unsigned int)graph.GetNumVertices());
        fprintf(metafile, "edges=%llu\n", (long long unsigned int)graph.GetNumEdges());

        if (ferror(metafile))
            return NULL;

        fclose(metafile);
        
        // Open the binary file that will receive graph data itself.
        return fopen(filename, "wb");
    }

    // --------

    template <> FILE* XStreamWriter<uint64_t>::OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination)
    {
        // X-Stream does not support integer-weighted graphs.
        return NULL;
    }

    // --------

    template <typename TEdgeData> void XStreamWriter<TEdgeData>::WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination)
    {
        fwrite((void*)buf, sizeof(const SEdge<TEdgeData>), count, graphfile);
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class XStreamWriter<void>;
    template class XStreamWriter<uint64_t>;
    template class XStreamWriter<double>;
}
