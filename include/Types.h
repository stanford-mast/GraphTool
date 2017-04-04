/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Types.h
 *   Definitions of common types used throughout this program.
 *****************************************************************************/

#pragma once

#include <cstdint>


namespace GraphTool
{
    /// Specifies the type to use for identifying vertices.
    typedef uint64_t TVertexID;

    /// Specifies the type to use for counting vertices.
    typedef uint64_t TVertexCount;

    /// Specifies the type to use for counting edges.
    typedef uint64_t TEdgeCount;
}
