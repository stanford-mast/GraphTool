/*****************************************************************************
* Grazelle
*      High performance, hardware-optmized graph processing engine.
*      Targets a single machine with one or more x86-based sockets.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* (c) 2015-2017
*****************************************************************************
* Benchmark.cpp
*      Platform-dependent implementation of functions for performing
*      benchmarking operations.
*****************************************************************************/

#include "VersionInfo.h"

#ifdef __PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <time.h>
#endif


namespace GraphTool
{
    /* -------- LOCALS ----------------------------------------------------- */
    
    /// Keeps track of the number of milliseconds that have passed during benchmarking.
    static double timeCounter = -1.0;
    
    
    /* -------- FUNCTIONS -------------------------------------------------- */
    // See "Benchmark.h" for documentation.
    
    void BenchmarkStart()
    {
    #ifdef __PLATFORM_WINDOWS
        timeCounter = (double)GetTickCount64();
    #else
        struct timespec time_val;
        clock_gettime(CLOCK_MONOTONIC_RAW, &time_val);
        timeCounter = ((double)time_val.tv_sec * 1000.0) + ((double)time_val.tv_nsec / 1000000.0);
    #endif
    }
    
    // ---------
    
    double BenchmarkStop()
    {
        double timeElapsed = 0.0;
    
    #ifdef __PLATFORM_WINDOWS
        timeElapsed = (double)GetTickCount64() - timeCounter;
    #else
        struct timespec time_val;
        clock_gettime(CLOCK_MONOTONIC_RAW, &time_val);
        timeElapsed = ((double)time_val.tv_sec * 1000.0) + ((double)time_val.tv_nsec / 1000000.0) - timeCounter;
    #endif
    
        timeCounter = -1.0;
    
        return timeElapsed;
    }
}
