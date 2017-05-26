/*****************************************************************************
* GraphTool
*   Tools for manipulating graphs.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file Main.cpp
*   Program entry point and main control flow.
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


// -------- LOCAL TYPE DEFINITIONS ----------------------------------------- //

namespace GraphTool
{
    enum EEdgeDataType : int64_t
    {
        EdgeDataTypeVoid,                                                   ///< No edge data (i.e. an unweighted graph).
        EdgeDataTypeInteger,                                                ///< Edge data are integral. Currently this means 64-bit unsigned.
        EdgeDataTypeFloatingPoint                                           ///< Edge data are floating-point. Currently this means double-precision.
    };
}


// -------- LOCALS --------------------------------------------------------- //

namespace GraphTool
{
    /// Lists the command-line options that can be used to request help.
    static const std::vector<std::string> cmdlineHelpStrings = {
    #ifdef __PLATFORM_WINDOWS
        "?",
    #endif
        "help",
    };

    /// Lists the prefixes that are allowed at the start of command-line options.
    static const std::vector<std::string> cmdlinePrefixStrings = {
    #ifdef __PLATFORM_WINDOWS
        "/",
    #endif
        "--",
    };

    /// Holds a mapping from command-line option string to edge data type enumeration value.
    static const std::map<std::string, int64_t> cmdlineEdgeDataTypeStrings = {
        { "void",                                   EEdgeDataType::EdgeDataTypeVoid },
        { "none",                                   EEdgeDataType::EdgeDataTypeVoid },
        { "int",                                    EEdgeDataType::EdgeDataTypeInteger },
        { "integer",                                EEdgeDataType::EdgeDataTypeInteger },
        { "uint",                                   EEdgeDataType::EdgeDataTypeInteger },
        { "float",                                  EEdgeDataType::EdgeDataTypeFloatingPoint },
        { "double",                                 EEdgeDataType::EdgeDataTypeFloatingPoint },
    };
    
    /// Holds all specified command-line options, mapped from the strings used to identify them.
    static std::map<std::string, OptionContainer*> cmdlineSpecifiedOptions = {
        { kOptionEdgeData,                          new EnumOptionContainer(cmdlineEdgeDataTypeStrings, EEdgeDataType::EdgeDataTypeVoid, 1) },
        { kOptionInputFile,                         new OptionContainer(EOptionValueType::OptionValueTypeString) },
        { kOptionInputFormat,                       new OptionContainer(EOptionValueType::OptionValueTypeString) },
        { kOptionInputOptions,                      new OptionContainer("") },
        { kOptionOutputFile,                        new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputFormat,                      new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputOptions,                     new OptionContainer("", OptionContainer::kUnlimitedValueCount) },
    };

    /// Holds all supported command-line option aliases, mapped from the strings that could be specified on the command-line to the strings they represent.
    static const std::map<std::string, std::string> cmdlineSupportedAliases = {
        { "noedgedata",                             "edgedata=void" },
    };
}


// -------- FUNCTIONS ------------------------------------------------------ //

/// Runs the main graph manipulation operations.
/// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
template <typename TEdgeData> int run(void)
{
    return 0;
}

/// Program entry point.
/// @param [in] argc Number of command-line arguments.
/// @param [in] argv Command-line argument strings.
/// @return Program exit code. 0 means successful termination, anything else indicates an error.
int main(int argc, const char* argv[])
{
    const std::string cmdline = argv[0];
    Options commandLineOptions(cmdline, cmdlineSpecifiedOptions, &cmdlineSupportedAliases, &cmdlinePrefixStrings, &cmdlineHelpStrings);

    // Submit all command-line options for parsing.
    if (!(commandLineOptions.FillFromStringArray(argc - 1, &argv[1])))
        return 1;
    
    // Validate that all required values are present and required relationships hold.
    if (!(commandLineOptions.ValidateOptions() && commandLineOptions.VerifyEqualValueCount(kOptionOutputFile.c_str(), kOptionOutputFormat.c_str())))
        return 2;
    
    // Branch based on the edge data type specified.
    const OptionContainer* edgeDataOptionValue = commandLineOptions.GetOptionValues(kOptionEdgeData);
    int64_t edgeDataValue = 0ll;

    if (NULL == edgeDataOptionValue)
        return 3;

    if (!(edgeDataOptionValue->QueryValue(edgeDataValue)))
        return 4;
    
    switch ((EEdgeDataType)edgeDataValue)
    {
    case EEdgeDataType::EdgeDataTypeVoid:
        return run<void>();

    case EEdgeDataType::EdgeDataTypeInteger:
        return run<uint64_t>();

    case EEdgeDataType::EdgeDataTypeFloatingPoint:
        return run<double>();

    default:
        return 5;
    }
}
