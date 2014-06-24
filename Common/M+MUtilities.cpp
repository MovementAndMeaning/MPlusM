//--------------------------------------------------------------------------------------
//
//  File:       M+MUtilities.cpp
//
//  Project:    M+M
//
//  Contains:   The function and variable declarations for utilities for M+M clients and
//              services.
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
//  Created:    2014-03-19
//
//--------------------------------------------------------------------------------------

#include "M+MUtilities.h"
#include "M+MBaseClient.h"
#include "M+MChannelStatusReporter.h"
#include "M+MClientChannel.h"
#include "M+MRequests.h"
#include "M+MServiceRequest.h"
#include "M+MServiceResponse.h"

//#include "ODEnableLogging.h"
#include "ODLogging.h"

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
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/Protocol.h>
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 
 @brief The function and variable definitions for utilities for M+M clients and services. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;
using namespace MplusM::Utilities;
using std::cout;
using std::endl;
using std::cerr;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

/*! @brief The indicator string for the beginning of new information received. */
static const char * kLineMarker = "registration name ";

/*! @brief The part name being used for probing connections. */
static const char * kMagicName = "<!!!>";

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

#if (! MAC_OR_LINUX_)
//ASSUME WINDOWS
# define strtok_r strtok_s /* Equivalent routine for Windows. */
#endif // defined (! MAC_OR_LINUX_)

/*! @brief Check if the response is for an input connection.
 @param response The response from the port that is being checked.
 @param inputs The collected inputs for the port. */
static void checkForInputConnection(const yarp::os::Bottle & response,
                                    ChannelVector &          inputs)
{
    OD_LOG_ENTER();//####
    OD_LOG_S1("response = ", response.toString().c_str());//####
    OD_LOG_P1("inputs = ", &inputs);//####
    bool         sawConnection = false;
    const char * matchString[] = { "There", "is", "an", "input", "connection", "from", NULL, "to", NULL, "using",
                                    NULL };
    int          respLen = response.size();
    int          matchLen = (sizeof(matchString) / sizeof(*matchString));
    
    if (respLen >= matchLen)
    {
        bool matched = true;
        
        for (int ii = 0; matched && (ii < matchLen); ++ii)
        {
            yarp::os::ConstString element(response.get(ii).asString());
            
            if (matchString[ii])
            {
                if (element != matchString[ii])
                {
                    matched = false;
                }
            }
        }
        if (matched)
        {
            yarp::os::ConstString mode(response.get(matchLen - 1).asString());
            yarp::os::ConstString destination(response.get(matchLen - 3).asString());
            yarp::os::ConstString source(response.get(matchLen - 5).asString());
            
            if ((source != kMagicName) && (destination != kMagicName))
            {
                ChannelDescription connection;
                
                connection._portName = source;
                if (mode == "tcp")
                {
                    connection._portMode = Common::kChannelModeTCP;
                }
                else if (mode == "udp")
                {
                    connection._portMode = Common::kChannelModeUDP;
                }
                else
                {
                    connection._portMode = Common::kChannelModeOther;
                }
                inputs.push_back(connection);
            }
        }
    }
    OD_LOG_EXIT();//####
} // checkForInputConnection

/*! @brief Check if the response is for an output connection.
 @param response The response from the port that is being checked.
 @param outputs The collected outputs for the port. */
