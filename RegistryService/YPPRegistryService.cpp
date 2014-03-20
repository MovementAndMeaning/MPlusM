//--------------------------------------------------------------------------------------
//
//  File:       YPPRegistryService.cpp
//
//  Project:    YarpPlusPlus
//
//  Contains:   The class definition for the Service Registry Yarp++ service.
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
//  Created:    2014-02-06
//
//--------------------------------------------------------------------------------------

#include "YPPRegistryService.h"
//#define ENABLE_OD_SYSLOG /* */
#include "ODSyslog.h"
#include "YPPMatchRequestHandler.h"
#include "YPPRegisterRequestHandler.h"
#include "YPPRequests.h"
#include "YPPServiceRequest.h"
#include "YPPUnregisterRequestHandler.h"
#if defined(__APPLE__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc++11-long-long"
#endif // defined(__APPLE__)
#include <sqlite3.h>
#if defined(__APPLE__)
# pragma clang diagnostic pop
#endif // defined(__APPLE__)
#include <yarp/os/Time.h>

using namespace YarpPlusPlus;

#if defined(__APPLE__)
# pragma mark Private structures, constants and variables
#endif // defined(__APPLE__)

#define USE_TEST_DATABASE /* */

/*! @brief The name of the 'keywords' table. */
#define KEYWORDS_T_         "Keywords"
/*! @brief The name of the 'requests' table. */
#define REQUESTS_T_         "Requests"
/*! @brief The name of the 'requests-keywords' table. */
#define REQUESTSKEYWORDS_T_ "RequestsKeywords"
/*! @brief The name of the 'services' table. */
#define SERVICES_T_         "Services"

/*! @brief The name of the index for the 'portName' column of the 'requests' table. */
#define REQUESTS_PORTNAME_I_            "Requests_portname_idx"
/*! @brief The name of the index for the 'requests' column of the 'requests' table. */
#define REQUESTS_REQUEST_I_             "Requests_request_idx"
/*! @brief The name of the index for the 'keywords_id' column of the 'requests-keywords' table. */
#define REQUESTSKEYWORDS_KEYWORDS_ID_I_ "RequestsKeywords_Keywords_id_idx"
/*! @brief The name of the index for the 'requests_id' column of the 'requests-keywords' table. */
#define REQUESTSKEYWORDS_REQUESTS_ID_I_ "RequestsKeywords_Requests_id_idx"
/*! @brief The name of the index for the 'name' column of the 'services' table. */
#define SERVICES_NAME_I_                "Services_name_idx"

/*! @brief The named parameter for the 'details' column. */
#define DESCRIPTION_C_ "description"
/*! @brief The named parameter for the 'details' column. */
#define DETAILS_C_     "details"
/*! @brief The named parameter for the 'input' column. */
#define INPUT_C_       "input"
/*! @brief The named parameter for the 'key' column. */
#define KEY_C_         "key"
/*! @brief The named parameter for the 'keyword' column. */
#define KEYWORD_C_     "keyword"
/*! @brief The named parameter for the 'Keywords_id' column. */
#define KEYWORDS_ID_C_ "keywords_id"
/*! @brief The named parameter for the 'name' column. */
#define NAME_C_        "name"
/*! @brief The named parameter for the 'output' column. */
#define OUTPUT_C_      "output"
/*! @brief The named parameter for the 'portName' column. */
#define PORTNAME_C_    "portname"
/*! @brief The named parameter for the 'request' column. */
#define REQUEST_C_     "request"
/*! @brief The named parameter for the 'Keywords_id' column. */
#define REQUESTS_ID_C_ "requests_id"
/*! @brief The named parameter for the 'version' column. */
#define VERSION_C_     "version"

/*! @brief The command to initiate an SQL transaction. */
static const char * kBeginTransaction = "BEGIN TRANSACTION";
/*! @brief The command to successfully complete an SQL transaction. */
static const char * kEndTransaction = "END TRANSACTION";

/*! @brief The operation timeout to use with YARP. */
static const float kRegistryServiceTimeout = 5.0;

namespace YarpPlusPlus
{
    /*! @brief A function that provides bindings for parameters in an SQL statement.
     @param statement The prepared statement that is to be updated.
     @param stuff The source of data that is to be bound.
     @returns The SQLite error from the bind operation. */
    typedef int (*BindFunction)
        (sqlite3_stmt * statement,
         const void *   stuff);
    
    /*! @brief The data needed to add a request-keyword entry into the database. */
    struct RequestKeywordData
    {
        /*! @brief The name of the request. */
        yarp::os::ConstString _request;
        /*! @brief The service port for the request. */
        yarp::os::ConstString _port;
        /*! @brief A keyword for the request. */
        yarp::os::ConstString _key;
    }; // RequestKeywordData
    
    /*! @brief The data needed to add a service entry into the database. */
    struct ServiceData
    {
        /*! @brief The service port for the service. */
        yarp::os::ConstString _port;
        /*! @brief The name for the service. */
        yarp::os::ConstString _name;
        /*! @brief The description of the service. */
        yarp::os::ConstString _description;
    }; // ServiceData
    
} // YarpPlusPlus

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

