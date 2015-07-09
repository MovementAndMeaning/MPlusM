//--------------------------------------------------------------------------------------------------
//
//  File:       m+mAddressClientMain.cpp
//
//  Project:    m+m
//
//  Contains:   The main application for the client of the Address service.
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
//  Created:    2015-02-11
//
//--------------------------------------------------------------------------------------------------

#include "m+mAddressClient.h"
#include "m+mAddressRequests.h"

#include <m+m/m+mStringArgumentDescriptor.h>
#include <m+m/m+mUtilities.h>

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunknown-pragmas"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file 
 @brief The main application for the client of the Address service. */

/*! @dir AddressClient
 @brief The set of files that implement the Address client. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Namespace references
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Address;
using namespace MplusM::Common;
using std::cerr;
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

/*! @brief Process the arguments for the application.
 @param outputMode The mode ('both', 'address', 'port') of the output.
 @param tag The tag for the service to be connected to.
 @param namePattern The generated search value.
 @param needsAddress @c true if the IP address is requested and @c false otherwise.
 @param needsPort @c true if the port is requested and @c false otherwise.
 @returns @c true if the mode was recognized and @c false otherwise. */
static bool processArguments(const YarpString & outputMode,
                             const YarpString & tag,
                             YarpString &       namePattern,
                             bool &             needsAddress,
                             bool &             needsPort)
{
    OD_LOG_ENTER(); //####
    OD_LOG_S2s("outputMode = ", outputMode, "tag = ", tag); //####
    OD_LOG_P3("namePattern = ", &namePattern, "needsAddress = ", &needsAddress, //####
              "needsPort = ", &needsPort); //####
    bool okSoFar = true;
    
    if (0 < outputMode.length())
    {
        if (outputMode == "address")
        {
            needsAddress = true;
            needsPort = false;
        }
        else if (outputMode == "both")
        {
            needsAddress = needsPort = true;
        }
        else if (outputMode == "port")
        {
            needsAddress = false;
            needsPort = true;
        }
        else
        {
            okSoFar = false;
        }
    }
    else
    {
        needsAddress = needsPort = true;
    }
    if (0 < tag.length())
    {
        YarpString singleQuote("'");

        namePattern = singleQuote + namePattern + " " + tag + singleQuote;
    }
    OD_LOG_EXIT_B(okSoFar); //####
    return okSoFar;
} // processArguments

/*! @brief Set up the environment and perform the operation.
 @param outputMode The mode ('both', 'address', 'port') of the output.
 @param tag The tag for the service to be connected to.
 @param flavour The format for the output. */
#if defined(MpM_ReportOnConnections)
static void setUpAndGo(const YarpString &      outputMode,
                       const YarpString &      tag,
                       const OutputFlavour     flavour,
                       ChannelStatusReporter * reporter)
#else // ! defined(MpM_ReportOnConnections)
static void setUpAndGo(const YarpString &  outputMode,
                       const YarpString &  tag,
                       const OutputFlavour flavour)
