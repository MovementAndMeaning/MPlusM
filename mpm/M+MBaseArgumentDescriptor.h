//--------------------------------------------------------------------------------------------------
//
//  File:       mpm/M+MBaseArgumentDescriptor.h
//
//  Project:    M+M
//
//  Contains:   The class declaration for the minimal functionality required to represent a
//              command-line argument.
//
//  Written by: Norman Jaffe
//
//  Copyright:  (c) 2015 by H Plus Technologies Ltd. and Simon Fraser University.
//
//              All rights reserved. Redistribution and use in source and binary forms, with or
//              without modification, are permitted provided that the following conditions are met:
//                * Redistributions of source code must retain the above copyright notice, this list
//                  of conditions and the following disclaimer.
//                * Redistributions in binary form must reproduce the above copyright notice, this
//                  list of conditions and the following disclaimer in the documentation and / or
//                  other materials provided with the distribution.
//                * Neither the name of the copyright holders nor the names of its contributors may
//                  be used to endorse or promote products derived from this software without
//                  specific prior written permission.
//
//              THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//              EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//              OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//              SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//              INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//              TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//              BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//              CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//              ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//              DAMAGE.
//
//  Created:    2015-05-15
//
//--------------------------------------------------------------------------------------------------

#if (! defined(MpMBaseArgumentDescriptor_H_))
# define MpMBaseArgumentDescriptor_H_ /* Header guard */

# include <mpm/M+MCommon.h>

# if defined(__APPLE__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wnon-virtual-dtor"
# endif // defined(__APPLE__)
# include <mpm/optionparser.h>
# if defined(__APPLE__)
#  pragma clang diagnostic pop
# endif // defined(__APPLE__)

# if defined(__APPLE__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunknown-pragmas"
#  pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
# endif // defined(__APPLE__)
/*! @file
 @brief The class declaration for the minimal functionality required to represent a command-line
 argument. */
# if defined(__APPLE__)
#  pragma clang diagnostic pop
# endif // defined(__APPLE__)

namespace MplusM
{
    namespace Utilities
    {
        /*! @brief An argument description. */
        class BaseArgumentDescriptor
        {
        public :
            
            /*! @brief The constructor.
             @param argName The name of the command-line argument.
             @param argDescription A description of the command-line argument.
             @param isOptional @c true if the argument is optional and @c false otherwise. */
            BaseArgumentDescriptor(const Common::YarpString & argName,
                                   const Common::YarpString & argDescription,
                                   const bool                 isOptional);
            
            /*! @brief The destructor. */
            virtual ~BaseArgumentDescriptor(void);
            
            /*! @brief Return the description of the command-line argument.
             @returns The description of the command-line argument. */
            inline const Common::YarpString & argumentDescription(void)
            const
            {
                return _argDescription;
            } // argumentDescription

            /*! @brief Return the name of the command-line argument.
             @returns The name of the command-line argument. */
            inline const Common::YarpString & argumentName(void)
            const
            {
                return _argName;
            } // argumentName
            
            /*! @brief Return the default value.
             @returns The default value. */
            virtual Common::YarpString getDefaultValue(void)
            const = 0;

            /*! @brief Return the processed value.
             @returns The processed value. */
            virtual Common::YarpString getProcessedValue(void)
            const = 0;
            
            /*! @brief Return @c true if the argument is optional and @c false otherwise.
             @returns @c true if the argument is optional and @c false otherwise. */
            inline bool isOptional(void)
            const
            {
                return _isOptional;
            } // isOptional
            
            /*! @brief Set the associated variable to the default value. */
            virtual void setToDefault(void)
            const = 0;

            /*! @brief Convert to a printable representation.
             @returns A printable representation of the descriptor. */
            virtual Common::YarpString toString(void)
            const = 0;
            
            /*! @brief Check an input value against the constraints of the descriptor.
             @param value The value to be checked.
             @returns @c true if the value is within the domain of the descriptor and @c false
             otherwise. */
            virtual bool validate(const Common::YarpString & value)
            const = 0;
            
        protected :

            /*! @brief Partition a string that is in 'arguments' format into a sequence of strings.
             @param inString The string to be partitioned.
             @param indexOfDefaultValue The position in the input string where the default value
             will appear.
             @param result The partitioned string.
             @returns @c true if the correct number of fields appear within the input string and
             @c false otherwise. */
            static bool partitionString(const Common::YarpString & inString,
                                        const int                  indexOfDefaultValue,
                                        Common::YarpStringVector & result);

            /*! @brief Returns a string that contains a printable representation of the standard
             prefix fields for a command-line argument.
             @param tagForMandatoryField The tag value to use if the field is mandatory.
             @param tagForOptionalField The tag value to use if the field is optional.
             @returns A string that contains a printable representation of the standard prefix
             fields for a command-line argument. */
            Common::YarpString prefixFields(const Common::YarpString & tagForMandatoryField,
                                            const Common::YarpString & tagForOptionalField)
            const;

            /*! @brief Returns a string that contains a printable representation of the standard
             fields for a command-line argument.
             @returns A string that contains a printable representation of the standard fields for
             a command-line argument. */
            Common::YarpString suffixFields(void)
            const;
            
        private :
            
            COPY_AND_ASSIGNMENT_(BaseArgumentDescriptor);
            
        public :
        
        protected :
            
            /*! @brief The separator string to use when converting to a string. */
            static Common::YarpString _parameterSeparator;
        
        private :
            
            /*! @brief The description of the command-line argument for the adapter. */
            Common::YarpString _argDescription;

            /*! @brief The name of the command-line argument. */
            Common::YarpString _argName;
            
//            /*! @brief The default value for the command-line argument. */
//            Common::YarpString _defaultValue;
//            
            /*! @brief @c true if the argument is optional and @c false otherwise. */
            bool _isOptional;
            
# if defined(__APPLE__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-private-field"
# endif // defined(__APPLE__)
            /*! @brief Filler to pad to alignment boundary */
            char _filler[7];
# if defined(__APPLE__)
#  pragma clang diagnostic pop
# endif // defined(__APPLE__)
        
        }; // BaseArgumentDescriptor
        
        /*! @brief A sequence of argument descriptors. */
        typedef std::vector<BaseArgumentDescriptor *> DescriptorVector;
        
        /*! @brief Generate the standard 'argument list' description from an argument sequence.
         @param arguments The argument sequence.
         @returns A string containing the standard 'argument list' representation of the argument
         sequence. */
        Common::YarpString ArgumentsToArgString(const DescriptorVector & arguments);
        
        /*! @brief Generate the standard 'argument description' from an argument sequence.
         @param arguments The argument sequence.
         @param output The generated argument descriptions.
         @param minSpace The number of characters between the argument names and their
         descriptions. */
        void ArgumentsToDescriptionArray(const DescriptorVector &   arguments,
                                         Common::YarpStringVector & output,
                                         const size_t               minSpace);
        
        /*! @brief Return the resulting argument values.
         @param arguments The argument sequence.
         @param sep The separator string between the argument values.
         @returns The argument values, separated by 'sep'. */
        Common::YarpString CombineArguments(const DescriptorVector &   arguments,
                                            const Common::YarpString & sep);
        
        /*! @brief Update the arguments data from the parsed argument list.
         @param arguments The argument sequence.
         @param parseResult The parsed argument list.
         @returns @c true if the parsed argument list matches the argument sequence and @c false
         otherwise. */
        bool ProcessArguments(const DescriptorVector & arguments,
                              Option_::Parser &        parseResult);

    } // Utilities
    
} // MplusM

#endif // ! defined(MpMBaseArgumentDescriptor_H_)
