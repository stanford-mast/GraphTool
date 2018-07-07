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


namespace GraphTool
{
    // -------- CONSTRUCTION AND DESTRUCTION ------------------------------- //
    // See "Options.h" for documentation.

    Options::Options(const char* const commandLine, std::map<std::string, OptionContainer*>& specifiedOptions, const std::vector<std::string>* const prefixStrings, const std::vector<std::string>* const versionStrings, const std::vector<std::string>* const helpStrings, const std::string* const documentationString, const std::string* versionString) : commandLine(commandLine), helpStrings(helpStrings), prefixStrings(prefixStrings), versionStrings(versionStrings), documentationString(documentationString), versionString(versionString), specifiedOptions(specifiedOptions)
    {

    }

    // -------- HELPERS ---------------------------------------------------- //
    // See "Options.h" for documentation.

    bool Options::IsHelpString(const char* optionString) const
    {
        if (UsingHelpStrings() && (NULL != optionString))
        {
            std::string testString(optionString);

            for (auto it = helpStrings->cbegin(); it != helpStrings->cend(); ++it)
            {
                if (*it == testString)
                    return true;
            }
        }

        return false;
    }
    
    // --------
    
    bool Options::IsVersionString(const char* optionString) const
    {
        if (UsingVersionStrings() && (NULL != optionString))
        {
            std::string testString(optionString);

            for (auto it = versionStrings->cbegin(); it != versionStrings->cend(); ++it)
            {
                if (*it == testString)
                    return true;
            }
        }

        return false;
    }

    // --------

    size_t Options::PrefixLength(const char* optionString) const
    {
        if (UsingPrefixStrings() && (NULL != optionString))
        {
            for (auto it = prefixStrings->cbegin(); it != prefixStrings->cend(); ++it)
            {
                if (0 == strncmp(optionString, it->c_str(), it->size()))
                    return it->size();
            }
        }

        return 0;
    }

    // --------

    void Options::PrintErrorCommon(void) const
    {
        if (UsingHelpStrings())
            fprintf(stderr, "Try '%s %s%s' for more information.\n", commandLine, (UsingPrefixStrings() ? (*prefixStrings)[0].c_str() : ""), (*helpStrings)[0].c_str());
    }

    // --------

    void Options::PrintErrorInternal(void) const
    {
        fprintf(stderr, "%s: Internal error while processing options.\n", commandLine);
    }

    // --------

    void Options::PrintErrorMalformed(const char* optionString) const
    {
        fprintf(stderr, "%s: Invalid option '%s'.\n", commandLine, optionString);
        PrintErrorCommon();
    }

    // --------

    void Options::PrintErrorMissing(const char* optionName) const
    {
        fprintf(stderr, "%s: Missing required option '%s'.\n", commandLine, optionName);
        PrintErrorCommon();
    }

    // --------

    void Options::PrintErrorQuantityMismatch(const char* optionName1, const char* optionName2) const
    {
        fprintf(stderr, "%s: Mismatch between options '%s' and '%s'.\n", commandLine, optionName1, optionName2);
        PrintErrorCommon();
    }

    // --------

    void Options::PrintErrorTooMany(const char* optionName) const
    {
        fprintf(stderr, "%s: Option '%s' specified too many times.\n", commandLine, optionName);
        PrintErrorCommon();
    }

    // --------

    void Options::PrintErrorUnsupported(const char* optionName) const
    {
        fprintf(stderr, "%s: Invalid option '%s'.\n", commandLine, optionName);
        PrintErrorCommon();
    }

    // --------

    void Options::PrintErrorValueRejected(const char* optionName, const char* optionValue) const
    {
        fprintf(stderr, "%s: Invalid value '%s' for option '%s'.\n", commandLine, optionValue, optionName);
        PrintErrorCommon();
    }

    // --------

    void Options::PrintHelp(void) const
    {
        if (NULL != documentationString)
            fputs(documentationString->c_str(), stderr);
    }
    
    // --------
    
    void Options::PrintVersion(void) const
    {
        if (NULL != versionString)
            fputs(versionString->c_str(), stderr);
    }

    // --------

    bool Options::UsingHelpStrings(void) const
    {
        return (NULL != helpStrings);
    }

