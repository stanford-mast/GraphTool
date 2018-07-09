/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphTransformFactory.h
 *   Factory for creating IGraphTransform objects of various types.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <cstdint>
#include <map>
#include <string>


namespace GraphTool
{
    class IGraphTransform;


    /// Enumerates known graph transformation object types.
    /// Each corresponds to a different graph transformation that can be applied.
    enum EGraphTransformType : int64_t
    {
        GraphTransformTypeHashEdgeData,                                     ///< HashEdgeDataTransform
        GraphTransformTypeNullIntEdgeData,                                  ///< NullEdgeDataTransform<uint64_t>
        GraphTransformTypeNullFloatEdgeData,                                ///< NullEdgeDataTransform<double>
    };
    
    /// Factory for creating IGraphTransform objects of various types.
    class GraphTransformFactory
    {
    private:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Default constructor.
        /// This is a deleted function.
        GraphTransformFactory(void) = delete;
        
        
    public:
        // -------- CLASS METHODS ------------------------------------------ //
        
        /// Creates and returns a pointer to an IGraphTransform object of the specified type.
        /// @param [in] type Type of IGraphTransform object to create.
        /// @return Pointer to the IGraphTransform object, or `NULL` in the event of an error.
        static IGraphTransform* CreateGraphTransform(EGraphTransformType type);

        /// Returns a pointer to a mapping from strings to EGraphTransformType enumerators.
        /// @return Pointer to the enumerator mapping.
        static const std::map<std::string, int64_t>* GetGraphTransformStrings(void);
    };
}
