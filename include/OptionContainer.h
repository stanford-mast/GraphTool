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
#include <string>
#include <vector>


namespace GraphTool
{
    /// Holds all values associated with a single command-line option.
    class OptionContainer
    {
    public:
        // -------- TYPE DEFINITIONS --------------------------------------- //
        
        /// Specifies the internal type of the option value.
        /// All values held by each OptionContainer object have the same type.
        enum EOptionValueType
        {
            OptionValueTypeBoolean,                                         ///< Boolean `true` or `false`.
            OptionValueTypeInteger,                                         ///< Signed integer.
            OptionValueTypeString,                                          ///< String.
        };

        /// Specifies the result of an attempt to submit a command-line option value.
        enum EOptionValueSubmitResult
        {
            OptionValueSubmitResultOk,                                      ///< Success.
            OptionValueSubmitResultWrongType,                               ///< Incorrect type for submitted value.
            OptionValueSubmitResultTooMany,                                 ///< Number of values for the command-line option is already at its maximum.
        };

        /// Holds an option value itself.
        /// One field exists for each possible supported type of option value.
        union UOptionValue
        {
            bool booleanValue;                                              ///< Boolean value.
            int64_t integerValue;                                           ///< Signed integer value.
            std::string* stringValue;                                       ///< String value.

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
        
        
    private:
        // -------- HELPERS ------------------------------------------------ //

        /// Internal operation that specifies the number of submitted values.
        size_t GetSubmittedValueCount(void) const;
        
        /// Internal operation that submits a type-validated value to this object.
        EOptionValueSubmitResult SubmitValue(UOptionValue& value);

        /// Internal operation that obtains the value at a specific index.
        /// If the specified index is zero and a default value exists, uses that.
        /// @param [in] index Position of interest.
        /// @param [out] value Variable to be filled with the value, if available.
        bool QueryValueAt(size_t index, UOptionValue& value) const;


    public:
        // -------- INSTANCE METHODS --------------------------------------- //

        /// Indicates whether or not this object is in a valid state.
        /// A valid state indicates that the number of specified options is within the valid range.
        /// If a default is specified, this object is always in a valid state.
        /// If not, then it is only in a valid state if at least one value has been specified.
        /// @return `true` if so, `false` if not.
        bool AreValuesValid(void) const;
        
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

        /// Attempts to submit a Boolean-typed value to this object.
        /// If successful, the value is appended to those contained in this object.
        /// @param [in] value Value to submit.
        /// @return Enumerator indicating the result of the value submission attempt.
        EOptionValueSubmitResult SubmitValue(bool value);

        /// Attempts to submit an integer-typed value to this object.
        /// If successful, the value is appended to those contained in this object.
        /// @param [in] value Value to submit.
        /// @return Enumerator indicating the result of the value submission attempt.
        EOptionValueSubmitResult SubmitValue(int64_t value);

        /// Attempts to submit a string-typed value to this object.
        /// If successful, the value is appended to those contained in this object.
        /// @param [in] value Value to submit.
        /// @return Enumerator indicating the result of the value submission attempt.
        EOptionValueSubmitResult SubmitValue(std::string& value);

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
}
