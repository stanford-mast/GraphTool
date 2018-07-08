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
#include "TextAdjacencyListWriter.h"
#include "TextEdgeListWriter.h"
#include "Types.h"
#include "XStreamWriter.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    // -------- LOCALS ----------------------------------------------------- //

    /// Maps strings to the internal edge list type.
    static const std::map<std::string, int64_t> graphWriterStrings = {
        { "binaryedgelist",                         EGraphWriterType::GraphWriterTypeBinaryEdgeList },
        { "binaryEdgeList",                         EGraphWriterType::GraphWriterTypeBinaryEdgeList },
        { "BinaryEdgeList",                         EGraphWriterType::GraphWriterTypeBinaryEdgeList },

        { "adjacencygraph",                         EGraphWriterType::GraphWriterTypeTextAdjacencyList },
        { "AdjacencyGraph",                         EGraphWriterType::GraphWriterTypeTextAdjacencyList },
        { "textadjacencylist",                      EGraphWriterType::GraphWriterTypeTextAdjacencyList },
        { "textAdjacencyList",                      EGraphWriterType::GraphWriterTypeTextAdjacencyList },
        { "TextAdjacencyList",                      EGraphWriterType::GraphWriterTypeTextAdjacencyList },
        
        { "textedgelist",                           EGraphWriterType::GraphWriterTypeTextEdgeList },
        { "textEdgeList",                           EGraphWriterType::GraphWriterTypeTextEdgeList },
        { "TextEdgeList",                           EGraphWriterType::GraphWriterTypeTextEdgeList },

        { "XStream",                                EGraphWriterType::GraphWriterTypeXStream },
        { "Xstream",                                EGraphWriterType::GraphWriterTypeXStream },
        { "xstream",                                EGraphWriterType::GraphWriterTypeXStream },
        { "X-Stream",                               EGraphWriterType::GraphWriterTypeXStream },
        { "X-stream",                               EGraphWriterType::GraphWriterTypeXStream },
        { "x-stream",                               EGraphWriterType::GraphWriterTypeXStream },
    };


    // -------- HELPERS ---------------------------------------------------- //
    
    /// Creates and returns a pointer to a GraphWriter object of the specified type.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    /// @param [in] type Type of GraphWriter object to create.
    /// @return Pointer to the GraphWriter object, or `NULL` in the event of an error.
    template <typename TEdgeData> static GraphWriter<TEdgeData>* CreateGraphWriterInternal(EGraphWriterType type)
    {
        GraphWriter<TEdgeData>* result = NULL;

        switch (type)
        {
        case EGraphWriterType::GraphWriterTypeBinaryEdgeList:
            result = new BinaryEdgeListWriter<TEdgeData>();
            break;

        case EGraphWriterType::GraphWriterTypeTextAdjacencyList:
            result = new TextAdjacencyListWriter<TEdgeData>();
            break;
        
        case EGraphWriterType::GraphWriterTypeTextEdgeList:
            result = new TextEdgeListWriter<TEdgeData>();
            break;

        case EGraphWriterType::GraphWriterTypeXStream:
            result = new XStreamWriter<TEdgeData>();
            break;

        default:
            break;
        }

        return result;
    }
    
    
    // -------- CLASS METHODS ---------------------------------------------- //
    // See "GraphWriterFactory.h" for documentation.

    GraphWriter<void>* GraphWriterFactory::CreateGraphWriter(EGraphWriterType type, EEdgeDataType edgedatatype)
    {
        GraphWriter<void>* result = NULL;
        
        switch (edgedatatype)
        {
        case EEdgeDataType::EdgeDataTypeVoid:
            result = CreateGraphWriterInternal<void>(type);
            break;
        
        case EEdgeDataType::EdgeDataTypeInteger:
            result = CreateGraphWriterInternal<void>(type);
            break;
            
        case EEdgeDataType::EdgeDataTypeFloatingPoint:
            result = CreateGraphWriterInternal<void>(type);
            break;
            
        default:
            break;
        }
        
        return result;
    }

    // --------

    const std::map<std::string, int64_t>* GraphWriterFactory::GetGraphWriterStrings(void)
    {
        return &graphWriterStrings;
    }
}
