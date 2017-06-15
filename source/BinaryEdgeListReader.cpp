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
            uint64_t headerQuantities = 0ull;
            
            // Read the number of vertices.
            if (1 != fread((void*)&headerQuantities, sizeof(headerQuantities), 1, graphfile))
            {
                fclose(graphfile);
                return NULL;
            }
            
            GraphReader<TEdgeData>::numVerticesInFile = (TVertexCount)headerQuantities;
            
            // Read the number of edges.
            if (1 != fread((void*)&headerQuantities, sizeof(headerQuantities), 1, graphfile))
            {
                fclose(graphfile);
                return NULL;
            }
            
            GraphReader<TEdgeData>::numEdgesInFile = (TEdgeCount)headerQuantities;
        }

        return graphfile;
    }

    // --------

    template <typename TEdgeData> TEdgeCount BinaryEdgeListReader<TEdgeData>::ReadEdgesToBuffer(FILE* const graphfile, SEdge<TEdgeData>* buf, const size_t count)
    {
        return (TEdgeCount)fread(buf, sizeof(SEdge<TEdgeData>), count, graphfile);
    }


    // -------- EXPLICIT TEMPLATE INSTANTIATIONS --------------------------- //

    template class BinaryEdgeListReader<void>;
    template class BinaryEdgeListReader<uint64_t>;
    template class BinaryEdgeListReader<double>;
}
