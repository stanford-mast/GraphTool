/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Options.h
 *   Declaration of command-line-option-handling functionality.
 *****************************************************************************/

#pragma once

#include "OptionContainer.h"

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>


namespace GraphTool
{
    /// Provides class methods and fields for holding command-line-option values.
    /// Implements rudimentary checks for detecting user errors, based on supported options and parameter values.
    class Options
    {
    public:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Command-line option that specifies edge data type.
        static const std::string kOptionEdgeData;
        
        /// Command-line option that specifies input file name.
        static const std::string kOptionInputFile;
        
        /// Command-line option that specifies input file format.
        static const std::string kOptionInputFormat;
        
        /// Command-line option that specifies input processing options.
        static const std::string kOptionInputOptions;
        
        /// Command-line option that specifies output file name.
        static const std::string kOptionOutputFile;
        
        /// Command-line option that specifies output file format.
        static const std::string kOptionOutputFormat;
        
        /// Command-line option that specifies output processing options.
        static const std::string kOptionOutputOptions;

    private:
        // -------- CLASS VARIABLES ---------------------------------------- //

        /// Holds all supported strings for requesting help.
        static std::vector<std::string> helpStrings;

        /// Holds all supported command-line option prefix strings.
        static std::vector<std::string> prefixStrings;
        
        /// Holds all supplied command-line option values, which the user would supply as --[option]=[value].
        /// The map itself is statically initialized with mappings from supported command-line options to containers for specified option values.
        static std::unordered_map<std::string, OptionContainer*> specifiedCommandLineOptions;

        /// Holds all supported command-line aliases, which do not accept any argument values and would be specified as --[alias].
        /// Maps from possible command-line options to their expanded forms, which would then be parsed normally.
        static std::unordered_map<std::string, std::string> supportedCommandLineAliases;
        
        
        // -------- HELPERS ------------------------------------------------ //

        /// Checks if the specified option string is a help-requesting string.
        /// @param [in] optionString String to check.
        /// @return `true` if the string is a supported help-requesting string, `false` otherwise.
        static bool IsHelpString(const char* optionString);
        
        /// Specifies the number of characters of command-line option prefix to skip.
        /// Checks the input string for a valid prefix and returns its length.
        /// For example, if the input string starts with "--" and this is a valid prefix then the length is 2.
        /// @param [in] optionString String to check.
        /// @return Length of the prefix within the string being checked, with 0 indicating no valid prefix found (and this is likely an error).
        static size_t PrefixLength(const char* optionString);

        /// Prints an error for a command-line option alias that conflicts with an already-specified command-line option.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionAlias Alias specified that is causing the conflict.
        /// @param [in] optionName Option with which the alias conflicts.
        static void PrintErrorAliasConflict(const char* cmdline, const char* optionAlias, const char* optionName);

        /// Prints the final part of a command-line option error.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        static void PrintErrorCommon(const char* cmdline);

        /// Prints an error that is internal and should never happen.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        static void PrintErrorInternal(const char* cmdline);
        
        /// Prints an error for a command-line option that is malformed.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionString Command-line option string that is the source of the error.
        static void PrintErrorMalformed(const char* cmdline, const char* optionString);

        /// Prints an error for a command-line option that is missing but required.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionName Command-line option name that is the source of the error.
        static void PrintErrorMissing(const char* cmdline, const char* optionName);

        /// Prints an error for command-line options whose quantities are not compatible.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionName1 First command-line option in question.
        /// @param [in] optionName2 Second command-line option in question.
        static void PrintErrorQuantityMismatch(const char* cmdline, const char* optionName1, const char* optionName2);
        
        /// Prints an error for a command-line option specified too many times.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionName Command-line option name that is the source of the error.
        static void PrintErrorTooMany(const char* cmdline, const char* optionName);

        /// Prints an error for an unrecognized command-line option.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionName Command-line option name that is the source of the error.
        static void PrintErrorUnsupported(const char* cmdline, const char* optionName);

        /// Prints an error for a command-line option that cannot be parsed, or whose value is rejected.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionName Command-line option name that is the source of the error.
        /// @param [in] optionValue Command-line option value that was rejected.
        static void PrintErrorValueRejected(const char* cmdline, const char* optionName, const char* optionValue);

        /// Prints the help message for this application.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        static void PrintHelp(const char* cmdline);
        
    public:
        // -------- CLASS METHODS ------------------------------------------ //

        /// Retrieves the container that holds values for the specified option.
        /// @param [in] optionName Name of the option. Should be one of the constants defined in this class.
        /// @return Pointer to a read-only container holding option values (ownership does not transfer to the caller) or `NULL` in the event of an error.
        static const OptionContainer* GetOptionValues(const std::string& optionName);
        
        /// Submits a command-line option string to the option system.
        /// Prints errors to standard error if there is an issue.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @param [in] optionString String that holds the command-line option and value to be parsed and submitted.
        /// @return `true` if the option string was accepted, `false` otherwise.
        static bool SubmitOption(const char* cmdline, const char* optionString);

        /// Validates all previously-supplied command-line options, ensuring those required are present.
        /// Prints errors to standard error if there is an issue.
        /// @param [in] cmdline Command-line with which this application was started, typically `argv[0]` in `main()`.
        /// @return `true` if all required options were specified, `false` otherwise.
        static bool ValidateOptions(const char* cmdline);
    };
}
