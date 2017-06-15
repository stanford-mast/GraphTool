/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Benchmark.h
 *   Functions for performing benchmark operations.
 *****************************************************************************/

#pragma once


namespace GraphTool
{
    /* -------- FUNCTIONS -------------------------------------------------- */
    
    /// Starts the benchmark.
    /// Sets the internal time counter to the current time.
    void BenchmarkStart(void);
    
    /// Stops the benchmark.
    /// Can only be called after benchmarking has been started.
    /// Resets the internal time counter.
    /// @return Milliseconds that have passed since starting.
    double BenchmarkStop(void);
}
