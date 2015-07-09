//--------------------------------------------------------------------------------------------------
//
//  File:       UnrealOutputServiceMain.cpp
//
//  Project:    m+m
//
//  Contains:   The main application for the Unreal output service.
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
//  Created:    2014-11-18
//
//--------------------------------------------------------------------------------------------------

#include "m+mUnrealOutputService.h"

#include <m+m/m+mDoubleArgumentDescriptor.h>
#include <m+m/m+mEndpoint.h>
#include <m+m/m+mPortArgumentDescriptor.h>

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunknown-pragmas"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 @brief The main application for the %Unreal output service. */

/*! @dir Unreal
 @brief The set of files that implement the %Unreal output service. */

/*! @dir UnrealOutputService
 @brief The set of files that implement the %Unreal output service. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Namespace references
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;
using namespace MplusM::Unreal;
using std::cerr;
using std::cin;
using std::cout;
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

/*! @brief Display the available commands. */
static void displayCommands(void)
{
    OD_LOG_ENTER(); //####
    cout << "Commands:" << endl;
    cout << "  ? - display this list" << endl;
    cout << "  b - start (begin) the output stream, sending bursts of data" << endl;
    cout << "  c - configure the service by providing the port number and translation scale" <<
            endl;
    cout << "  e - stop (end) the output stream" << endl;
    cout << "  q - quit the application" << endl;
    cout << "  r - restart the output stream" << endl;
    cout << "  u - reset the configuration (unconfigure) so that it will be reprocessed" << endl;
    OD_LOG_EXIT(); //####
} // displayCommands

/*! @brief Set up the environment and start the %Unreal output service.
 @param outPort The port to use to connect.
 @param translationScale The translation scale.
 @param argumentList Descriptions of the arguments to the executable.
 @param progName The path to the executable.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the %Unreal output service.
 @param tag The modifier for the service name and port names.
 @param serviceEndpointName The YARP name to be assigned to the new service.
 @param servicePortNumber The port being used by the service.
 @param goWasSet @c true if the service is to be started immediately.
 @param stdinAvailable @c true if running in the foreground and @c false otherwise.
 @param reportOnExit @c true if service metrics are to be reported on exit and @c false otherwise.
 */
