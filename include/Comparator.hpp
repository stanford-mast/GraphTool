/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Comparator.hpp
 *   Declaration of function objects used to compare edges in a graph.
 *   Used to manipulate graphs by rearranging the edges.
 *****************************************************************************/

#pragma once

#include "Edge.hpp"
#include "Types.h"


namespace GraphTool
{
    template <typename TEdgeData> class Graph;
    
    
    /// Base class for function objects used to compare edges in a graph.
    /// Subclasses must provide a function call operator to perform the actual comparison.
    /// Functions that use comparators should accept parameters of this type, but concrete implementations should not be derived from it.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct Comparator
    {
        // -------- ABSTRACT OPERATORS ------------------------------------- //
        
        /// Defines the actual comparison taking place and must be implemented by subclasses.
        /// This operator will be invoked to determine the desired order of edges.
        /// It may be invoked by multiple threads and so must run correctly in that context.
        /// @param [in] first First operand of the comparison.
        /// @param [in] second Second operand of the comparison.
        /// @return `true` if `first` should come before `second` in the ordering, `false` otherwise.
        virtual bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second) const = 0;
    };

    /// Abstract subclass used to mark comparators as not requiring any access to graph state during comparisons.
    /// Concrete implementations that do not require access to state should be derived from this class rather than using Comparator directly.
    template <typename TEdgeData> struct StatelessComparator : public Comparator<TEdgeData>
    {
        // Empty.
    };
    
    /// Abstract subclass designed to enable querying the state of the graph during comparisons.
    /// Adds instance variables designed to enable querying the graph for additional information when comparing edges.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct StatefulComparator : public Comparator<TEdgeData>
    {
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Graph object which contains the edges being compared.
        /// It may not be modified.
        const Graph<TEdgeData>& graph;
        
        /// If `true`, specifies that the edges are taken from the destination-grouped index rather than the source-grouped index.
        bool usingDestinationIndex;
        
        /// Specifies the indexed vertex associated with the edges being compared.
        /// Each edge only specifies one end of the edge, the other being inferred from the index position.
        /// Before using this object to compare edges, the indexed vertex must be set.
        /// This way, it can be used when comparing edges to one another.
        TVertexID indexedVertex;
        
        
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------- //
        
        /// Initialization constructor, linking a graph to this comparator object.
        StatefulComparator(const Graph<TEdgeData>& graph) : graph(graph), usingDestinationIndex(), indexedVertex()
        {
            // Nothing to do here.
        }
        
        /// Initialization constructor, setting all fields.
        StatefulComparator(const Graph<TEdgeData>& graph, bool usingDestinationIndex, TVertexID indexedVertex) : graph(graph), usingDestinationIndex(usingDestinationIndex), indexedVertex(indexedVertex)
        {
            // Nothing to do here.
        }
    };
    
    /// Concrete wrapper class intended to facilitate the use of a comparator object with value-type semantics.
    /// For internal use only. Not intended to be instantiated used as a base class.
    template <typename TEdgeData> class WrappedComparator
    {
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //
        
        /// Comparator object, used to provide the actual comparisons.
        Comparator<TEdgeData>& compare;
        
        
    public:
        // -------- OPERATORS ---------------------------------------------- //
        
        /// Invokes the wrapped comparison operation.
        /// @param [in] first First operand of the comparison.
        /// @param [in] second Second operand of the comparison.
        /// @return `true` if `first` should come before `second` in the ordering, `false` otherwise.
        inline bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second) const
        {
            return compare(first, second);
        }
        
        
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------- //
        
        /// Initialization constructor, linking a comparator object to this object.
        WrappedComparator(Comparator<TEdgeData>& compare) : compare(compare)
        {
            // Nothing to do here.
        }
    };
}
