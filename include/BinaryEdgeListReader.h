/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file BinaryEdgeListReader.h
 *   Declaration of the interface to a graph reader that can interpret graphs
 *   stored as binary edge list files.
 *****************************************************************************/

#pragma once

#include <Graph.h>
#include <GraphReader.h>

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>


namespace GraphTool
{
    /// Reads and interprets graphs stored in binary edge list files.
    class BinaryEdgeListReader : public GraphReader
    {
    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Default constructor.
        BinaryEdgeListReader();


    protected:
        // -------- CONCRETE INSTANCE METHODS -------------------------------------- //
        // See "GraphReader.h" for documentation.
        
        virtual Graph* AllocateGraphObject(void);
        virtual FILE* OpenAndInitializeGraphFile(const std::string& filename);
        virtual TEdgeCount ReadEdgesToBuffer(FILE* graphfile, void* buf, size_t size);
    };
}
