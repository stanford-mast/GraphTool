/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReader.cpp
 *   Implementation of common functionality for reading and interpreting graph
 *   files of various formats.
 *****************************************************************************/

#include "Edge.hpp"
#include "EdgeIndex.hpp"
#include "Graph.hpp"
#include "GraphReader.h"
#include "SequenceComparator.hpp"
#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <silo.h>
#include <spindle.h>

using namespace GraphTool;


// -------- LOCAL TYPE DEFINITIONS --------------------------------- //

namespace GraphTool
{
    /// Provides all information needed to specify a graph read operation.
    template <typename TEdgeData> struct SGraphReadSpec
    {
        FILE* file;                                                         ///< File handle.
        Graph<TEdgeData>* graph;                                            ///< Graph object to be filled.
        EdgeIndex<TEdgeData>* edgeIndicesByDestination;                     ///< Array of destination-grouped edge indices. One is created per thread.
        EdgeIndex<TEdgeData>* edgeIndicesBySource;                          ///< Array of source-grouped edge indices. One is created per thread.
        GraphReader<TEdgeData>* reader;                                     ///< Graph reader object.
        SEdgeBufferData<TEdgeData>* bufs[2];                                ///< Edge data buffers.
        TEdgeCount counts[2];                                               ///< Edge data buffer counts.
        bool readSuccessfulSoFar;                                           ///< Indicates the continued success of the read operation.
    };
}


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "GraphReader.h" for documentation.

template <typename TEdgeData> GraphReader<TEdgeData>::GraphReader(void)
{
    // Nothing to do here.
}

// --------

template <typename TEdgeData> GraphReader<TEdgeData>::~GraphReader(void)
{
    // Nothing to do here.
}


// -------- CLASS METHODS -------------------------------------------------- //
// See "GraphReader.h" for documentation.

template <typename TEdgeData> void GraphReader<TEdgeData>::EdgeConsumer(void* arg)
{
    SGraphReadSpec<TEdgeData>* readSpec = (SGraphReadSpec<TEdgeData>*)arg;
    uint32_t currentIndex = 0;
    uint64_t totalEdgesRead = 0ull;

    // Allocate each thread's edge index and initialize the edge counter. Only one thread needs to do this.
    if (0 == spindleGetLocalThreadID())
    {
        readSpec->edgeIndicesByDestination = new EdgeIndex<TEdgeData>[spindleGetLocalThreadCount()];
        readSpec->edgeIndicesBySource = new EdgeIndex<TEdgeData>[spindleGetLocalThreadCount()];
    }

    spindleBarrierLocal();

    // Iteratively consume edges that the edge producer loads into the edge buffers.
    while (true)
    {
        // Wait for the buffer to be filled with edges.
        spindleBarrierGlobal();

        // Check for termination.
        if (0 == readSpec->counts[currentIndex])
            break;

        // Check for I/O errors.
        if (false == readSpec->readSuccessfulSoFar)
            return;

        // Read the buffer into the graph.
        for (TEdgeCount i = spindleGetLocalThreadID(); i < readSpec->counts[currentIndex]; i += spindleGetLocalThreadCount())
        {
            // Add each edge assigned to this thread to the edge index.
            Edge<TEdgeData> edgeInfo;

            edgeInfo.FillFromDestinationEdgeBuffer(readSpec->bufs[currentIndex][i]);
            edgeInfo.seq = totalEdgesRead + i;
            readSpec->edgeIndicesBySource[spindleGetLocalThreadID()].InsertEdge(readSpec->bufs[currentIndex][i].sourceVertex, edgeInfo);

            edgeInfo.vertex = readSpec->bufs[currentIndex][i].sourceVertex;
            edgeInfo.seq = totalEdgesRead + i;
            readSpec->edgeIndicesByDestination[spindleGetLocalThreadID()].InsertEdge(readSpec->bufs[currentIndex][i].destinationVertex, edgeInfo);
        }

        // Update the total number of edges read so far.
        totalEdgesRead += readSpec->counts[currentIndex];

        // Switch to the other buffer to consume in parallel with edge production.
        currentIndex = (~currentIndex) & 1;
    }

    // Merge all edge indices together into a single final edge index.
    // This will be done using a parallel tree merge pattern.
    // The value of this variable is an indicator of the current level of the tree.
    // Each iteration it is multiplied by two to indicate the number of consecutive edge indices that end up being merged together.
    // At the end, all edges are merged into the graph, and within each indexed vertex edges will appear in the same order as they appeared in the input file.
    currentIndex = 2;

    while (currentIndex < (spindleGetLocalThreadCount() << 1))
    {
        SequenceAscendingComparator<TEdgeData> comparator;

        // Each iteration, the total number of merged indices doubles and hence the number of threads needed halves.
        const uint32_t selectionMask = currentIndex - 1;
        const uint32_t selectionValue = spindleGetLocalThreadID() & selectionMask;
        const uint32_t mergeWithOffset = (currentIndex >> 1);

        // The thread that will do the source merge is the first in the group.
        const uint32_t selectionValueMergeSource = 0;

        // The thread that will do the destination merge is either going to be the second or third thread in the group.
        // In the first iteration, or if less than three threads are available, always use the second thread in the group because this is all that is available.
        // In the subsequent iterations, in which some threads are sitting idle, use the third thread in the group.
        // This task is created with preference for logical cores, so using the third thread in the group means potentially using a different physical core to merge destinations than to merge sources.
        const uint32_t selectionValueMergeDestination = (selectionMask < 3 || spindleGetLocalThreadCount() < 3 ? 1 : 2);

        spindleBarrierLocal();

        // Only some threads are active during the current iteration.
        // This formula is equivalent to checking if the local thread ID is divisible by the current tree level index.
        // Additionally parallelize across source and destination vertex indices.
        // File ordering is preserved using the sequence number field of each edge.
        // Since each individual index is already sorted by sequence number just by virtue of reading the file in order, merging by sequence number produces the correct order.
        if (selectionValueMergeSource == selectionValue)
        {
            if ((spindleGetLocalThreadID() - selectionValueMergeSource + mergeWithOffset) < spindleGetLocalThreadCount())
                readSpec->edgeIndicesBySource[spindleGetLocalThreadID() - selectionValueMergeSource].MergeEdges(readSpec->edgeIndicesBySource[spindleGetLocalThreadID() - selectionValueMergeSource + mergeWithOffset], comparator);
        }
        else if (selectionValueMergeDestination == selectionValue)
        {
            if ((spindleGetLocalThreadID() - selectionValueMergeDestination + mergeWithOffset) < spindleGetLocalThreadCount())
                readSpec->edgeIndicesByDestination[spindleGetLocalThreadID() - selectionValueMergeDestination].MergeEdges(readSpec->edgeIndicesByDestination[spindleGetLocalThreadID() - selectionValueMergeDestination + mergeWithOffset], comparator);
        }

        currentIndex <<= 1;
    }

    spindleBarrierLocal();

    // Merging complete. Set the graph to have the merged edges.
    if (0 == spindleGetLocalThreadID())
        readSpec->graph->SetEdgeIndices(readSpec->edgeIndicesByDestination[0], readSpec->edgeIndicesBySource[0]);
}

