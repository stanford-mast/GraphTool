/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Edge.hpp
 *   Implementation of a templated data structure that holds data for a single
 *   edge in a graph.
 *****************************************************************************/

#pragma once

#include "Types.h"

#include <map>
#include <list>


namespace GraphTool
{
    /// Represents an individual edge within an edge index data structure.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct Edge
    {
        // -------- INSTANCE VARIABLES --------------------------------------------- //

        /// Identifies this edge based on its order of occurrence within the input file.
        TEdgeID seq;
        
        /// Identifier of one end of the edge. The other end is inferred from position within the index.
        TVertexID vertex;

        /// Data associated with the edge, such as a weight.
        TEdgeData edgeData;


        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Default constructor.
        Edge(void) : seq(), vertex(), edgeData()
        {
            // Nothing to do here.
        }


        // -------- OPERATORS ---------------------------------------------- //

        /// Compares this instance with another.
        /// Equality is only determined by vertex, not by edge data.
        /// @param [in] other Reference to the other instance.
        /// @return `true` if this instance is equal to the other instance, `false` otherwise.
        inline bool operator==(const Edge& other) const
        {
            return (this->vertex == other.vertex);
        }


        // -------- INSTANCE METHODS --------------------------------------- //

        /// Exports the information from this structure to edge buffer data, using this structure's vertex as a destination vertex.
        /// @param [in] sourceVertex Source vertex to fill into the edge buffer.
        /// @param [in] edgeBuffer Edge buffer to which to export.
        inline void ExportToDestinationEdgeBuffer(TVertexID sourceVertex, SEdgeBufferData<TEdgeData>& edgeBuffer) const
        {
            edgeBuffer.sourceVertex = sourceVertex;
            edgeBuffer.destinationVertex = this->vertex;
            edgeBuffer.edgeData = this->edgeData;
        }

        /// Exports the information from this structure to edge buffer data, using this structure's vertex as a source vertex.
        /// @param [in] destinationVertex Destination vertex to fill into the edge buffer.
        /// @param [in] edgeBuffer Edge buffer to which to export.
        inline void ExportToSourceEdgeBuffer(TVertexID destinationVertex, SEdgeBufferData<TEdgeData>& edgeBuffer) const
        {
            edgeBuffer.sourceVertex = this->vertex;
            edgeBuffer.destinationVertex = destinationVertex;
            edgeBuffer.edgeData = this->edgeData;
        }
        
        /// Fills information in this structure from edge buffer data, using the destination vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromDestinationEdgeBuffer(SEdgeBufferData<TEdgeData>& edgeBuffer)
        {
            this->vertex = edgeBuffer.destinationVertex;
            this->edgeData = edgeBuffer.edgeData;
        }

        /// Fills information in this structure from edge buffer data, using the source vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromSourceEdgeBuffer(SEdgeBufferData<TEdgeData>& edgeBuffer)
        {
            this->vertex = edgeBuffer.sourceVertex;
            this->edgeData = edgeBuffer.edgeData;
        }
    };

    /// Represents an individual edge within an edge index data structure, specialized for unweighted graphs.
    template <> struct Edge<void>
    {
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Identifies this edge based on its order of occurrence within the input file.
        TEdgeID seq;
        
        /// Identifier of one end of the edge. The other end is inferred from position within the index.
        TVertexID vertex;


        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        Edge(void) : seq(), vertex()
        {
            // Nothing to do here.
        }

        /// Enables implicit conversion to this type from a vertex identifier.
        Edge(TVertexID vertex) : seq(), vertex(vertex)
        {
            // Nothing to do here.
        }


        // -------- OPERATORS ---------------------------------------------- //

        /// Compares this instance with another.
        /// Equality is only determined by vertex, not by edge data.
        /// @param [in] other Reference to the other instance.
        /// @return `true` if this instance is equal to the other instance, `false` otherwise.
        inline bool operator==(const Edge& other) const
        {
            return (this->vertex == other.vertex);
        }


        // -------- INSTANCE METHODS --------------------------------------- //

        /// Exports the information from this structure to edge buffer data, using this structure's vertex as a destination vertex.
        /// @param [in] sourceVertex Source vertex to fill into the edge buffer.
        /// @param [in] edgeBuffer Edge buffer to which to export.
        inline void ExportToDestinationEdgeBuffer(TVertexID sourceVertex, SEdgeBufferData<void>& edgeBuffer) const
        {
            edgeBuffer.sourceVertex = sourceVertex;
            edgeBuffer.destinationVertex = this->vertex;
        }

        /// Exports the information from this structure to edge buffer data, using this structure's vertex as a source vertex.
        /// @param [in] destinationVertex Destination vertex to fill into the edge buffer.
        /// @param [in] edgeBuffer Edge buffer to which to export.
        inline void ExportToSourceEdgeBuffer(TVertexID destinationVertex, SEdgeBufferData<void>& edgeBuffer) const
        {
            edgeBuffer.sourceVertex = this->vertex;
            edgeBuffer.destinationVertex = destinationVertex;
        }
        
        /// Fills information in this structure from edge buffer data, using the destination vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromDestinationEdgeBuffer(SEdgeBufferData<void>& edgeBuffer)
        {
            this->vertex = edgeBuffer.destinationVertex;
        }

        /// Fills information in this structure from edge buffer data, using the source vertex.
        /// @param [in] edgeBuffer Edge buffer data to use.
        inline void FillFromSourceEdgeBuffer(SEdgeBufferData<void>& edgeBuffer)
        {
            this->vertex = edgeBuffer.sourceVertex;
        }
    };

    /// Function object for comparing two edges, using a less-than operator, based on their sequence numbers.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct EdgeCompareLessBySeq
    {
        inline bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second) const
        {
            return (first.seq < second.seq);
        }
    };

    /// Function object for comparing two edges, using a less-than operator, based on their vertex identifiers.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    template <typename TEdgeData> struct EdgeCompareLessByVertex
    {
        inline bool operator()(Edge<TEdgeData>& first, Edge<TEdgeData>& second) const
        {
            return (first.vertex < second.vertex);
        }
    };

    /// Convenience alias for edges that do not use edge data or weights.
    typedef Edge<void> UnweightedEdge;
}
