/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file IGraphWriter.h
 *   Declaration of the interface to objects that write graphs to files.
 *****************************************************************************/

#pragma once


namespace GraphTool
{
    class Graph;

    
    /// Interface for objects that write graphs to files.
    class IGraphWriter
    {
    public:
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //

        /// Writes a graph to the specified file.
        /// @param [in] filename File name of the file to be written.
        /// @param [in] graph Graph object to be written to the file.
        /// @param [in] groupedByDestination Indicates that graph edges should be grouped by destination instead of by source.
        /// @return `true` if the graph was written successfully, `false` otherwise.
        virtual bool WriteGraphToFile(const char* const filename, const Graph& graph, const bool groupedByDestination = false) = 0;
    };
}
