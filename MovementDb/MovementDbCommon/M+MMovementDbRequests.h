//--------------------------------------------------------------------------------------------------
//
//  File:       M+MMovementDbRequests.h
//
//  Project:    M+M
//
//  Contains:   The common macro definitions for requests and responses for the movement database
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

#if (! defined(MpMMovementDbRequests_H_))
# define MpMMovementDbRequests_H_ /* Header guard */

# include <mpm/M+MRequests.h>

# if defined(__APPLE__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
# endif // defined(__APPLE__)
/*! @file
 @brief The common macro definitions for requests and responses for the movement database
 service. */

/*! @namespace MplusM::MovementDb
 @brief The classes that support communicating with the movement database. */
# if defined(__APPLE__)
#  pragma clang diagnostic pop
# endif // defined(__APPLE__)

/*! @brief The channel-independent name of the movement database service. */
# define MpM_MOVEMENTDB_CANONICAL_NAME "MovementDb"

/*! @brief The name for the 'addfile' request. */
# define MpM_ADDFILE_REQUEST  "addfile"

/*! @brief The name for the 'setemail' request. */
# define MpM_SETDATATRACK_REQUEST  "setdatatrack"

/*! @brief The name for the 'setemail' request. */
# define MpM_SETEMAIL_REQUEST  "setemail"

/*! @brief The name for the 'stopdb' request. */
# define MpM_STOPDB_REQUEST  "stopdb"

#endif // ! defined(MpMMovementDbRequests_H_)
