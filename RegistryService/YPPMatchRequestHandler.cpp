//--------------------------------------------------------------------------------------
//
//  File:       YPPMatchRequestHandler.cpp
//
//  Project:    YarpPlusPlus
//
//  Contains:   The class definition for the request handler for the standard 'match'
//              request.
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
//  Created:    2014-03-10
//
//--------------------------------------------------------------------------------------

#include "YPPMatchRequestHandler.h"
//#define ENABLE_OD_SYSLOG /* */
#include "ODSyslog.h"
#include "YPPEndpoint.h"
#include "YPPRegistryService.h"
#include "YPPRequests.h"
#include <cstdlib>

using namespace YarpPlusPlus;
using namespace YarpPlusPlusParser;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

/*! @brief The protocol version number for the 'list' request. */
#define MATCH_REQUEST_VERSION_NUMBER "1.0"

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Class methods
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Constructors and destructors
#endif // defined(__APPLE__)

MatchRequestHandler::MatchRequestHandler(RegistryService &  service,
                                         FieldNameValidator validator) :
        inherited(YPP_MATCH_REQUEST), _service(service), _validator(validator)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P1("service = ", &service);//####
    OD_SYSLOG_EXIT_P(this);//####
} // MatchRequestHandler::MatchRequestHandler

MatchRequestHandler::~MatchRequestHandler(void)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_EXIT();//####
} // MatchRequestHandler::~MatchRequestHandler

#if defined(__APPLE__)
# pragma mark Actions
#endif // defined(__APPLE__)

void MatchRequestHandler::fillInDescription(yarp::os::Property & info)
{
    OD_SYSLOG_ENTER();//####
    info.put(YPP_REQREP_DICT_REQUEST_KEY, YPP_MATCH_REQUEST);
    info.put(YPP_REQREP_DICT_INPUT_KEY, YPP_REQREP_STRING YPP_REQREP_1_OR_MORE);
    info.put(YPP_REQREP_DICT_OUTPUT_KEY, YPP_REQREP_LIST_START YPP_REQREP_STRING YPP_REQREP_0_OR_MORE
             YPP_REQREP_LIST_END);
    info.put(YPP_REQREP_DICT_VERSION_KEY, MATCH_REQUEST_VERSION_NUMBER);
    info.put(YPP_REQREP_DICT_DESCRIPTION_KEY, "Find a matching service");
    yarp::os::Value    keywords;
    yarp::os::Bottle * asList = keywords.asList();
    
    asList->addString(YPP_MATCH_REQUEST);
    asList->addString("find");
    info.put(YPP_REQREP_DICT_KEYWORDS_KEY, keywords);
    OD_SYSLOG_EXIT();//####
} // MatchRequestHandler::fillInDescription

bool MatchRequestHandler::operator() (const yarp::os::Bottle &     restOfInput,
                                      yarp::os::ConnectionWriter * replyMechanism)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_S1("restOfInput = ", restOfInput.toString().c_str());//####
    OD_SYSLOG_P1("replyMechanism = ", replyMechanism);//####
    bool result = true;
    
    if (replyMechanism)
    {
        yarp::os::Bottle reply;
        
        // We are expecting just one string as the parameter
        if (1 == restOfInput.size())
        {
            yarp::os::Value argument(restOfInput.get(0));
            
            if (argument.isString())
            {
                yarp::os::ConstString argAsString(argument.toString());
            
                OD_SYSLOG_S1("argAsString <- ", argAsString.c_str());//####
                int               endPos;
                MatchExpression * matcher = MatchExpression::CreateMatcher(argAsString, argAsString.length(), 0, endPos,
                                                                           _validator);
                
                if (matcher)
                {
                    // Hand off the processing to the registry service. First, put the 'OK' response in the output
                    // buffer, as we have successfully parsed the request.
                    reply.addString(YPP_OK_RESPONSE);
                    if (! _service.processMatchRequest(matcher, reply))
                    {
                        reply = yarp::os::Bottle::getNullBottle();
                        reply.addString(YPP_FAILED_RESPONSE);
                        reply.addString("Invalid criteria");
                    }
                    delete matcher;
                }
                else
                {
                    reply.addString(YPP_FAILED_RESPONSE);
                    reply.addString("Invalid criteria");
                }
            }
            else
            {
                reply.addString(YPP_FAILED_RESPONSE);
                reply.addString("Invalid criteria");
            }
        }
        else
        {
            reply.addString(YPP_FAILED_RESPONSE);
            reply.addString("Missing criteria or extra arguments to request");
        }
        OD_SYSLOG_S1("reply <- ", reply.toString().c_str());
        reply.write(*replyMechanism);
    }
    OD_SYSLOG_EXIT_B(result);//####
    return result;
} // MatchRequestHandler::operator()

#if defined(__APPLE__)
# pragma mark Accessors
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)