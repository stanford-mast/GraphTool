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

#include "Benchmark.h"
#include "Graph.hpp"
#include "GraphReader.h"
#include "GraphReaderFactory.h"
#include "GraphWriter.h"
#include "GraphWriterFactory.h"
#include "OptionContainer.h"
#include "Options.h"
#include "VersionInfo.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
#include <map>
#include <string>
#include <vector>


namespace GraphTool
{
    // -------- CONSTANTS -------------------------------------------------- //

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


    // -------- TYPE DEFINITIONS ------------------------------------------- //

    /// Enumerates all supported data types for edge data (i.e. edge weights).
    enum EEdgeDataType : int64_t
    {
        EdgeDataTypeVoid,                                                   ///< No edge data (i.e. an unweighted graph).
        EdgeDataTypeInteger,                                                ///< Edge data are integral. Currently this means 64-bit unsigned.
        EdgeDataTypeFloatingPoint                                           ///< Edge data are floating-point. Currently this means double-precision.
    };


    // -------- LOCALS ----------------------------------------------------- //

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
        { kOptionInputFormat,                       new EnumOptionContainer(*(GraphReaderFactory<void>::GetGraphReaderStrings()), OptionContainer::kUnlimitedValueCount) },
        { kOptionInputOptions,                      new OptionContainer("") },
        { kOptionOutputFile,                        new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputFormat,                      new EnumOptionContainer(*(GraphWriterFactory<void>::GetGraphWriterStrings()), OptionContainer::kUnlimitedValueCount) },
        { kOptionOutputOptions,                     new OptionContainer("", OptionContainer::kUnlimitedValueCount) },
    };

    /// Holds all supported command-line option aliases, mapped from the strings that could be specified on the command-line to the strings they represent.
    static const std::map<std::string, std::string> cmdlineSupportedAliases = {
        { "noedgedata",                             "edgedata=void" },
    };


    // -------- FUNCTIONS -------------------------------------------------- //

    /// Runs the main graph manipulation operations.
    /// @tparam TEdgeData Specifies the type of data, such as a weight, to hold for each edge.
    /// @param [in] commandLineOptions Top-level command-line options that have already been parsed.
    /// @return Program exit code. 0 means successful termination, anything else indicates an error.
    template <typename TEdgeData> int RunGraphTool(const Options& commandLineOptions)
    {
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

        // Create the graph reader object.
        optionValues = commandLineOptions.GetOptionValues(kOptionInputFormat);
        if (NULL == optionValues)
            return __LINE__;

        int64_t optionGraphFormatEnum;
        if (!(optionValues->QueryValue(optionGraphFormatEnum)))
            return __LINE__;

        GraphReader<TEdgeData>* reader = GraphReaderFactory<TEdgeData>::CreateGraphReader((EGraphReaderType)optionGraphFormatEnum);
        if (NULL == reader)
            return __LINE__;

        // Create the graph writer objects.
        optionValues = commandLineOptions.GetOptionValues(kOptionOutputFormat);
        if (NULL == optionValues)
            return __LINE__;

        std::vector<GraphWriter<TEdgeData>*> writers(optionValues->GetValueCount());

        for (size_t i = 0; i < optionValues->GetValueCount(); ++i)
        {
            if (!(optionValues->QueryValueAt(i, optionGraphFormatEnum)))
                return __LINE__;

            GraphWriter<TEdgeData>* writer = GraphWriterFactory<TEdgeData>::CreateGraphWriter((EGraphWriterType)optionGraphFormatEnum);
            if (NULL == writer)
                return __LINE__;

            writers[i] = writer;
        }

        // Consistency check.
        if (!(writers.size() == outputGraphFiles.size()))
            return __LINE__;

        // Submit input and output options to all of the readers and writers, respectively.
        // TODO

        // Read the input graph.
        Graph<TEdgeData> graph;
        double timeElapsed = 0.0;
        
        BenchmarkStart();
        
        if (!(reader->ReadGraphFromFile(inputGraphFile.c_str(), graph)))
            return __LINE__;
        
        timeElapsed = BenchmarkStop();
        printf("Reading graph took %.2lf msec.\n", timeElapsed);

        // Perform transformations.
        // TODO

        // Write the output graphs.
        //for (size_t i = 0; i < writers.size(); ++i)
        //{
        //    if (!(writers[i]->WriteGraphToFile(outputGraphFiles[i].c_str(), graph, true)))
        //        return __LINE__;
        //}
        
        
        BenchmarkStart();
        
        uint64_t numDestinationEdges = 0ull;
        for (auto vit = graph.VertexIteratorDestinationBegin(); vit != graph.VertexIteratorDestinationEnd(); ++vit)
        {
            if (NULL == *vit) continue;
            
            for (auto eit = (*vit)->BeginIterator(); eit != (*vit)->EndIterator(); ++eit)
            {
                numDestinationEdges += (uint64_t)(_mm_popcnt_u32((unsigned int)eit->second.edges));
            }
        }
        
        uint64_t numSourceEdges = 0ull;
        for (auto vit = graph.VertexIteratorSourceBegin(); vit != graph.VertexIteratorSourceEnd(); ++vit)
        {
            if (NULL == *vit) continue;
            
            for (auto eit = (*vit)->BeginIterator(); eit != (*vit)->EndIterator(); ++eit)
            {
                numSourceEdges += (uint64_t)(_mm_popcnt_u32((unsigned int)eit->second.edges));
            }
        }
        
        timeElapsed = BenchmarkStop();
        
        printf("Traversal took %.2lf msec (%.0lf Medges/sec)\n", timeElapsed, (double)graph.GetNumEdges() / timeElapsed / 500.0);
        
        if (graph.GetNumEdges() == numDestinationEdges && graph.GetNumEdges() == numSourceEdges)
            printf("Edge consistency check: PASS.\n");
        else
            printf("Edge consistency check: FAIL.\n");
        
        printf("Number of destination vectors = %llu\n", (unsigned long long)graph.GetNumVectorsDestination());
        printf("Number of source vectors      = %llu\n", (unsigned long long)graph.GetNumVectorsSource());

        return 0;
    }
}


// -------- ENTRY POINT ---------------------------------------------------- //

using namespace GraphTool;

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
        return __LINE__;
    
    // Validate that all required values are present and required relationships hold.
    if (!(commandLineOptions.ValidateOptions() && commandLineOptions.VerifyEqualValueCount(kOptionOutputFile.c_str(), kOptionOutputFormat.c_str())))
        return __LINE__;
    
    // Branch based on the edge data type specified.
    const OptionContainer* edgeDataOptionValue = commandLineOptions.GetOptionValues(kOptionEdgeData);
    int64_t edgeDataValue = 0ll;

    if (NULL == edgeDataOptionValue)
        return __LINE__;

    if (!(edgeDataOptionValue->QueryValue(edgeDataValue)))
        return __LINE__;
    
    switch ((EEdgeDataType)edgeDataValue)
    {
    case EEdgeDataType::EdgeDataTypeVoid:
        return RunGraphTool<void>(commandLineOptions);

    case EEdgeDataType::EdgeDataTypeInteger:
        return RunGraphTool<uint64_t>(commandLineOptions);

    case EEdgeDataType::EdgeDataTypeFloatingPoint:
        return RunGraphTool<double>(commandLineOptions);

    default:
        return __LINE__;
    }
}
