/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphWriterFactory.h
 *   Factory for creating IGraphWriter objects of various types.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    class IGraphWriter;


    /// Enumerates known graph writer object types.
    /// Each corresponds to a different graph file format that can be written.
    enum EGraphWriterType : int64_t
    {
        GraphWriterTypeBinaryEdgeList,                                      ///< BinaryEdgeListWriter
        GraphWriterTypeTextAdjacencyList,                                   ///< TextAdjacencyListWriter
        GraphWriterTypeTextEdgeList,                                        ///< TextEdgeListWriter
        GraphWriterTypeXStream,                                             ///< XStreamWriter
    };

    /// Factory for creating IGraphWriter objects of various types.
    class GraphWriterFactory
    {
    private:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        /// This is a deleted function.
        GraphWriterFactory(void) = delete;


    public:
        // -------- CLASS METHODS ------------------------------------------ //

        /// Creates and returns a pointer to an IGraphWriter object of the specified type.
        /// @param [in] type Type of IGraphWriter object to create.
        /// @param [in] edgedatatype Type of data that the IGraphWriter object should write for each edge.
        /// @return Pointer to the IGraphWriter object, or `NULL` in the event of an error.
        static IGraphWriter* CreateGraphWriter(EGraphWriterType type, EEdgeDataType edgedatatype);

        /// Returns a pointer to a mapping from strings to EGraphWriterType enumerators.
        /// @return Pointer to the enumerator mapping.
        static const std::map<std::string, int64_t>* GetGraphWriterStrings(void);
    };
}
