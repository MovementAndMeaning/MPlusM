//--------------------------------------------------------------------------------------------------
//
//  File:       M+MStopServiceMain.cpp
//
//  Project:    M+M
//
//  Contains:   A utility application to stop a service.
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
//  Created:    2015-04-27
//
//--------------------------------------------------------------------------------------------------

#include <mpm/M+MUtilities.h>
#include <mpm/M+MRequests.h>

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunknown-pragmas"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 @brief A utility application to stop a service. */

/*! @dir StopRegistry
 @brief The set of files that implement the Stop Service application. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Namespace references
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;
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

/*! @brief Set up the environment and perform the operation.
 @param arguments The arguments to analyze. */
static void setUpAndGo(const StringVector & arguments)
{
    OD_LOG_ENTER(); //####
    OD_LOG_P1("arguments = ", &arguments); //####
    yarp::os::ConstString channelName;
    
    if (0 < arguments.size())
    {
        channelName = arguments[0];
    }
    if (0 < channelName.size())
    {
        Utilities::StopAService(channelName, STANDARD_WAIT_TIME);
    }
    OD_LOG_EXIT(); //####
} // setUpAndGo

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)

/*! @brief The entry point for stopping a service.
 
 The first argument is the name of the primary channel for the service.
 Nothing is written to standard output.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the application.
 @returns @c 0 on a successful test and @c 1 on failure. */
int main(int      argc,
         char * * argv)
{
    OD_LOG_INIT(*argv, kODLoggingOptionIncludeProcessID | kODLoggingOptionIncludeThreadID | //####
                kODLoggingOptionEnableThreadSupport | kODLoggingOptionWriteToStderr); //####
    OD_LOG_ENTER(); //####
#if MAC_OR_LINUX_
    SetUpLogger(*argv);
#endif // MAC_OR_LINUX_
    OutputFlavour flavour;
    StringVector  arguments;
    
    if (Utilities::ProcessStandardUtilitiesOptions(argc, argv, " channel\n\n"
                                                   "  channel    Channel name for service", 2015,
                                                   STANDARD_COPYRIGHT_NAME, flavour, &arguments))
    
    {
        try
        {
			Utilities::SetUpGlobalStatusReporter();
			Utilities::CheckForNameServerReporter();
            if (Utilities::CheckForValidNetwork())
            {
                yarp::os::Network yarp; // This is necessary to establish any connections to the
                                        // YARP infrastructure
                
                Initialize(*argv);
                if (Utilities::CheckForRegistryService())
                {
                    setUpAndGo(arguments);
                }
                else
                {
                    OD_LOG("! (Utilities::CheckForRegistryService())"); //####
#if MAC_OR_LINUX_
                    GetLogger().fail("Registry Service not running.");
#else // ! MAC_OR_LINUX_
                    cerr << "Registry Service not running." << endl;
#endif // ! MAC_OR_LINUX_
                }
            }
            else
            {
                OD_LOG("! (Utilities::CheckForValidNetwork())"); //####
#if MAC_OR_LINUX_
                GetLogger().fail("YARP network not running.");
#else // ! MAC_OR_LINUX_
                cerr << "YARP network not running." << endl;
#endif // ! MAC_OR_LINUX_
            }
			Utilities::ShutDownGlobalStatusReporter();
		}
        catch (...)
        {
            OD_LOG("Exception caught"); //####
        }
        yarp::os::Network::fini();
    }
    OD_LOG_EXIT_L(0); //####
    return 0;
} // main