// --------

template <typename TEdgeData> void GraphReader<TEdgeData>::EdgeProducer(void* arg)
{
    SGraphReadSpec<TEdgeData>* readSpec = (SGraphReadSpec<TEdgeData>*)arg;
    uint32_t currentBufferIndex = 0;

    while (true)
    {
        // Fill the buffer with edges.
        readSpec->counts[currentBufferIndex] = readSpec->reader->ReadEdgesToBuffer(readSpec->file, readSpec->bufs[currentBufferIndex], (kGraphReadBufferSize / sizeof(SEdgeBufferData<void>)));

        // Check for any I/O errors.
        if (ferror(readSpec->file))
            readSpec->readSuccessfulSoFar = false;

        // Synchronize with consumers.
        spindleBarrierGlobal();

        // Check for termination or I/O errors detected previously.
        if (0 == readSpec->counts[currentBufferIndex] || false == readSpec->readSuccessfulSoFar)
            break;

        // Switch to the other buffer to read from file during a consumption operation.
        currentBufferIndex = (~currentBufferIndex) & 1;
    }
}


// -------- INSTANCE METHODS ----------------------------------------------- //
// See "GraphReader.h" for documentation.

template <typename TEdgeData> TEdgeCount GraphReader<TEdgeData>::ReadGraphFromFile(const char* const filename, Graph<TEdgeData>& graph)
{
    // First, open the file.
    FILE* graphfile = this->OpenAndInitializeGraphFileForRead(filename);
    if (NULL == graphfile)
        return false;

    // Allocate some buffers for read data.
    SEdgeBufferData<TEdgeData>* bufs[] = { (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]), (SEdgeBufferData<TEdgeData>*)(new uint8_t[kGraphReadBufferSize]) };

    // Define the graph read task.
    SGraphReadSpec<TEdgeData> readSpec;

    readSpec.file = graphfile;
    readSpec.graph = &graph;
    readSpec.reader = this;
    readSpec.bufs[0] = bufs[0];
    readSpec.bufs[1] = bufs[1];
    readSpec.counts[0] = 0;
    readSpec.counts[1] = 0;
    readSpec.readSuccessfulSoFar = true;

    // Define the parallelization strategy.
    SSpindleTaskSpec taskSpec[2];

    taskSpec[0].func = &EdgeProducer;
    taskSpec[0].arg = (void*)&readSpec;
    taskSpec[0].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
    taskSpec[0].numThreads = 1;
    taskSpec[0].smtPolicy = SpindleSMTPolicyPreferLogical;

    taskSpec[1].func = &EdgeConsumer;
    taskSpec[1].arg = (void*)&readSpec;
    taskSpec[1].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
    taskSpec[1].numThreads = 0;
    taskSpec[1].smtPolicy = SpindleSMTPolicyPreferLogical;

    // Launch the graph read task.
    spindleThreadsSpawn(taskSpec, sizeof(taskSpec) / sizeof(taskSpec[0]), true);

    // Clean up.
    fclose(graphfile);
    delete[] bufs[0];
    delete[] bufs[1];

    return readSpec.readSuccessfulSoFar;
}


// -------- EXPLICIT TEMPLATE INSTANTIATIONS ------------------------------- //

template class GraphReader<void>;
template class GraphReader<uint64_t>;
template class GraphReader<double>;