static void checkForOutputConnection(const yarp::os::Bottle & response,
                                     ChannelVector &          outputs)
{
    OD_LOG_ENTER();//####
    OD_LOG_S1("response = ", response.toString().c_str());//####
    OD_LOG_P1("outputs = ", &outputs);//####
    const char * matchString[] = { "There", "is", "an", "output", "connection", "from", NULL, "to", NULL, "using",
                                    NULL };
    int          respLen = response.size();
    int          matchLen = (sizeof(matchString) / sizeof(*matchString));
    
    if (respLen >= matchLen)
    {
        bool matched = true;
        
        for (int ii = 0; matched && (ii < matchLen); ++ii)
        {
            yarp::os::ConstString element(response.get(ii).asString());
            
            if (matchString[ii])
            {
                if (element != matchString[ii])
                {
                    matched = false;
                }
            }
        }
        if (matched)
        {
            yarp::os::ConstString mode(response.get(matchLen - 1).asString());
            yarp::os::ConstString destination(response.get(matchLen - 3).asString());
            yarp::os::ConstString source(response.get(matchLen - 5).asString());

            if ((source != kMagicName) && (destination != kMagicName))
            {
                ChannelDescription connection;
                
                connection._portName = destination;
                if (mode == "tcp")
                {
                    connection._portMode = Common::kChannelModeTCP;
                }
                else if (mode == "udp")
                {
                    connection._portMode = Common::kChannelModeUDP;
                }
                else
                {
                    connection._portMode = Common::kChannelModeOther;
                }
                outputs.push_back(connection);
            }
        }
    }
    OD_LOG_EXIT();//####
} // checkForOutputConnection

/*! @brief Check the response to the 'getAssociates' request for validity.
 @param response The response to be checked.
 @param associates The associated ports for the port.
 @returns @c true if the response was valid and @c false otherwise. */
