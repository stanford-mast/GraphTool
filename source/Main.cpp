/*****************************************************************************
* GraphTool
*   Tools for manipulating graphs.
*****************************************************************************
* Authored by Samuel Grossman
* Department of Electrical Engineering, Stanford University
* Copyright (c) 2016-2017
*************************************************************************//**
* @file Main.cpp
*   Program entry point and primary control flow.
*****************************************************************************/

#include "Graph.h"
#include "GraphReaderFactory.h"
#include "GraphTransformFactory.h"
#include "GraphWriterFactory.h"
#include "IGraphReader.h"
#include "IGraphTransform.h"
#include "IGraphWriter.h"
#include "OptionContainer.h"
#include "Options.h"
#include "VersionInfo.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <vector>


namespace GraphTool
{
    // -------- CONSTANTS -------------------------------------------------- //

    /// Command-line option that specifies input file name.
    static const std::string kOptionInputFile = "inputfile";

    /// Command-line option that specifies input file format.
    static const std::string kOptionInputFormat = "inputformat";
    
    /// Command-line option that specifies input weight type.
    static const std::string kOptionInputWeights = "inputweights";

    /// Command-line option that specifies input processing options.
    static const std::string kOptionInputOptions = "inputoptions";

    /// Command-line option that specifies output file name.
    static const std::string kOptionOutputFile = "outputfile";

    /// Command-line option that specifies output file format.
    static const std::string kOptionOutputFormat = "outputformat";
    
    /// Command-line option that specifies output weight type.
    static const std::string kOptionOutputWeights = "outputweights";
    
    /// Command-line option that specifies output vertex grouping.
    static const std::string kOptionOutputGrouping = "outputgroup";
    
    /// Command-line option that specifies output processing options.
    static const std::string kOptionOutputOptions = "outputoptions";
    
    /// Command-line option that specifies a graph transformation operation.
    static const std::string kOptionTransform = "transform";


    // -------- LOCALS ----------------------------------------------------- //

    /// Application version information string.
    static const std::string applicationVersionString = __PROGRAM_NAME " v" __PROGRAM_VERSION " for " __PLATFORM_NAME ", compiled on " __DATE__ " at " __TIME__ ".\n";
    
    /// Holds a mapping from edge data type to string.
    static const std::map<EEdgeDataType, std::string> edgeDataTypeStrings = {
        { EEdgeDataType::EdgeDataTypeVoid,                                  "unweighted" },
        { EEdgeDataType::EdgeDataTypeInteger,                               "integer-weighted" },
        { EEdgeDataType::EdgeDataTypeFloatingPoint,                         "floating-point-weighted" },
    };
    
    /// Holds a mapping from graph file result code to error string.
    static const std::map<EGraphResult, std::string> graphErrorStrings = {
        { EGraphResult::GraphResultSuccess,                                 "Success" },
        { EGraphResult::GraphResultErrorNoMemory,                           "Failed to allocate memory" },
        { EGraphResult::GraphResultErrorCannotOpenFile,                     "Unable to open file" },
        { EGraphResult::GraphResultErrorIO,                                 "I/O error" },
        { EGraphResult::GraphResultErrorFormat,                             "Graph format error" },
        { EGraphResult::GraphResultErrorUnknown,                            "Unknown error" },
    };
    
    /// Lists the command-line options that can be used to request help.
    static const std::vector<std::string> cmdlineHelpStrings = {
#ifdef __PLATFORM_WINDOWS
        "?",
#endif
        "help",
    };
    
