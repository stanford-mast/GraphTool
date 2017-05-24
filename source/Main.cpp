/*****************************************************************************
* GraphTool
*   Tools for manipulating graphs.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file Main.cpp
*   Program entry point.
*****************************************************************************/

#include "Options.h"

using namespace GraphTool;


// -------- FUNCTIONS ------------------------------------------------------ //

/// Program entry point.
/// @param [in] argc Number of command-line arguments.
/// @param [in] argv Command-line argument strings.
/// @return Program exit code. 0 means successful termination, anything else indicates an error.
int main(int argc, char* argv[])
{
    // Submit all command-line options for parsing.
    bool commandLineOptionsSuccess = true;
    
    for (int i = 1; (i < argc) && (true == commandLineOptionsSuccess); ++i)
        commandLineOptionsSuccess = Options::SubmitOption(argv[0], argv[i]);

    if (!commandLineOptionsSuccess)
        return 1;
    
    // Validate that all required values are present.
    commandLineOptionsSuccess = Options::ValidateOptions(argv[0]);

    if (!commandLineOptionsSuccess)
        return 2;
    
    return 0;
}
