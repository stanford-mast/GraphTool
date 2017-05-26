/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReaderFactory.cpp
 *   Factory for creating GraphReader objects of various types.
 *****************************************************************************/

#include "BinaryEdgeListReader.h"
#include "GraphReader.h"
#include "GraphReaderFactory.h"
#include "TextEdgeListReader.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>

using namespace GraphTool;


// -------- LOCALS --------------------------------------------------------- //

namespace GraphTool
{
    static const std::map<std::string, int64_t> graphReaderStrings = {
        { "binaryedgelist",                         EGraphReaderType::GraphReaderTypeBinaryEdgeList },
        { "binaryEdgeList",                         EGraphReaderType::GraphReaderTypeBinaryEdgeList },
        { "BinaryEdgeList",                         EGraphReaderType::GraphReaderTypeBinaryEdgeList },

        { "textedgelist",                           EGraphReaderType::GraphReaderTypeTextEdgeList },
        { "textEdgeList",                           EGraphReaderType::GraphReaderTypeTextEdgeList },
        { "TextEdgeList",                           EGraphReaderType::GraphReaderTypeTextEdgeList },
    };
}


// -------- CLASS METHODS -------------------------------------------------- //
// See "GraphReaderFactory.h" for documentation.

template <typename TEdgeData> GraphReader<TEdgeData>* GraphReaderFactory<TEdgeData>::CreateGraphReader(EGraphReaderType type)
{
    GraphReader<TEdgeData>* result = NULL;
    
    switch (type)
    {
    case EGraphReaderType::GraphReaderTypeBinaryEdgeList:
        result = new BinaryEdgeListReader<TEdgeData>();
        break;

    case EGraphReaderType::GraphReaderTypeTextEdgeList:
        result = new TextEdgeListReader<TEdgeData>();
        break;

    default:
        break;
    }
    
    return result;
}

// --------

template <typename TEdgeData> const std::map<std::string, int64_t>* GraphReaderFactory<TEdgeData>::GetGraphReaderStrings(void)
{
    return &graphReaderStrings;
}


// -------- EXPLICIT TEMPLATE INSTANTIATIONS ------------------------------- //

template class GraphReaderFactory<void>;
template class GraphReaderFactory<uint64_t>;
template class GraphReaderFactory<double>;
