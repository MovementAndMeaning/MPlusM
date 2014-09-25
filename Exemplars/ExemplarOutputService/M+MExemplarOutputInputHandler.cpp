//--------------------------------------------------------------------------------------------------
//
//  File:       M+MExemplarOutputInputHandler.cpp
//
//  Project:    M+M
//
//  Contains:   The class definition for the input channel input handler used by the exemplar output
//              service.
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
//                  list of conditions and the following disclaimer in the documentation and/or
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
//  Created:    2014-09-15
//
//--------------------------------------------------------------------------------------------------

#include "M+MExemplarOutputInputHandler.h"

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief The class definition for the input channel input handler used by the exemplar output
 service. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;
using namespace MplusM::Exemplar;

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

ExemplarOutputInputHandler::ExemplarOutputInputHandler(void) :
    inherited(), _outFile(nullptr)
{
    OD_LOG_ENTER(); //####
    OD_LOG_EXIT_P(this); //####
} // ExemplarOutputInputHandler::ExemplarOutputInputHandler

ExemplarOutputInputHandler::~ExemplarOutputInputHandler(void)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_OBJEXIT(); //####
} // ExemplarOutputInputHandler::~ExemplarOutputInputHandler

#if defined(__APPLE__)
# pragma mark Actions and Accessors
#endif // defined(__APPLE__)

bool ExemplarOutputInputHandler::handleInput(const yarp::os::Bottle &      input,
                                             const yarp::os::ConstString & senderChannel,
                                             yarp::os::ConnectionWriter *  replyMechanism)
{
#if (! defined(OD_ENABLE_LOGGING))
# if MAC_OR_LINUX_
#  pragma unused(senderChannel,replyMechanism)
# endif // MAC_OR_LINUX_
#endif // ! defined(OD_ENABLE_LOGGING)
    OD_LOG_OBJENTER(); //####
    OD_LOG_S2s("senderChannel = ", senderChannel, "got ", input.toString()); //####
    OD_LOG_P1("replyMechanism = ", replyMechanism); //####
    bool result = true;
    
    try
    {
        if (_outFile)
        {
            OD_LOG("(_outFile)"); //####
            bool sawValue = false;
            
            for (int ii = 0, mm = input.size(); mm > ii; ++ii)
            {
                yarp::os::Value aValue(input.get(ii));
                
                if (aValue.isInt())
                {
                    if (sawValue)
                    {
                        fputc(' ', _outFile);
                    }
                    fprintf(_outFile, "%d", aValue.asInt());
                    sawValue = true;
                }
            }
            if (sawValue)
            {
                fputc('\n', _outFile);
                fflush(_outFile);
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
} // ExemplarOutputInputHandler::handleInput

void ExemplarOutputInputHandler::setFile(FILE * outFile)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_P1("outFile = ", outFile); //####
    _outFile = outFile;
    OD_LOG_OBJEXIT(); //####
} // ExemplarOutputInputHandler::setFile

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)