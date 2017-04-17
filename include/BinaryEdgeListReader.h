/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file BinaryEdgeListReader.h
 *   Declaration of a graph reader for binary edge list files.
 *****************************************************************************/

#pragma once

#include "GraphReader.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    /// Reader object class for graphs represented in binary edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class BinaryEdgeListReader : public GraphReader<TEdgeData>
    {
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphReader.h" for documentation.
        
        virtual FILE* OpenAndInitializeGraphFileForRead(const char* const filename);
        virtual TEdgeCount ReadEdgesToBuffer(FILE* const graphfile, SEdgeBufferData<TEdgeData>* buf, const size_t count);
    };
}
