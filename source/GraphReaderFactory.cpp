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
#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    // -------- LOCALS ----------------------------------------------------- //

    /// Maps strings to the internal edge list type.
    static const std::map<std::string, int64_t> graphReaderStrings = {
        { "binaryedgelist",                         EGraphReaderType::GraphReaderTypeBinaryEdgeList },
        { "binaryEdgeList",                         EGraphReaderType::GraphReaderTypeBinaryEdgeList },
        { "BinaryEdgeList",                         EGraphReaderType::GraphReaderTypeBinaryEdgeList },

        { "textedgelist",                           EGraphReaderType::GraphReaderTypeTextEdgeList },
        { "textEdgeList",                           EGraphReaderType::GraphReaderTypeTextEdgeList },
        { "TextEdgeList",                           EGraphReaderType::GraphReaderTypeTextEdgeList },
    };

    
    // -------- HELPERS ---------------------------------------------------- //
    
    /// Creates and returns a pointer to a GraphReader object of the specified type.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    /// @param [in] type Type of GraphReader object to create.
    /// @return Pointer to the GraphReader object, or `NULL` in the event of an error.
    template <typename TEdgeData> static GraphReader<TEdgeData>* CreateGraphReaderInternal(EGraphReaderType type)
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
    
    // -------- CLASS METHODS ---------------------------------------------- //
    // See "GraphReaderFactory.h" for documentation.

    GraphReader<void>* GraphReaderFactory::CreateGraphReader(EGraphReaderType type, EEdgeDataType edgedatatype)
    {
        GraphReader<void>* result = NULL;
        
        switch (edgedatatype)
        {
        case EEdgeDataType::EdgeDataTypeVoid:
            result = CreateGraphReaderInternal<void>(type);
            break;
        
        case EEdgeDataType::EdgeDataTypeInteger:
            result = CreateGraphReaderInternal<void>(type);
            break;
            
        case EEdgeDataType::EdgeDataTypeFloatingPoint:
            result = CreateGraphReaderInternal<void>(type);
            break;
            
        default:
            break;
        }
        
        return result;
    }

    // --------

    const std::map<std::string, int64_t>* GraphReaderFactory::GetGraphReaderStrings(void)
    {
        return &graphReaderStrings;
    }
}
