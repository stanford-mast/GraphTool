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

#include <cstddef>
#include <list>
#include <map>
#include <string>
#include <utility>

using namespace GraphTool;


// -------- CLASS VARIABLES ------------------------------------------------ //
// See "Options.h" for documentation.

std::unordered_map<std::string, OptionContainer*> Options::specifiedCommandLineOptions = {
    { "inputfile",                              new OptionContainer(OptionContainer::EOptionValueType::OptionValueTypeString) },
    { "outputfile",                             new OptionContainer(OptionContainer::EOptionValueType::OptionValueTypeString) },
    { "inputformat",                            new OptionContainer(OptionContainer::EOptionValueType::OptionValueTypeString) },
    { "outputformat",                           new OptionContainer(OptionContainer::EOptionValueType::OptionValueTypeString) },
    { "inputoptions",                           new OptionContainer("") },
    { "outputoptions",                          new OptionContainer("") },
    { "edgedata",                               new OptionContainer("void") }
};

std::unordered_map<std::string, std::string> Options::supportedCommandLineAliases = {
    { "noedgedata",                             "edgedata=void" },
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
    

    return EOptionSubmitResult::OptionSubmitResultOk;
}