/*! @brief The prefix to be used when generating SQL for a 'description' request. */
#define DESCRIPTION_PREFIX_ PORTNAME_C_ " IN (SELECT DISTINCT " PORTNAME_C_ " FROM " SERVICES_T_ " WHERE "
/*! @brief The suffix to be used when generating SQL for a 'description' request. */
#define DESCRIPTION_SUFFIX_ ")"
/*! @brief The prefix to be used when generating SQL for a 'keyword' request. */
#define KEYWORD_PREFIX_     "KEY IN (SELECT DISTINCT " REQUESTS_ID_C_ " FROM " REQUESTSKEYWORDS_T_ " WHERE "
/*! @brief The suffix to be used when generating SQL for a 'keyword' request. */
#define KEYWORD_SUFFIX_     ")"
/*! @brief The prefix to be used when generating SQL for a 'name' request. */
#define NAME_PREFIX_        PORTNAME_C_ " IN (SELECT DISTINCT " PORTNAME_C_ " FROM " SERVICES_T_ " WHERE "
/*! @brief The suffix to be used when generating SQL for a 'name' request. */
#define NAME_SUFFIX_        ")"

/*! @brief the valid field names that may be used. Note that the strings are all lower-case for comparison purposes. */
static const char * kColumnNames[] =
{
    // Name to match   Name to use     Prefix to be used    Suffix to be used
    DESCRIPTION_C_, DESCRIPTION_C_, DESCRIPTION_PREFIX_, DESCRIPTION_SUFFIX_,
    DETAILS_C_,     DETAILS_C_,     NULL,                NULL,
    INPUT_C_,       INPUT_C_,       NULL,                NULL,
    KEYWORD_C_,     KEYWORDS_ID_C_, KEYWORD_PREFIX_,     KEYWORD_SUFFIX_,
    NAME_C_,        NAME_C_,        NAME_PREFIX_,        NAME_SUFFIX_,
    OUTPUT_C_,      OUTPUT_C_,      NULL,                NULL,
    PORTNAME_C_,    PORTNAME_C_,    NULL,                NULL,
    REQUEST_C_,     REQUEST_C_,     NULL,                NULL,
    VERSION_C_,     VERSION_C_,     NULL,                NULL
}; // kColumnNames

/*! @brief The number of valid field names. */
static const size_t kColumnNamesCount = (sizeof(kColumnNames) / sizeof(*kColumnNames));

#if defined(__APPLE__)
# pragma mark Local functions
#endif // defined(__APPLE__)

/*! @brief Check a candidate field name against the list of legal field names.
 @param aString The string to be checked.
 @param prefixString If non-@c NULL, a pointer to the string to be used in the SQL prefix for this field.
 @param suffixString If non-@c NULL, a pointer to the string to be used in the SQL suffix for this field.
 @returns The actual field name to be used or @c NULL if the field name was unmatched. */
