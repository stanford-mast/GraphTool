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

#include <cstddef>
#include <list>
#include <map>
#include <string>
#include <utility>


namespace GraphTool
{
    /// Provides class methods and fields for holding command-line-option values.
    /// Implements rudimentary checks for detecting user errors, based on supported options and parameter values.
    class Options
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Specifies the result of an attempt to submit a new option configuration value.
        enum EOptionSubmitResult
        {
            OptionSubmitResultOk,                                           ///< Option was accepted.
            OptionSubmitResultErrorUnsupported,                             ///< Option name is not recognized as a supported name.
            OptionSubmitResultErrorMalformed,                               ///< Option name-value pair is malformed.
            OptionSubmitResultErrorTooMany,                                 ///< Option was supplied too many times.
            OptionSubmitResultErrorAliasTooMany,                            ///< Option alias leads to an option being supplied too many times.
        };


    private:
        // -------- CLASS VARIABLES ---------------------------------------- //

        /// Holds all supplied command-line option values, which the user would supply as --[option]=[value].
        /// The map itself is statically initialized with mappings from supported command-line options to numbers and empty lists.
        /// Each number specifies the maximum number of values allowed for each command-line option and is statically initialized, with 0 meaning unlimited.
        /// The lists are filled at run-time with values supplied by the user.
        static std::map<std::string, std::pair<size_t, std::list<std::string>>> specifiedCommandLineOptions;

        /// Holds all supported command-line aliases, which do not accept any argument values and would be specified as --[alias].
        /// Maps from possible command-line options to their expanded forms, which would then be parsed normally.
        static std::map<std::string, std::string> supportedCommandLineAliases;
        
        
    public:
        // -------- CLASS METHODS ------------------------------------------ //

        static EOptionSubmitResult SubmitOption(const std::string& optionString);
    };
}
