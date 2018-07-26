/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file TextEdgeListWriter.h
 *   Declaration of a graph writer for text edge list files.
 *****************************************************************************/

#pragma once

#include "GraphWriter.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    /// Writer object class for graphs represented in text edge list format.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class TextEdgeListWriter : public GraphWriter<TEdgeData>
    {
    private:
        // -------- CLASS METHODS ------------------------------------------ //

        /// Creates a string from the edge data in the specified edge.
        /// Formatting is type-dependent, so a specialized version must be provided for each supported type.
        /// @param [in] edgeBuf Edge buffer of interest.
        /// @param [out] edgeDataString String representation of the edge data.
        /// @param [in] edgeDataStringCount Size of the provided string buffer, in number of characters.
        /// @return Number of characters written, which could be zero depending on the type, and where negative indicates an error.
        static int StringFromEdgeData(const SEdge<TEdgeData>& edgeBuf, char* const edgeDataString, const size_t edgeDataStringCount);
        
        
    public:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphWriter.h" for documentation.

        virtual FILE* OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph& graph, const bool groupedByDestination);
        virtual void WriteEdgesToFile(FILE* const graphfile, const Graph& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination, const unsigned int currentPass);
    };
}