static bool processGetAssociatesResponse(const Package &   response,
                                         PortAssociation & associates)
{
    OD_LOG_ENTER();//####
    OD_LOG_S1("response = ", response.toString().c_str());//####
    bool result = false;
    
    try
    {
        if (MpM_EXPECTED_GETASSOCIATES_RESPONSE_SIZE <= response.size())
        {
            // The first element of the response should be 'OK' or 'FAILED'.
            yarp::os::Value responseFirst(response.get(0));
            
            if (responseFirst.isString())
            {
                yarp::os::ConstString responseFirstAsString(responseFirst.toString());
                
                if (! strcmp(MpM_OK_RESPONSE, responseFirstAsString.c_str()))
                {
                    yarp::os::Value responseSecond(response.get(1));
                    yarp::os::Value responseThird(response.get(2));
                    yarp::os::Value responseFourth(response.get(3));
                    
                    if (responseSecond.isInt() && responseThird.isList() && responseFourth.isList())
                    {
                        associates._valid = true;
                        associates._primary = (0 != responseSecond.asInt());
                        Package * thirdAsList = responseThird.asList();
                        Package * fourthAsList = responseFourth.asList();
                        
                        for (int ii = 0, mm = thirdAsList->size(); mm > ii; ++ii)
                        {
                            yarp::os::Value aPort(thirdAsList->get(ii));
                            
                            if (aPort.isString())
                            {
                                associates._inputs.push_back(aPort.toString());
                            }
                        }
                        for (int ii = 0, mm = fourthAsList->size(); mm > ii; ++ii)
                        {
                            yarp::os::Value aPort(fourthAsList->get(ii));
                            
                            if (aPort.isString())
                            {
                                associates._outputs.push_back(aPort.toString());
                            }
                        }
                        result = true;
                    }
                    else
                    {
                        OD_LOG("! (responseSecond.isInt() && responseThird.isList() && "//####
                               "responseFourth.isList())");//####
                    }
                }
                else if (strcmp(MpM_FAILED_RESPONSE, responseFirstAsString.c_str()))
                {
                    OD_LOG("! (! strcmp(MpM_FAILED_RESPONSE, responseFirstAsString.c_str()))");//####
                }
            }
            else
            {
                OD_LOG("! (responseFirst.isString())");//####
            }
        }
        else
        {
            OD_LOG("! (MpM_EXPECTED_GETASSOCIATES_RESPONSE_SIZE <= response.size())");//####
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught");//####
        throw;
    }
    OD_LOG_EXIT_B(result);//####
    return result;
} // processGetAssociatesResponse

/*! @brief Process the response from the name server.
 
 Note that each line of the response, except the last, is started with 'registration name'. This is followed by the
 port name, 'ip', the IP address, 'port' and the port number.
 @param received The response to be processed.
 @param ports The list of non-default ports/ipaddress/portnumber found. */
static void processNameServerResponse(const yarp::os::ConstString & received,
                                      PortVector &                  ports)
{
    OD_LOG_ENTER();//####
    OD_LOG_S1("received = ", received.c_str());//####
    size_t                lineMakerLength = strlen(kLineMarker);
    yarp::os::ConstString nameServerName(yarp::os::Network::getNameServerName());
    yarp::os::ConstString workingCopy(received);
    
    OD_LOG_S1("nameServerName = ", nameServerName.c_str());//####
    for (size_t nextPos = 0; yarp::os::ConstString::npos != nextPos; )
    {
        nextPos = workingCopy.find(kLineMarker);
        if (yarp::os::ConstString::npos != nextPos)
        {
            workingCopy = workingCopy.substr(nextPos + lineMakerLength);
            size_t chopPos = workingCopy.find(kLineMarker);
            
            if (yarp::os::ConstString::npos != chopPos)
            {
                char *                channelName;
                yarp::os::ConstString chopped(workingCopy.substr(0, chopPos));
                char *                choppedAsChars = strdup(chopped.c_str());
                char *                ipAddress;
                char *                saved;
                char *                pp = strtok_r(choppedAsChars, " ", &saved);
                
                if (pp)
                {
                    // Port name
                    if ('/' == *pp)
                    {
                        channelName = pp;
                        if (nameServerName == channelName)
                        {
                            pp = NULL;
                        }
                        else
                        {
                            pp = strtok_r(NULL, " ", &saved);
                        }
                    }
                    else
                    {
                        pp = NULL;
                    }
                }
                if (pp)
                {
                    // 'ip'
                    if (strcmp(pp, "ip"))
                    {
                        pp = NULL;
                    }
                    else
                    {
                        pp = strtok_r(NULL, " ", &saved);
                    }
                }
                if (pp)
                {
                    ipAddress = pp;
                    pp = strtok_r(NULL, " ", &saved);
                }
                if (pp)
                {
                    // 'port'
                    if (strcmp(pp, "port"))
                    {
                        pp = NULL;
                    }
                    else
                    {
                        pp = strtok_r(NULL, " ", &saved);
                    }
                }
                if (pp)
                {
                    PortDescriptor aDescriptor;
                    
                    aDescriptor._portName = channelName;
                    aDescriptor._portIpAddress = ipAddress;
                    aDescriptor._portPortNumber = pp;
                    ports.push_back(aDescriptor);
                }
                free(choppedAsChars);
            }
        }
    }
    OD_LOG_EXIT();//####
} // processNameServerResponse

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)

bool MplusM::Utilities::AddConnection(const yarp::os::ConstString & fromPortName,
                                      const yarp::os::ConstString & toPortName,
                                      const double                  timeToWait,
                                      const bool                    isUDP)
{
    OD_LOG_ENTER();//####
    OD_LOG_S2("fromPortName = ", fromPortName.c_str(), "toPortName = ", toPortName.c_str());//####
    OD_LOG_D1("timeToWait = ", timeToWait);//####
    OD_LOG_B1("isUDP = ", isUDP);//####
    bool result = NetworkConnectWithRetries(fromPortName, toPortName, timeToWait, isUDP);
    
    OD_LOG_EXIT_B(result);//####
    return result;
} // MplusM::Utilities::AddConnection

bool MplusM::Utilities::CheckForRegistryService(const PortVector & ports)
{
    OD_LOG_ENTER();//####
    OD_LOG_P1("ports = ", &ports);//####
    bool result = false;
    
    for (PortVector::const_iterator walker(ports.begin()); ports.end() != walker; ++walker)
    {
        if (walker->_portName == MpM_REGISTRY_CHANNEL_NAME)
        {
            result = true;
            break;
        }
    }
    return result;
} // MplusM::Utilities::CheckForRegistryService

