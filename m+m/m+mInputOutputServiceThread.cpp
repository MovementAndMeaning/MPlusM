//--------------------------------------------------------------------------------------------------
//
//  File:       m+m/m+mInputOutputServiceThread.cpp
//
//  Project:    m+m
//
//  Contains:   The class definition for an input / output service thread for m+m.
//
//  Written by: Norman Jaffe
//
//  Copyright:  (c) 2016 by H Plus Technologies Ltd. and Simon Fraser University.
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
//  Created:    2016-05-09
//
//--------------------------------------------------------------------------------------------------

#include "m+mInputOutputServiceThread.hpp"

#include <m+m/m+mBaseInputOutputService.hpp>

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunknown-pragmas"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 @brief The class definition for an input / output service thread for m+m. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Namespace references
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;

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

InputOutputServiceThread::InputOutputServiceThread(BaseInputOutputService * service,
                                                   const YarpString &       helpText,
                                                   const bool               goWasSet,
                                                   const bool               stdinAvailable,
                                                   const bool               reportOnExit) :
    inherited(), _service(service), _helpText(helpText), _goWasSet(goWasSet),
    _reportOnExit(reportOnExit), _stdinAvailable(stdinAvailable)
{
    ODL_ENTER(); //####
    ODL_P1("service = ", service); //####
    ODL_S1s("helpText = ", helpText); //####
    ODL_B3("goWasSet = ", goWasSet, "stdinAvailable = ", stdinAvailable, "reportOnExit = ", //####
           reportOnExit); //####
    ODL_EXIT_P(this); //####
} // InputOutputServiceThread::InputOutputServiceThread

InputOutputServiceThread::~InputOutputServiceThread(void)
{
    ODL_OBJENTER(); //####
    delete _service;
    ODL_OBJEXIT(); //####
} // InputOutputServiceThread::~InputOutputServiceThread

#if defined(__APPLE__)
# pragma mark Actions and Accessors
#endif // defined(__APPLE__)

void
InputOutputServiceThread::run(void)
{
    ODL_OBJENTER(); //####
    if (_service)
    {
        _service->performLaunch(_helpText, _goWasSet, _stdinAvailable, _reportOnExit);
    }
    ODL_OBJEXIT(); //####
} // InputOutputServiceThread::run

bool
InputOutputServiceThread::threadInit(void)
{
    ODL_OBJENTER(); //####
    bool result = true;

    ODL_OBJEXIT_B(result); //####
    return result;
} // InputOutputServiceThread::threadInit

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)