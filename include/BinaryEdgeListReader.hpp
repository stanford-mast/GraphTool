/*****************************************************************************
* GraphTool
*   Tools for manipulating graphs.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file BinaryEdgeListReader.hpp
*   Implementation of an object capable of reading from graphs represented
*   in binary edge list format.
*****************************************************************************/

#pragma once

#include "GraphReader.hpp"
#include "Types.h"


namespace GraphTool
{
    /// Reader object class for graphs represented in binary edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class BinaryEdgeListReader : public GraphReader<TEdgeData>
    {
        // -------- CONCRETE INSTANCE METHODS -------------------------------------- //
        // See "GraphReader.hpp" for documentation.
        
        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename)
        {
            // This class reads files in binary mode.
            FILE* graphfile = fopen(filename.c_str(), "rb");

            if (NULL != graphfile)
            {
                // Skip over the two values that indicate the number of edges and vertices in a graph.
                fseek(graphfile, 16, SEEK_SET);
            }

            return graphfile;
        }

        // --------

        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, SEdgeBufferData<TEdgeData>* buf, size_t size)
        {
            const size_t edgeSize = sizeof(SEdgeBufferData<TEdgeData>);
            const size_t edgeCount = (size / edgeSize);

            // Simple binary read is all that is required.
            return (TEdgeCount)fread(buf, edgeSize, edgeCount, graphfile);
        }
    };
}
