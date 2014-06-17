//--------------------------------------------------------------------------------------
//
//  File:       ServiceLister/main.cpp
//
//  Project:    M+M
//
//  Contains:   A utility application to list the available services.
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
//  Created:    2014-03-12
//
//--------------------------------------------------------------------------------------

#include "M+MUtilities.h"

//#include "ODEnableLogging.h"
#include "ODLogging.h"

#include <cstring>
#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc++11-extensions"
# pragma clang diagnostic ignored "-Wdocumentation"
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
# pragma clang diagnostic ignored "-Wpadded"
# pragma clang diagnostic ignored "-Wshadow"
# pragma clang diagnostic ignored "-Wunused-parameter"
# pragma clang diagnostic ignored "-Wweak-vtables"
#endif // defined(__APPLE__)
#include <yarp/os/all.h>
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief A utility application to list the available services. */

/*! @dir ServiceLister
 @brief The ServiceLister application. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using std::cout;
using std::cerr;
using std::endl;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)

/*! @brief The entry point for listing the available services.

 There are no input arguments and standard output will receive a list of the available services.
 @param argc The number of arguments in 'argv'.
 @param argv The arguments to be used with the example client.
 @returns @c 0 on a successful test and @c 1 on failure. */
int main(int      argc,
         char * * argv)
{
    OD_LOG_INIT(*argv, kODLoggingOptionIncludeProcessID | kODLoggingOptionIncludeThreadID |//####
                kODLoggingOptionEnableThreadSupport | kODLoggingOptionWriteToStderr);//####
    OD_LOG_ENTER();//####
    MplusM::Common::OutputFlavour flavour = MplusM::Common::kOutputFlavourNormal;
    int                           cc;
    
    opterr = 0; // Suppress the error message resulting from an unknown option.
    for (cc = getopt(argc, argv, STANDARD_OPTIONS); -1 != cc; cc = getopt(argc, argv, STANDARD_OPTIONS))
    {
        switch (cc)
        {
            case 'j':
                flavour = MplusM::Common::kOutputFlavourJSON;
                break;
                
            case 't':
                flavour = MplusM::Common::kOutputFlavourTabs;
                break;
                
            default:
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
            yarp::os::Network yarp; // This is necessary to establish any connection to the YARP infrastructure
            
            MplusM::Common::Initialize(*argv);
            bool                         reported = false;
            MplusM::Common::StringVector services;
            
            MplusM::Utilities::GetServiceNames(services);
            if (MplusM::Common::kOutputFlavourJSON == flavour)
            {
                cout << "[ ";
            }
            for (MplusM::Common::StringVector::const_iterator walker(services.begin()); services.end() != walker;
                 ++walker)
            {
                MplusM::Utilities::ServiceDescriptor descriptor;

                if (MplusM::Utilities::GetNameAndDescriptionForService(*walker, descriptor, STANDARD_WAIT_TIME))
                {
                    bool                  sawInputs = false;
                    bool                  sawOutputs = false;
                    yarp::os::ConstString description;
                    yarp::os::ConstString inChannelNames;
                    yarp::os::ConstString outChannelNames;
                    yarp::os::ConstString requests;
                    yarp::os::ConstString serviceName;
                    yarp::os::ConstString servicePortName;
                    
                    switch (flavour)
                    {
                        case MplusM::Common::kOutputFlavourTabs:
                            if (reported)
                            {
                                cout << endl;
                            }
                            break;
                            
                        case MplusM::Common::kOutputFlavourJSON:
                            if (reported)
                            {
                                cout << "," << endl;
                            }
                            cout << "{ ";
                            break;
                            
                        default:
                            if (! reported)
                            {
                                cout << "Services: " << endl;
                            }
                            cout << endl;
                            break;
                            
                    }
                    reported = true;
                    if (MplusM::Common::kOutputFlavourJSON == flavour)
                    {
                        inChannelNames = "[ ";
                        outChannelNames = "[ ";
                    }
                    for (MplusM::Common::StringVector::const_iterator walker(descriptor._inputChannels.begin());
                         descriptor._inputChannels.end() != walker; ++walker)
                    {
                        if (MplusM::Common::kOutputFlavourJSON == flavour)
                        {
                            if (sawInputs)
                            {
                                inChannelNames += ", ";
                            }
                            inChannelNames += CHAR_DOUBLEQUOTE;
                            inChannelNames += MplusM::SanitizeString(*walker);
                            inChannelNames += CHAR_DOUBLEQUOTE;
                        }
                        else
                        {
                            if (sawInputs)
                            {
                                inChannelNames += " ";
                            }
                            inChannelNames += *walker;
                        }
                        sawInputs = true;
                    }
                    if (MplusM::Common::kOutputFlavourJSON == flavour)
                    {
                        inChannelNames += " ]";
                    }
                    for (MplusM::Common::StringVector::const_iterator walker(descriptor._outputChannels.begin());
                         descriptor._outputChannels.end() != walker; ++walker)
                    {
                        if (MplusM::Common::kOutputFlavourJSON == flavour)
                        {
                            if (sawOutputs)
                            {
                                outChannelNames += ", ";
                            }
                            outChannelNames += CHAR_DOUBLEQUOTE;
                            outChannelNames += MplusM::SanitizeString(*walker);
                            outChannelNames += CHAR_DOUBLEQUOTE;
                        }
                        else
                        {
                            if (sawOutputs)
                            {
                                outChannelNames += " ";
                            }
                            outChannelNames += *walker;
                        }
                        sawOutputs = true;
                    }
                    if (MplusM::Common::kOutputFlavourJSON == flavour)
                    {
                        outChannelNames += " ]";
                    }
                    switch (flavour)
                    {
                        case MplusM::Common::kOutputFlavourJSON:
                            servicePortName = MplusM::SanitizeString(*walker);
                            cout << T_(CHAR_DOUBLEQUOTE "ServicePort" CHAR_DOUBLEQUOTE ": " CHAR_DOUBLEQUOTE) <<
                                    servicePortName.c_str() << T_(CHAR_DOUBLEQUOTE ", ");
                            serviceName = MplusM::SanitizeString(descriptor._canonicalName);
                            cout << T_(CHAR_DOUBLEQUOTE "ServiceName" CHAR_DOUBLEQUOTE ": " CHAR_DOUBLEQUOTE) <<
                                    serviceName.c_str() << T_(CHAR_DOUBLEQUOTE ", ");
                            description = MplusM::SanitizeString(descriptor._description);
                            cout << T_(CHAR_DOUBLEQUOTE "Description" CHAR_DOUBLEQUOTE ": " CHAR_DOUBLEQUOTE) <<
                                    description.c_str() << T_(CHAR_DOUBLEQUOTE ", ");
                            requests = MplusM::SanitizeString(descriptor._requestsDescription);
                            cout << T_(CHAR_DOUBLEQUOTE "Requests" CHAR_DOUBLEQUOTE ": " CHAR_DOUBLEQUOTE) <<
                                    requests.c_str() << T_(CHAR_DOUBLEQUOTE ", ");
                            cout << T_(CHAR_DOUBLEQUOTE "Path" CHAR_DOUBLEQUOTE ": " CHAR_DOUBLEQUOTE) <<
                                    descriptor._path.c_str() << T_(CHAR_DOUBLEQUOTE ", ");
                            cout << T_(CHAR_DOUBLEQUOTE "SecondaryInputs" CHAR_DOUBLEQUOTE ": ") <<
                                    inChannelNames.c_str() << ", ";
                            cout << T_(CHAR_DOUBLEQUOTE "SecondaryOutputs" CHAR_DOUBLEQUOTE ": ") <<
                                    outChannelNames.c_str() << " }";
                            break;
                            
                        case MplusM::Common::kOutputFlavourTabs:
                            servicePortName = MplusM::SanitizeString(*walker, true);
                            cout << servicePortName.c_str() << "\t";
                            serviceName = MplusM::SanitizeString(descriptor._canonicalName, true);
                            cout << serviceName.c_str() << "\t";
                            description = MplusM::SanitizeString(descriptor._description, true);
                            cout << description.c_str() << "\t";
                            requests = MplusM::SanitizeString(descriptor._requestsDescription, true);
                            cout << requests.c_str() << "\t" << descriptor._path.c_str() << "\t" <<
                                    inChannelNames.c_str() << "\t" << outChannelNames.c_str();
                            break;
                            
                        default:
                            cout << "Service port:      " << walker->c_str() << endl;
                            cout << "Service name:      " << descriptor._canonicalName.c_str() << endl;
                            MplusM::OutputDescription(cout, "Description:       ", descriptor._description);
                            MplusM::OutputDescription(cout, "Requests:          ", descriptor._requestsDescription);
                            cout << "Path:              " << descriptor._path.c_str() << endl;
                            MplusM::OutputDescription(cout, "Secondary inputs:  ", inChannelNames);
                            MplusM::OutputDescription(cout, "Secondary outputs: ", outChannelNames);
                            break;
                            
                    }
                }
            }
            switch (flavour)
            {
                case MplusM::Common::kOutputFlavourTabs:
                    if (reported)
                    {
                        cout << endl;
                    }
                    break;
                    
                case MplusM::Common::kOutputFlavourJSON:
                    cout << " ]" << endl;
                    break;
                    
                default:
                    if (reported)
                    {
                        cout << endl;
                    }
                    else
                    {
                        cout << "No services found." << endl;
                    }
                    break;

            }
        }
#if CheckNetworkWorks_
        else
        {
            OD_LOG("! (yarp::os::Network::checkNetwork())");//####
            cerr << "YARP network not running." << endl;
        }
#endif // CheckNetworkWorks_
    }
    catch (...)
    {
        OD_LOG("Exception caught");//####
    }
    yarp::os::Network::fini();
    OD_LOG_EXIT_L(0);//####
    return 0;
} // main
