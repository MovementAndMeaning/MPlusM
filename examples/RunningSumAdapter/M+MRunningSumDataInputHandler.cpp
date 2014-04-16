//--------------------------------------------------------------------------------------
//
//  File:       M+MRunningSumDataInputHandler.cpp
//
//  Project:    M+M
//
//  Contains:   The class definition for the custom data channel input handler used by
//              the running sum adapter.
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
//  Created:    2014-03-24
//
//--------------------------------------------------------------------------------------

#include "M+MRunningSumDataInputHandler.h"
#include "M+MAdapterChannel.h"
#include "M+MRunningSumAdapterData.h"
#include "M+MRunningSumClient.h"
#include "M+MRunningSumRequests.h"

//#include "ODEnableLogging.h"
#include "ODLogging.h"

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief The class definition for the custom data channel input handler used by the
 running sum adapter. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using namespace MplusM::Example;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Class methods
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Constructors and destructors
#endif // defined(__APPLE__)

RunningSumDataInputHandler::RunningSumDataInputHandler(RunningSumAdapterData & shared) :
        inherited(), _shared(shared)
{
    OD_LOG_ENTER();//####
    OD_LOG_P1("shared = ", &shared);//####
    OD_LOG_EXIT_P(this);//####
} // RunningSumDataInputHandler::RunningSumDataInputHandler

RunningSumDataInputHandler::~RunningSumDataInputHandler(void)
{
    OD_LOG_OBJENTER();//####
    OD_LOG_OBJEXIT();//####
} // RunningSumDataInputHandler::~RunningSumDataInputHandler

#if defined(__APPLE__)
# pragma mark Actions
#endif // defined(__APPLE__)

bool RunningSumDataInputHandler::handleInput(const Common::Package &       input,
                                             const yarp::os::ConstString & senderChannel,
                                             yarp::os::ConnectionWriter *  replyMechanism)
{
#if (! defined(OD_ENABLE_LOGGING))
# pragma unused(senderChannel,replyMechanism)
#endif // ! defined(OD_ENABLE_LOGGING)
    OD_LOG_OBJENTER();//####
    OD_LOG_S2("senderChannel = ", senderChannel.c_str(), "got ", input.toString().c_str());//####
    OD_LOG_P1("replyMechanism = ", replyMechanism);//####
    bool result = true;
    
    try
    {
        int howMany = input.size();
        
        if (0 < howMany)
        {
            Common::AdapterChannel * theOutput = _shared.getOutput();
            RunningSumClient *       theClient = (RunningSumClient *) _shared.getClient();
            
            if (_shared.isActive() && theClient && theOutput)
            {
                if (1 == howMany)
                {
                    bool            gotValue = false;
                    double          inValue = 0.0;
                    yarp::os::Value argValue(input.get(0));
                    
                    if (argValue.isInt())
                    {
                        inValue = argValue.asInt();
                        gotValue = true;
                    }
                    else if (argValue.isDouble())
                    {
                        inValue = argValue.asDouble();
                        gotValue = true;
                    }
                    if (gotValue)
                    {
                        double outValue;

                        _shared.lock();
                        if (theClient->addToSum(inValue, outValue))
                        {
                            Common::Package message;
                            
                            message.addDouble(outValue);
                            if (! theOutput->write(message))
                            {
                                OD_LOG("(! theOutput->write(message))");//####
#if defined(MpM_STALL_ON_SEND_PROBLEM)
                                Common::Stall();
#endif // defined(MpM_STALL_ON_SEND_PROBLEM)
                            }
                        }
                        else
                        {
                            OD_LOG("! (theClient->startSum())");//####
                        }
                        _shared.unlock();
                    }
                }
                else
                {
                    bool                 gotValues = true;
                    Common::DoubleVector values;
                    
                    for (int ii = 0; gotValues && (ii < howMany); ++ii)
                    {
                        double          inValue;
                        yarp::os::Value aValue(input.get(ii));
                        
                        if (aValue.isInt())
                        {
                            inValue = aValue.asInt();
                        }
                        else if (aValue.isDouble())
                        {
                            inValue = aValue.asDouble();
                        }
                        else
                        {
                            gotValues = false;
                        }
                        if (gotValues)
                        {
                            values.push_back(inValue);
                        }
                    }
                    if (gotValues)
                    {
                        double outValue;
                        
                        _shared.lock();
                        if (theClient->addToSum(values, outValue))
                        {
                            Common::Package message;
                            
                            message.addDouble(outValue);
                            if (! theOutput->write(message))
                            {
                                OD_LOG("(! theOutput->write(message))");//####
#if defined(MpM_STALL_ON_SEND_PROBLEM)
                                Common::Stall();
#endif // defined(MpM_STALL_ON_SEND_PROBLEM)
                            }
                        }
                        else
                        {
                            OD_LOG("! (theClient->startSum())");//####
                        }
                        _shared.unlock();
                    }
                }                
            }
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught");//####
        throw;
    }
    OD_LOG_OBJEXIT_B(result);//####
    return result;
} // RunningSumDataInputHandler::handleInput

#if defined(__APPLE__)
# pragma mark Accessors
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)