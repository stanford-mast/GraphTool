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

#include "OptionContainer.h"
#include "Options.h"
#include "VersionInfo.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

using namespace GraphTool;


// -------- CONSTANTS ------------------------------------------------------ //

namespace GraphTool
{
    /// Command-line option that specifies edge data type.
    static const std::string kOptionEdgeData = "edgedata";

    /// Command-line option that specifies input file name.
    static const std::string kOptionInputFile = "inputfile";

    /// Command-line option that specifies input file format.
    static const std::string kOptionInputFormat = "inputformat";

    /// Command-line option that specifies input processing options.
    static const std::string kOptionInputOptions = "inputoptions";

    /// Command-line option that specifies output file name.
    static const std::string kOptionOutputFile = "outputfile";

    /// Command-line option that specifies output file format.
    static const std::string kOptionOutputFormat = "outputformat";

    /// Command-line option that specifies output processing options.
    static const std::string kOptionOutputOptions = "outputoptions";
}


// -------- LOCALS --------------------------------------------------------- //

namespace GraphTool
{
    static std::vector<std::string> cmdlineHelpStrings = {
    #ifdef __PLATFORM_WINDOWS
        "?",
    #endif
        "help",
    };

    static std::vector<std::string> cmdlinePrefixStrings = {
    #ifdef __PLATFORM_WINDOWS
        "/",
    #endif
        "--",
    };

    static std::map<std::string, OptionContainer*> cmdlineSpecifiedOptions = {
        { kOptionEdgeData,                          new OptionContainer("void") },
        { kOptionInputFile,                         new OptionContainer(EOptionValueType::OptionValueTypeString) },
        { kOptionInputFormat,                       new OptionContainer(EOptionValueType::OptionValueTypeString) },
        { kOptionInputOptions,                      new OptionContainer("") },
        { kOptionOutputFile,                        new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputFormat,                      new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputOptions,                     new OptionContainer("", OptionContainer::kUnlimitedValueCount) },
    };

    static std::map<std::string, std::string> cmdlineSupportedAliases = {
        { "noedgedata",                             "edgedata=void" },
    };
}


// -------- FUNCTIONS ------------------------------------------------------ //

/// Program entry point.
/// @param [in] argc Number of command-line arguments.
/// @param [in] argv Command-line argument strings.
/// @return Program exit code. 0 means successful termination, anything else indicates an error.
int main(int argc, char* argv[])
{
    const std::string cmdline = argv[0];
    Options commandLineOptions(cmdline, cmdlineSpecifiedOptions, &cmdlineSupportedAliases, &cmdlinePrefixStrings, &cmdlineHelpStrings);

    // Submit all command-line options for parsing.
    bool commandLineOptionsSuccess = true;
    
    for (int i = 1; (i < argc) && (true == commandLineOptionsSuccess); ++i)
        commandLineOptionsSuccess = commandLineOptions.SubmitOption(argv[i]);

    if (!commandLineOptionsSuccess)
        return 1;
    
    // Validate that all required values are present and required relationships hold.
    commandLineOptionsSuccess = (commandLineOptions.ValidateOptions() && commandLineOptions.VerifyEqualValueCount(kOptionOutputFile.c_str(), kOptionOutputFormat.c_str()));

    if (!commandLineOptionsSuccess)
        return 2;
    
    return 0;
}
