/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file OptionContainer.cpp
 *   Implementation of an object that holds all values associated with a
 *   single command-line option.
 *****************************************************************************/

#include "OptionContainer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <set>
#include <string>
#include <vector>

using namespace GraphTool;


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "OptionContainer.h" for documentation.

UOptionValue::UOptionValue(void)
{

}

// --------

UOptionValue::UOptionValue(bool value) : booleanValue(value)
{

}

// --------

UOptionValue::UOptionValue(int64_t value) : integerValue(value)
{

}

// --------

UOptionValue::UOptionValue(std::string* value) : stringValue(value)
{

}

// --------

OptionContainer::OptionContainer(EOptionValueType type) : type(type), defaultValue(), defaultValueSpecified(false), maxValueCount(1), values()
{
    values.reserve(1);
}

// --------

OptionContainer::OptionContainer(const EOptionValueType type, const size_t maxValueCount) : type(type), defaultValue(), defaultValueSpecified(false), maxValueCount(maxValueCount), values()
{
    values.reserve(1);
}

// --------

OptionContainer::OptionContainer(const bool defaultValue) : type(EOptionValueType::OptionValueTypeBoolean), defaultValue(defaultValue), defaultValueSpecified(true), maxValueCount(1), values()
{
    
}

// --------

OptionContainer::OptionContainer(const int64_t defaultValue) : type(EOptionValueType::OptionValueTypeInteger), defaultValue(defaultValue), defaultValueSpecified(true), maxValueCount(1), values()
{
    
}

// --------

OptionContainer::OptionContainer(const char* defaultValue) : type(EOptionValueType::OptionValueTypeString), defaultValue(new std::string(defaultValue)), defaultValueSpecified(true), maxValueCount(1), values()
{

}

// --------

OptionContainer::OptionContainer(const std::string& defaultValue) : type(EOptionValueType::OptionValueTypeString), defaultValue(new std::string(defaultValue)), defaultValueSpecified(true), maxValueCount(1), values()
{
    
}

// --------

OptionContainer::OptionContainer(const bool defaultValue, const size_t maxValueCount) : type(EOptionValueType::OptionValueTypeBoolean), defaultValue(defaultValue), defaultValueSpecified(true), maxValueCount(maxValueCount), values()
{

}

// --------

OptionContainer::OptionContainer(const int64_t defaultValue, const size_t maxValueCount) : type(EOptionValueType::OptionValueTypeInteger), defaultValue(defaultValue), defaultValueSpecified(true), maxValueCount(maxValueCount), values()
{

}

// --------

OptionContainer::OptionContainer(const char* defaultValue, const size_t maxValueCount) : type(EOptionValueType::OptionValueTypeString), defaultValue(new std::string(defaultValue)), defaultValueSpecified(true), maxValueCount(maxValueCount), values()
{

}

// --------

OptionContainer::OptionContainer(const std::string& defaultValue, const size_t maxValueCount) : type(EOptionValueType::OptionValueTypeString), defaultValue(new std::string(defaultValue)), defaultValueSpecified(true), maxValueCount(maxValueCount), values()
{

}

// --------

OptionContainer::OptionContainer(const OptionContainer& other) : type(other.type), defaultValueSpecified(other.defaultValueSpecified), maxValueCount(other.maxValueCount), values()
{
    if (EOptionValueType::OptionValueTypeString == other.type)
    {
        if (other.defaultValueSpecified)
            defaultValue.stringValue = new std::string(*other.defaultValue.stringValue);

        for (uint64_t i = 0; i < other.values.size(); ++i)
            values[i].stringValue = new std::string(*other.values[i].stringValue);
    }
    else
    {
        defaultValue = other.defaultValue;
        values = other.values;
    }
}

// --------

OptionContainer::~OptionContainer()
{
    // Destroy all allocated strings.
    if (OptionValueTypeString == this->type)
    {
        if (defaultValueSpecified)
            delete defaultValue.stringValue;
        
        if (values.size() > 0)
        {
            for (auto it = values.begin(); it != values.end(); ++it)
                delete it->stringValue;
        }
    }
}

