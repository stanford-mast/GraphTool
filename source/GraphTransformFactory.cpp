/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphTransformFactory.cpp
 *   Factory for creating IGraphTransform objects of various types.
 *****************************************************************************/

#include "GraphTransformFactory.h"
#include "IGraphTransform.h"
#include "NullEdgeDataTransform.h"
#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    // -------- LOCALS ----------------------------------------------------- //

    /// Maps strings to the internal edge list type.
    static const std::map<std::string, int64_t> graphTransformStrings = {
        { "nullintedgedata",                                                EGraphTransformType::GraphTransformTypeNullIntEdgeData },
        { "nullIntEdgeData",                                                EGraphTransformType::GraphTransformTypeNullIntEdgeData },
        { "NullIntEdgeData",                                                EGraphTransformType::GraphTransformTypeNullIntEdgeData },

        { "nullfloatedgedata",                                              EGraphTransformType::GraphTransformTypeNullFloatEdgeData },
        { "nullFloatEdgeData",                                              EGraphTransformType::GraphTransformTypeNullFloatEdgeData },
        { "NullFloatEdgeData",                                              EGraphTransformType::GraphTransformTypeNullFloatEdgeData },
    };

    
    // -------- CLASS METHODS ---------------------------------------------- //
    // See "GraphTransformFactory.h" for documentation.

    IGraphTransform* GraphTransformFactory::CreateGraphTransform(EGraphTransformType type)
    {
        IGraphTransform* result = NULL;

        switch (type)
        {
        case EGraphTransformType::GraphTransformTypeNullIntEdgeData:
            result = new NullEdgeDataTransform<uint64_t>;
            break;

        case EGraphTransformType::GraphTransformTypeNullFloatEdgeData:
            result = new NullEdgeDataTransform<double>;
            break;

        default:
            break;
        }

        return result;
    }

    // --------

    const std::map<std::string, int64_t>* GraphTransformFactory::GetGraphTransformStrings(void)
    {
        return &graphTransformStrings;
    }
}





