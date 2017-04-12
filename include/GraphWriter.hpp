/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphWriter.hpp
 *   Declaration of the common interface to objects capable of producing
 *   graph files of various formats, templated by edge data type.
 *****************************************************************************/

#pragma once

#include "Graph.hpp"
#include "Types.h"

#include <cstdio>
#include <string>


namespace GraphTool
{
    /// Base class for all object types that are used to produce graph files of various formats.
    /// Objects in this hierarchy consume graph objects and produce files.
    /// Some common functionality is implemented directly in the base class.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class GraphWriter
    {
    private:
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Opens and performs any initial file reading tasks required to prepare the graph file for writing of graph data.
        /// @param [in] filename File name of the file to be opened for writing.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return File handle for the opened file.
        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename, bool groupedByDestination) = 0;
        
        /// Writes edge data from the specified buffer into the specified file.
        /// @param [in] graphfile File handle for the open graph file.
        /// @param [in] graph Graph to be written.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return `true` if all edges were successfully written, `false` otherwise.
        virtual bool WriteGraphContentToFile(FILE* graphfile, Graph<TEdgeData>& graph, bool groupedByDestination) = 0;
        
        
    public:
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Writes a graph to the specified file.
        /// @param [in] filename File name of the file to be written.
        /// @param [in] graph Graph object to be written to the file.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return `true` if the graph was written successfully, `false` otherwise.
        bool WriteGraphToFile(const std::string& filename, Graph<TEdgeData>& graph, bool groupedByDestination = false)
        {
            // First, open the file.
            FILE* graphfile = this->OpenAndInitializeGraphFile(filename, groupedByDestination);
            if (NULL == graphfile)
                return false;
            
            // Next, write out the graph.
            bool result = WriteGraphContentToFile(graphfile, graph, groupedByDestination);

            // Close the file and return the result.
            fclose(graphfile);
            return result;
        }
    };
}
