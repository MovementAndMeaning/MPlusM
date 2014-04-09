//--------------------------------------------------------------------------------------
//
//  File:       MoMeRandomNumberDataInputHandler.cpp
//
//  Project:    MoAndMe
//
//  Contains:   The class definition for the custom data channel input handler used by
//              the random number adapter.
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
//  Created:    2014-03-27
//
//--------------------------------------------------------------------------------------

#include "MoMeRandomNumberDataInputHandler.h"
#include "MoMeAdapterChannel.h"
#include "MoMeRandomNumberAdapterData.h"
#include "MoMeRandomNumberClient.h"

#include "ODEnableLogging.h"
#include "ODLogging.h"

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief The class definition for the custom data channel input handler used by the
 random number adapter. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using namespace MoAndMe::Example;

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

RandomNumberDataInputHandler::RandomNumberDataInputHandler(RandomNumberAdapterData & shared) :
        inherited(), _shared(shared)
{
    OD_LOG_ENTER();//####
    OD_LOG_P1("shared = ", &shared);//####
    OD_LOG_EXIT_P(this);//####
} // RandomNumberDataInputHandler::RandomNumberDataInputHandler

RandomNumberDataInputHandler::~RandomNumberDataInputHandler(void)
{
    OD_LOG_OBJENTER();//####
    OD_LOG_OBJEXIT();//####
} // RandomNumberDataInputHandler::~RandomNumberDataInputHandler

#if defined(__APPLE__)
# pragma mark Actions
#endif // defined(__APPLE__)

bool RandomNumberDataInputHandler::handleInput(const Common::Package &       input,
                                               const yarp::os::ConstString & senderChannel,
                                               yarp::os::ConnectionWriter *  replyMechanism)
{
#if (! defined(OD_ENABLE_LOGGING))
# pragma unused(senderChannel,replyMechanism)
#endif // ! defined(OD_ENABLE_LOGGING)
    OD_LOG_OBJENTER();//####
    OD_LOG_S2("senderChannel = ", senderChannel.c_str(), "got ", input.toString().c_str());//####
    OD_LOG_P1("replyMechanism = ", replyMechanism);//####
    bool result = false;
    
    try
    {
        if (0 < input.size())
        {
            Common::AdapterChannel * theOutput = _shared.getOutput();
            RandomNumberClient *     theClient = (RandomNumberClient *) _shared.getClient();
            
            if (theClient && theOutput)
            {
                int             count;
                yarp::os::Value argValue(input.get(0));
                
                if (argValue.isInt())
                {
                    count = argValue.asInt();
                }
                else if (argValue.isDouble())
                {
                    count = static_cast<int>(argValue.asDouble());
                }
                else
                {
                    count = 1;
                }
                if (0 >= count)
                {
                    count = 1;
                }
                if (1 < count)
                {
                    Common::DoubleVector randResult;
                    
                    if (theClient->getRandomNumbers(count, randResult))
                    {
                        Common::Package message;
                        
                        for (Common::DoubleVector::const_iterator it(randResult.cbegin()); randResult.cend() != it;
                             ++it)
                        {
                            message.addDouble(*it);
                        }
                        _shared.lock();
                        OD_LOG_LL1("theOutput->getOutputCount = ", theOutput->getOutputCount());//####
                        if (theOutput->write(message))
                        {
                            result = true;
                        }
                        else
                        {
                            OD_LOG("! (theOutput->write(message))");//####
                        }
                        _shared.unlock();
                    }
                    else
                    {
                        OD_LOG("! (theClient->getRandomNumbers(count, randResult))");//####
                    }
                }
                else
                {
                    double randResult;
                    
                    if (theClient->getOneRandomNumber(randResult))
                    {
                        Common::Package message;
                        
                        message.addDouble(randResult);
                        _shared.lock();
                        OD_LOG_LL1("theOutput->getOutputCount = ", theOutput->getOutputCount());//####
                        if (theOutput->write(message))
                        {
                            result = true;
                        }
                        else
                        {
                            OD_LOG("! (theOutput->write(message))");//####
                        }
                        _shared.unlock();
                    }
                    else
                    {
                        OD_LOG("! (theClient->getOneRandomNumber(randResult))");//####
                    }
                }
            }
        }
        else
        {
            result = true;
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught");//####
        throw;
    }
    OD_LOG_OBJEXIT_B(result);//####
    return result;
} // RandomNumberDataInputHandler::handleInput

#if defined(__APPLE__)
# pragma mark Accessors
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)
