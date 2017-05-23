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
#include <string>
#include <vector>

using namespace GraphTool;


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "OptionContainer.h" for documentation.

OptionContainer::UOptionValue::UOptionValue(void)
{

}

// --------

OptionContainer::UOptionValue::UOptionValue(bool value) : booleanValue(value)
{

}

// --------

OptionContainer::UOptionValue::UOptionValue(int64_t value) : integerValue(value)
{

}

// --------

OptionContainer::UOptionValue::UOptionValue(std::string* value) : stringValue(value)
{

}

// --------

OptionContainer::OptionContainer(EOptionValueType type) : type(type), defaultValue(), defaultValueSpecified(false), maxValueCount(1), values(1)
{

}

// --------

OptionContainer::OptionContainer(const EOptionValueType type, const size_t maxValueCount) : type(type), defaultValue(), defaultValueSpecified(false), maxValueCount(maxValueCount), values()
{

}

// --------

OptionContainer::OptionContainer(const bool defaultValue) : type(EOptionValueType::OptionValueTypeBoolean), defaultValue(defaultValue), defaultValueSpecified(true), maxValueCount(1), values(1)
{

}

// --------

OptionContainer::OptionContainer(const int64_t defaultValue) : type(EOptionValueType::OptionValueTypeInteger), defaultValue(defaultValue), defaultValueSpecified(true), maxValueCount(1), values(1)
{

}

// --------

OptionContainer::OptionContainer(const std::string& defaultValue) : type(EOptionValueType::OptionValueTypeString), defaultValue(new std::string(defaultValue)), defaultValueSpecified(true), maxValueCount(1), values(1)
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

OptionContainer::OptionContainer(const std::string& defaultValue, const size_t maxValueCount) : type(EOptionValueType::OptionValueTypeString), defaultValue(new std::string(defaultValue)), defaultValueSpecified(true), maxValueCount(maxValueCount), values()
{

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


// -------- HELPERS -------------------------------------------------------- //
// See "OptionContainer.h" for documentation.

size_t OptionContainer::GetSubmittedValueCount(void) const
{
    return values.size();
}

// --------

OptionContainer::EOptionValueSubmitResult OptionContainer::SubmitValue(UOptionValue& value)
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
    else if ((0 == index) && defaultValueSpecified)
    {
        value = defaultValue;
        return true;
    }
    else
    {
        return false;
    }
}


// -------- INSTANCE METHODS ----------------------------------------------- //
// See "OptionContainer.h" for documentation.

bool OptionContainer::AreValuesValid(void) const
{
    const size_t valueCount = GetValueCount();

    return ((valueCount > 0) && (valueCount < GetMaxValueCount()));
}

// --------

size_t OptionContainer::GetMaxValueCount(void) const
{
    return maxValueCount;
}

// --------

size_t OptionContainer::GetValueCount(void) const
{
    size_t valueCount = GetSubmittedValueCount();

    if (0 == valueCount && defaultValueSpecified)
        valueCount = 1;

    return valueCount;
}

// --------

OptionContainer::EOptionValueType OptionContainer::GetValueType(void) const
{
    return type;
}

// --------

OptionContainer::EOptionValueSubmitResult OptionContainer::SubmitValue(bool value)
{
    if (type != EOptionValueType::OptionValueTypeBoolean)
        return EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
    else
    {
        UOptionValue uValue = {value};
        return SubmitValue(uValue);
    }
}

// --------

OptionContainer::EOptionValueSubmitResult OptionContainer::SubmitValue(int64_t value)
{
    if (type != EOptionValueType::OptionValueTypeInteger)
        return EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
    else
    {
        UOptionValue uValue = {value};
        return SubmitValue(uValue);
    }
}

// --------

OptionContainer::EOptionValueSubmitResult OptionContainer::SubmitValue(std::string& value)
{
    if (type != EOptionValueType::OptionValueTypeString)
        return EOptionValueSubmitResult::OptionValueSubmitResultWrongType;
    else
    {
        UOptionValue uValue = {new std::string(value)};
        EOptionValueSubmitResult result = SubmitValue(uValue);

        if (EOptionValueSubmitResult::OptionValueSubmitResultOk != result)
            delete uValue.stringValue;

        return result;
    }
}

// --------

bool OptionContainer::QueryValue(bool& value)
{
    return QueryValueAt(0, value);
}

// --------

bool OptionContainer::QueryValue(int64_t& value)
{
    return QueryValueAt(0, value);
}

// --------

bool OptionContainer::QueryValue(std::string& value)
{
    return QueryValueAt(0, value);
}

// --------

bool OptionContainer::QueryValueAt(size_t index, bool& value)
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

bool OptionContainer::QueryValueAt(size_t index, int64_t& value)
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

bool OptionContainer::QueryValueAt(size_t index, std::string& value)
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
