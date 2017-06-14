/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file EdgeList.h
 *   Declaration of a container for holding edges that correspond to a single
 *   top-level vertex, optimized for fast traversal.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <vector>


namespace GraphTool
{
    /// Holds edges in a way optimized for fast traversal.
    /// Represents graph topology data and can hold edge data, such as weights, as well.
    /// This indexed data structure represents unidirectional edges but does not specify the direction.
    /// Direction information depends on the usage semantics and is governed by the code that instantiates objects of this type.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> class EdgeList
    {
    
    };

    /// Holds edges in a way optimized for fast traversal, specialized for unweighted graphs.
    template <> class EdgeList<void>
    {
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Block index for each edge bit-mask.
        /// Specifies the starting vertex for each corresponding position in the edge bit-mask.
        std::vector<TVertexID> base;
        
        /// Edge bit-mask.
        /// Holds individual edge information.
        /// A '1' bit means the edge is present, '0' means it is absent.
        /// Element 'i' holds information for vertices `(base[i] * 64)` to `(base[i] * 64) + 63`.
        std::vector<uint64_t> edges;


    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        EdgeList(void);


        // -------- INSTANCE METHODS --------------------------------------- //


    };
}
