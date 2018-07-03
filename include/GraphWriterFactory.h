/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphWriterFactory.h
 *   Factory for creating GraphWriter objects of various types.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    template <typename TEdgeData> class GraphWriter;


    /// Enumerates known GraphWriter object types.
    /// Each corresponds to a different graph file format that can be written.
    enum EGraphWriterType : int64_t
    {
        GraphWriterTypeBinaryEdgeList,                                      ///< BinaryEdgeListWriter
        GraphWriterTypeTextAdjacencyList,                                   ///< TextAdjacencyListWriter
        GraphWriterTypeTextEdgeList,                                        ///< TextEdgeListWriter
        GraphWriterTypeXStream,                                             ///< XStreamWriter
    };

    /// Factory for creating GraphWriter objects of various types.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class GraphWriterFactory
    {
    private:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor. Should never be invoked.
        GraphWriterFactory(void);


    public:
        // -------- CLASS METHODS ------------------------------------------ //

        /// Creates and returns a pointer to a GraphWriter object of the specified type.
        /// @param [in] type Type of GraphWriter object to create.
        /// @return Pointer to the GraphWriter object, or `NULL` in the event of an error.
        static GraphWriter<TEdgeData>* CreateGraphWriter(EGraphWriterType type);

        /// Returns a pointer to a mapping from strings to EGraphWriterType enumerators.
        /// @return Pointer to the enumerator mapping.
        static const std::map<std::string, int64_t>* GetGraphWriterStrings(void);
    };
}