    // --------

    bool Options::UsingPrefixStrings(void) const
    {
        return (NULL != prefixStrings);
    }
    
    // --------
    
    bool Options::UsingVersionStrings(void) const
    {
        return (NULL != versionStrings);
    }


    // -------- INSTANCE METHODS ------------------------------------------- //
    // See "Options.h" for documentation.

    bool Options::FillFromStringArray(const size_t count, const char* strings[])
    {
        if (NULL == strings)
            return false;

        for (size_t i = 0; i < count; ++i)
        {
            const bool submitResult = SubmitOption(strings[i]);

            if (true != submitResult)
                return false;
        }

        return true;
    }

    // --------

    const OptionContainer* Options::GetOptionValues(const std::string& optionName) const
    {
        if (0 != specifiedOptions.count(optionName))
            return specifiedOptions.at(optionName);
        else
            return NULL;
    }

    // --------

    bool Options::SubmitOption(const char* optionString)
    {
        const char* stringToParse = optionString;

        // Handle the command-line option prefix. It is an error for it to be missing if prefixes are enabled.
        if (UsingPrefixStrings())
        {
            const size_t optionPrefixLength = PrefixLength(optionString);
            stringToParse += optionPrefixLength;

            if (0 == optionPrefixLength)
            {
                PrintErrorMalformed(optionString);
                return false;
            }
        }

        // Check if the option is a help string.
        if (UsingHelpStrings() && IsHelpString(stringToParse))
        {
            PrintHelp();
            return false;
        }
        
        // Check if the option is a version information string.
        if (UsingVersionStrings() && IsVersionString(stringToParse))
        {
            PrintVersion();
            return false;
        }

        // Parse the string into a name and a value.
        const char* posEqualsSign = strchr(stringToParse, '=');

        // No equals sign means the input string is malformed.
        if (NULL == posEqualsSign)
        {
            PrintErrorMalformed(optionString);
            return false;
        }

        // Separate the name and value portions.
        std::string optionName(stringToParse, posEqualsSign - stringToParse);
        std::string optionValue(posEqualsSign + 1);

        // Reject empty option values.
        if (optionValue.empty())
        {
            PrintErrorMalformed(optionString);
            return false;
        }

        // Check for single-quotes or double-quotes surrounding the option value and remove them if present.
        if ((optionValue.front() == optionValue.back()) && ((optionValue.front() == '\"') || (optionValue.front() == '\'')) && (optionValue.size() >= 2))
        {
            optionValue.erase(0, 1);
            optionValue.pop_back();
        }

        // Check if the option name is supported.
        if (0 == specifiedOptions.count(optionName))
        {
            PrintErrorUnsupported(optionName.c_str());
            return false;
        }

        // Attempt to submit the option value.
        OptionContainer* optionContainer = specifiedOptions.at(optionName.c_str());
        
        switch (optionContainer->ParseAndSubmitValue(optionValue))
        {
        case EOptionValueSubmitResult::OptionValueSubmitResultOk:
            break;
            
        case EOptionValueSubmitResult::OptionValueSubmitResultTooMany:
            PrintErrorTooMany(optionName.c_str());
            return false;
            
        default:
            PrintErrorValueRejected(optionName.c_str(), optionValue.c_str());
            return false;
        }
        
        return true;
    }

    // --------

    bool Options::ValidateOptions(void) const
    {
        // Verify that required options are all supplied.
        for (auto it = specifiedOptions.cbegin(); it != specifiedOptions.cend(); ++it)
        {
            if (!(it->second->AreValuesValid()))
            {
                PrintErrorMissing(it->first.c_str());
                return false;
            }
        }

        return true;
    }

    // --------

    bool Options::VerifyEqualValueCount(const char* optionName1, const char* optionName2) const
    {
        // Get the option containers for the two option names.
        const OptionContainer* options1 = GetOptionValues(optionName1);
        const OptionContainer* options2 = GetOptionValues(optionName2);

        if (NULL == options1 || NULL == options2)
        {
            PrintErrorInternal();
            return false;
        }

        if (options1->GetValueCount() != options2->GetValueCount())
        {
            PrintErrorQuantityMismatch(optionName1, optionName2);
            return false;
        }

        return true;
    }
}
