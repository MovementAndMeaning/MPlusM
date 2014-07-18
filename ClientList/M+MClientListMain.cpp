//--------------------------------------------------------------------------------------------------
//
//  File:       ClientListMain.cpp
//
//  Project:    M+M
//
//  Contains:   A utility application to list the clients of a service or all services.
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
//  Created:    2014-03-12
//
//--------------------------------------------------------------------------------------------------

#include "M+MBaseClient.h"
#include "M+MClientChannel.h"
#include "M+MRequests.h"
#include "M+MServiceRequest.h"
#include "M+MServiceResponse.h"

//#include "ODEnableLogging.h"
#include "ODLogging.h"
#if (!MAC_OR_LINUX) //ASSUME WINDOWS
#include "getopt.h"
#endif //(!MAC_OR_LINUX)

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief A utility application to list the clients of a service or all services. */

/*! @dir ClientList
 @brief The set of files that implement the client list application. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using std::cout;
using std::endl;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

/*! @brief Process the response to the 'list' request sent to a service.
 @param flavour The format for the output.
 @param serviceName The name of the service that generated the response.
 @param response The response to be processed.
 @param sawResponse @c true if there was already a response output and @c false if this is the first.
 @returns @c true if some output was generated and @c false otherwise. */
static bool processResponse(MplusM::Common::OutputFlavour           flavour,
                            const yarp::os::ConstString &           serviceName,
                            const MplusM::Common::ServiceResponse & response,
                            const bool                              sawResponse)
{
    OD_LOG_ENTER(); //####
    OD_LOG_S1s("serviceName = ", serviceName); //####
    OD_LOG_P1("response = ", &response); //####
    bool                  result = false;
    yarp::os::ConstString cleanServiceName(MplusM::SanitizeString(serviceName,
                                                  MplusM::Common::kOutputFlavourJSON != flavour));
    
    OD_LOG_S1s("response = ", response.asString()); //####
    for (int ii = 0, howMany = response.count(); ii < howMany; ++ii)
    {
        yarp::os::Value element(response.element(ii));
        
        if (element.isString())
        {
            yarp::os::ConstString clientString(element.toString());
            
            switch (flavour)
            {
                case MplusM::Common::kOutputFlavourJSON :
                    if (result || sawResponse)
                    {
                        cout << "," << endl;
                    }
                    cout << T_("{ " CHAR_DOUBLEQUOTE "Service" CHAR_DOUBLEQUOTE ": "
                               CHAR_DOUBLEQUOTE) << cleanServiceName.c_str() <<
                            T_(CHAR_DOUBLEQUOTE ", " CHAR_DOUBLEQUOTE "Client" CHAR_DOUBLEQUOTE ": "
                               CHAR_DOUBLEQUOTE) <<
                            MplusM::SanitizeString(clientString, true).c_str() <<
                            T_(CHAR_DOUBLEQUOTE " }");
                    break;
                    
                case MplusM::Common::kOutputFlavourTabs :
                    cout << cleanServiceName.c_str() << "\t" <<
                            MplusM::SanitizeString(clientString).c_str() << endl;
                    break;
                    
                default :
                    if (! result)
                    {
                        cout << "Service: " << cleanServiceName.c_str() << endl << "Clients: " <<
                        endl;
                    }
                    cout << "   " << MplusM::SanitizeString(clientString).c_str() << endl;
                    break;
                    
            }
            result = true;
        }
    }
    OD_LOG_EXIT_B(result); //####
    return result;
} // processResponse

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)

/*! @brief The entry point for listing the clients of a service.
 
 The optional argument is the name of the channel for the service. If the channel is not specified,
 all service channels will be reported. Standard output will receive a list of the specified
 clients.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the example client.
 @returns @c 0 on a successful test and @c 1 on failure. */
