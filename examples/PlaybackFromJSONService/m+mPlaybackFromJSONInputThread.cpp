//--------------------------------------------------------------------------------------------------
//
//  File:       m+mPlaybackFromJSONInputThread.cpp
//
//  Project:    m+m
//
//  Contains:   The class definition for an output-generating thread for m+m.
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
//  Created:    2015-08-24
//
//--------------------------------------------------------------------------------------------------

#include "m+mPlaybackFromJSONInputThread.h"

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunknown-pragmas"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 @brief The class definition for an output-generating thread for m+m. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Namespace references
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;
using namespace MplusM::Example;
using std::cerr;
using std::endl;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Global constants and variables
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

PlaybackFromJSONInputThread::PlaybackFromJSONInputThread(Common::GeneralChannel * outChannel,
                                                         yarp::os::Bottle &       outMessage,
                                                         const double             playbackRatio,
                                                         const double             initialDelay,
                                                         const bool               loopPlayback) :
    inherited(), _outChannel(outChannel), _outMessage(outMessage), _nextIndex(0),
    _initialDelay(initialDelay), _playbackRatio(playbackRatio), _loopPlayback(loopPlayback)
{
    OD_LOG_ENTER(); //####
    OD_LOG_P2("outChannel = ", outChannel, "outMessage = ", &outMessage); //####
    OD_LOG_D2("playbackRatio = ", playbackRatio, "initialDelay = ", initialDelay); //####
    OD_LOG_B1("loopPlayback = ", loopPlayback);
    OD_LOG_EXIT_P(this); //####
} // PlaybackFromJSONInputThread::PlaybackFromJSONInputThread

PlaybackFromJSONInputThread::~PlaybackFromJSONInputThread(void)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_OBJEXIT(); //####
} // PlaybackFromJSONInputThread::~PlaybackFromJSONInputThread

#if defined(__APPLE__)
# pragma mark Actions and Accessors
#endif // defined(__APPLE__)

void PlaybackFromJSONInputThread::clearOutputChannel(void)
{
    OD_LOG_OBJENTER(); //####
    _outChannel = NULL;
    OD_LOG_OBJEXIT(); //####
} // PlaybackFromJSONInputThread::clearOutputChannel

void PlaybackFromJSONInputThread::run(void)
{
    OD_LOG_OBJENTER(); //####
    bool atEnd = false;
    
    for ( ; (! atEnd) && (! isStopping()); )
    {
        if (_nextTime <= yarp::os::Time::now())
        {
            OD_LOG("(_nextTime <= yarp::os::Time::now())"); //####
            if (_nextIndex < _outMessage.size())
            {
                yarp::os::Value & nextValue = _outMessage.get(_nextIndex++);
                
                if (nextValue.isList())
                {
                    yarp::os::Bottle * toWrite = nextValue.asList();
                    
                    if (toWrite && _outChannel)
                    {
                        if (! _outChannel->write(*toWrite))
                        {
                            OD_LOG("(! _outChannel->write(*toWrite))"); //####
#if defined(MpM_StallOnSendProblem)
                            Stall();
#endif // defined(MpM_StallOnSendProblem)
                        }
                    }
                }
                if (_nextIndex < _outMessage.size())
                {
                    double            deltaTime;
                    yarp::os::Value & timeValue = _outMessage.get(_nextIndex++);
                    
                    if (timeValue.isInt())
                    {
                        deltaTime = timeValue.asInt();
                    }
                    else if (timeValue.isDouble())
                    {
                        deltaTime = timeValue.asDouble();
                    }
                    else
                    {
                        deltaTime = 1;
                    }
                    _nextTime = yarp::os::Time::now() + (deltaTime * _playbackRatio);
                }
                else if (_loopPlayback)
                {
                    _nextTime = yarp::os::Time::now() + _initialDelay;
                    _nextIndex = 0;
                }
            }
            else if (_loopPlayback)
            {
                _nextTime = yarp::os::Time::now() + _initialDelay;
                _nextIndex = 0;
            }
            else
            {
                cerr << "All data sent." << endl;
                atEnd = true;
            }
        }
        yarp::os::Time::yield();
    }
    // If we get here, the data was only sent once, without loopback...
    for ( ; (! isStopping()); )
    {
        yarp::os::Time::yield();
    }
    OD_LOG_OBJEXIT(); //####
} // PlaybackFromJSONInputThread::run

bool PlaybackFromJSONInputThread::threadInit(void)
{
    OD_LOG_OBJENTER(); //####
    bool result = true;
    
    _nextTime = yarp::os::Time::now() + _initialDelay;
    OD_LOG_OBJEXIT_B(result); //####
    return result;
} // PlaybackFromJSONInputThread::threadInit

void PlaybackFromJSONInputThread::threadRelease(void)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_OBJEXIT(); //####
} // PlaybackFromJSONInputThread::threadRelease

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)