static const char * columnNameValidator(const char *  aString,
                                        const char ** prefixString,
                                        const char ** suffixString)
{
    const char * result = NULL;
    
    try
    {
        const char * resultPrefix = NULL;
        const char * resultSuffix = NULL;

        for (size_t ii = 0; ii < kColumnNamesCount; ii += 4)
        {
            if (! strcmp(aString, kColumnNames[ii]))
            {
                result = kColumnNames[ii + 1];
                resultPrefix = kColumnNames[ii + 2];
                resultSuffix = kColumnNames[ii + 3];
                break;
            }
            
        }
        if (prefixString)
        {
            *prefixString = resultPrefix;
        }
        if (suffixString)
        {
            *suffixString = resultSuffix;
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    return result;
} // columnNameValidator

/*! @brief Perform a simple operation on the database.
 @param database The database to be modified.
 @param sqlStatement The operation to be performed.
 @param doBinds A function that will fill in any parameters in the statement.
 @param data The custom information used with the binding function.
 @returns @c true if the operation was successfully performed and @c false otherwise. */
static bool performSQLstatementWithNoResults(sqlite3 *    database,
                                             const char * sqlStatement,
                                             BindFunction doBinds = NULL,
                                             const void * data = NULL)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("database = ", database, "data = ", data);//####
    OD_SYSLOG_S1("sqlStatement = ", sqlStatement);//####
    bool okSoFar = true;
    
    try
    {
        if (database)
        {
            sqlite3_stmt * prepared = NULL;
            int            sqlRes = sqlite3_prepare_v2(database, sqlStatement, static_cast<int>(strlen(sqlStatement)),
                                                       &prepared, NULL);
            
            OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
            if ((SQLITE_OK == sqlRes) && prepared)
            {
                if (doBinds)
                {
                    sqlRes = doBinds(prepared, data);
                    OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
                    okSoFar = (SQLITE_OK == sqlRes);
                }
                if (okSoFar)
                {
                    sqlRes = sqlite3_step(prepared);
                    OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
                    while (SQLITE_BUSY == sqlRes)
                    {
                        yarp::os::Time::delay(1.0);
                        sqlRes = sqlite3_step(prepared);
                        OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
                    }
                    if (SQLITE_DONE != sqlRes)
                    {
                        OD_SYSLOG("(SQLITE_DONE != sqlRes)");//####
                        okSoFar = false;
                    }
                }
                sqlite3_finalize(prepared);
            }
            else
            {
                OD_SYSLOG("! ((SQLITE_OK == sqlRes) && prepared)");//####
                okSoFar = false;
            }
        }
        else
        {
            OD_SYSLOG("! (database)");//####
            okSoFar = false;
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // performSQLstatementWithNoResults

/*! @brief Perform an operation that can return multiple rows of results.
 @param database The database to be modified.
 @param resultList The list to be filled in with the values from the column of interest.
 @param sqlStatement The operation to be performed.
 @param columnOfInterest The column containing the value of interest.
 @param doBinds A function that will fill in any parameters in the statement.
 @param data The custom information used with the binding function.
 @returns @c true if the operation was successfully performed and @c false otherwise. */
static bool performSQLstatementWithResults(sqlite3 *          database,
                                           yarp::os::Bottle & resultList,
                                           const char *       sqlStatement,
                                           const int          columnOfInterest = 0,
                                           BindFunction       doBinds = NULL,
                                           const void *       data = NULL)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P3("database = ", database, "resultList = ", &resultList, "data = ", data);//####
    OD_SYSLOG_LL1("columnOfInterest = ", columnOfInterest);//####
    OD_SYSLOG_S1("sqlStatement = ", sqlStatement);//####
    bool okSoFar = true;
    
    try
    {
        if (database && (0 <= columnOfInterest))
        {
            sqlite3_stmt * prepared = NULL;
            int            sqlRes = sqlite3_prepare_v2(database, sqlStatement, static_cast<int>(strlen(sqlStatement)),
                                                       &prepared, NULL);
            
            OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
            if ((SQLITE_OK == sqlRes) && prepared)
            {
                if (doBinds)
                {
                    sqlRes = doBinds(prepared, data);
                    OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
                    okSoFar = (SQLITE_OK == sqlRes);
                }
                if (okSoFar)
                {
                    for (sqlRes = SQLITE_ROW; SQLITE_ROW == sqlRes; )
                    {
                        sqlRes = sqlite3_step(prepared);
                        OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
                        while (SQLITE_BUSY == sqlRes)
                        {
                            yarp::os::Time::delay(1.0);
                            sqlRes = sqlite3_step(prepared);
                            OD_SYSLOG_LL1("sqlRes <- ", sqlRes);//####
                        }
                        if (SQLITE_ROW == sqlRes)
                        {
                            // Gather the column data...
                            int colCount = sqlite3_column_count(prepared);
                            
                            OD_SYSLOG_LL1("colCount <- ", colCount);//####
                            if ((0 < colCount) && (columnOfInterest < colCount))
                            {
                                const char * value = reinterpret_cast<const char *>(sqlite3_column_text(prepared,
                                                                                                    columnOfInterest));
                                
                                OD_SYSLOG_S1("value <- ", value);//####
                                if (value)
                                {
                                    resultList.addString(value);
                                }
                            }
                        }
                    }
                    if (SQLITE_DONE != sqlRes)
                    {
                        OD_SYSLOG("(SQLITE_DONE != sqlRes)");//####
                        okSoFar = false;
                    }
                }
                sqlite3_finalize(prepared);
            }
            else
            {
                OD_SYSLOG("! ((SQLITE_OK == sqlRes) && prepared)");//####
                okSoFar = false;
            }
        }
        else
        {
            OD_SYSLOG("! (database && (0 <= columnOfInterest))");//####
            okSoFar = false;
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
   
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // performSQLstatementWithResults

/*! @brief Construct the tables needed in the database.
 @param database The database to be modified.
 @returns @c true if the tables were successfully built and @c false otherwise. */
static bool constructTables(sqlite3 * database)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P1("database = ", database);//####
    bool okSoFar = true;
    
    try
    {
        if (database)
        {
            static const char * tableSQL[] =
            {
#if defined(USE_TEST_DATABASE)
                "DROP INDEX IF EXISTS " REQUESTS_REQUEST_I_,
                "DROP INDEX IF EXISTS " REQUESTS_PORTNAME_I_,
                "DROP INDEX IF EXISTS " REQUESTSKEYWORDS_KEYWORDS_ID_I_,
                "DROP INDEX IF EXISTS " REQUESTSKEYWORDS_REQUESTS_ID_I_,
                "DROP INDEX IF EXISTS " SERVICES_NAME_I_,
                "DROP TABLE IF EXISTS " REQUESTSKEYWORDS_T_,
                "DROP TABLE IF EXISTS " REQUESTS_T_,
                "DROP TABLE IF EXISTS " KEYWORDS_T_,
                "DROP TABLE IF EXISTS " SERVICES_T_,
#endif // defined(USE_TEST_DATABASE)
                "CREATE TABLE IF NOT EXISTS " SERVICES_T_ "("
                " " PORTNAME_C_ "    Text NOT NULL DEFAULT _ PRIMARY KEY ON CONFLICT REPLACE,"
                " " NAME_C_ "        Text NOT NULL DEFAULT _,"
                " " DESCRIPTION_C_ " Text NOT NULL DEFAULT _)",
                "CREATE INDEX IF NOT EXISTS " SERVICES_NAME_I_ " ON " SERVICES_T_ "(" NAME_C_ ")",
                "CREATE TABLE IF NOT EXISTS " KEYWORDS_T_ "("
                " " KEYWORD_C_ " Text NOT NULL DEFAULT _ PRIMARY KEY ON CONFLICT IGNORE)",
                "CREATE TABLE IF NOT EXISTS " REQUESTS_T_ "("
                " " PORTNAME_C_ " Text NOT NULL DEFAULT _ REFERENCES " SERVICES_T_ "(" PORTNAME_C_ "),"
                " " REQUEST_C_ "  Text NOT NULL DEFAULT _,"
                " " INPUT_C_ "    Text,"
                " " OUTPUT_C_ "   Text,"
                " " VERSION_C_ "  Text,"
                " " DETAILS_C_ "  Text,"
                " " KEY_C_ "      Integer PRIMARY KEY)",
                "CREATE INDEX IF NOT EXISTS " REQUESTS_REQUEST_I_ " ON " REQUESTS_T_ "(" REQUEST_C_ ")",
                "CREATE INDEX IF NOT EXISTS " REQUESTS_PORTNAME_I_ " ON " REQUESTS_T_ "(" PORTNAME_C_ ")",
                "CREATE TABLE " REQUESTSKEYWORDS_T_ "("
                " " KEYWORDS_ID_C_ " Text REFERENCES " KEYWORDS_T_ "(" KEYWORD_C_ "),"
                " " REQUESTS_ID_C_ " Integer REFERENCES " REQUESTS_T_ "(" KEY_C_ "))",
                "CREATE INDEX IF NOT EXISTS " REQUESTSKEYWORDS_KEYWORDS_ID_I_ " ON " REQUESTSKEYWORDS_T_ "("
                KEYWORDS_ID_C_ ")",
                "CREATE INDEX IF NOT EXISTS " REQUESTSKEYWORDS_REQUESTS_ID_I_ " ON " REQUESTSKEYWORDS_T_ "("
                REQUESTS_ID_C_ ")"
            };
            int numTables = (sizeof(tableSQL) / sizeof(*tableSQL));
            
            okSoFar = performSQLstatementWithNoResults(database, kBeginTransaction);
            for (int ii = 0; okSoFar && (ii < numTables); ++ii)
            {
                okSoFar = performSQLstatementWithNoResults(database, tableSQL[ii]);
            }
            if (okSoFar)
            {
                okSoFar = performSQLstatementWithNoResults(database, kEndTransaction);
            }
        }
        else
        {
            OD_SYSLOG("! (database)");//####
            okSoFar = false;
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // constructTables

/*! @brief Bind the values that are to be inserted into the Keywords table.
 @param statement The prepared statement that is to be updated.
 @param stuff The source of data that is to be bound.
 @returns The SQLite error from the bind operation. */
static int setupInsertForKeywords(sqlite3_stmt * statement,
                                  const void *   stuff)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("statement = ", statement, "stuff = ", stuff);//####
    int result = SQLITE_MISUSE;
    
    try
    {
        int keywordIndex = sqlite3_bind_parameter_index(statement, "@" KEYWORD_C_);

        if (0 < keywordIndex)
        {
            const char * nameString = static_cast<const char *>(stuff);
            
            OD_SYSLOG_S1("nameString <- ", nameString);//####
            result = sqlite3_bind_text(statement, keywordIndex, nameString, static_cast<int>(strlen(nameString)),
                                       SQLITE_TRANSIENT);
            if (SQLITE_OK != result)
            {
                OD_SYSLOG_S1("error description: ", sqlite3_errstr(result));//####
            }
        }
        else
        {
            OD_SYSLOG("! (0 < keywordIndex)");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_LL(result);
    return result;
} // setupInsertForKeywords

/*! @brief Bind the values that are to be inserted into the Requests table.
 @param statement The prepared statement that is to be updated.
 @param stuff The source of data that is to be bound.
 @returns The SQLite error from the bind operation. */
static int setupInsertForRequests(sqlite3_stmt * statement,
                                  const void *   stuff)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("statement = ", statement, "stuff = ", stuff);//####
    int result = SQLITE_MISUSE;

    try
    {
        int detailsIndex = sqlite3_bind_parameter_index(statement, "@" DETAILS_C_);
        int inputIndex = sqlite3_bind_parameter_index(statement, "@" INPUT_C_);
        int outputIndex = sqlite3_bind_parameter_index(statement, "@" OUTPUT_C_);
        int portNameIndex = sqlite3_bind_parameter_index(statement, "@" PORTNAME_C_);
        int requestIndex = sqlite3_bind_parameter_index(statement, "@" REQUEST_C_);
        int versionIndex = sqlite3_bind_parameter_index(statement, "@" VERSION_C_);
        
        if ((0 < detailsIndex) && (0 < inputIndex) && (0 < outputIndex) && (0 < portNameIndex) && (0 < requestIndex) &&
            (0 < versionIndex))
        {
            const RequestDescription * descriptor = static_cast<const RequestDescription *>(stuff);
            const char *               portName = descriptor->_port.c_str();
            
            OD_SYSLOG_S1("portName <- ", portName);//####
            result = sqlite3_bind_text(statement, portNameIndex, portName, static_cast<int>(strlen(portName)),
                                       SQLITE_TRANSIENT);
            if (SQLITE_OK == result)
            {
                const char * request = descriptor->_request.c_str();
                
                OD_SYSLOG_S1("request <- ", request);//####
                result = sqlite3_bind_text(statement, requestIndex, request, static_cast<int>(strlen(request)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK == result)
            {
                const char * input = descriptor->_inputs.c_str();
                
                OD_SYSLOG_S1("input <- ", input);//####
                result = sqlite3_bind_text(statement, inputIndex, input, static_cast<int>(strlen(input)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK == result)
            {
                const char * output = descriptor->_outputs.c_str();
                
                OD_SYSLOG_S1("output <- ", output);//####
                result = sqlite3_bind_text(statement, outputIndex, output, static_cast<int>(strlen(output)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK == result)
            {
                const char * version = descriptor->_version.c_str();
                
                OD_SYSLOG_S1("version <- ", version);//####
                result = sqlite3_bind_text(statement, versionIndex, version, static_cast<int>(strlen(version)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK == result)
            {
                const char * details = descriptor->_details.c_str();
                
                OD_SYSLOG_S1("details <- ", details);//####
                result = sqlite3_bind_text(statement, detailsIndex, details, static_cast<int>(strlen(details)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK != result)
            {
                OD_SYSLOG_S1("error description: ", sqlite3_errstr(result));//####
            }
        }
        else
        {
            OD_SYSLOG("! ((0 < detailsIndex) && (0 < inputIndex) && (0 < outputIndex) && (0 < portNameIndex) && "//####
                      "(0 < requestIndex) && (0 < versionIndex))");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_LL(result);
    return result;
} // setupInsertForRequests

/*! @brief Bind the values that are to be inserted into the RequestsKeywords table.
 @param statement The prepared statement that is to be updated.
 @param stuff The source of data that is to be bound.
 @returns The SQLite error from the bind operation. */
static int setupInsertForRequestsKeywords(sqlite3_stmt * statement,
                                          const void *   stuff)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("statement = ", statement, "stuff = ", stuff);//####
    int result = SQLITE_MISUSE;

    try
    {
        int keywordIndex = sqlite3_bind_parameter_index(statement, "@" KEYWORD_C_);
        int portNameIndex = sqlite3_bind_parameter_index(statement, "@" PORTNAME_C_);
        int requestIndex = sqlite3_bind_parameter_index(statement, "@" REQUEST_C_);
        
        if ((0 < keywordIndex) && (0 < portNameIndex) && (0 < requestIndex))
        {
            const RequestKeywordData * descriptor = static_cast<const RequestKeywordData *>(stuff);
            const char *               keyword = descriptor->_key.c_str();
            
            OD_SYSLOG_S1("keyword <- ", keyword);//####
            result = sqlite3_bind_text(statement, keywordIndex, keyword, static_cast<int>(strlen(keyword)),
                                       SQLITE_TRANSIENT);
            if (SQLITE_OK == result)
            {
                const char * request = descriptor->_request.c_str();
                
                OD_SYSLOG_S1("request <- ", request);//####
                result = sqlite3_bind_text(statement, requestIndex, request, static_cast<int>(strlen(request)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK == result)
            {
                const char * portName = descriptor->_port.c_str();
                
                OD_SYSLOG_S1("portName <- ", portName);//####
                result = sqlite3_bind_text(statement, portNameIndex, portName, static_cast<int>(strlen(portName)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK != result)
            {
                OD_SYSLOG_S1("error description: ", sqlite3_errstr(result));//####
            }
        }
        else
        {
            OD_SYSLOG("! ((0 < keywordIndex) && (0 < portNameIndex) && (0 < requestIndex))");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_LL(result);
    return result;
} // setupInsertForRequestsKeywords

/*! @brief Bind the values that are to be inserted into the Services table.
 @param statement The prepared statement that is to be updated.
 @param stuff The source of data that is to be bound.
 @returns The SQLite error from the bind operation. */
static int setupInsertForServices(sqlite3_stmt * statement,
                                  const void *   stuff)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("statement = ", statement, "stuff = ", stuff);//####
    int result = SQLITE_MISUSE;

    try
    {
        int descriptionIndex = sqlite3_bind_parameter_index(statement, "@" DESCRIPTION_C_);
        int nameIndex = sqlite3_bind_parameter_index(statement, "@" NAME_C_);
        int portNameIndex = sqlite3_bind_parameter_index(statement, "@" PORTNAME_C_);
        
        if ((0 < descriptionIndex) && (0 < nameIndex) && (0 < portNameIndex))
        {
            const ServiceData * descriptor = static_cast<const ServiceData *>(stuff);
            const char *        portName = descriptor->_port.c_str();
            
            OD_SYSLOG_S1("portName <- ", portName);//####
            result = sqlite3_bind_text(statement, portNameIndex, portName, static_cast<int>(strlen(portName)),
                                       SQLITE_TRANSIENT);
            if (SQLITE_OK == result)
            {
                const char * name = descriptor->_name.c_str();
                
                OD_SYSLOG_S1("name <- ", name);//####
                result = sqlite3_bind_text(statement, nameIndex, name, static_cast<int>(strlen(name)),
                                           SQLITE_TRANSIENT);
            }
            if (SQLITE_OK == result)
            {
                const char * description = descriptor->_description.c_str();
                
                OD_SYSLOG_S1("description <- ", description);//####
                result = sqlite3_bind_text(statement, descriptionIndex, description,
                                           static_cast<int>(strlen(description)), SQLITE_TRANSIENT);
            }
            if (SQLITE_OK != result)
            {
                OD_SYSLOG_S1("error description: ", sqlite3_errstr(result));//####
            }
        }
        else
        {
            OD_SYSLOG("! ((0 < descriptionIndex) && (0 < nameIndex) && (0 < portNameIndex))");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_LL(result);
    return result;
} // setupInsertForServices

/*! @brief Bind the values that are to be removed from the Requests table.
 @param statement The prepared statement that is to be updated.
 @param stuff The source of data that is to be bound.
 @returns The SQLite error from the bind operation. */
static int setupRemoveForRequests(sqlite3_stmt * statement,
                                  const void *   stuff)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("statement = ", statement, "stuff = ", stuff);//####
    int result = SQLITE_MISUSE;

    try
    {
        int portNameIndex = sqlite3_bind_parameter_index(statement, "@" PORTNAME_C_);
        
        if (0 < portNameIndex)
        {
            const char * portName = static_cast<const char *>(stuff);
            
            result = sqlite3_bind_text(statement, portNameIndex, portName, static_cast<int>(strlen(portName)),
                                       SQLITE_TRANSIENT);
            if (SQLITE_OK != result)
            {
                OD_SYSLOG_S1("error description: ", sqlite3_errstr(result));//####
            }
        }
        else
        {
            OD_SYSLOG("! (0 < portNameIndex)");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_LL(result);
    return result;
} // setupRemoveForRequests

/*! @brief Bind the values that are to be removed from the RequestsKeywords table.
 @param statement The prepared statement that is to be updated.
 @param stuff The source of data that is to be bound.
 @returns The SQLite error from the bind operation. */
static int setupRemoveForRequestsKeywords(sqlite3_stmt * statement,
                                          const void *   stuff)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("statement = ", statement, "stuff = ", stuff);//####
    int result = SQLITE_MISUSE;

    try
    {
        int portNameIndex = sqlite3_bind_parameter_index(statement, "@" PORTNAME_C_);
        
        if (0 < portNameIndex)
        {
            const char * portName = static_cast<const char *>(stuff);
            
            result = sqlite3_bind_text(statement, portNameIndex, portName, static_cast<int>(strlen(portName)),
                                       SQLITE_TRANSIENT);
            if (SQLITE_OK != result)
            {
                OD_SYSLOG_S1("error description: ", sqlite3_errstr(result));//####
            }
        }
        else
        {
            OD_SYSLOG("! (0 < portNameIndex)");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_LL(result);
    return result;
} // setupRemoveForRequestsKeywords

/*! @brief Bind the values that are to be removed from the Services table.
 @param statement The prepared statement that is to be updated.
 @param stuff The source of data that is to be bound.
 @returns The SQLite error from the bind operation. */
static int setupRemoveForServices(sqlite3_stmt * statement,
                                  const void *   stuff)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P2("statement = ", statement, "stuff = ", stuff);//####
    int result = SQLITE_MISUSE;

    try
    {
        int portNameIndex = sqlite3_bind_parameter_index(statement, "@" PORTNAME_C_);
        
        if (0 < portNameIndex)
        {
            const char * portName = static_cast<const char *>(stuff);
            
            result = sqlite3_bind_text(statement, portNameIndex, portName, static_cast<int>(strlen(portName)),
                                       SQLITE_TRANSIENT);
            if (SQLITE_OK != result)
            {
                OD_SYSLOG_S1("error description: ", sqlite3_errstr(result));//####
            }
        }
        else
        {
            OD_SYSLOG("! (0 < portNameIndex)");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_LL(result);
    return result;
} // setupRemoveForServices

#if defined(__APPLE__)
# pragma mark Class methods
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Constructors and destructors
#endif // defined(__APPLE__)

RegistryService::RegistryService(const bool                    useInMemoryDb,
                                 const yarp::os::ConstString & serviceHostName,
                                 const yarp::os::ConstString & servicePortNumber) :
        inherited(true, YPP_REGISTRY_CANONICAL_NAME, "The Service Registry service", YPP_SERVICE_REGISTRY_PORT_NAME,
                  serviceHostName, servicePortNumber), _db(NULL), _inMemory(useInMemoryDb), _isActive(false)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_B1("useInMemoryDb = ", useInMemoryDb);//####
    OD_SYSLOG_S2("serviceHostName = ", serviceHostName.c_str(), "servicePortNumber = ",//####
                 servicePortNumber.c_str());//####
    setUpRequestHandlers();
    OD_SYSLOG_EXIT_P(this);//####
} // RegistryService::RegistryService

RegistryService::~RegistryService(void)
{
    OD_SYSLOG_ENTER();//####
    if (_db)
    {
        sqlite3_close(_db);
    }
    OD_SYSLOG_EXIT();//####
} // RegistryService::~RegistryService

#if defined(__APPLE__)
# pragma mark Actions
#endif // defined(__APPLE__)

bool RegistryService::addRequestRecord(const yarp::os::Bottle &   keywordList,
                                       const RequestDescription & description)
{
    OD_SYSLOG_ENTER();//####
    bool okSoFar = false;
    
    try
    {
        static const char * insertIntoKeywords = "INSERT INTO " KEYWORDS_T_ "(" KEYWORD_C_ ") VALUES(@" KEYWORD_C_ ")";
        static const char * insertIntoRequests = "INSERT INTO " REQUESTS_T_ "(" PORTNAME_C_ "," REQUEST_C_ "," INPUT_C_
                                                    "," OUTPUT_C_ "," VERSION_C_ "," DETAILS_C_ ") VALUES(@"
                                                    PORTNAME_C_ ",@" REQUEST_C_ ",@" INPUT_C_ ",@" OUTPUT_C_ ",@"
                                                    VERSION_C_ ",@" DETAILS_C_ ")";
        static const char * insertIntoRequestsKeywords = "INSERT INTO " REQUESTSKEYWORDS_T_ "(" KEYWORDS_ID_C_ ","
                                                            REQUESTS_ID_C_ ") SELECT @" KEYWORD_C_ ", " KEY_C_ " FROM "
                                                            REQUESTS_T_ " WHERE " REQUEST_C_ " = @" REQUEST_C_ " AND "
                                                            PORTNAME_C_ " = @" PORTNAME_C_;
        
        okSoFar = performSQLstatementWithNoResults(_db, kBeginTransaction);
        if (okSoFar)
        {
            // Add the request.
            okSoFar = performSQLstatementWithNoResults(_db, insertIntoRequests, setupInsertForRequests,
                                                       static_cast<const void *>(&description));
        }
        if (okSoFar)
        {
            // Add the keywords.
            int                numKeywords = keywordList.size();
            RequestKeywordData reqKeyData;
            
            reqKeyData._request = description._request;
            reqKeyData._port = description._port;
            for (int ii = 0; okSoFar && (ii < numKeywords); ++ii)
            {
                yarp::os::Value & aKeyword(keywordList.get(ii));
                
                if (aKeyword.isString())
                {
                    reqKeyData._key = aKeyword.toString();
                    okSoFar = performSQLstatementWithNoResults(_db, insertIntoKeywords, setupInsertForKeywords,
                                                               static_cast<const void *>(reqKeyData._key.c_str()));
                    if (okSoFar)
                    {
                        okSoFar = performSQLstatementWithNoResults(_db, insertIntoRequestsKeywords,
                                                                   setupInsertForRequestsKeywords, &reqKeyData);
                    }
                }
                else
                {
                    OD_SYSLOG("! (aKeyword.isString())");//####
                    okSoFar = false;
                }
            }
        }
        if (okSoFar)
        {
            okSoFar = performSQLstatementWithNoResults(_db, kEndTransaction);
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // RegistryService::addRequestRecord

bool RegistryService::addServiceRecord(const yarp::os::ConstString & portName,
                                       const yarp::os::ConstString & name,
                                       const yarp::os::ConstString & description)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_S2("portName = ", portName.c_str(), "name = ", name.c_str());//####
    bool okSoFar = false;
    
    try
    {
        static const char * insertIntoServices = "INSERT INTO " SERVICES_T_ "(" PORTNAME_C_ "," NAME_C_ ","
                                                    DESCRIPTION_C_ ") VALUES(@" PORTNAME_C_ ",@" NAME_C_ ",@"
                                                    DESCRIPTION_C_ ")";
        
        okSoFar = performSQLstatementWithNoResults(_db, kBeginTransaction);
        if (okSoFar)
        {
            // Add the service port name.
            ServiceData servData;
            
            servData._port = portName;
            servData._name = name;
            servData._description = description;
            okSoFar = performSQLstatementWithNoResults(_db, insertIntoServices, setupInsertForServices,
                                                       static_cast<const void *>(&servData));
        }
        if (okSoFar)
        {
            okSoFar = performSQLstatementWithNoResults(_db, kEndTransaction);
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // RegistryService::addServiceRecord

bool RegistryService::processMatchRequest(YarpPlusPlusParser::MatchExpression * matcher,
                                          yarp::os::Bottle &                    reply)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_P1("matcher = ", matcher);//####
    bool okSoFar = false;
    
    try
    {
        if (matcher)
        {
            yarp::os::ConstString requestAsSQL(matcher->asSQLString("SELECT DISTINCT " PORTNAME_C_ " FROM " REQUESTS_T_
                                                                    " WHERE "));
            
            OD_SYSLOG_S1("requestAsSQL <- ", requestAsSQL.c_str());//####
            okSoFar = performSQLstatementWithNoResults(_db, kBeginTransaction);
            if (okSoFar)
            {
                yarp::os::Bottle & subList = reply.addList();
                
                okSoFar = performSQLstatementWithResults(_db, subList, requestAsSQL.c_str());
            }
            if (okSoFar)
            {
                okSoFar = performSQLstatementWithNoResults(_db, kEndTransaction);
            }
        }
        else
        {
            OD_SYSLOG("! (matcher)");//####
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // RegistryService::processMatchRequest

bool RegistryService::removeServiceRecord(const yarp::os::ConstString & servicePortName)
{
    OD_SYSLOG_ENTER();//####
    OD_SYSLOG_S1("servicePortName = ", servicePortName.c_str());//####
    bool okSoFar = false;
    
    try
    {
        static const char * removeFromRequests = "DELETE FROM " REQUESTS_T_ " WHERE " PORTNAME_C_ " = @" PORTNAME_C_;
        static const char * removeFromRequestsKeywords = "DELETE FROM " REQUESTSKEYWORDS_T_ " WHERE " REQUESTS_ID_C_
                                                            " IN (SELECT " KEY_C_ " FROM " REQUESTS_T_ " WHERE "
                                                            PORTNAME_C_ " = @" PORTNAME_C_ ")";
        static const char * removeFromServices = "DELETE FROM " SERVICES_T_ " WHERE " PORTNAME_C_ " = @" PORTNAME_C_;
        
        okSoFar = performSQLstatementWithNoResults(_db, kBeginTransaction);
        if (okSoFar)
        {
            // Remove the service port requests.
            okSoFar = performSQLstatementWithNoResults(_db, removeFromRequestsKeywords, setupRemoveForRequestsKeywords,
                                                       static_cast<const void *>(servicePortName.c_str()));
        }
        if (okSoFar)
        {
            // Remove the service port requests.
            okSoFar = performSQLstatementWithNoResults(_db, removeFromRequests, setupRemoveForRequests,
                                                       static_cast<const void *>(servicePortName.c_str()));
        }
        if (okSoFar)
        {
            // Remove the service port name.
            okSoFar = performSQLstatementWithNoResults(_db, removeFromServices, setupRemoveForServices,
                                                       static_cast<const void *>(servicePortName.c_str()));
        }
        if (okSoFar)
        {
            okSoFar = performSQLstatementWithNoResults(_db, kEndTransaction);
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // RegistryService::removeServiceRecord

bool RegistryService::setUpDatabase(void)
{
    OD_SYSLOG_ENTER();//####
    bool okSoFar = true;

    try
    {
        int  sqlRes;
        
        if (! _db)
        {
            const char * dbFileName;
            
#if defined(USE_TEST_DATABASE)
            dbFileName = "test.db";
#else // ! defined(USE_TEST_DATABASE)
            if (_inMemory)
            {
                dbFileName = ":memory:";
            }
            else
            {
                dbFileName = "";
            }
#endif // ! defined(USE_TEST_DATABASE)
            sqlRes = sqlite3_open_v2(dbFileName, &_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
            if (SQLITE_OK != sqlRes)
            {
                OD_SYSLOG("(SQLITE_OK != sqlRes)");//####
                okSoFar = false;
                sqlite3_close(_db);
                _db = NULL;
            }
        }
        if (_db)
        {
            okSoFar = constructTables(_db);
            if (! okSoFar)
            {
                OD_SYSLOG("(! okSoFar)");//####
                sqlite3_close(_db);
                _db = NULL;
            }
        }
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(okSoFar);//####
    return okSoFar;
} // RegistryService::setUpDatabase

void RegistryService::setUpRequestHandlers(void)
{
    OD_SYSLOG_ENTER();//####
    try
    {
        _requestHandlers.registerRequestHandler(new MatchRequestHandler(*this, columnNameValidator));
        _requestHandlers.registerRequestHandler(new RegisterRequestHandler(*this));
        _requestHandlers.registerRequestHandler(new UnregisterRequestHandler(*this));
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT();//####
} // RegistryService::setUpRequestHandlers

bool RegistryService::start(void)
{
    OD_SYSLOG_ENTER();//####
    bool result = false;
    
    try
    {
        if ((! isActive()) && (! isStarted()))
        {
            setTimeout(kRegistryServiceTimeout);
            inherited::start();
            if (isStarted() && setUpDatabase())
            {
                // Register ourselves!!!
                yarp::os::Bottle parameters(YPP_SERVICE_REGISTRY_PORT_NAME);
                ServiceRequest   request(YPP_REGISTER_REQUEST, parameters);
                ServiceResponse  response;
                
                if (request.send(getEndpoint(), NULL, &response))
                {
                    // Check that we got a successful self-registration!
                    if (1 == response.count())
                    {
                        yarp::os::Value theValue = response.element(0);
                        
                        OD_SYSLOG_S1("theValue <- ", theValue.toString().c_str());//####
                        if (theValue.isString())
                        {
                            _isActive = (theValue.toString() == YPP_OK_RESPONSE);
                            OD_SYSLOG_B1("_isActive <- ", _isActive);//####
                        }
                        else
                        {
                            OD_SYSLOG("! (theValue.isString())");//####
                        }
                    }
                    else
                    {
                        OD_SYSLOG("! (1 == response.count())");//####
                    }
                }
                else
                {
                    OD_SYSLOG("! (request.send(getEndpoint(), NULL, &response))");//####
                }
            }
            else
            {
                OD_SYSLOG("! (isStarted() && setUpDatabase())");//####
            }
        }
        result = isStarted();
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(result);//####
    return result;
} // RegistryService::start

bool RegistryService::stop(void)
{
    OD_SYSLOG_ENTER();//####
    bool result = false;
    
    try
    {
        result = inherited::stop();
        _isActive = false;
        OD_SYSLOG_B1("_isActive <- ", _isActive);//####
    }
    catch (...)
    {
        OD_SYSLOG("Exception caught");//####
        throw;
    }
    OD_SYSLOG_EXIT_B(result);//####
    return result;
} // RegistryService::stop

#if defined(__APPLE__)
# pragma mark Accessors
#endif // defined(__APPLE__)

#if defined(__APPLE__)
# pragma mark Global functions
#endif // defined(__APPLE__)
