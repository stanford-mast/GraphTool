/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReaderFactory.h
 *   Factory for creating GraphReader objects of various types.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    template <typename TEdgeData> class GraphReader;


    /// Enumerates known GraphReader object types.
    /// Each corresponds to a different graph file format that can be read.
    enum EGraphReaderType : int64_t
    {
        GraphReaderTypeBinaryEdgeList,                                      ///< BinaryEdgeListReader
        GraphReaderTypeTextEdgeList,                                        ///< TextEdgeListReader
    };
    
    /// Factory for creating GraphReader objects of various types.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class GraphReaderFactory
    {
    private:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor. Should never be invoked.
        GraphReaderFactory(void);
        
        
    public:
        // -------- CLASS METHODS ------------------------------------------ //
        
        /// Creates and returns a pointer to a GraphReader object of the specified type.
        /// @param [in] type Type of GraphReader object to create.
        /// @return Pointer to the GraphReader object, or `NULL` in the event of an error.
        static GraphReader<TEdgeData>* CreateGraphReader(EGraphReaderType type);

        /// Returns a pointer to a mapping from strings to EGraphReaderType enumerators.
        /// @return Pointer to the enumerator mapping.
        static const std::map<std::string, int64_t>* GetGraphReaderStrings(void);
    };
}