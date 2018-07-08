/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file IGraphReader.h
 *   Declaration of the interface to objects that read graphs from files.
 *****************************************************************************/

#pragma once

#include "Types.h"


namespace GraphTool
{
    class Graph;

    
    /// Interface for objects that read graphs from files.
    class IGraphReader
    {   
    public:
        // -------- ABSTRACT INSTANCE METHODS ------------------------------ //
        
        /// Reads a graph from the specified file.
        /// @param [in] filename File name of the file to be read.
        /// @param [out] graph Graph object to be filled. Not modified if an error occurs during reading.
        /// @return Result of the read operation.
        virtual EGraphFileResult ReadGraphFromFile(const char* const filename, Graph& graph) = 0;
    };
}
