/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file GraphReader.cpp
 *   Implementation of the common functionality for objects capable of
 *   interpreting graph files of various formats.
 *****************************************************************************/

#include "Graph.h"
#include "GraphReader.h"
#include "Types.h"

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <silo.h>
#include <spindle.h>
#include <string>

using namespace GraphTool;


// -------- CLASS METHODS -------------------------------------------------- //
// See "GraphReader.h" for documentation.

void GraphReader::EdgeConsumer(void* arg)
{
    SGraphReadSpec* readSpec = (SGraphReadSpec*)arg;
    uint8_t currentBufferIndex = 0ull;

    while (true)
    {
        // Wait for the buffer to be filled with edges.
        spindleBarrierGlobal();

        // Check for termination.
        if (0 == readSpec->counts[currentBufferIndex])
            break;
        
        // Read the buffer into the graph.
        for (TEdgeCount i = spindleGetLocalThreadID(); i < readSpec->counts[currentBufferIndex]; i += spindleGetLocalThreadCount())
        {
            // TODO: Consume the edge at the specified index.
        }

        // Switch to the other buffer to consume in parallel with edge production.
        currentBufferIndex = (~currentBufferIndex) & 1ull;
    }
}

// --------

void GraphReader::EdgeProducer(void* arg)
{
    SGraphReadSpec* readSpec = (SGraphReadSpec*)arg;
    uint8_t currentBufferIndex = 0ull;

    while (true)
    {
        // Fill the buffer with edges.
        readSpec->counts[currentBufferIndex] = readSpec->reader->ReadEdgesToBuffer(readSpec->file, readSpec->bufs[currentBufferIndex], kGraphReadBufferSize);

        // Synchronize with consumers.
        spindleBarrierGlobal();
        
        // Check for termination.
        if (0 == readSpec->counts[currentBufferIndex])
            break;

        // Switch to the other buffer to read from file during a consumption operation.
        currentBufferIndex = (~currentBufferIndex) & 1ull;
    }
}


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "GraphReader.h" for documentation.

GraphReader::GraphReader(void)
{
    // Nothing to do here.
}

// --------

GraphReader::~GraphReader(void)
{
    // Nothing to do here.
}


// -------- INSTANCE METHODS ----------------------------------------------- //
// See "GraphReader.h" for documentation.

Graph* GraphReader::ReadGraphFromFile(const std::string& filename)
{
    // First, open the file.
    FILE* graphfile = this->OpenAndInitializeGraphFile(filename);
    if (NULL == graphfile)
        return NULL;

    // Next, allocate the graph object.
    Graph* graph = this->AllocateGraphObject();
    
    // Allocate some buffers for read data.
    void* bufs[] = { (void*)(new uint8_t[kGraphReadBufferSize]), (void*)(new uint8_t[kGraphReadBufferSize]) };
    
    // Define the graph read task.
    SGraphReadSpec readSpec;
    
    readSpec.file = graphfile;
    readSpec.graph = graph;
    readSpec.reader = this;
    readSpec.bufs[0] = bufs[0];
    readSpec.bufs[1] = bufs[1];
    readSpec.counts[0] = 0;
    readSpec.counts[1] = 0;

    // Define the parallelization strategy.
    SSpindleTaskSpec taskSpec[2];
    
    taskSpec[0].func = &EdgeProducer;
    taskSpec[0].arg = (void*)&readSpec;
    taskSpec[0].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
    taskSpec[0].numThreads = 1;
    taskSpec[0].smtPolicy = SpindleSMTPolicyPreferPhysical;
    
    taskSpec[1].func = &EdgeConsumer;
    taskSpec[1].arg = (void*)&readSpec;
    taskSpec[1].numaNode = siloGetNUMANodeForVirtualAddress(bufs[0]);
    taskSpec[1].numThreads = 0;
    taskSpec[1].smtPolicy = SpindleSMTPolicyPreferPhysical;

    // Launch the graph read task.
    spindleThreadsSpawn(taskSpec, sizeof(taskSpec) / sizeof(taskSpec[0]), true);
    
    // Clean up.
    fclose(graphfile);
    delete[] bufs[0];
    delete[] bufs[1];

    return NULL;
}