    /// Lists the command-line options that can be used to request the application version.
    static const std::vector<std::string> cmdlineVersionStrings = {
        "version",
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
        { "void",                                                           EEdgeDataType::EdgeDataTypeVoid },
        { "none",                                                           EEdgeDataType::EdgeDataTypeVoid },
        { "unweighted",                                                     EEdgeDataType::EdgeDataTypeVoid },
        { "Void",                                                           EEdgeDataType::EdgeDataTypeVoid },
        { "None",                                                           EEdgeDataType::EdgeDataTypeVoid },
        { "Unweighted",                                                     EEdgeDataType::EdgeDataTypeVoid },

        { "int",                                                            EEdgeDataType::EdgeDataTypeInteger },
        { "integer",                                                        EEdgeDataType::EdgeDataTypeInteger },
        { "uint",                                                           EEdgeDataType::EdgeDataTypeInteger },
        { "Int",                                                            EEdgeDataType::EdgeDataTypeInteger },
        { "Integer",                                                        EEdgeDataType::EdgeDataTypeInteger },
        { "Uint",                                                           EEdgeDataType::EdgeDataTypeInteger },
        { "UInt",                                                           EEdgeDataType::EdgeDataTypeInteger },

        { "float",                                                          EEdgeDataType::EdgeDataTypeFloatingPoint },
        { "floatingpoint",                                                  EEdgeDataType::EdgeDataTypeFloatingPoint },
        { "double",                                                         EEdgeDataType::EdgeDataTypeFloatingPoint },
        { "Float",                                                          EEdgeDataType::EdgeDataTypeFloatingPoint },
        { "Floatingpoint",                                                  EEdgeDataType::EdgeDataTypeFloatingPoint },
        { "FloatingPoint",                                                  EEdgeDataType::EdgeDataTypeFloatingPoint },
        { "Double",                                                         EEdgeDataType::EdgeDataTypeFloatingPoint },
    };
    
    /// Holds a mapping from command-line option string to output edge grouping value.
    static const std::map<std::string, int64_t> cmdlineOutputGroupingEnum = {
        { "s",                                                              0ll },
        { "src",                                                            0ll },
        { "source",                                                         0ll },
        { "sourcevertex",                                                   0ll },
        { "S",                                                              0ll },
        { "Src",                                                            0ll },
        { "Source",                                                         0ll },
        { "Sourcevertex",                                                   0ll },
        { "SourceVertex",                                                   0ll },
        
        { "d",                                                              1ll },
        { "dst",                                                            1ll },
        { "dest",                                                           1ll },
        { "destination",                                                    1ll },
        { "destinationvertex",                                              1ll },
        { "D",                                                              1ll },
        { "Dst",                                                            1ll },
        { "Dest",                                                           1ll },
        { "Destination",                                                    1ll },
        { "Destinationvertex",                                              1ll },
        { "DestinationVertex",                                              1ll },
    };

