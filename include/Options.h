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
#include <map>
#include <string>
#include <vector>


namespace GraphTool
{
    /// Provides methods and fields for holding command-line-option values.
    /// Implements rudimentary checks for detecting user errors, based on supported options and parameter values.
    class Options
    {
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds the command-line string to use for printing messages.
        const std::string& commandLine;
        
        /// Holds all supported strings for requesting help.
        /// May be `NULL`, in which case help strings are not used.
        const std::vector<std::string>* helpStrings;

        /// Holds all supported command-line option prefix strings.
        const std::vector<std::string>& prefixStrings;
        
        /// Holds all supplied command-line option values, which the user would supply as --[option]=[value].
        std::map<std::string, OptionContainer*>& specifiedOptions;

        /// Holds all supported command-line aliases, which do not accept any argument values and would be specified as --[alias].
        /// Maps from possible command-line options to their expanded forms, which would then be parsed normally.
        /// May be `NULL`, in which case aliases are not used at all.
        const std::map<std::string, std::string>* supportedAliases;
        
    
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Constructs an instance of this object, given all required strings and maps.
        Options(const std::string& commandLine, const std::vector<std::string>& prefixStrings, std::map<std::string, OptionContainer*>& specifiedOptions, const std::map<std::string, std::string>* supportedAliases = NULL, const std::vector<std::string>* helpStrings = NULL);


    private:
        // -------- HELPERS ------------------------------------------------ //

        /// Checks if the specified option string is a help-requesting string.
        /// @param [in] optionString String to check.
        /// @return `true` if the string is a supported help-requesting string, `false` otherwise.
        bool IsHelpString(const char* optionString) const;
        
        /// Specifies the number of characters of command-line option prefix to skip.
        /// Checks the input string for a valid prefix and returns its length.
        /// For example, if the input string starts with "--" and this is a valid prefix then the length is 2.
        /// @param [in] optionString String to check.
        /// @return Length of the prefix within the string being checked, with 0 indicating no valid prefix found (and this is likely an error).
        size_t PrefixLength(const char* optionString) const;

        /// Prints an error for a command-line option alias that conflicts with an already-specified command-line option.
        /// @param [in] optionAlias Alias specified that is causing the conflict.
        /// @param [in] optionName Option with which the alias conflicts.
        void PrintErrorAliasConflict(const char* optionAlias, const char* optionName) const;

        /// Prints the final part of a command-line option error.
        void PrintErrorCommon(void) const;

        /// Prints an error that is internal and should never happen.
        void PrintErrorInternal(void) const;
        
        /// Prints an error for a command-line option that is malformed.
        /// @param [in] optionString Command-line option string that is the source of the error.
        void PrintErrorMalformed(const char* optionString) const;

        /// Prints an error for a command-line option that is missing but required.
        /// @param [in] optionName Command-line option name that is the source of the error.
        void PrintErrorMissing(const char* optionName) const;

        /// Prints an error for command-line options whose quantities are not compatible.
        /// @param [in] optionName1 First command-line option in question.
        /// @param [in] optionName2 Second command-line option in question.
        void PrintErrorQuantityMismatch(const char* optionName1, const char* optionName2) const;
        
        /// Prints an error for a command-line option specified too many times.
        /// @param [in] optionName Command-line option name that is the source of the error.
        void PrintErrorTooMany(const char* optionName) const;

        /// Prints an error for an unrecognized command-line option.
        /// @param [in] optionName Command-line option name that is the source of the error.
        void PrintErrorUnsupported(const char* optionName) const;

        /// Prints an error for a command-line option that cannot be parsed, or whose value is rejected.
        /// @param [in] optionName Command-line option name that is the source of the error.
        /// @param [in] optionValue Command-line option value that was rejected.
        void PrintErrorValueRejected(const char* optionName, const char* optionValue) const;

        /// Prints the help message for this application.
        void PrintHelp(void) const;
        
    public:
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Retrieves the container that holds values for the specified option.
        /// @param [in] optionName Name of the option. Should be one of the constants defined in this class.
        /// @return Pointer to a read-only container holding option values (ownership does not transfer to the caller) or `NULL` in the event of an error.
        const OptionContainer* GetOptionValues(const std::string& optionName) const;
        
        /// Submits a command-line option string to the option system.
        /// Prints errors to standard error if there is an issue.
        /// @param [in] optionString String that holds the command-line option and value to be parsed and submitted.
        /// @return `true` if the option string was accepted, `false` otherwise.
        bool SubmitOption(const char* optionString);
        
        /// Validates all previously-supplied command-line options, ensuring those required are present.
        /// Prints errors to standard error if there is an issue.
        /// @return `true` if all required options were specified, `false` otherwise.
        bool ValidateOptions(void) const;

        /// Verifies that the two supplied command-line options have the same number of specified values and prints an error if not.
        /// @param [in] optionName1 First option name to check.
        /// @param [in] optionName2 Second option name to check.
        /// @return `true` if the check passes, `false` otherwise.
        bool VerifyEqualValueCount(const char* optionName1, const char* optionName2) const;
    };
}