void MplusM::Utilities::GatherPortConnections(const yarp::os::ConstString & portName,
                                              ChannelVector &               inputs,
                                              ChannelVector &               outputs,
                                              const InputOutputFlag         which,
                                              const bool                    quiet)
{
    OD_LOG_ENTER();//####
    OD_LOG_P2("inputs = ", &inputs, "outputs = ", &outputs);//####
    OD_LOG_L1("which = ", static_cast<int>(which));//####
    OD_LOG_B1("quiet = ", quiet);//####
    yarp::os::Contact address = yarp::os::Network::queryName(portName.c_str());
    
    inputs.clear();
    outputs.clear();
    if (address.isValid())
    {
        if ((address.getCarrier() == "tcp") || (address.getCarrier() == "fast_tcp") ||
            (address.getCarrier() == "xmlrpc"))
        {
            // Note that the following connect() call will hang indefinitely if the address given is for an 'output'
            // port that is connected to another 'output' port. 'yarp ping /port' will hang as well.
            yarp::os::OutputProtocol * out = yarp::os::impl::Carriers::connect(address);
            
            if (out)
            {
                yarp::os::Route rr(kMagicName, portName.c_str(), "text_ack");
                
                if (out->open(rr))
                {
                    yarp::os::Bottle                         resp;
                    yarp::os::impl::BufferedConnectionWriter bw(out->getConnection().isTextMode());
                    yarp::os::InputStream &                  is = out->getInputStream();
                    yarp::os::OutputStream &                 os = out->getOutputStream();
                    yarp::os::impl::PortCommand              pc(0, "*");
                    yarp::os::impl::StreamConnectionReader   reader;
                    
                    pc.write(bw);
                    bw.write(os);
                    reader.reset(is, NULL, rr, 0, true);
                    for (bool done = false; ! done; )
                    {
                        resp.read(reader);
                        yarp::os::ConstString checkString(resp.get(0).asString());
                        
                        if (checkString == "<ACK>")
                        {
                            done = true;
                        }
                        else if (checkString == "There")
                        {
                            if (which & kInputAndOutputInput)
                            {
                                checkForInputConnection(resp, inputs);
                            }
                            if (which & kInputAndOutputOutput)
                            {
                                checkForOutputConnection(resp, outputs);                                
                            }
                        }
                    }
                }
                else if (! quiet)
                {
                    cerr << "Could not open route to port." << endl;
                }
                delete out;
            }
            else if (! quiet)
            {
                cerr << "Could not connect to port." << endl;
            }
        }
        else if (! quiet)
        {
            cerr << "Port not using recognized connection type." << endl;
        }
    }
    else if (! quiet)
    {
        cerr << "Port name not recognized." << endl;
    }
    OD_LOG_EXIT();//####
} // MplusM::Utilities::GatherPortConnections