// --------

EnumOptionContainer::EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap) : OptionContainer(EOptionValueType::OptionValueTypeInteger), enumMap(enumMap), validIntegers()
{

}

// --------

EnumOptionContainer::EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap, const size_t maxValueCount) : OptionContainer(EOptionValueType::OptionValueTypeInteger, maxValueCount), enumMap(enumMap), validIntegers()
{

}

// --------

EnumOptionContainer::EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap, const int64_t defaultValue) : OptionContainer(defaultValue), enumMap(enumMap), validIntegers()
{

}

// --------

EnumOptionContainer::EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap, const int64_t defaultValue, const size_t maxValueCount) : OptionContainer(defaultValue, maxValueCount), enumMap(enumMap), validIntegers()
{

}


// -------- HELPERS -------------------------------------------------------- //
// See "OptionContainer.h" for documentation.

const UOptionValue& OptionContainer::GetDefaultValue(void) const
{
    return defaultValue;
}

// --------

size_t OptionContainer::GetSubmittedValueCount(void) const
{
    return values.size();
}

// --------

bool OptionContainer::IsDefaultValuePresent(void) const
{
    return defaultValueSpecified;
}

// --------

bool OptionContainer::ParseBoolean(std::string& stringToParse, bool& boolValue)
{
    static const std::string trueStrings[] = { "t", "true", "on", "y", "yes", "enabled", "1" };
    static const std::string falseStrings[] = { "f", "false", "off", "n", "no", "disabled", "0" };

    // Check if the string represents a value of 'true'.
    for (size_t i = 0; i < sizeof(trueStrings) / sizeof(trueStrings[0]); ++i)
    {
        if (0 == _strnicmp(stringToParse.c_str(), trueStrings[i].c_str(), stringToParse.size()))
        {
            boolValue = true;
            return true;
        }
    }

    // Check if the string represents a value of 'false'.
    for (size_t i = 0; i < sizeof(falseStrings) / sizeof(falseStrings[0]); ++i)
    {
        if (0 == _strnicmp(stringToParse.c_str(), falseStrings[i].c_str(), stringToParse.size()))
        {
            boolValue = false;
            return true;
        }
    }

    return false;
}

// --------

EOptionValueSubmitResult OptionContainer::SubmitValue(UOptionValue& value)
{
    if (GetSubmittedValueCount() < GetMaxValueCount())
    {
        values.push_back(value);
        return EOptionValueSubmitResult::OptionValueSubmitResultOk;
    }
    else
        return EOptionValueSubmitResult::OptionValueSubmitResultTooMany;
}

// --------

bool OptionContainer::QueryValueAt(size_t index, UOptionValue& value) const
{
    if (index < GetSubmittedValueCount())
    {
        value = values[index];
        return true;
    }
    else if (IsDefaultValuePresent())
    {
        value = defaultValue;
        return true;
    }
    else
    {
        return false;
    }
}


// -------- VIRTUAL INSTANCE METHODS --------------------------------------- //
// See "OptionContainer.h" for documentation.

bool OptionContainer::AreValuesValid(void) const
{
    const size_t valueCount = GetValueCount();

    return ((valueCount > 0) && (valueCount <= GetMaxValueCount()));
}

// --------

EOptionValueSubmitResult OptionContainer::ParseAndSubmitValue(std::string& valueString)
{
    EOptionValueSubmitResult result = EOptionValueSubmitResult::OptionValueSubmitResultOk;
    char* parsedEndPos = NULL;
    UOptionValue parsedValue;

    switch (type)
    {
    case EOptionValueType::OptionValueTypeBoolean:

        // Attempt to parse the value as a Boolean value.
        if (false == ParseBoolean(valueString, parsedValue.booleanValue))
            result = EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
        else
            result = SubmitValue(parsedValue.booleanValue);

        break;

    case EOptionValueType::OptionValueTypeInteger:

        // Attempt to parse the value as an integer.
        parsedValue.integerValue = (int64_t)strtoll(valueString.c_str(), &parsedEndPos, 0);

        if ('\0' != *parsedEndPos)
            result = EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
        else
            result = SubmitValue(parsedValue.integerValue);

        break;

    case EOptionValueType::OptionValueTypeString:

        // Attempt to submit the string value directly.
        result = SubmitValue(valueString);
        break;

    default:

        // This should never happen.
        result = EOptionValueSubmitResult::OptionValueSubmitResultInternalError;
        break;
    }

    return result;
}