#endif // ! defined(MpM_ReportOnConnections)
{
    OD_LOG_ENTER(); //####
    OD_LOG_S2s("outputMode = ", outputMode, "tag = ", tag); //####
#if defined(MpM_ReportOnConnections)
    OD_LOG_P1("reporter = ", reporter); //####
#endif // defined(MpM_ReportOnConnections)
    AddressClient * aClient = new AddressClient;
    
    if (aClient)
    {
        bool       needsAddress;
        bool       needsPort;
        YarpString channelNameRequest(MpM_REQREP_DICT_NAME_KEY_ ":");
        YarpString namePattern(MpM_ADDRESS_CANONICAL_NAME_);
        
#if defined(MpM_ReportOnConnections)
        aClient->setReporter(*reporter, true);
#endif // defined(MpM_ReportOnConnections)
        if (processArguments(outputMode, tag, namePattern, needsAddress, needsPort))
        {
            channelNameRequest += namePattern;
            if (aClient->findService(channelNameRequest.c_str()))
            {
                if (aClient->connectToService())
                {
                    YarpString address;
                    int        port;
                    
                    if (aClient->getAddress(address, port))
                    {
                        switch (flavour)
                        {
                            case kOutputFlavourJSON :
                                cout << "{ ";
                                if (needsAddress)
                                {
                                    cout << T_(CHAR_DOUBLEQUOTE_ "Address" CHAR_DOUBLEQUOTE_ ": "
                                               CHAR_DOUBLEQUOTE_);
                                    cout << SanitizeString(address).c_str() <<
                                            T_(CHAR_DOUBLEQUOTE_);
                                }
                                if (needsPort)
                                {
                                    if (needsAddress)
                                    {
                                        cout << ", ";
                                    }
                                    cout << T_(CHAR_DOUBLEQUOTE_ "Address" CHAR_DOUBLEQUOTE_ ": "
                                               CHAR_DOUBLEQUOTE_);
                                    cout << port << T_(CHAR_DOUBLEQUOTE_);
                                }
                                cout << " }" << endl;
                                break;
                                
                            case kOutputFlavourTabs :
                                if (needsAddress)
                                {
                                    cout << address.c_str();
                                }
                                if (needsPort)
                                {
                                    if (needsAddress)
                                    {
                                        cout << "\t";
                                    }
                                    cout << port;
                                }
                                cout << endl;
                                break;
                                
                            case kOutputFlavourNormal :
                                if (needsAddress)
                                {
                                    cout << "Address: " << address.c_str();
                                }
                                if (needsPort)
                                {
                                    if (needsAddress)
                                    {
                                        cout << ", ";
                                    }
                                    cout << "Port: " << port;
                                }
                                cout << endl;
                                break;
                                
                            default :
                                break;
                                
                        }
                    }
                    else
                    {
                        OD_LOG("! (aClient->getAddress(address, port))"); //####
#if MAC_OR_LINUX_
                        GetLogger().fail("Problem fetching the address information.");
#endif // MAC_OR_LINUX_
                    }
                }
                else
                {
                    OD_LOG("! (aClient->connectToService())"); //####
#if MAC_OR_LINUX_
                    GetLogger().fail("Could not connect to the required service.");
#else // ! MAC_OR_LINUX_
                    cerr << "Could not connect to the required service." << endl;
#endif // ! MAC_OR_LINUX_
                }
            }
            else
            {
                OD_LOG("! (aClient->findService(channelNameRequest)"); //####
#if MAC_OR_LINUX_
                GetLogger().fail("Could not find the required service.");
#else // ! MAC_OR_LINUX_
                cerr << "Could not find the required service." << endl;
#endif // ! MAC_OR_LINUX_
            }            
        }
        else
        {
            cout << "Invalid mode argument." << endl;
        }
        delete aClient;
    }
    else
    {
        OD_LOG("! (aClient)"); //####
    }
    OD_LOG_EXIT(); //####
} // setUpAndGo

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)

#if (! MAC_OR_LINUX_)
# pragma warning(push)
# pragma warning(disable: 4100)
#endif // ! MAC_OR_LINUX_
/*! @brief The entry point for communicating with the Address service.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the Address client.
 @returns @c 0 on a successful test and @c 1 on failure. */
int main(int      argc,
         char * * argv)
{
#if MAC_OR_LINUX_
# pragma unused(argc)
#endif // MAC_OR_LINUX_
    YarpString progName(*argv);

    OD_LOG_INIT(progName.c_str(), kODLoggingOptionIncludeProcessID | //####
                kODLoggingOptionIncludeThreadID | kODLoggingOptionEnableThreadSupport | //####
                kODLoggingOptionWriteToStderr); //####
    OD_LOG_ENTER(); //####
#if MAC_OR_LINUX_
    SetUpLogger(progName);
#endif // MAC_OR_LINUX_
    try
    {
        YarpString                          outputMode;
        YarpString                          tag;
        Utilities::StringArgumentDescriptor firstArg("outputMode", T_("The mode of the output "
                                                                      "['address', 'port' or "
                                                                      "'both']"),
                                                     Utilities::kArgModeOptional, "both",
                                                     &outputMode);
        Utilities::StringArgumentDescriptor secondArg("tag", T_("The tag for the service to be "
                                                                "connnected to"),
                                                      Utilities::kArgModeOptional, "", &tag);
        Utilities::DescriptorVector         argumentList;
        OutputFlavour                       flavour;
        
        argumentList.push_back(&firstArg);
        argumentList.push_back(&secondArg);
        if (Utilities::ProcessStandardClientOptions(argc, argv, argumentList,
                                                    "The client for the Address service", 2015,
                                                    STANDARD_COPYRIGHT_NAME_, flavour))
        {
            try
            {
				Utilities::SetUpGlobalStatusReporter();
				Utilities::CheckForNameServerReporter();
                if (Utilities::CheckForValidNetwork())
                {
#if defined(MpM_ReportOnConnections)
					ChannelStatusReporter * reporter = Utilities::GetGlobalStatusReporter();
#endif // defined(MpM_ReportOnConnections)
                    yarp::os::Network       yarp; // This is necessary to establish any connections
                                                  // to the YARP infrastructure
                    
                    Initialize(progName);
                    if (Utilities::CheckForRegistryService())
                    {
#if defined(MpM_ReportOnConnections)
                        setUpAndGo(outputMode, tag, flavour, reporter);
#else // ! defined(MpM_ReportOnConnections)
                        setUpAndGo(outputMode, tag, flavour);
#endif // ! defined(MpM_ReportOnConnections)
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
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
    }
    yarp::os::Network::fini();
    OD_LOG_EXIT_L(0); //####
    return 0;
} // main
#if (! MAC_OR_LINUX_)
# pragma warning(pop)
#endif // ! MAC_OR_LINUX_