bool MplusM::Utilities::GetAssociatedPorts(const yarp::os::ConstString & portName,
                                           PortAssociation &             associates,
                                           const double                  timeToWait,
                                           const bool                    quiet)
{
    OD_LOG_ENTER();//####
    OD_LOG_S1("portName = ", portName.c_str());//####
    OD_LOG_D1("timeToWait = ", timeToWait);//####
    OD_LOG_B1("quiet = ", quiet);//####
    bool result = false;
    
    associates._inputs.clear();
    associates._outputs.clear();
    associates._primary = associates._valid = false;
    try
    {
        yarp::os::ConstString aName(GetRandomChannelName("_getassociates_/" DEFAULT_CHANNEL_ROOT));
        ClientChannel *       newChannel = new ClientChannel;
        
        if (newChannel)
        {
#if defined(MpM_ReportOnConnections)
            newChannel->setReporter(ChannelStatusReporter::gReporter);
#endif // defined(MpM_ReportOnConnections)
            if (newChannel->openWithRetries(aName, timeToWait))
            {
                if (NetworkConnectWithRetries(aName, MpM_REGISTRY_CHANNEL_NAME, timeToWait, false))
                {
                    Package parameters;
                    
                    parameters.addString(portName);
                    ServiceRequest  request(MpM_GETASSOCIATES_REQUEST, parameters);
                    ServiceResponse response;
                    
                    if (request.send(*newChannel, &response))
                    {
                        OD_LOG_S1("response <- ", response.asString().c_str());//####
                        result = processGetAssociatesResponse(response.values(), associates);
                    }
                    else
                    {
                        OD_LOG("! (request.send(*newChannel, &response))");//####
                    }
#if defined(MpM_DoExplicitDisconnect)
                    if (! NetworkDisconnectWithRetries(aName, MpM_REGISTRY_CHANNEL_NAME, timeToWait))
                    {
                        OD_LOG("(! NetworkDisconnectWithRetries(aName, MpM_REGISTRY_CHANNEL_NAME, timeToWait))");//####
                    }
#endif // defined(MpM_DoExplicitDisconnect)
                }
                else
                {
                    OD_LOG("! (NetworkConnectWithRetries(aName, MpM_REGISTRY_CHANNEL_NAME, timeToWait, false))");//####
                }
#if defined(MpM_DoExplicitClose)
                newChannel->close();
#endif // defined(MpM_DoExplicitClose)
            }
            else
            {
                OD_LOG("! (newChannel->openWithRetries(aName, timeToWait))");//####
            }
            ClientChannel::RelinquishChannel(newChannel);
            newChannel = NULL;
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught");//####
        throw;
    }
    OD_LOG_EXIT_B(result);//####
    return result;
} // MplusM::Utilities::GetAssociatedPorts

void MplusM::Utilities::GetDetectedPortList(PortVector & ports)
{
    OD_LOG_ENTER();//####
    OD_LOG_P1("ports = ", &ports);//####
    Package                request;
    Package                response;
    yarp::os::ContactStyle contactInfo;
    
    ports.clear();
    request.addString("list");
    contactInfo.timeout = 5.0;
    if (yarp::os::Network::writeToNameServer(request, response, contactInfo))
    {
        if (1 == response.size())
        {
            yarp::os::Value responseValue(response.get(0));
            
            if (responseValue.isString())
            {
                processNameServerResponse(responseValue.asString(), ports);
            }
            else
            {
                OD_LOG("! (responseValue.isString())");//####
            }
        }
        else
        {
            OD_LOG("! (1 == response.size())");//####
            OD_LOG_S1("response = ", response.toString().c_str());//####
        }
    }
    else
    {
        OD_LOG("! (yarp::os::Network::writeToNameServer(request, response))");//####
    }
    OD_LOG_EXIT();//####
} // MplusM::Utilities::GetDetectedPortList

bool MplusM::Utilities::GetNameAndDescriptionForService(const yarp::os::ConstString & serviceChannelName,
                                                        ServiceDescriptor &           descriptor,
                                                        const double                  timeToWait)
{
    OD_LOG_ENTER();//####
    OD_LOG_S1("serviceChannelName = ", serviceChannelName.c_str());//####
    OD_LOG_P1("descriptor = ", &descriptor);//####
    OD_LOG_D1("timeToWait = ", timeToWait);//####
    bool                  result = false;
    yarp::os::ConstString aName(GetRandomChannelName("_servicelister_/" DEFAULT_CHANNEL_ROOT));
    ClientChannel *       newChannel = new ClientChannel;
    
    if (newChannel)
    {
        if (newChannel->openWithRetries(aName, timeToWait))
        {
            if (NetworkConnectWithRetries(aName, serviceChannelName, timeToWait, false))
            {
                Package         parameters1;
                ServiceRequest  request1(MpM_NAME_REQUEST, parameters1);
                ServiceResponse response1;
                
                if (request1.send(*newChannel, &response1))
                {
                    OD_LOG_S1("response1 <- ", response1.asString().c_str());//####
                    if (MpM_EXPECTED_NAME_RESPONSE_SIZE == response1.count())
                    {
                        yarp::os::Value theCanonicalName(response1.element(0));
                        yarp::os::Value theDescription(response1.element(1));
                        yarp::os::Value theKind(response1.element(2));
                        yarp::os::Value thePath(response1.element(3));
                        yarp::os::Value theRequestsDescription(response1.element(4));
                        
                        OD_LOG_S4("theCanonicalName <- ", theCanonicalName.toString().c_str(),//####
                                  "theDescription <- ", theDescription.toString().c_str(), "theKind <- ",//####
                                  theKind.toString().c_str(), "thePath <- ", thePath.toString().c_str());
                        OD_LOG_S1("theRequestsDescription = ", theRequestsDescription.toString().c_str());//####
                        if (theCanonicalName.isString() && theDescription.isString() && theKind.isString() &&
                            thePath.isString() && theRequestsDescription.isString())
                        {
                            descriptor._channelName = serviceChannelName;
                            descriptor._canonicalName = theCanonicalName.toString();
                            descriptor._description = theDescription.toString();
                            descriptor._kind = theKind.toString();
                            descriptor._path = thePath.toString();
                            descriptor._requestsDescription = theRequestsDescription.toString();
                            result = true;
                        }
                        else
                        {
                            OD_LOG("! (theCanonicalName.isString() && theDescription.isString() && "//####
                                   "theKind.isString() && thePath.isString() && "//####
                                   "theRequestsDescription.isString())");//####
                        }
                    }
                    else
                    {
                        OD_LOG("! (MpM_EXPECTED_NAME_RESPONSE_SIZE == response1.count())");//####
                        OD_LOG_S1("response1 = ", response1.asString().c_str());//####
                    }
                }
                else
                {
                    OD_LOG("! (request1.send(*newChannel, &response1))");//####
                }
                if (result)
                {
                    Package         parameters2;
                    ServiceRequest  request2(MpM_CHANNELS_REQUEST, parameters2);
                    ServiceResponse response2;
                    
                    if (request2.send(*newChannel, &response2))
                    {
                        if (MpM_EXPECTED_CHANNELS_RESPONSE_SIZE == response2.count())
                        {
                            yarp::os::Value theInputChannels(response2.element(0));
                            yarp::os::Value theOutputChannels(response2.element(1));
                        
                            OD_LOG_S2("theInputChannels <- ", theInputChannels.toString().c_str(),//####
                                      "theOutputChannels <- ", theOutputChannels.toString().c_str());//####
                            if (theInputChannels.isList() && theOutputChannels.isList())
                            {
                                Package * inputChannelsAsList = theInputChannels.asList();
                                Package * outputChannelsAsList = theOutputChannels.asList();
                                
                                for (int ii = 0, howMany = inputChannelsAsList->size(); ii < howMany; ++ii)
                                {
                                    yarp::os::Value element(inputChannelsAsList->get(ii));
                                    
                                    if (element.isList())
                                    {
                                        Package * inputChannelAsList = element.asList();
                                        
                                        if (MpM_EXPECTED_CHANNEL_DESCRIPTOR_SIZE == inputChannelAsList->size())
                                        {
                                            yarp::os::Value firstValue(inputChannelAsList->get(0));
                                            yarp::os::Value secondValue(inputChannelAsList->get(1));
                                            
                                            if (firstValue.isString() && secondValue.isString())
                                            {
                                                ChannelDescription aChannel;

                                                aChannel._portName = firstValue.asString();
                                                aChannel._portProtocol = secondValue.asString();
                                                aChannel._portMode = kChannelModeOther;
                                                descriptor._inputChannels.push_back(aChannel);
                                            }
                                        }
                                    }
                                }
                                for (int ii = 0, howMany = outputChannelsAsList->size(); ii < howMany; ++ii)
                                {
                                    yarp::os::Value element(outputChannelsAsList->get(ii));
                                    
                                    if (element.isList())
                                    {
                                        Package * outputChannelAsList = element.asList();
                                        
                                        if (MpM_EXPECTED_CHANNEL_DESCRIPTOR_SIZE == outputChannelAsList->size())
                                        {
                                            yarp::os::Value firstValue(outputChannelAsList->get(0));
                                            yarp::os::Value secondValue(outputChannelAsList->get(1));
                                            
                                            if (firstValue.isString() && secondValue.isString())
                                            {
                                                ChannelDescription aChannel;
                                                
                                                aChannel._portName = firstValue.asString();
                                                aChannel._portProtocol = secondValue.asString();
                                                aChannel._portMode = kChannelModeOther;
                                                descriptor._outputChannels.push_back(aChannel);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                OD_LOG("! (theInputChannels.isList() && theOutputChannels.isList())");
                            }
                        }
                        else
                        {
                            OD_LOG("! (MpM_EXPECTED_CHANNELS_RESPONSE_SIZE == response2.count())");//####
                            OD_LOG_S1("response2 = ", response2.asString().c_str());//####
                        }
                    }
                    else
                    {
                        OD_LOG("! (request2.send(*newChannel, &response2))");//####
                        result = false;
                    }
                }
#if defined(MpM_DoExplicitDisconnect)
                if (! NetworkDisconnectWithRetries(aName, serviceChannelName, timeToWait))
                {
                    OD_LOG("(! NetworkDisconnectWithRetries(aName, destinationName, timeToWait))");//####
                }
#endif // defined(MpM_DoExplicitDisconnect)
            }
            else
            {
                OD_LOG("! (NetworkConnectWithRetries(aName, serviceChannelName, timetoWait, false))");//####
            }
#if defined(MpM_DoExplicitClose)
            newChannel->close();
#endif // defined(MpM_DoExplicitClose)
        }
        else
        {
            OD_LOG("! (newChannel->openWithRetries(aName, timeToWait))");//####
        }
        delete newChannel;
    }
    else
    {
        OD_LOG("! (newChannel)");//####
    }
    OD_LOG_EXIT_B(result);//####
    return result;
} // MplusM::Utilities::GetNameAndDescriptionForService

MplusM::Utilities::PortKind MplusM::Utilities::GetPortKind(const yarp::os::ConstString & portName)
{
    const char * portNameChars = portName.c_str();
    const size_t kAdapterPortNameBaseLen = sizeof(ADAPTER_PORT_NAME_BASE) - 1;
    const size_t kClientPortNameBaseLen = sizeof(CLIENT_PORT_NAME_BASE) - 1;
    const size_t kDefaultServiceNameBaseLen = sizeof(DEFAULT_SERVICE_NAME_BASE) - 1;
    PortKind     result;
    
    if (! strcmp(MpM_REGISTRY_CHANNEL_NAME, portNameChars))
    {
        result = kPortKindServiceRegistry;
    }
    else if (! strncmp(DEFAULT_SERVICE_NAME_BASE, portNameChars, kDefaultServiceNameBaseLen))
    {
        result = kPortKindService;
    }
    else if (! strncmp(ADAPTER_PORT_NAME_BASE, portNameChars, kAdapterPortNameBaseLen))
    {
        result = kPortKindAdapter;
    }
    else if (! strncmp(CLIENT_PORT_NAME_BASE, portNameChars, kClientPortNameBaseLen))
    {
        result = kPortKindClient;
    }
    else
    {
        result = kPortKindStandard;
    }
    return result;
} // MplusM::Utilities::GetPortKind

void MplusM::Utilities::GetServiceNames(StringVector & services,
                                        const bool     quiet)
{
    OD_LOG_ENTER();//####
    OD_LOG_P1("services = ", &services);//####
    OD_LOG_B1("quiet = ", quiet);//####
    Package matches(FindMatchingServices(MpM_REQREP_DICT_REQUEST_KEY ":*"));
    
    services.clear();
    if (MpM_EXPECTED_MATCH_RESPONSE_SIZE == matches.size())
    {
        // First, check if the search succeeded.
        yarp::os::ConstString matchesFirstString(matches.get(0).toString());
        
        if (strcmp(MpM_OK_RESPONSE, matchesFirstString.c_str()))
        {
            OD_LOG("(strcmp(MpM_OK_RESPONSE, matchesFirstString.c_str()))");//####
            if (! quiet)
            {
                yarp::os::ConstString reason(matches.get(1).toString());
                
                cerr << "Failed: " << reason.c_str() << "." << endl;
            }
        }
        else
        {
            // Now, process the second element.
            Package * matchesList = matches.get(1).asList();
            
            if (matchesList)
            {
                for (int ii = 0, matchesCount = matchesList->size(); ii < matchesCount; ++ii)
                {
                    services.push_back(matchesList->get(ii).toString());
                }
            }
        }
    }
    else
    {
        OD_LOG("! (MpM_EXPECTED_MATCH_RESPONSE_SIZE == matches.size())");//####
        if (! quiet)
        {
            cerr << "Problem getting information from the Service Registry." << endl;            
        }
    }
    OD_LOG_EXIT();//####
} // MplusM::Utilities::GetServiceNames

const char * MplusM::Utilities::MapServiceKindToString(const Common::ServiceKind kind)
{
    OD_LOG_ENTER();//####
    OD_LOG_L1("kind = ", static_cast<int>(kind));//####
    const char * result;
    
    switch (kind)
    {
        case kServiceKindFilter:
            result = "Filter";
            break;
            
        case kServiceKindInput:
            result = "Input";
            break;
            
        case kServiceKindOutput:
            result = "Output";
            break;
            
        case kServiceKindRegistry:
            result = "Registry";
            break;
            
        default:
            result = "Normal";
            break;
            
    }
    OD_LOG_EXIT_S(result);//####
    return result;
} // MplusM::Utilities::MapServiceKindToString

Common::ServiceKind MplusM::Utilities::MapStringToServiceKind(const yarp::os::ConstString & kindString)
{
    OD_LOG_ENTER();//####
    OD_LOG_S1("kindString = ", kindString.c_str());//####
    Common::ServiceKind result;
    const char *        kindStringChars = kindString.c_str();
    
    if (! strcmp("Filter", kindStringChars))
    {
        result = kServiceKindFilter;
    }
    else if (! strcmp("Input", kindStringChars))
    {
        result = kServiceKindInput;
    }
    else if (! strcmp("Output", kindStringChars))
    {
        result = kServiceKindOutput;
    }
    else if (! strcmp("Registry", kindStringChars))
    {
        result = kServiceKindRegistry;
    }
    else
    {
        result = kServiceKindNormal;
    }
    OD_LOG_EXIT_L(static_cast<int>(result));//####
    return result;
} // MplusM::Utilities::MapStringToServiceKind

bool MplusM::Utilities::RemoveConnection(const yarp::os::ConstString & fromPortName,
                                         const yarp::os::ConstString & toPortName)
{
    OD_LOG_ENTER();//####
    OD_LOG_S2("fromPortName = ", fromPortName.c_str(), "toPortName = ", toPortName.c_str());//####
    bool result = NetworkDisconnectWithRetries(fromPortName, toPortName, STANDARD_WAIT_TIME);

    OD_LOG_EXIT_B(result);//####
    return result;
} // MplusM::Utilities::RemoveConnection
