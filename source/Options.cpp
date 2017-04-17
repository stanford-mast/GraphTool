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

#include "Options.h"

#include <cstddef>
#include <list>
#include <map>
#include <string>
#include <utility>


using namespace GraphTool;


// -------- CLASS VARIABLES ------------------------------------------------ //
// See "Options.h" for documentation.

std::map<std::string, std::pair<size_t, std::list<std::string>>> Options::specifiedCommandLineOptions = {
    { "inputfile",                              { 1, std::list<std::string>() } },
    { "outputfile",                             { 1, std::list<std::string>() } },
    { "inputformat",                            { 1, std::list<std::string>() } },
    { "outputformat",                           { 1, std::list<std::string>() } },
    { "inputoptions",                           { 1, std::list<std::string>() } },
    { "outputoptions",                          { 1, std::list<std::string>() } },
    { "transform",                              { 0, std::list<std::string>() } },
    { "edgedata",                               { 1, std::list<std::string>() } },
};

std::map<std::string, std::string> Options::supportedCommandLineAliases = {
    { "noedgedata",                              "edgedata=void" },
};


// -------- CLASS METHODS -------------------------------------------------- //
// See "Options.h" for documentation.

Options::EOptionSubmitResult Options::SubmitOption(const std::string& optionString)
{
    bool usedAlias = false;
    const std::string* stringToParse = &optionString;
    
    // Check if an alias is being used and, if so, get the actual command-line value string for it.
    if (0 != supportedCommandLineAliases.count(optionString))
    {
        usedAlias = true;
        stringToParse = &(supportedCommandLineAliases.at(optionString));
    }
    
    // Parse the string into a name and a value.
    const size_t posEqualsSign = stringToParse->find_first_of('=');
    
    // No equals sign means the input string is malformed.
    if (std::string::npos == posEqualsSign)
        return EOptionSubmitResult::OptionSubmitResultErrorMalformed;

    // Separate the name and value portions.
    std::string optionName = stringToParse->substr(0, posEqualsSign);
    std::string optionValue = stringToParse->substr(posEqualsSign + 1);

    // Reject empty option values.
    if (optionValue.empty())
        return EOptionSubmitResult::OptionSubmitResultErrorMalformed;
    
    // Check for double-quotes surrounding the option value and remove them if present.
    if ((optionValue.front() == '\"') && (optionValue.back() == '\"') && (optionValue.size() >= 2))
    {
        optionValue.erase(0, 1);
        optionValue.pop_back();
    }
    
    // Check if the option name is supported.
    if (0 == specifiedCommandLineOptions.count(optionName))
        return EOptionSubmitResult::OptionSubmitResultErrorUnsupported;

    // Attempt to submit the option value.
    std::pair<size_t, std::list<std::string>>& optionData = specifiedCommandLineOptions.at(optionName);
    EOptionSubmitResult result = EOptionSubmitResult::OptionSubmitResultOk;

    if ((optionData.second.size() < optionData.first) || (0 == optionData.first))
        optionData.second.push_back(optionValue);
    else
    {
        if (usedAlias)
            result = OptionSubmitResultErrorAliasTooMany;
        else
            result = OptionSubmitResultErrorTooMany;
    }

    return result;
}
