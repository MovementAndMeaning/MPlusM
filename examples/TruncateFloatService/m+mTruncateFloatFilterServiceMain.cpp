//--------------------------------------------------------------------------------------------------
//
//  File:       m+mTruncateFloatFilterServiceMain.cpp
//
//  Project:    m+m
//
//  Contains:   The main application for the Truncate Float filter service.
//
//  Written by: Norman Jaffe
//
//  Copyright:  (c) 2014 by H Plus Technologies Ltd. and Simon Fraser University.
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

#include "m+mTruncateFloatFilterService.h"

#include <m+m/m+mEndpoint.h>
#include <m+m/m+mUtilities.h>

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunknown-pragmas"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 @brief The main application for the Truncate Float filter service. */

/*! @dir TruncateFloatService
 @brief The set of files that implement the Truncate Float filter service. */
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

/*! @brief Set up the environment and start the Truncate Float service.
 @param argumentList Descriptions of the arguments to the executable.
 @param progName The path to the executable.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the Truncate Float service.
 @param tag The modifier for the service name and port names.
 @param serviceEndpointName The YARP name to be assigned to the new service.
 @param servicePortNumber The port being used by the service.
 @param goWasSet @c true if the service is to be started immediately.
 @param stdinAvailable @c true if running in the foreground and @c false otherwise.
 @param reportOnExit @c true if service metrics are to be reported on exit and @c false otherwise.
 */
static void setUpAndGo(const Utilities::DescriptorVector & argumentList,
                       const YarpString &                  progName,
                       const int                           argc,
                       char * *                            argv,
                       const YarpString &                  tag,
                       const YarpString &                  serviceEndpointName,
                       const YarpString &                  servicePortNumber,
                       const bool                          goWasSet,
                       const bool                          stdinAvailable,
                       const bool                          reportOnExit)
{
    OD_LOG_ENTER(); //####
    OD_LOG_P2("argumentList = ", &argumentList, "argv = ", argv); //####
    OD_LOG_S4s("progName = ", progName, "tag = ", tag, "serviceEndpointName = ", //####
               serviceEndpointName, "servicePortNumber = ", servicePortNumber); //####
    OD_LOG_LL1("argc = ", argc); //####
    OD_LOG_B3("goWasSet = ", goWasSet, "stdinAvailable = ", stdinAvailable, //####
              "reportOnExit = ", reportOnExit); //####
    TruncateFloatFilterService * aService = new TruncateFloatFilterService(progName, argc, argv,
                                                                           tag, serviceEndpointName,
                                                                           servicePortNumber);
    
    if (aService)
    {
        aService->performLaunch(argumentList, "", goWasSet, stdinAvailable, reportOnExit);
        delete aService;
    }
    else
    {
        OD_LOG("! (aService)"); //####
    }
    OD_LOG_EXIT(); //####
} // setUpAndGo

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)

/*! @brief The entry point for running the Truncate Float filter service.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the Truncate Float filter service.
 @returns @c 0 on a successful test and @c 1 on failure. */
int main(int      argc,
         char * * argv)
{
    YarpString progName(*argv);

#if defined(MpM_ServicesLogToStandardError)
    OD_LOG_INIT(progName.c_str(), kODLoggingOptionIncludeProcessID | //####
                kODLoggingOptionIncludeThreadID | kODLoggingOptionWriteToStderr | //####
                kODLoggingOptionEnableThreadSupport); //####
#else // ! defined(MpM_ServicesLogToStandardError)
    OD_LOG_INIT(progName.c_str(), kODLoggingOptionIncludeProcessID | //####
                kODLoggingOptionIncludeThreadID | kODLoggingOptionEnableThreadSupport); //####
#endif // ! defined(MpM_ServicesLogToStandardError)
    OD_LOG_ENTER(); //####
#if MAC_OR_LINUX_
    SetUpLogger(progName);
#endif // MAC_OR_LINUX_
    try
    {
        bool                        goWasSet = false;
        bool                        nameWasSet; // not used
        bool                        reportOnExit = false;
        bool                        stdinAvailable = CanReadFromStandardInput();
        YarpString                  serviceEndpointName;
        YarpString                  servicePortNumber;
        YarpString                  tag;
        Utilities::DescriptorVector argumentList;

        if (ProcessStandardServiceOptions(argc, argv, argumentList,
                                          DEFAULT_TRUNCATEFLOATFILTER_SERVICE_NAME_,
                                          TRUNCATEFLOATFILTER_SERVICE_DESCRIPTION_, "", 2014,
                                          STANDARD_COPYRIGHT_NAME_, goWasSet, nameWasSet,
                                          reportOnExit, tag, serviceEndpointName,
                                          servicePortNumber))
        {
			Utilities::SetUpGlobalStatusReporter();
			Utilities::CheckForNameServerReporter();
            if (Utilities::CheckForValidNetwork())
            {
                yarp::os::Network yarp; // This is necessary to establish any connections to the
                                        // YARP infrastructure
                
                Initialize(progName);
                if (Utilities::CheckForRegistryService())
                {
                    setUpAndGo(argumentList, progName, argc, argv, tag, serviceEndpointName,
                               servicePortNumber, goWasSet, stdinAvailable, reportOnExit);
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
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
    }
    yarp::os::Network::fini();
    OD_LOG_EXIT_L(0); //####
    return 0;
} // main
