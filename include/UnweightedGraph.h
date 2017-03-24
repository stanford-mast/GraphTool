/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file UnweightedGraph.h
 *   Declaration of functionality needed to represent graphs without edge
 *   weights.
 *****************************************************************************/

#pragma once

#include "Graph.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>


namespace GraphTool
{
    /// Represents a graph that does not contain edge weights.
    class UnweightedGraph : public Graph
    {
    public:
        // -------- CONSTANTS ------------------------------------------------------ //

        /// One of the possible options to specify when exporting edges.
        /// Indicates that source vertices should be excluded from the output.
        static const uint64_t kExportOptionsExcludeSource = 1ull;

        /// One of the possible options to specify when exporting edges.
        /// Indicates that destination vertices should be excluded from the output.
        static const uint64_t kExportOptionsExcludeDestination = 2ull;
        
        
        // -------- TYPE DEFINITIONS ----------------------------------------------- //
        
        /// Specifies the structure of edge data expected in incoming buffers.
        /// Used when adding edges to the graph.
        struct SEdge
        {
            TVertexID sourceVertex;                                         ///< Identifier of the source vertex.
            TVertexID destinationVertex;                                    ///< Identifier of the destination vertex.
        };
        
        
    private:
        // -------- INSTANCE VARIABLES --------------------------------------------- //

        /// Destination-grouped vertex data structure.
        /// Maps from a destination vertex ID to a set of vertices from which in-edges exist.
        std::map<TVertexID, std::set<TVertexID>> verticesByDestination;

        /// Source-grouped vertex data structure.
        /// Maps from a source vertex ID to a set of vertices to which out-edges exist.
        std::map<TVertexID, std::set<TVertexID>> verticesBySource;


    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Default constructor.
        UnweightedGraph(void);


        // -------- CONCRETE INSTANCE METHODS -------------------------------------- //
        // See "Graph.h" for documentation.
        
        virtual void AddInEdges(void* buffer, size_t count);
        virtual void AddOutEdges(void* buffer, size_t count);
        virtual TEdgeCount ExportVertexInEdges(void* buffer, TVertexID vertex, uint64_t options);
        virtual TEdgeCount ExportVertexOutEdges(void* buffer, TVertexID vertex, uint64_t options);
        virtual size_t GetExportEdgeSizeWithOptions(uint64_t options);
        virtual TVertexCount GetMaximumIndegree(void);
        virtual TVertexCount GetMaximumOutdegree(void);
        virtual TEdgeCount GetNumEdges(void);
        virtual TVertexCount GetNumVertices(void);
        virtual TVertexCount GetVertexIndegree(TVertexID vertex);
        virtual TVertexCount GetVertexOutdegree(TVertexID vertex);
        virtual void RemoveEdge(TVertexID fromVertex, TVertexID toVertex);
        virtual void RemoveVertex(TVertexID vertex);
    };
}
