//--------------------------------------------------------------------------------------------------
//
//  File:       M+MRandomBurstService.cpp
//
//  Project:    M+M
//
//  Contains:   The class definition for the random burst input service.
//
//  Written by: Norman Jaffe
//
//  Copyright:  (c) 2014 by HPlus Technologies Ltd. and Simon Fraser University.
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
//  Created:    2014-06-24
//
//--------------------------------------------------------------------------------------------------

#include "M+MRandomBurstService.h"
#include "M+MRandomBurstRequests.h"
#include "M+MRandomBurstThread.h"

#include <mpm/M+MEndpoint.h>

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 @brief The class definition for the random burst input service. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;
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
# pragma mark Constructors and Destructors
#endif // defined(__APPLE__)

RandomBurstService::RandomBurstService(const yarp::os::ConstString & launchPath,
                                       const yarp::os::ConstString & tag,
                                       const yarp::os::ConstString & serviceEndpointName,
                                       const yarp::os::ConstString & servicePortNumber) :
    inherited(launchPath, tag, true, MpM_RANDOMBURST_CANONICAL_NAME,
              "The random burst input service", "", serviceEndpointName, servicePortNumber),
    _generator(NULL), _burstPeriod(1), _burstSize(1)
{
    OD_LOG_ENTER(); //####
    OD_LOG_S4s("launchPath = ", launchPath, "tag = ", tag, "serviceEndpointName = ", //####
               serviceEndpointName, "servicePortNumber = ", servicePortNumber); //####
    OD_LOG_EXIT_P(this); //####
} // RandomBurstService::RandomBurstService

RandomBurstService::~RandomBurstService(void)
{
    OD_LOG_OBJENTER(); //####
    stopStreams();
    OD_LOG_OBJEXIT(); //####
} // RandomBurstService::~RandomBurstService

#if defined(__APPLE__)
# pragma mark Actions and Accessors
#endif // defined(__APPLE__)

bool RandomBurstService::configure(const yarp::os::Bottle & details)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_P1("details = ", &details); //####
    bool result = false;
    
    try
    {
        if (! isActive())
        {
            if (2 == details.size())
            {
                yarp::os::Value firstValue(details.get(0));
                yarp::os::Value secondValue(details.get(1));
                
                if (firstValue.isDouble() && secondValue.isInt())
                {
                    double firstNumber = firstValue.asDouble();
                    int    secondNumber = secondValue.asInt();
                    
                    if ((0 < firstNumber) && (0 < secondNumber))
                    {
                        _burstPeriod = firstNumber;
                        _burstSize = secondNumber;
                        result = true;
                    }
                }
            }
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(result); //####
    return result;
} // RandomBurstService::configure

void RandomBurstService::restartStreams(void)
{
    OD_LOG_OBJENTER(); //####
    try
    {
        // No special processing needed.
        stopStreams();
        startStreams();
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT(); //####
} // RandomBurstService::restartStreams

bool RandomBurstService::setUpStreamDescriptions(void)
{
    OD_LOG_OBJENTER(); //####
    bool                  result = true;
    ChannelDescription    description;
    yarp::os::ConstString rootName(getEndpoint().getName() + "/");
    
    _outDescriptions.clear();
    description._portName = rootName + "output";
    description._portProtocol = "d+";
    description._protocolDescription = "One or more numeric values";
    _outDescriptions.push_back(description);
    OD_LOG_OBJEXIT_B(result); //####
    return result;
} // RandomBurstService::setUpStreamDescriptions

bool RandomBurstService::shutDownOutputStreams(void)
{
    OD_LOG_OBJENTER(); //####
    bool result = inherited::shutDownOutputStreams();
    
    if (_generator)
    {
        _generator->clearOutputChannel();
    }
    OD_LOG_EXIT_B(result); //####
    return result;
} // RandomBurstService::shutDownOutputStreams

bool RandomBurstService::start(void)
{
    OD_LOG_OBJENTER(); //####
    try
    {
        if (! isStarted())
        {
            inherited::start();
            if (isStarted())
            {
            
            }
            else
            {
                OD_LOG("! (isStarted())"); //####
            }
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(isStarted()); //####
    return isStarted();
} // RandomBurstService::start

void RandomBurstService::startStreams(void)
{
    OD_LOG_OBJENTER(); //####
    try
    {
        if (! isActive())
        {
            _generator = new RandomBurstThread(_outStreams.at(0), _burstPeriod, _burstSize);
            _generator->start();
            setActive();
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT(); //####
} // RandomBurstService::startStreams

bool RandomBurstService::stop(void)
{
    OD_LOG_OBJENTER(); //####
    bool result;
    
    try
    {
        result = inherited::stop();
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(result); //####
    return result;
} // RandomBurstService::stop

void RandomBurstService::stopStreams(void)
{
    OD_LOG_OBJENTER(); //####
    try
    {
        if (isActive())
        {
            _generator->stop();
            for ( ; _generator->isRunning(); )
            {
                yarp::os::Time::delay(_burstSize / 3.9);
            }
            delete _generator;
            _generator = NULL;
            clearActive();
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT(); //####
} // RandomBurstService::stopStreams

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)
