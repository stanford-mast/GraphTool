/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphWriterFactory.cpp
 *   Factory for creating GraphWriter objects of various types.
 *****************************************************************************/

#include "BinaryEdgeListWriter.h"
#include "GraphWriter.h"
#include "GraphWriterFactory.h"
#include "TextEdgeListWriter.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>

using namespace GraphTool;


// -------- LOCALS --------------------------------------------------------- //

namespace GraphTool
{
    static const std::map<std::string, int64_t> graphWriterStrings = {
        { "binaryedgelist",                         EGraphWriterType::GraphWriterTypeBinaryEdgeList },
        { "binaryEdgeList",                         EGraphWriterType::GraphWriterTypeBinaryEdgeList },
        { "BinaryEdgeList",                         EGraphWriterType::GraphWriterTypeBinaryEdgeList },

        { "textedgelist",                           EGraphWriterType::GraphWriterTypeTextEdgeList },
        { "textEdgeList",                           EGraphWriterType::GraphWriterTypeTextEdgeList },
        { "TextEdgeList",                           EGraphWriterType::GraphWriterTypeTextEdgeList },
    };
}


// -------- CLASS METHODS -------------------------------------------------- //
// See "GraphWriterFactory.h" for documentation.

template <typename TEdgeData> GraphWriter<TEdgeData>* GraphWriterFactory<TEdgeData>::CreateGraphWriter(EGraphWriterType type)
{
    GraphWriter<TEdgeData>* result = NULL;

    switch (type)
    {
    case EGraphWriterType::GraphWriterTypeBinaryEdgeList:
        result = new BinaryEdgeListWriter<TEdgeData>();
        break;

    case EGraphWriterType::GraphWriterTypeTextEdgeList:
        result = new TextEdgeListWriter<TEdgeData>();
        break;

    default:
        break;
    }

    return result;
}

// --------

template <typename TEdgeData> const std::map<std::string, int64_t>* GraphWriterFactory<TEdgeData>::GetGraphWriterStrings(void)
{
    return &graphWriterStrings;
}


// -------- EXPLICIT TEMPLATE INSTANTIATIONS ------------------------------- //

template class GraphWriterFactory<void>;
template class GraphWriterFactory<uint64_t>;
template class GraphWriterFactory<double>;
