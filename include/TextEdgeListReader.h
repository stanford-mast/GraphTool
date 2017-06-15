/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextEdgeListReader.h
 *   Declaration of a graph reader for text edge list files.
 *****************************************************************************/

#pragma once

#include "GraphReader.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    /// Reader object class for graphs represented in text edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class TextEdgeListReader : public GraphReader<TEdgeData>
    {
    private:
        // -------- CLASS METHODS ------------------------------------------ //
        
        /// Parses edge data from the given string.
        /// Actual expected string format is determined by the type of data of interest, which would need to be specialized for each supported type.
        /// @param [in] edgeDataString String from which to parse edge data.
        /// @param [out] edgeDataBuf Edge data buffer into which to place parsed edge data, if parsing succeeds.
        /// @return `true` if parsing was successful, `false` otherwise.
        static bool ParseEdgeData(const char* const edgeDataString, SEdge<TEdgeData>& edgeDataBuf);
        
        
    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphReader.h" for documentation.

        virtual FILE* OpenAndInitializeGraphFileForRead(const char* const filename);
        virtual TEdgeCount ReadEdgesToBuffer(FILE* const graphfile, SEdge<TEdgeData>* buf, const size_t size);
    };
}
