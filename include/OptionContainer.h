/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file OptionContainer.h
 *   Declaration of an object that holds all values associated with a single
 *   command-line option.
 *****************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


namespace GraphTool
{
    /// Specifies the internal type of the option value.
    /// All values held by each OptionContainer object have the same type.
    enum EOptionValueType
    {
        OptionValueTypeBoolean,                                             ///< Boolean `true` or `false`.
        OptionValueTypeInteger,                                             ///< Signed integer.
        OptionValueTypeString,                                              ///< String.
    };

    /// Specifies the result of an attempt to submit a command-line option value.
    enum EOptionValueSubmitResult
    {
        OptionValueSubmitResultOk,                                          ///< Success.
        OptionValueSubmitResultWrongType,                                   ///< Incorrect type for submitted value.
        OptionValueSubmitResultTooMany,                                     ///< Number of values for the command-line option is already at its maximum.
        OptionValueSubmitResultOutOfRange,                                  ///< Submitted value is outside the range of acceptable values.
        OptionValueSubmitResultInternalError,                               ///< Something unexpected happened, resulting in an error.
    };

    /// Holds an option value itself.
    /// One field exists for each possible supported type of option value.
    union UOptionValue
    {
        bool booleanValue;                                                  ///< Boolean value.
        int64_t integerValue;                                               ///< Signed integer value.
        std::string* stringValue;                                           ///< String value.

        /// Default constructor.
        UOptionValue(void);

        /// Convenience constructor from a Boolean-typed value.
        /// @param [in] value Value to initialize.
        UOptionValue(bool value);

        /// Convenience constructor from an integer-typed value.
        /// @param [in] value Value to initialize.
        UOptionValue(int64_t value);

        /// Convenience constructor from a string-typed value.
        /// @param [in] value Value to initialize.
        UOptionValue(std::string* value);
    };
    
    /// Holds all values associated with a single command-line option.
    class OptionContainer
    {
    public:
        // -------- CONSTANTS ---------------------------------------------- //

        /// Specifies that there is no limit to the number of values accepted for a particular command-line option.
        static const size_t kUnlimitedValueCount = SIZE_MAX;


    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Holds the type of option value.
        const EOptionValueType type;
        
        /// Holds the default value for this command-line option.
        UOptionValue defaultValue;

        /// Specifies whether or not this command-line option has a default.
        /// If so, it is used if no values are specified (and therefore this command-line option is optional).
        /// If not, it is an error if no values are specified.
        const bool defaultValueSpecified;
        
        /// Specifies the maximum number of values that can be supplied for this command-line option to validate.
        /// Defaults to 1, meaning that the command-line option takes at most a single value.
        const size_t maxValueCount;
        
        /// Holds the option values themselves.
        std::vector<UOptionValue> values;
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //
        
        /// Constructs a command-line option value container.
        /// @param [in] type Value type to use.
        OptionContainer(const EOptionValueType type);

        /// Constructs a command-line option value container.
        /// @param [in] type Value type to use.
        /// @param [in] maxValueCount Maximum number of values allowable.
        OptionContainer(const EOptionValueType type, const size_t maxValueCount);

        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        OptionContainer(const bool defaultValue);

        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        OptionContainer(const int64_t defaultValue);

        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        OptionContainer(const char* defaultValue);
        
        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        OptionContainer(const std::string& defaultValue);

        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        /// @param [in] maxValueCount Maximum number of values allowable.
        OptionContainer(const bool defaultValue, const size_t maxValueCount);

        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        /// @param [in] maxValueCount Maximum number of values allowable.
        OptionContainer(const int64_t defaultValue, const size_t maxValueCount);

        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        /// @param [in] maxValueCount Maximum number of values allowable.
        OptionContainer(const char* defaultValue, const size_t maxValueCount);

        /// Constructs a command-line option value container.
        /// @param [in] defaultValue Default value for this option.
        /// @param [in] maxValueCount Maximum number of values allowable.
        OptionContainer(const std::string& defaultValue, const size_t maxValueCount);
        
        /// Copy constructor.
        OptionContainer(const OptionContainer& other);
        
        /// Default destructor.
        virtual ~OptionContainer(void);
        
        
    protected:
        // -------- HELPERS ------------------------------------------------ //

        /// Provides read-only access to the default value.
        const UOptionValue& GetDefaultValue(void) const;
        
        /// Internal operation that specifies the number of submitted values.
        size_t GetSubmittedValueCount(void) const;
        
        /// Specifies if a default value has been supplied.
        bool IsDefaultValuePresent(void) const;
        
        /// Attempts to parse a Boolean-typed value from a string.
        /// @param [in] stringToParse String to be parsed.
        /// @param [out] boolValue Boolean variable to be updated with the result of the parse.
        /// @return `true` if parsing was successful, `false` otherwise (in which case no update takes place).
        static bool ParseBoolean(std::string& stringToParse, bool& boolValue);
        
        /// Internal operation that submits a type-validated value to this object.
        EOptionValueSubmitResult SubmitValue(UOptionValue& value);

        /// Internal operation that obtains the value at a specific index.
        /// Uses the default value if it exists and the index is out of range.
        /// @param [in] index Position of interest.
        /// @param [out] value Variable to be filled with the value, if available.
        bool QueryValueAt(size_t index, UOptionValue& value) const;


    public:
        // -------- VIRTUAL INSTANCE METHODS ------------------------------- //

        /// Indicates whether or not this object is in a valid state.
        /// A valid state indicates that the number of specified options is within the valid range.
        /// If a default is specified, this object is always in a valid state.
        /// If not, then it is only in a valid state if at least one value has been specified.
        /// This is potentially subject to additional conditions imposed by subclasses.
        /// @return `true` if so, `false` if not.
        virtual bool AreValuesValid(void) const;

