//--------------------------------------------------------------------------------------------------
//
//  File:       M+MMovementDbService.cpp
//
//  Project:    M+M
//
//  Contains:   The class definition for a service that collects statistic on requests.
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
//  Created:    2014-09-02
//
//--------------------------------------------------------------------------------------------------

#include "M+MMovementDbService.h"
#include "M+MAddFileRequestHandler.h"
#include "M+MMovementDbContext.h"
#include "M+MMovementDbRequests.h"
#include "M+MSetDataTrackRequestHandler.h"
#include "M+MSetEmailRequestHandler.h"
#include "M+MStopDbRequestHandler.h"

//#include <odl/ODEnableLogging.h>
#include <odl/ODLogging.h>

#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif // defined(__APPLE__)
/*! @file
 @brief The class definition for a service that collects statistic on requests. */
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)

using namespace MplusM;
using namespace MplusM::Common;
using namespace MplusM::MovementDb;

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

MovementDbService::MovementDbService(const yarp::os::ConstString & launchPath,
                                     const yarp::os::ConstString & tag,
                                     const yarp::os::ConstString & databaseServerAddress,
                                     const yarp::os::ConstString & serviceEndpointName,
                                     const yarp::os::ConstString & servicePortNumber) :
    inherited(kServiceKindNormal, launchPath, tag, true, MpM_MOVEMENTDB_CANONICAL_NAME,
              "The Movement database service",
              "reset - clear the request counter and the elapsed time" , serviceEndpointName,
              servicePortNumber), _databaseAddress(databaseServerAddress), _addFileHandler(NULL),
    _setDataTrackHandler(NULL), _setEmailHandler(NULL), _stopDbHandler(NULL)
{
    OD_LOG_ENTER(); //####
    OD_LOG_S4s("launchPath = ", launchPath, "tag = ", tag, "databaseServerAddress = ", //####
               databaseServerAddress, "serviceEndpointName = ", serviceEndpointName); //####
    OD_LOG_S1s("servicePortNumber = ", servicePortNumber); //####
    attachRequestHandlers();
    OD_LOG_EXIT_P(this); //####
} // MovementDbService::MovementDbService

MovementDbService::~MovementDbService(void)
{
    OD_LOG_OBJENTER(); //####
    detachRequestHandlers();
    OD_LOG_OBJEXIT(); //####
} // MovementDbService::~MovementDbService

#if defined(__APPLE__)
# pragma mark Actions and Accessors
#endif // defined(__APPLE__)

bool MovementDbService::addFileToDb(const yarp::os::ConstString & key,
                                    const yarp::os::ConstString & filePath)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_S2s("key = ", key, "filePath = ", filePath); //####
    bool okSoFar = false;
    
    try
    {
        MovementDbContext * context = (MovementDbContext *) findContext(key);
        
        if (! context)
        {
            context = new MovementDbContext;
            addContext(key, context);
        }
        // Add file using context->dataTrack(), context->emailAddress() and filePath.
        // TBD!!!!
        okSoFar = true;
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(); //####
    return okSoFar;
} // MovementDbService::addFileToDb

void MovementDbService::attachRequestHandlers(void)
{
    OD_LOG_OBJENTER(); //####
    try
    {
        _addFileHandler = new AddFileRequestHandler(*this);
        _setDataTrackHandler = new SetDataTrackRequestHandler(*this);
        _setEmailHandler = new SetEmailRequestHandler(*this);
        _stopDbHandler = new StopDbRequestHandler(*this);
        if (_addFileHandler && _setDataTrackHandler && _setEmailHandler && _stopDbHandler)
        {
            registerRequestHandler(_addFileHandler);
            registerRequestHandler(_setDataTrackHandler);
            registerRequestHandler(_setEmailHandler);
            registerRequestHandler(_stopDbHandler);
        }
        else
        {
            OD_LOG("! (_addFileHandler && _setDataTrackHandler && _setEmailHandler && " //####
                   " _stopDbHandler)"); //####
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT(); //####
} // MovementDbService::attachRequestHandlers

void MovementDbService::detachRequestHandlers(void)
{
    OD_LOG_OBJENTER(); //####
    try
    {
        if (_addFileHandler)
        {
            unregisterRequestHandler(_addFileHandler);
            delete _addFileHandler;
            _addFileHandler = NULL;
        }
        if (_setDataTrackHandler)
        {
            unregisterRequestHandler(_setDataTrackHandler);
            delete _setDataTrackHandler;
            _setDataTrackHandler = NULL;
        }
        if (_setEmailHandler)
        {
            unregisterRequestHandler(_setEmailHandler);
            delete _setEmailHandler;
            _setEmailHandler = NULL;
        }
        if (_stopDbHandler)
        {
            unregisterRequestHandler(_stopDbHandler);
            delete _stopDbHandler;
            _stopDbHandler = NULL;
        }
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT(); //####
} // MovementDbService::detachRequestHandlers

bool MovementDbService::setDataTrack(const yarp::os::ConstString & key,
                                     const yarp::os::ConstString & dataTrack)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_S2s("key = ", key, "dataTrack = ", dataTrack); //####
    bool okSoFar = false;
    
    try
    {
        MovementDbContext * context = (MovementDbContext *) findContext(key);
        
        if (! context)
        {
            context = new MovementDbContext;
            addContext(key, context);
        }
        context->dataTrack() = dataTrack;
        okSoFar = true;
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(); //####
    return okSoFar;
} // MovementDbService::setDataTrack

bool MovementDbService::setEmailAddress(const yarp::os::ConstString & key,
                                        const yarp::os::ConstString & emailAddress)
{
    OD_LOG_OBJENTER(); //####
    OD_LOG_S2s("key = ", key, "emailAddress = ", emailAddress); //####
    bool okSoFar = false;
    
    try
    {
        MovementDbContext * context = (MovementDbContext *) findContext(key);
        
        if (! context)
        {
            context = new MovementDbContext;
            addContext(key, context);
        }
        context->emailAddress() = emailAddress;
        okSoFar = true;
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(); //####
    return okSoFar;
} // MovementDbService::setEmailAddress

bool MovementDbService::start(void)
{
    OD_LOG_OBJENTER(); //####
    bool result = false;
    
    try
    {
        if (! isStarted())
        {
            inherited::start();
            if (isStarted())
            {
                
            }
            else
            {
                OD_LOG("! (isStarted())"); //####
            }
        }
        result = isStarted();
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(result); //####
    return result;
} // MovementDbService::start

bool MovementDbService::stop(void)
{
    OD_LOG_OBJENTER(); //####
    bool result = false;
    
    try
    {
        result = inherited::stop();
    }
    catch (...)
    {
        OD_LOG("Exception caught"); //####
        throw;
    }
    OD_LOG_OBJEXIT_B(result); //####
    return result;
} // MovementDbService::stop

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)
