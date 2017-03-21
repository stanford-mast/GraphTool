/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Graph.h
 *   Declaration of the common interface to an object that represents a graph.
 *****************************************************************************/

#pragma once

#include <cstdint>


namespace GraphTool
{
    /// Specifies the type to use for identifying vertices.
    typedef uint64_t TVertexID;

    /// Specifies the function type to use for comparing vertices when sorting.
    /// This function defines a weak element ordering.
    /// It should return `true` if `first` comes before `second` or `false` otherwise.
    typedef bool (* TVertexCompareFunc)(TVertexID first, TVertexID second);
    
    /// Interface specification to an object type used to represent a graph.
    /// Abstract base class.
    class Graph
    {
    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Default constructor.
        Graph();

        /// Default destructor.
        virtual ~Graph();


        // -------- INSTANCE METHODS ----------------------------------------------- //

        /// Adds an edge to the graph.
        /// @param [in] fromVertex Identifies the source vertex of the edge.
        /// @param [in] toVertex Identifies the destination vertex of the edge.
        virtual void AddEdge(TVertexID fromVertex, TVertexID toVertex) = 0;

        /// Removes an edge from the graph.
        /// @param [in] fromVertex Identifies the source vertex of the edge.
        /// @param [in] toVertex Identifies the destination vertex of the edge.
        virtual void RemoveEdge(TVertexID fromVertex, TVertexID toVertex) = 0;

        /// Sorts the vertices in the graph within their respective groupings (subclass-defined concept).
        /// For example, if vertices are grouped by source, sorts destinations within each source, and vice versa if grouped by destination.
        /// Requires a user-specified comparator function, which must be completely thread-safe.
        /// @param [in] comparator Binary predicate to use for comparing vertices.
        virtual void SortVertices(TVertexCompareFunc comparator) = 0;

        /// Sorts the vertices in the graph, providing a built-in comparator for producing an ascending sort.
        /// Vertices with lower identifiers occur earlier.
        /// See #SortVertices for more information.
        void SortVerticesAscending(void);

        /// Sorts the vertices in the graph, providing a built-in comparator for producing a descending sort.
        /// Vertices with higher identifiers occur earlier.
        /// See #SortVertices for more information.
        void SortVerticesDescending(void);
    };
}
