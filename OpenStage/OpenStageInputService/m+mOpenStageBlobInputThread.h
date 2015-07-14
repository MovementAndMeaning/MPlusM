//--------------------------------------------------------------------------------------------------
//
//  File:       m+mOpenStageBlobInputThread.h
//
//  Project:    m+m
//
//  Contains:   The class declaration for a thread that generates output from Organic Motion
//				OpenStage data.
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
//  Created:    2015-07-14
//
//--------------------------------------------------------------------------------------------------

#if (! defined(MpMOpenStageBlobInputThread_H_))
# define MpMOpenStageBlobInputThread_H_ /* Header guard */

# include <m+m/m+mGeneralChannel.h>

# include <om/sdk2/client.h>
# include <om/sdk2/types.h>
# include <om/sdk2/actor_stream.h>
# include <om/sdk2/actor_view.h>
# include <om/sdk2/error.h>
# include <om/sdk2/tree.h>
# include "om/sdk2/util.h"

# if defined(__APPLE__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunknown-pragmas"
#  pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
# endif // defined(__APPLE__)
/*! @file
 @brief The class declaration for a thread that generates output from Organic Motion %OpenStage
 data. */
# if defined(__APPLE__)
#  pragma clang diagnostic pop
# endif // defined(__APPLE__)

namespace MplusM
{
	namespace OpenStageBlob
    {
		/*! @brief A class to generate output from Organic Motion %OpenStage data. */
		class OpenStageBlobInputThread : public yarp::os::Thread
        {
        public :
            
            /*! @brief The constructor.
             @param outChannel The channel to send data bursts to.
			 @param name The host name to connect to the Organic Motion %OpenStage  server.
			 @param port The host port to connect to the Organic Motion %OpenStage server. */
            OpenStageBlobInputThread(Common::GeneralChannel * outChannel,
                                     const YarpString &       name,
                                     const int                port);
            
            /*! @brief The destructor. */
			virtual ~OpenStageBlobInputThread(void);
            
            /*! @brief Stop using the output channel. */
            void clearOutputChannel(void);
            
            /*! @brief Set the translation scale.
             @param newScale The scale factor for translation values. */
            void setScale(const double newScale);
            
        protected :
            
        private :
            
			/*! @brief Process the received data.
			 @param actorData The data to be processed. */
			void processData(om::sdk2::ActorDataListConstPtr & actorData);

            /*! @brief The thread main body. */
            virtual void run(void);
            
            /*! @brief The thread initialization method.
             @returns @c true if the thread is ready to run. */
            virtual bool threadInit(void);
            
            /*! @brief The thread termination method. */
            virtual void threadRelease(void);
            
			COPY_AND_ASSIGNMENT_(OpenStageBlobInputThread);
            
        public :
        
        protected :
        
        private :
            
            /*! @brief The class that this class is derived from. */
            typedef yarp::os::Thread inherited;
            
            /*! @brief The translation scale to be used. */
            double _scale;
                        
			/*! @brief The address of the Organic Motion %OpenStage device. */
			YarpString _address;

			/*! @brief The port of the Organic Motion %OpenStage device. */
			int _port;

            /*! @brief The channel to send data bursts to. */
            Common::GeneralChannel * _outChannel;
            
			/*! @brief The connection to the device. */
			om::sdk2::ClientPtr _client;

			/*! @brief The stream of actor motion data. */
			om::sdk2::ActorStreamPtr _actorStream;

			/*! @brief A view into the actor motion data. */
			om::sdk2::ActorViewJointPtr _actorViewJoint;

# if defined(__APPLE__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-private-field"
# endif // defined(__APPLE__)
            /*! @brief Filler to pad to alignment boundary */
            char _filler[4];
# if defined(__APPLE__)
#  pragma clang diagnostic pop
# endif // defined(__APPLE__)
            
        }; // OpenStageBlobInputThread
        
    } // OpenStageBlob
    
} // MplusM

#endif // ! defined(MpMOpenStageBlobInputThread_H_)