    /// Holds all specified command-line options, mapped from the strings used to identify them.
    static std::map<std::string, OptionContainer*> cmdlineSpecifiedOptions = {
        { kOptionInputFile,                                                 new OptionContainer(EOptionValueType::OptionValueTypeString) },
        { kOptionInputFormat,                                               new EnumOptionContainer(*(GraphReaderFactory::GetGraphReaderStrings()), OptionContainer::kUnlimitedValueCount) },
        { kOptionInputWeights,                                              new EnumOptionContainer(cmdlineEdgeDataTypeStrings, EEdgeDataType::EdgeDataTypeVoid, 1) },
        { kOptionInputOptions,                                              new OptionContainer("") },
        { kOptionOutputFile,                                                new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputFormat,                                              new EnumOptionContainer(*(GraphWriterFactory::GetGraphWriterStrings()), OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputWeights,                                             new EnumOptionContainer(cmdlineEdgeDataTypeStrings, EEdgeDataType::EdgeDataTypeVoid, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputGrouping,                                            new EnumOptionContainer(cmdlineOutputGroupingEnum, 0ll, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputOptions,                                             new OptionContainer("", OptionContainer::kUnlimitedValueCount) },
        { kOptionTransform,                                                 new EnumOptionContainer(*(GraphTransformFactory::GetGraphTransformStrings()), INT64_MAX, OptionContainer::kUnlimitedValueCount) },
    };
    
    /// Dynamically builds the documentation string to use to display help to the user.
    /// @param [in] cmdline Command-line path to display in the string.
    /// @return Fully-formatted documentation string.
    std::string MakeDocumentationString(const char* const cmdline)
    {
        // Build the usage line.
        std::string docstring = "Usage: ";
        docstring += cmdline;
        docstring += " [options]\n";
        
        // Build any subsequent usage lines.
        if (0 != cmdlineHelpStrings.size())
        {
            docstring += "       ";
            docstring += cmdline;
            docstring += " ";
            docstring += cmdlinePrefixStrings[0];
            docstring += cmdlineHelpStrings[0];
            docstring += "\n";
        }
        
        if (0 != cmdlineVersionStrings.size())
        {
            docstring += "       ";
            docstring += cmdline;
            docstring += " ";
            docstring += cmdlinePrefixStrings[0];
            docstring += cmdlineVersionStrings[0];
            docstring += "\n";
        }
        
        // Add the required options header.
        docstring += "\nRequired:\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionInputFile;
        docstring += "=<input-graph>\n";
        docstring += "        Path of the file containing the input graph.\n";
        docstring += "        Must be specified exactly once.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionInputFormat;
        docstring += "=<input-format>\n";
        docstring += "        Format of the input graph.\n";
        docstring += "        Must be specified exactly once.\n";
        docstring += "        See documentation for supported values.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionOutputFile;
        docstring += "=<output-graph>\n";
        docstring += "        Path of the file to which the output graph should be written.\n";
        docstring += "        Must be specified at least once, but may be specified multiple times.\n";
        docstring += "        One output file is produced for each filename given.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionOutputFormat;
        docstring += "=<input-format>\n";
        docstring += "        Format of each output graph.\n";
        docstring += "        Must be specified once per output file.\n";
        docstring += "        See documentation for supported values.\n";
        
        
        // Add the options header.
        docstring += "\nOptions:\n";
        
        // Print any help string information.
        if (0 != cmdlineHelpStrings.size())
        {
            docstring += "  ";
            docstring += cmdlinePrefixStrings[0];
            docstring += cmdlineHelpStrings[0];
            docstring += "\n";
            docstring += "        Prints this information and exits.\n";
        }
        
        // Prints any version string information.
        if (0 != cmdlineVersionStrings.size())
        {
            docstring += "  ";
            docstring += cmdlinePrefixStrings[0];
            docstring += cmdlineVersionStrings[0];
            docstring += "\n";
            docstring += "        Prints version information and exits.\n";
        }
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionInputOptions;
        docstring += "=<input-options-string>\n";
        docstring += "        Comma-delimited list of input graph options and values.\n";
        docstring += "        Fine-tunes the behavior of graph reading functionality.\n";
        docstring += "        Optional; may be specified at most once.\n";
        docstring += "        See documentation for supported values and defaults.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionInputWeights;
        docstring += "=<input-weights-string>\n";
        docstring += "        Type of weights, if any, to be read from the input file.\n";
        docstring += "        Optional; may be specified at most once.\n";
        docstring += "        See documentation for supported values and defaults.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionOutputGrouping;
        docstring += "=<output-vertex-grouping-string>\n";
        docstring += "        Output edge grouping mode.\n";
        docstring += "        Specifies that edges should be grouped by source or destination vertex.\n";
        docstring += "        Optional; may be specified at most once per output file.\n";
        docstring += "        See documentation for supported values and defaults.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionOutputOptions;
        docstring += "=<output-options-string>\n";
        docstring += "        Comma-delimited list of output graph options and values.\n";
        docstring += "        Fine-tunes the behavior of graph writing functionality.\n";
        docstring += "        Optional; may be specified at most once per output file.\n";
        docstring += "        See documentation for supported values and defaults.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionOutputWeights;
        docstring += "=<output-weights-string>\n";
        docstring += "        Type of weights, if any, to be written to the input file.\n";
        docstring += "        Optional; may be specified at most once per output file.\n";
        docstring += "        See documentation for supported values and defaults.\n";
        
        docstring += "  ";
        docstring += cmdlinePrefixStrings[0];
        docstring += kOptionTransform;
        docstring += "=<transform-string>\n";
        docstring += "        Transformation operation to be applied.\n";
        docstring += "        Transformation operations are applied in command-line order.\n";
        docstring += "        Optional; may be specified as many times as needed.\n";
        docstring += "        See documentation for supported values and defaults.\n";
        
        return docstring;
    }
    
    /// Prints an error message about a graph file operation to the standard error console.
    /// @param [in] cmdline Command-line path to display in the string.
    /// @param [in] filename Filename to display in the string.
    /// @param [in] code Error code that should be printed.
    /// @param [in] operationIsRead `true` to show that an error occurred during read, `false` to show that it happened during write.
    void PrintGraphFileError(const char* const cmdline, const char* const filename, const EGraphResult code, const bool operationIsRead)
    {
        fprintf(stderr, "%s: Error %s %s: %s.\n", cmdline, (operationIsRead ? "reading" : "writing"), filename, graphErrorStrings.at(code).c_str());
    }
}


// -------- ENTRY POINT ---------------------------------------------------- //

using namespace GraphTool;

/// Program entry point.
/// @param [in] argc Number of command-line arguments.
/// @param [in] argv Command-line argument strings.
/// @return Program exit code. 0 means successful termination, anything else indicates an error.
int main(const int argc, const char* const argv[])
{
    // Build the string to display when requesting help.
    const std::string documentationString = MakeDocumentationString(argv[0]);
    
    // Initialize the command-line option parser.
    Options commandLineOptions(argv[0], cmdlineSpecifiedOptions, &cmdlinePrefixStrings, &cmdlineVersionStrings, &cmdlineHelpStrings, &documentationString, &applicationVersionString);

    // Submit all command-line options for parsing.
    if (!(commandLineOptions.FillFromStringArray(argc - 1, &argv[1])))
        return __LINE__;
    
    // Validate that all required values are present and required relationships hold.
    if (!(commandLineOptions.ValidateOptions() && commandLineOptions.VerifyEqualValueCount(kOptionOutputFile.c_str(), kOptionOutputFormat.c_str())))
        return __LINE__;
    
    // Get the input file name.
    const OptionContainer* optionValues = commandLineOptions.GetOptionValues(kOptionInputFile);
    if (NULL == optionValues)
        return __LINE__;

    std::string inputGraphFile;
    if (!(optionValues->QueryValue(inputGraphFile)))
        return __LINE__;

    // Get the output file names.
    optionValues = commandLineOptions.GetOptionValues(kOptionOutputFile);
    if (NULL == optionValues)
        return __LINE__;

    std::vector<std::string> outputGraphFiles(optionValues->GetValueCount());

    for (size_t i = 0; i < optionValues->GetValueCount(); ++i)
    {
        if (!(optionValues->QueryValueAt(i, outputGraphFiles[i])))
            return __LINE__;
    }

    // Figure out the input edge data type.
    optionValues = commandLineOptions.GetOptionValues(kOptionInputWeights);
    if (NULL == optionValues)
        return __LINE__;
    
    int64_t readerEdgeDataTypeEnum;
    if (!(optionValues->QueryValue(readerEdgeDataTypeEnum)))
        return __LINE__;
    
    // Create the graph reader object.
    optionValues = commandLineOptions.GetOptionValues(kOptionInputFormat);
    if (NULL == optionValues)
        return __LINE__;

    int64_t optionGraphFormatEnum;
    if (!(optionValues->QueryValue(optionGraphFormatEnum)))
        return __LINE__;

    IGraphReader* reader = GraphReaderFactory::CreateGraphReader((EGraphReaderType)optionGraphFormatEnum, (EEdgeDataType)readerEdgeDataTypeEnum);
    if (NULL == reader)
        return __LINE__;

    // Figure out the output edge data types and create the graph writer objects.
    optionValues = commandLineOptions.GetOptionValues(kOptionOutputFormat);
    if (NULL == optionValues)
        return __LINE__;

    std::vector<IGraphWriter*> writers(optionValues->GetValueCount());
    std::vector<EEdgeDataType> writersEdgeDataType(optionValues->GetValueCount());
    
    {
        const OptionContainer* optionValuesOutputEdgeDataType = commandLineOptions.GetOptionValues(kOptionOutputWeights);
        if (NULL == optionValuesOutputEdgeDataType)
            return __LINE__;
        
        for (size_t i = 0; i < optionValues->GetValueCount(); ++i)
        {
            int64_t writerEdgeDataTypeEnum;
            
            if (!(optionValuesOutputEdgeDataType->QueryValueAt(i, writerEdgeDataTypeEnum)))
                return __LINE__;
            
            if (!(optionValues->QueryValueAt(i, optionGraphFormatEnum)))
                return __LINE__;
    
            IGraphWriter* writer = GraphWriterFactory::CreateGraphWriter((EGraphWriterType)optionGraphFormatEnum, (EEdgeDataType)writerEdgeDataTypeEnum);
            if (NULL == writer)
                return __LINE__;
    
            writers[i] = writer;
            writersEdgeDataType[i] = (EEdgeDataType)writerEdgeDataTypeEnum;
        }
    }
    
    // Figure out the correct grouping mode for each writer.
    optionValues = commandLineOptions.GetOptionValues(kOptionOutputGrouping);
    if (NULL == optionValues)
        return __LINE__;
    
    std::vector<bool> writerGroupByDestination(writers.size());
    
    for (size_t i = 0; i < writers.size(); ++i)
    {
        int64_t optionOutputGroupingEnum;
        
        if (!(optionValues->QueryValueAt(i, optionOutputGroupingEnum)))
            return __LINE__;
        
        writerGroupByDestination[i] = ((0ll == optionOutputGroupingEnum) ? false : true);
    }

    // Consistency check.
    if (!(writers.size() == outputGraphFiles.size()))
        return __LINE__;

    // Create transformation objects.
    optionValues = commandLineOptions.GetOptionValues(kOptionTransform);
    if (NULL == optionValues)
        return __LINE__;
    
    std::vector<IGraphTransform*> transforms(optionValues->GetValueCount());
    
    for (size_t i = 0; i < optionValues->GetValueCount(); ++i)
    {
        int64_t transformTypeEnum;
        
        if (!(optionValues->QueryValueAt(i, transformTypeEnum)))
            return __LINE__;
        
        transforms[i] = GraphTransformFactory::CreateGraphTransform((EGraphTransformType)transformTypeEnum);
    }
    
    // Submit input and output options to all of the readers and writers, respectively.
    optionValues = commandLineOptions.GetOptionValues(kOptionInputOptions);
    if (NULL == optionValues)
        return __LINE__;
    
    {
        std::string inputOptions;
        if (!(optionValues->QueryValue(inputOptions)))
            return __LINE__;
    
        // TODO: submit reader options
    }
    
    optionValues = commandLineOptions.GetOptionValues(kOptionOutputOptions);
    if (NULL == optionValues)
        return __LINE__;
    
    for (size_t i = 0; i < optionValues->GetValueCount(); ++i)
    {
        std::string outputOptions;
        
        if (!(optionValues->QueryValueAt(i, outputOptions)))
            return __LINE__;
        
        // TODO: submit writer options
    }
    
    // Read the input graph.
    Graph graph;
    EGraphResult fileResult = reader->ReadGraphFromFile(inputGraphFile.c_str(), graph);
    
    if (EGraphResult::GraphResultSuccess != fileResult)
    {
        PrintGraphFileError(argv[0], inputGraphFile.c_str(), fileResult, true);
        return __LINE__;
    }
    else
    {
        printf("Read graph %s.\n", inputGraphFile.c_str());
        printf("Graph contains %llu vertices and %llu edges.\n", (long long unsigned int)graph.GetNumVertices(), (long long unsigned int)graph.GetNumEdges());
    }

    // Perform transformations.
    for (size_t i = 0; i < transforms.size(); ++i)
    {
        if (NULL != transforms[i])
        {
            const EGraphResult transformResult = transforms[i]->ApplyTransformation(graph);
            
            if (EGraphResult::GraphResultSuccess == transformResult)
                puts("Applied transform.");
            else
            {
                puts("Failed to apply transform.");
                return __LINE__;
            }
        }
    }

    // Write the output graphs.
    for (size_t i = 0; i < writers.size(); ++i)
    {
        fileResult = writers[i]->WriteGraphToFile(outputGraphFiles[i].c_str(), graph, writerGroupByDestination[i]);
        if (EGraphResult::GraphResultSuccess != fileResult)
        {
            PrintGraphFileError(argv[0], outputGraphFiles[i].c_str(), fileResult, false);
        }
        else
        {
            printf("Wrote %s-grouped %s graph %s.\n", (writerGroupByDestination[i] ? "destination" : "source"), edgeDataTypeStrings.at(writersEdgeDataType[i]).c_str(), outputGraphFiles[i].c_str());
        }
    }
    
    // Print final messages and exit.
    printf("Exiting.\n");
    exit(0);
}
