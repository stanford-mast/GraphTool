/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReaderFactory.h
 *   Factory for creating IGraphReader objects of various types.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    class IGraphReader;


    /// Enumerates known graph reader object types.
    /// Each corresponds to a different graph file format that can be read.
    enum EGraphReaderType : int64_t
    {
        GraphReaderTypeBinaryEdgeList,                                      ///< BinaryEdgeListReader
        GraphReaderTypeTextEdgeList,                                        ///< TextEdgeListReader
    };
    
    /// Factory for creating IGraphReader objects of various types.
    class GraphReaderFactory
    {
    private:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor.
        /// This is a deleted function.
        GraphReaderFactory(void) = delete;
        
        
    public:
        // -------- CLASS METHODS ------------------------------------------ //
        
        /// Creates and returns a pointer to an IGraphReader object of the specified type.
        /// @param [in] type Type of IGraphReader object to create.
        /// @param [in] edgedatatype Type of data that the IGraphReader object should read for each edge.
        /// @return Pointer to the IGraphReader object, or `NULL` in the event of an error.
        static IGraphReader* CreateGraphReader(EGraphReaderType type, EEdgeDataType edgedatatype);

        /// Returns a pointer to a mapping from strings to EGraphReaderType enumerators.
        /// @return Pointer to the enumerator mapping.
        static const std::map<std::string, int64_t>* GetGraphReaderStrings(void);
    };
}
