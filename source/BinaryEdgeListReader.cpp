/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file BinaryEdgeListReader.cpp
 *   Implementation of a graph reader for binary edge list files.
 *****************************************************************************/

#include "BinaryEdgeListReader.h"
#include "GraphReader.h"
#include "Types.h"

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


namespace GraphTool
{
    // -------- CONCRETE INSTANCE METHODS ---------------------------------- //
    // See "GraphReader.h" for documentation.

    template <typename TEdgeData> FILE* BinaryEdgeListReader<TEdgeData>::OpenAndInitializeGraphFileForRead(const char* const filename)
    {
        // This class reads files in binary mode.
        FILE* graphfile = fopen(filename, "rb");

        if (NULL != graphfile)
        {
            // Skip over the two values that indicate the number of edges and vertices in a graph.
            fseek(graphfile, 16, SEEK_SET);
        }

        return graphfile;
    }

    // --------

    template <typename TEdgeData> TEdgeCount BinaryEdgeListReader<TEdgeData>::ReadEdgesToBuffer(FILE* const graphfile, SEdgeBufferData<TEdgeData>* buf, const size_t count)
    {
        return (TEdgeCount)fread(buf, sizeof(SEdgeBufferData<TEdgeData>), count, graphfile);
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class BinaryEdgeListReader<void>;
    template class BinaryEdgeListReader<uint64_t>;
    template class BinaryEdgeListReader<double>;
}
