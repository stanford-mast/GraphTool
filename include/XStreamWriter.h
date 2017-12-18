/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file XStreamWriter.h
 *   Declaration of a graph writer for the format used by X-Stream.
 *****************************************************************************/

#pragma once

#include "BinaryEdgeListWriter.h"
#include "Types.h"

#include <cstddef>
#include <cstdio>


namespace GraphTool
{
    /// Writer object class for graphs represented in the format used by X-Stream.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class XStreamWriter : public BinaryEdgeListWriter<TEdgeData>
    {
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        // String that identifies the graph type being written.
        // See X-Stream documentation for possible values.
        const char* const graphTypeIdentifier;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor.
        XStreamWriter(void);
        
        
    private:
        // -------- CONCRETE INSTANCE METHODS ------------------------------ //
        // See "GraphWriter.h" for documentation.
        
        virtual FILE* OpenAndInitializeGraphFileForWrite(const char* const filename, const Graph<TEdgeData>& graph, const bool groupedByDestination);
        virtual void WriteEdgesToFile(FILE* const graphfile, const Graph<TEdgeData>& graph, const SEdge<TEdgeData>* buf, const size_t count, const bool groupedByDestination);
    };
}
