/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Options.cpp
 *   Implementation of command-line-option-handling functionality.
 *****************************************************************************/

#include "OptionContainer.h"
#include "Options.h"
#include "VersionInfo.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <list>
#include <map>
#include <string>
#include <utility>

using namespace GraphTool;


// -------- CONSTANTS ------------------------------------------------------ //
// See "Options.h" for documentation.

const std::string Options::kOptionEdgeData      = "edgedata";
const std::string Options::kOptionInputFile     = "inputfile";
const std::string Options::kOptionInputFormat   = "inputformat";
const std::string Options::kOptionInputOptions  = "inputoptions";
const std::string Options::kOptionOutputFile    = "outputfile";
const std::string Options::kOptionOutputFormat  = "outputformat";
const std::string Options::kOptionOutputOptions = "outputoptions";


// -------- CLASS VARIABLES ------------------------------------------------ //
// See "Options.h" for documentation.

std::vector<std::string> Options::helpStrings = {
#ifdef __PLATFORM_WINDOWS
    "?",
#endif
    "help",
};

std::vector<std::string> Options::prefixStrings = {
#ifdef __PLATFORM_WINDOWS
    "/",
#endif
    "--",
};

std::unordered_map<std::string, OptionContainer*> Options::specifiedCommandLineOptions = {
    { kOptionEdgeData,                          new OptionContainer("void") },
    { kOptionInputFile,                         new OptionContainer(EOptionValueType::OptionValueTypeString) },
    { kOptionInputFormat,                       new OptionContainer(EOptionValueType::OptionValueTypeString) },
    { kOptionInputOptions,                      new OptionContainer("") },
    { kOptionOutputFile,                        new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
    { kOptionOutputFormat,                      new OptionContainer(EOptionValueType::OptionValueTypeString, OptionContainer::kUnlimitedValueCount) },
    { kOptionOutputOptions,                     new OptionContainer("", OptionContainer::kUnlimitedValueCount) },
};

std::unordered_map<std::string, std::string> Options::supportedCommandLineAliases = {
    { "noedgedata",                             "edgedata=void" },
};


// -------- HELPERS -------------------------------------------------------- //
// See "Options.h" for documentation.

bool Options::IsHelpString(const char* optionString)
{
    if (NULL != optionString)
    {
        std::string testString(optionString);

        for (auto it = helpStrings.cbegin(); it != helpStrings.cend(); ++it)
        {
            if (*it == testString)
                return true;
        }
    }

    return false;
}

// --------

size_t Options::PrefixLength(const char* optionString)
{
    if (NULL != optionString)
    {
        for (auto it = prefixStrings.cbegin(); it != prefixStrings.cend(); ++it)
        {
            if (0 == strncmp(optionString, it->c_str(), it->size()))
                return it->size();
        }
    }
    
    return 0;
}

// --------

void Options::PrintErrorAliasConflict(const char* cmdline, const char* optionAlias, const char* optionName)
{
    fprintf(stderr, "%s: '%s' conflicts with '%s'.\n", cmdline, optionAlias, optionName);
    PrintErrorCommon(cmdline);
}

// --------

void Options::PrintErrorCommon(const char* cmdline)
{
    fprintf(stderr, "Try '%s %s%s' for more information.\n", cmdline, prefixStrings[0].c_str(), helpStrings[0].c_str());
}

// --------

void Options::PrintErrorInternal(const char* cmdline)
{
    fprintf(stderr, "%s: Internal error while processing command-line options.\n", cmdline);
}

// --------

void Options::PrintErrorMalformed(const char* cmdline, const char* optionString)
{
    fprintf(stderr, "%s: Invalid option '%s'.\n", cmdline, optionString);
    PrintErrorCommon(cmdline);
}

// --------

void Options::PrintErrorMissing(const char* cmdline, const char* optionName)
{
    fprintf(stderr, "%s: Missing required option '%s'.\n", cmdline, optionName);
    PrintErrorCommon(cmdline);
}

// --------

void Options::PrintErrorQuantityMismatch(const char* cmdline, const char* optionName1, const char* optionName2)
{
    fprintf(stderr, "%s: Mismatch between options '%s' and '%s'.\n", cmdline, optionName1, optionName2);
    PrintErrorCommon(cmdline);
}

// --------

void Options::PrintErrorTooMany(const char* cmdline, const char* optionName)
{
    fprintf(stderr, "%s: Option '%s' specified too many times.\n", cmdline, optionName);
    PrintErrorCommon(cmdline);
}

// --------

void Options::PrintErrorUnsupported(const char* cmdline, const char* optionName)
{
    fprintf(stderr, "%s: Invalid option '%s'.\n", cmdline, optionName);
    PrintErrorCommon(cmdline);
}

// --------

void Options::PrintErrorValueRejected(const char* cmdline, const char* optionName, const char* optionValue)
{
    fprintf(stderr, "%s: Invalid value '%s' for option '%s'.\n", cmdline, optionValue, optionName);
    PrintErrorCommon(cmdline);
}

// --------

void Options::PrintHelp(const char* cmdline)
{
    fprintf(stderr, "TODO: Help message goes here.\n");
}


// -------- CLASS METHODS -------------------------------------------------- //
// See "Options.h" for documentation.

const OptionContainer* Options::GetOptionValues(const std::string& optionName)
{
    if (0 != specifiedCommandLineOptions.count(optionName))
        return specifiedCommandLineOptions.at(optionName);
    else
        return NULL;
}

// --------

bool Options::SubmitOption(const char* cmdline, const char* optionString)
{
    const char* aliasString = NULL;
    const char* stringToParse = optionString;
    const size_t optionPrefixLength = PrefixLength(optionString);
    
    // Handle the command-line option prefix. It is an error for it to be missing.
    stringToParse += optionPrefixLength;
    if (0 == optionPrefixLength)
    {
        PrintErrorMalformed(cmdline, optionString);
        return false;
    }

    // Check if the option is a help string.
    if (IsHelpString(stringToParse))
    {
        PrintHelp(cmdline);
        return false;
    }
    
    // Check if an alias is being used and, if so, get the actual command-line value string for it.
    if (0 != supportedCommandLineAliases.count(stringToParse))
    {
        aliasString = stringToParse;
        stringToParse = supportedCommandLineAliases.at(stringToParse).c_str();
    }
    
    // Parse the string into a name and a value.
    const char* posEqualsSign = strchr(stringToParse, '=');
    
    // No equals sign means the input string is malformed.
    if (NULL == posEqualsSign)
    {
        PrintErrorMalformed(cmdline, optionString);
        return false;
    }

    // Separate the name and value portions.
    std::string optionName(stringToParse, posEqualsSign - stringToParse);
    std::string optionValue(posEqualsSign + 1);

    // Reject empty option values.
    if (optionValue.empty())
    {
        PrintErrorMalformed(cmdline, optionString);
        return false;
    }
    
    // Check for single-quotes or double-quotes surrounding the option value and remove them if present.
    if ((optionValue.front() == optionValue.back()) && ((optionValue.front() == '\"') || (optionValue.front() == '\'')) && (optionValue.size() >= 2))
    {
        optionValue.erase(0, 1);
        optionValue.pop_back();
    }
    
    // Check if the option name is supported.
    if (0 == specifiedCommandLineOptions.count(optionName))
    {
        PrintErrorUnsupported(cmdline, optionName.c_str());
        return false;
    }

    // Attempt to submit the option value.
    OptionContainer* optionContainer = specifiedCommandLineOptions.at(optionName.c_str());
    
    if (EOptionValueSubmitResult::OptionValueSubmitResultOk != optionContainer->ParseAndSubmitValue(optionValue))
    {
        PrintErrorValueRejected(cmdline, optionName.c_str(), optionValue.c_str());
        return false;
    }

    return true;
}

// --------

bool Options::ValidateOptions(const char* cmdline)
{
    // Verify that required options are all supplied.
    for (auto it = specifiedCommandLineOptions.cbegin(); it != specifiedCommandLineOptions.cend(); ++it)
    {
        if (!(it->second->AreValuesValid()))
        {
            PrintErrorMissing(cmdline, it->first.c_str());
            return false;
        }
    }
    
    // Verify specific conditions and relationships between options.
    // Specifically, output file and format options must have the same number of values.
    const OptionContainer* outputFileValues = GetOptionValues(kOptionOutputFile);
    const OptionContainer* outputFormatValues = GetOptionValues(kOptionOutputFormat);

    if (NULL == outputFileValues || NULL == outputFormatValues)
    {
        PrintErrorInternal(cmdline);
        return false;
    }

    if (outputFileValues->GetValueCount() != outputFormatValues->GetValueCount())
    {
        PrintErrorQuantityMismatch(cmdline, kOptionOutputFile.c_str(), kOptionOutputFormat.c_str());
        return false;
    }


    return true;
}
