//--------------------------------------------------------------------------------------
//
//  File:       YPPMatchFieldWithValues.cpp
//
//  Project:    YarpPlusPlus
//
//  Contains:   The class definition for a pattern matcher for field/value pairs.
//
//  Written by: Norman Jaffe
//
//  Copyright:  (c) 2014 by HPlus Technologies Ltd. and Simon Fraser University.
//
//              All rights reserved. Redistribution and use in source and binary forms,
//              with or without modification, are permitted provided that the following
//              conditions are met:
//                * Redistributions of source code must retain the above copyright
//                  notice, this list of conditions and the following disclaimer.
//                * Redistributions in binary form must reproduce the above copyright
//                  notice, this list of conditions and the following disclaimer in the
//                  documentation and/or other materials provided with the
//                  distribution.
//                * Neither the name of the copyright holders nor the names of its
//                  contributors may be used to endorse or promote products derived
//                  from this software without specific prior written permission.
//
//              THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//              "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//              LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//              PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//              OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//              SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//              LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//              DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//              THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//              (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//              OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Created:    2014-03-07
//
//--------------------------------------------------------------------------------------

#include "YPPMatchFieldWithValues.h"
//#define ENABLE_OD_SYSLOG /* */
#include "ODSyslog.h"
#include "YPPMatchFieldName.h"
#include "YPPMatchValue.h"
#include "YPPMatchValueList.h"

using namespace YarpPlusPlusParser;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Class methods
#endif // defined(__APPLE__)

MatchFieldWithValues * MatchFieldWithValues::CreateMatcher(const yarp::os::ConstString & inString,
                                                           const int                     inLength,
                                                           const int                     startPos,
                                                           int &                         endPos,
                                                           FieldNameValidator            validator)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_S1("inString = ", inString.c_str());//####
    OD_SYSLOG_LL2("inLength = ", inLength, "startPos = ", startPos);//####
    int                    workPos = startPos;
    MatchFieldWithValues * result = NULL;
    MatchFieldName *       fieldName = MatchFieldName::CreateMatcher(inString, inLength, startPos, workPos, validator);

    if (fieldName)
    {
        workPos = SkipWhitespace(inString, inLength, workPos);
        if (workPos < inLength)
        {
            int nextPos;
            
            if (MatchValueList::ListInitiatorCharacter() == inString[workPos])
            {
                MatchValueList * asList = MatchValueList::CreateMatcher(inString, inLength, workPos, nextPos);
                
                if (asList)
                {
                    result = new MatchFieldWithValues(validator, fieldName, asList);
                }
            }
            else
            {
                MatchValue * asSingle = MatchValue::CreateMatcher(inString, inLength, workPos, nextPos);
                
                if (asSingle)
                {
                    result = new MatchFieldWithValues(validator, fieldName, asSingle);                    
                }
            }
            if (result)
            {
                endPos = nextPos;
            }
            else
            {
                delete fieldName;
            }
        }
        else
        {
            delete fieldName;
        }
    }
    OD_SYSLOG_EXIT_P(result);//####
    return result;
} // MatchFieldWithValues::CreateMatcher

#if defined(__APPLE__)
# pragma mark Constructors and destructors
#endif // defined(__APPLE__)

MatchFieldWithValues::MatchFieldWithValues(FieldNameValidator validator,
                                           MatchFieldName *   fieldName,
                                           MatchValue *       asSingle) :
        inherited(), _validator(validator), _fieldName(fieldName), _singleValue(asSingle), _values(NULL)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("fieldName = ", fieldName, "asSingle = ", asSingle);//####
    OD_SYSLOG_EXIT_P(this);//####
} // MatchFieldWithValues::MatchFieldWithValues

MatchFieldWithValues::MatchFieldWithValues(FieldNameValidator validator,
                                           MatchFieldName *   fieldName,
                                           MatchValueList *   asList) :
inherited(), _validator(validator), _fieldName(fieldName), _singleValue(NULL), _values(asList)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("fieldName = ", fieldName, "asList = ", asList);//####
    OD_SYSLOG_EXIT_P(this);//####
} // MatchFieldWithValues::MatchFieldWithValues

MatchFieldWithValues::~MatchFieldWithValues(void)
{
    OD_SYSLOG_ENTER();//####
    delete _fieldName;
    delete _singleValue;
    delete _values;
    OD_SYSLOG_EXIT();//####
} // MatchFieldWithValues::~MatchFieldWithValues

#if defined(__APPLE__)
# pragma mark Actions
#endif // defined(__APPLE__)

yarp::os::ConstString MatchFieldWithValues::asSQLString(void)
const
{
    OD_SYSLOG_ENTER();//####
    yarp::os::ConstString field(_fieldName->asString());
    const char *          prefixString = NULL;
    const char *          suffixString = NULL;
    const char *          trueName;
    yarp::os::ConstString converted;

    if (_validator)
    {
        trueName = _validator(field.c_str(), &prefixString, &suffixString);
    }
    else
    {
        trueName = field.c_str();
    }
    OD_SYSLOG_S1("trueName <- ", trueName);//####
    if (_singleValue)
    {
        if (prefixString)
        {
            converted += prefixString;
        }
        converted += trueName;
        if (_singleValue->hasWildcardCharacters())
        {
            converted += " LIKE ";
        }
        else
        {
            converted += " = ";
        }
        converted += _singleValue->asSQLString();
        if (suffixString)
        {
            converted += suffixString;
        }
    }
    else if (_values)
    {
        if (prefixString)
        {
            converted += prefixString;
        }
        converted += _values->asSQLString(trueName);
        if (suffixString)
        {
            converted += suffixString;
        }
    }
    OD_SYSLOG_EXIT_S(converted.c_str());//####
    return converted;
} // MatchFieldWithValues::asSQLString

yarp::os::ConstString MatchFieldWithValues::asString(void)
const
{
    yarp::os::ConstString result;
    
    result += _fieldName->asString();
    result += ": ";
    if (_singleValue)
    {
        result += _singleValue->asString();
    }
    else if (_values)
    {
        result += _values->asString();
    }
    return result;
} // MatchFieldWithValues::asString

#if defined(__APPLE__)
# pragma mark Accessors
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)