// --------

EOptionValueSubmitResult EnumOptionContainer::ParseAndSubmitValue(std::string& valueString)
{
    if (0 != enumMap.count(valueString))
    {
        int64_t valueToSubmit = enumMap.at(valueString);

        validIntegers.insert(valueToSubmit);
        return SubmitValue(valueToSubmit);
    }
    else
        return EOptionValueSubmitResult::OptionValueSubmitResultOutOfRange;
}

// --------

EOptionValueSubmitResult OptionContainer::SubmitValue(bool value)
{
    if (type != EOptionValueType::OptionValueTypeBoolean)
        return EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
    else
    {
        UOptionValue uValue(value);
        return SubmitValue(uValue);
    }
}

// --------

EOptionValueSubmitResult OptionContainer::SubmitValue(int64_t value)
{
    if (type != EOptionValueType::OptionValueTypeInteger)
        return EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
    else
    {
        UOptionValue uValue(value);
        return SubmitValue(uValue);
    }
}

// --------

EOptionValueSubmitResult EnumOptionContainer::SubmitValue(int64_t value)
{
    if (0 == validIntegers.count(value))
        return EOptionValueSubmitResult::OptionValueSubmitResultOutOfRange;
    else
        return OptionContainer::SubmitValue(value);
}

// --------

EOptionValueSubmitResult OptionContainer::SubmitValue(std::string& value)
{
    if (type != EOptionValueType::OptionValueTypeString)
        return EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
    else
    {
        UOptionValue uValue(new std::string(value));
        EOptionValueSubmitResult result = SubmitValue(uValue);

        if (EOptionValueSubmitResult::OptionValueSubmitResultOk != result)
            delete uValue.stringValue;

        return result;
    }
}


// -------- INSTANCE METHODS ----------------------------------------------- //
// See "OptionContainer.h" for documentation.

size_t OptionContainer::GetMaxValueCount(void) const
{
    return maxValueCount;
}

// --------

size_t OptionContainer::GetValueCount(void) const
{
    size_t valueCount = GetSubmittedValueCount();

    if (0 == valueCount && IsDefaultValuePresent())
        valueCount = 1;

    return valueCount;
}

// --------

EOptionValueType OptionContainer::GetValueType(void) const
{
    return type;
}

// --------

bool OptionContainer::QueryValue(bool& value) const
{
    return QueryValueAt(0, value);
}

// --------

bool OptionContainer::QueryValue(int64_t& value) const
{
    return QueryValueAt(0, value);
}

// --------

bool OptionContainer::QueryValue(std::string& value) const
{
    return QueryValueAt(0, value);
}

// --------

bool OptionContainer::QueryValueAt(size_t index, bool& value) const
{
    if (EOptionValueType::OptionValueTypeBoolean != type)
        return false;
    else
    {
        UOptionValue uValue;

        if (true == QueryValueAt(index, uValue))
        {
            value = uValue.booleanValue;
            return true;
        }
        else
            return false;
    }
}

// --------

bool OptionContainer::QueryValueAt(size_t index, int64_t& value) const
{
    if (EOptionValueType::OptionValueTypeInteger != type)
        return false;
    else
    {
        UOptionValue uValue;

        if (true == QueryValueAt(index, uValue))
        {
            value = uValue.integerValue;
            return true;
        }
        else
            return false;
    }
}

// --------

bool OptionContainer::QueryValueAt(size_t index, std::string& value) const
{
    if (EOptionValueType::OptionValueTypeString != type)
        return false;
    else
    {
        UOptionValue uValue;

        if (true == QueryValueAt(index, uValue))
        {
            value = *(uValue.stringValue);
            return true;
        }
        else
            return false;
    }
}