        /// Attempts to parse and submit a value to this object.
        /// Parsing behavior depends on this object's value type.
        /// Subclasses can customize the behavior of parsing, for example, to support special values or other mappings.
        /// @param [in] valueString String representation of the value to submit.
        /// @return Enumerator indicating the result of the value parsing and submission attempt.
        virtual EOptionValueSubmitResult ParseAndSubmitValue(std::string& valueString);

        /// Attempts to submit a Boolean-typed value to this object.
        /// If successful, the value is appended to those contained in this object.
        /// Subclasses can perform further filtering or checks on submission attempts.
        /// @param [in] value Value to submit.
        /// @return Enumerator indicating the result of the value submission attempt.
        virtual EOptionValueSubmitResult SubmitValue(bool value);

        /// Attempts to submit an integer-typed value to this object.
        /// If successful, the value is appended to those contained in this object.
        /// Subclasses can perform further filtering or checks on submission attempts.
        /// @param [in] value Value to submit.
        /// @return Enumerator indicating the result of the value submission attempt.
        virtual EOptionValueSubmitResult SubmitValue(int64_t value);

        /// Attempts to submit a string-typed value to this object.
        /// If successful, the value is appended to those contained in this object.
        /// Subclasses can perform further filtering or checks on submission attempts.
        /// @param [in] value Value to submit.
        /// @return Enumerator indicating the result of the value submission attempt.
        virtual EOptionValueSubmitResult SubmitValue(std::string& value);


        // -------- INSTANCE METHODS --------------------------------------- //
        
        /// Specifies the maximum number of values acceptable for this object, configured at construction time.
        /// @return Maximum number of values this object will accept.
        size_t GetMaxValueCount(void) const;
        
        /// Specifies the number of values available for this object.
        /// If values have been submitted, the default value is completely ignored.
        /// If no values have been submitted and the object has a default value, returns 1.
        /// If no values have been submitted and the object has no default, returns 0.
        /// @return Number of values avaliable for this object.
        size_t GetValueCount(void) const;
        
        /// Specifies the value type for this object.
        /// @return Enumerator that corresponds to this object's value type.
        EOptionValueType GetValueType(void) const;

        /// Queries the first value for this object's command-line setting.
        /// @param [out] value Variable to be filled with the value, if it is available.
        /// @return `true` if the correctly-typed value exists and was filled, `false` otherwise.
        bool QueryValue(bool& value) const;

        /// Queries the first value for this object's command-line setting.
        /// @param [out] value Variable to be filled with the value, if it is available.
        /// @return `true` if the correctly-typed value exists and was filled, `false` otherwise.
        bool QueryValue(int64_t& value) const;

        /// Queries the first value for this object's command-line setting.
        /// @param [out] value Variable to be filled with the value, if it is available.
        /// @return `true` if the correctly-typed value exists and was filled, `false` otherwise.
        bool QueryValue(std::string& value) const;

        /// Queries the value for this object's command-line setting at the specified position.
        /// @param [in] index Position of interest.
        /// @param [out] value Variable to be filled with the value, if it is available.
        /// @return `true` if the correctly-typed value exists and was filled, `false` otherwise.
        bool QueryValueAt(size_t index, bool& value) const;

        /// Queries the value for this object's command-line setting at the specified position.
        /// @param [in] index Position of interest.
        /// @param [out] value Variable to be filled with the value, if it is available.
        /// @return `true` if the correctly-typed value exists and was filled, `false` otherwise.
        bool QueryValueAt(size_t index, int64_t& value) const;

        /// Queries the value for this object's command-line setting at the specified position.
        /// @param [in] index Position of interest.
        /// @param [out] value Variable to be filled with the value, if it is available.
        /// @return `true` if the correctly-typed value exists and was filled, `false` otherwise.
        bool QueryValueAt(size_t index, std::string& value) const;
    };


    /// Custom command-line option container for handling enumerations.
    /// Internally uses integers to store information.
    /// Maps from strings to integers and submits the resulting integers during parsing.
    /// Intended to have values inserted by string parsing rather than by direct integer insertion.
    class EnumOptionContainer : public OptionContainer
    {
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Specifies the enumeration as a map from strings to integers.
        /// The map itself would mostly likely be a statically-created data structure, so only a reference to it is required.
        const std::unordered_map<std::string, int64_t>& enumMap;

        /// Holds all the integers that are allowed to be set.
        /// This is based on the contents of the enumeration.
        std::set<int64_t> validIntegers;
        
        
    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Constructs an instance of this object with the given enumeration map.
        /// @param [in] enumMap Enumeration map to use.
        EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap);

        /// Constructs an instance of this object with the given enumeration map.
        /// @param [in] enumMap Enumeration map to use.
        /// @param [in] maxValueCount Maximum number of values allowable.
        EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap, const size_t maxValueCount);

        /// Constructs an instance of this object with the given enumeration map.
        /// @param [in] enumMap Enumeration map to use.
        /// @param [in] defaultValue Default value to use (not verified to be part of the enumeration).
        EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap, const int64_t defaultValue);

        /// Constructs an instance of this object with the given enumeration map.
        /// @param [in] enumMap Enumeration map to use.
        /// @param [in] defaultValue Default value to use (not verified to be part of the enumeration).
        /// @param [in] maxValueCount Maximum number of values allowable.
        EnumOptionContainer(const std::unordered_map<std::string, int64_t>& enumMap, const int64_t defaultValue, const size_t maxValueCount);


        // -------- VIRTUAL INSTANCE METHODS ------------------------------- //
        // See above for documenatation.
        
        virtual EOptionValueSubmitResult ParseAndSubmitValue(std::string& valueString);
        virtual EOptionValueSubmitResult SubmitValue(int64_t value);
    };
}