int main(int     argc,
         char ** argv)
{
    OD_LOG_INIT(*argv, kODLoggingOptionIncludeProcessID | kODLoggingOptionIncludeThreadID | //####
                kODLoggingOptionEnableThreadSupport | kODLoggingOptionWriteToStderr); //####
    OD_LOG_ENTER(); //####
    MplusM::Common::SetUpLogger(*argv);
    MplusM::Common::OutputFlavour flavour = MplusM::Common::kOutputFlavourNormal;

    opterr = 0; // Suppress the error message resulting from an unknown option.
    for (int cc = getopt(argc, argv, STANDARD_OPTIONS); -1 != cc;
         cc = getopt(argc, argv, STANDARD_OPTIONS))
    {
        switch (cc)
        {
            case 'j' :
                flavour = MplusM::Common::kOutputFlavourJSON;
                break;
                
            case 't' :
                flavour = MplusM::Common::kOutputFlavourTabs;
                break;
                
            default :
                // Ignore unknown options.
                break;
                
        }
    }
    try
    {
#if CheckNetworkWorks_
        if (yarp::os::Network::checkNetwork())
#endif // CheckNetworkWorks_
        {
            yarp::os::Network     yarp; // This is necessary to establish any connection to the YARP
                                        // infrastructure
            yarp::os::ConstString channelNameRequest(MpM_REQREP_DICT_CHANNELNAME_KEY ":");
            
            MplusM::Common::Initialize(*argv);
            if (optind >= argc)
            {
                channelNameRequest += "*";
            }
            else
            {
                channelNameRequest += argv[optind];
            }
            yarp::os::Bottle matches(MplusM::Common::FindMatchingServices(channelNameRequest));
            
            if (MpM_EXPECTED_MATCH_RESPONSE_SIZE == matches.size())
            {
                // First, check if the search succeeded.
                yarp::os::ConstString matchesFirstString(matches.get(0).toString());
                
                if (strcmp(MpM_OK_RESPONSE, matchesFirstString.c_str()))
                {
                    OD_LOG("(strcmp(MpM_OK_RESPONSE, matchesFirstString.c_str()))"); //####
                    yarp::os::ConstString reason(matches.get(1).toString());
                    
                    MplusM::Common::GetLogger().fail(yarp::os::ConstString("Failed: ") + reason +
                                                     ".");
                }
                else
                {
                    // Now, process the second element.
                    yarp::os::Bottle * matchesList = matches.get(1).asList();
                    
                    if (matchesList)
                    {
                        int matchesCount = matchesList->size();
                        
                        if (matchesCount)
                        {
                            yarp::os::ConstString           aName =
                                        MplusM::Common::GetRandomChannelName(HIDDEN_CHANNEL_PREFIX
                                                                             "clientlist_/"
                                                                             DEFAULT_CHANNEL_ROOT);
                            MplusM::Common::ClientChannel * newChannel =
                                                                new MplusM::Common::ClientChannel;
                            
                            if (newChannel)
                            {
                                if (newChannel->openWithRetries(aName, STANDARD_WAIT_TIME))
                                {
                                    bool             sawRequestResponse = false;
                                    yarp::os::Bottle parameters;
                                    
                                    if (MplusM::Common::kOutputFlavourJSON == flavour)
                                    {
                                        cout << "[ ";
                                    }
                                    for (int ii = 0; ii < matchesCount; ++ii)
                                    {
                                        yarp::os::ConstString aMatch =
                                                                    matchesList->get(ii).toString();
                                        
                                        if (MplusM::Common::NetworkConnectWithRetries(aName, aMatch,
                                                                              STANDARD_WAIT_TIME,
                                                                                      false))
                                        {
                                            MplusM::Common::ServiceRequest
                                                                        request(MpM_CLIENTS_REQUEST,
                                                                                parameters);
                                            MplusM::Common::ServiceResponse response;
                                            
                                            if (request.send(*newChannel, &response))
                                            {
                                                OD_LOG("(request.send(*newChannel, " //####
                                                       "&response))"); //####
                                                if (0 < response.count())
                                                {
                                                    OD_LOG("(0 < response.count())"); //####
                                                    if (processResponse(flavour, aMatch, response,
                                                                        sawRequestResponse))
                                                    {
                                                        sawRequestResponse = true;
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                OD_LOG("! (request.send(*newChannel, " //####
                                                       "&response))"); //####
                                                yarp::os::impl::Logger & theLogger =
                                                                        MplusM::Common::GetLogger();
                                                
                                                theLogger.fail(yarp::os::ConstString("Problem "
                                                                         "communicating with ") +
                                                               aMatch + ".");
                                            }
#if defined(MpM_DoExplicitDisconnect)
                                        if (! MplusM::Common::NetworkDisconnectWithRetries(aName,
                                                                                           aMatch,
                                                                               STANDARD_WAIT_TIME))
                                            {
                                                OD_LOG("(! MplusM::Common::NetworkDisconnect" //####
                                                       "WithRetries(aName, aMatch, " //####
                                                       "STANDARD_WAIT_TIME))"); //####
                                            }
#endif // defined(MpM_DoExplicitDisconnect)
                                        }
                                        else
                                        {
                                            OD_LOG("! (MplusM::Common::NetworkConnectWith" //####
                                                   "Retries(aName, aMatch, " //####
                                                   "STANDARD_WAIT_TIME, false))"); //####
                                        }
                                    }
                                    if (MplusM::Common::kOutputFlavourJSON == flavour)
                                    {
                                        cout << " ]" << endl;
                                    }
                                    if (! sawRequestResponse)
                                    {
                                        switch (flavour)
                                        {
                                            case MplusM::Common::kOutputFlavourJSON :
                                            case MplusM::Common::kOutputFlavourTabs :
                                                break;
                                                
                                            default :
                                                cout << "No client connections found." << endl;
                                                break;
                                                
                                        }
                                    }
#if defined(MpM_DoExplicitClose)
                                    newChannel->close();
#endif // defined(MpM_DoExplicitClose)
                                }
                                else
                                {
                                    OD_LOG("! (newChannel->openWithRetries(aName, " //####
                                           "STANDARD_WAIT_TIME))"); //####
                                }
                                delete newChannel;
                            }
                            else
                            {
                                OD_LOG("! (newChannel)"); //####
                            }
                        }
                        else
                        {
                            switch (flavour)
                            {
                                case MplusM::Common::kOutputFlavourJSON :
                                case MplusM::Common::kOutputFlavourTabs :
                                    break;
                                    
                                default :
                                    cout << "No services found." << endl;
                                    break;
                                    
                            }
                        }
                    }
                    else
                    {
                        OD_LOG("! (matchesList)"); //####
                    }
                }
            }
            else
            {
                OD_LOG("! (MpM_EXPECTED_MATCH_RESPONSE_SIZE == matches.size())"); //####
                MplusM::Common::GetLogger().fail("Problem getting information from the Service "
                                                 "Registry.");
            }
        }
#if CheckNetworkWorks_
        else
        {
            OD_LOG("! (yarp::os::Network::checkNetwork())"); //####
            MplusM::Common::GetLogger().fail("YARP network not running.");
        }
#endif // CheckNetworkWorks_
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
    }
    yarp::os::Network::fini();
    OD_LOG_EXIT_L(0); //####
    return 0;
} // main