static void setUpAndGo(int &                               outPort,
                       double &                            translationScale,
                       const Utilities::DescriptorVector & argumentList,
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
    OD_LOG_LL2("outPort = ", outPort, "argc = ", argc); //####
    OD_LOG_D1("translationScale = ", translationScale); //####
    OD_LOG_S4s("progName = ", progName, "tag = ", tag, "serviceEndpointName = ", //####
               serviceEndpointName, "servicePortNumber = ", servicePortNumber); //####
    OD_LOG_P2("argumentList = ", &argumentList, "argv = ", argv); //####
    OD_LOG_LL1("argc = ", argc); //####
    OD_LOG_B3("goWasSet = ", goWasSet, "stdinAvailable = ", stdinAvailable, //####
              "reportOnExit = ", reportOnExit); //####
    UnrealOutputService * aService = new UnrealOutputService(progName, argc, argv, tag,
                                                             serviceEndpointName,
                                                             servicePortNumber);
    
    if (aService)
    {
        if (aService->start())
        {
            YarpString channelName(aService->getEndpoint().getName());
            
            OD_LOG_S1s("channelName = ", channelName); //####
            if (RegisterLocalService(channelName, *aService))
            {
                bool             configured = false;
                yarp::os::Bottle configureData;
                
                StartRunning();
                SetSignalHandlers(SignalRunningStop);
                aService->startPinger();
                if (goWasSet || (! stdinAvailable))   
                {
                    configureData.clear();
                    configureData.addInt(outPort);
                    configureData.addDouble(translationScale);
                    if (aService->configure(configureData))
                    {
                        aService->startStreams();
                    }
                }
                for ( ; IsRunning(); )
                {
                    if ((! goWasSet) && stdinAvailable)
                    {
                        char inChar;
                        
                        cout << "Operation: [? b c e q r u]? ";
                        cout.flush();
                        cin >> inChar;
                        switch (inChar)
                        {
                            case '?' :
                                // Help
                                displayCommands();
                                break;
                                
                            case 'b' :
                            case 'B' :
                                // Start streams
                                if (! configured)
                                {
                                    configureData.clear();
                                    configureData.addInt(outPort);
                                    configureData.addDouble(translationScale);
                                    if (aService->configure(configureData))
                                    {
                                        configured = true;
                                    }
                                }
                                if (configured)
                                {
                                    aService->startStreams();
                                }
                                break;
                                
                            case 'c' :
                            case 'C' :
                                // Configure
                                configured = Utilities::PromptForValues(argumentList);
                                if (configured)
                                {
                                    configureData.clear();
                                    configureData.addInt(outPort);
                                    configureData.addDouble(translationScale);
                                    if (aService->configure(configureData))
                                    {
                                        configured = true;
                                    }
                                }
                                else
                                {
                                    cout << "One or more values out of range." << endl;
                                }
                                break;
                                
                            case 'e' :
                            case 'E' :
                                // Stop streams
                                aService->stopStreams();
                                break;
                                
                            case 'q' :
                            case 'Q' :
                                // Quit
                                StopRunning();
                                break;
                                
                            case 'r' :
                            case 'R' :
                                // Restart streams
                                if (! configured)
                                {
                                    configureData.clear();
                                    configureData.addInt(outPort);
                                    configureData.addDouble(translationScale);
                                    if (aService->configure(configureData))
                                    {
                                        configured = true;
                                    }
                                }
                                if (configured)
                                {
                                    aService->restartStreams();
                                }
                                break;
                                
                            case 'u' :
                            case 'U' :
                                // Unconfigure
                                configured = false;
                                break;
                                
                            default :
                                cout << "Unrecognized request '" << inChar << "'." << endl;
                                break;
                                
                        }
                    }
                    else
                    {
#if defined(MpM_MainDoesDelayNotYield)
                        yarp::os::Time::delay(ONE_SECOND_DELAY_ / 10.0);
#else // ! defined(MpM_MainDoesDelayNotYield)
                        yarp::os::Time::yield();
#endif // ! defined(MpM_MainDoesDelayNotYield)
                    }
                }
                UnregisterLocalService(channelName, *aService);
                if (reportOnExit)
                {
                    yarp::os::Bottle metrics;
                    
                    aService->gatherMetrics(metrics);
                    YarpString converted(Utilities::ConvertMetricsToString(metrics));
                    
                    cout << converted.c_str() << endl;
                }
                aService->stop();
            }
            else
            {
                OD_LOG("! (RegisterLocalService(channelName, *aService))"); //####
#if MAC_OR_LINUX_
                GetLogger().fail("Service could not be registered.");
#else // ! MAC_OR_LINUX_
                cerr << "Service could not be registered." << endl;
#endif // ! MAC_OR_LINUX_
            }
        }
        else
        {
            OD_LOG("! (aService->start())"); //####
#if MAC_OR_LINUX_
            GetLogger().fail("Service could not be started.");
#else // ! MAC_OR_LINUX_
            cerr << "Service could not be started." << endl;
#endif // ! MAC_OR_LINUX_
        }
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

/*! @brief The entry point for running the %Unreal output service.
 
 The second, optional, argument is the translation scale factor and the first, optional, argument is
 the port to be written to.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the %Unreal output service.
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
        bool                                goWasSet = false;
        bool                                nameWasSet = false; // not used
        bool                                reportOnExit = false;
        bool                                stdinAvailable = CanReadFromStandardInput();
        double                              translationScale = 1.0;
        int                                 outPort = 9876;
        YarpString                          serviceEndpointName;
        YarpString                          servicePortNumber;
        YarpString                          tag;
        Utilities::PortArgumentDescriptor   firstArg("port", T_("Port to use to connect"),
                                                     Utilities::kArgModeOptional, 9876, false,
                                                     &outPort);
        Utilities::DoubleArgumentDescriptor secondArg("scale", T_("Translation scale"),
                                                      Utilities::kArgModeOptional, 1, true, 0,
                                                      false, 0, &translationScale);
        Utilities::DescriptorVector         argumentList;

        argumentList.push_back(&firstArg);
        argumentList.push_back(&secondArg);
        if (ProcessStandardServiceOptions(argc, argv, argumentList,
                                          DEFAULT_UNREALOUTPUT_SERVICE_NAME_,
                                          UNREALOUTPUT_SERVICE_DESCRIPTION_, "", 2014,
                                          STANDARD_COPYRIGHT_NAME_, goWasSet, nameWasSet,
                                          reportOnExit, tag, serviceEndpointName, servicePortNumber,
                                          kSkipNone))
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
                    setUpAndGo(outPort, translationScale, argumentList, progName, argc, argv, tag,
                               serviceEndpointName, servicePortNumber, goWasSet, stdinAvailable,
                               reportOnExit);
                    
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
