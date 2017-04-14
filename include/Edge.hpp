/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Edge.hpp
 *   Implementation of a single edge data structure.
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
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Identifies this edge based on its order of occurrence within the input file.
        TEdgeID seq;
        
        /// Identifier of one end of the edge. The other end is inferred from position within the index.
        TVertexID vertex;

        /// Data associated with the edge, such as a weight.
        TEdgeData edgeData;


        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor.
        Edge(void) : seq(), vertex(), edgeData()
        {
            // Nothing to do here.
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
    
    /// Convenience alias for edges that do not use edge data or weights.
    typedef Edge<void> UnweightedEdge;
}
