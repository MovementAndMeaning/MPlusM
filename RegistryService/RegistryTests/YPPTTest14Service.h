//--------------------------------------------------------------------------------------
//
//  File:       YPPTTest14Service.h
//
//  Project:    YarpPlusPlus
//
//  Contains:   The class declaration for a simple service used by the unit tests.
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
//  Created:    2014-03-05
//
//--------------------------------------------------------------------------------------

#if (! defined(YPPTTEST14SERVICE_H_))
# define YPPTTEST14SERVICE_H_ /* */

# include "YPPBaseService.h"

namespace YarpPlusPlusTest
{
    /*! @brief A test service. */
    class Test14Service : public YarpPlusPlus::BaseService
    {
    public:
        
        /*! @brief The constructor.
         @param argc The number of arguments in 'argv'.
         @param argv The arguments to be used to specify the new service. */
        Test14Service(const int argc,
                      char **   argv);
        
        /*! @brief The destructor. */
        virtual ~Test14Service(void);
        
    protected:
        
    private:
        
        /*! @brief The class that this class is derived from. */
        typedef BaseService inherited;
        
    }; // Test14Service
    
} // YarpPlusPlusTest

#endif // ! defined(YPPTTEST14SERVICE_H_)
