/**
  @file SsapMessageGenerator.h
  @author Indra Sistemas S.A.
  @author Carlo Gavazzi Automation S.p.A.
  @author Circutor S.A.
  @date Oct 29 2015
  @version 4.2
  @brief Functions that manipulate SSAP messages.
  @see http://sofia2.com/desarrollador_en.html
  
  @copyright Copyright 2013-15 Indra Sistemas S.A.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License. 
*/

/**
 * \cond
 */
#ifndef SSAPMESSAGEGENERATOR_H
#define	SSAPMESSAGEGENERATOR_H
/**
 * \endcond
 */

#ifdef WIN32
#ifdef SSAPMSGGENDLL_EXPORTS
#define SSAPMSGGEN_API __declspec(dllexport) 
#else
#define SSAPMSGGEN_API __declspec(dllimport) 
#endif
#else
#define SSAPMSGGEN_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
#include <stddef.h>
#endif

/**
 * SSAP message types enum
 */
typedef enum { JOIN, ///< Create session with the SIB 
    LEAVE, ///< End session with the SIB 
    INSERT, ///< Send data to be stored in the RTDB 
    UPDATE, ///< Update data stored in the RTDB
    DELETE, ///< Delete data stored in the RTDB,
    QUERY, ///< Retrieve stored data 
    SUBSCRIBE, ///< Enable updates on data changes 
    UNSUBSCRIBE, ///< Disable updates on data changes 
    INDICATION, ///< Data change indication
    BULK, ///< Bulk request,
    CONFIG ///< Configuration data request
    
} SSAPMessageTypes;

/**
 * SSAP message directions enum
 */
typedef enum { REQUEST, ///< Request send to the SIB
  RESPONSE, ///< Request processed at the SIB without errors
  ERROR ///< Request processed at the SIB with errors
} SSAPMessageDirection;

/**
 * SSAP persistence types enum
 */
typedef enum {MONGODB, ///< Data persistence in MongoDB
  INMEMORY ///< Data persistence in memory
} SSAPPersistenceType;

/**
 * SSAP query types enums.
 */
typedef enum { NATIVE, ///< MongoDB-like query
  SQLLIKE, ///< SQL-LIKE query
  SIB_DEFINED, ///< SIB defined query 
  BDH, ///< Historical database (i.e. Hadoop) query 
  CEP ///< CEP query
} SSAPQueryType;

/**
 * Structure to hold the data of an SSAP message.
 */
typedef struct{
        /**
	 * The unique identifier of the request.
	 */
	char* messageId;
	
	/**
	 * The unique identifier of the session.
	 */
	char* sessionKey;
	
	/**
	 * The identifier of the ontology that is referenced by the message.
	 */
	char* ontology;

	/**
	 * The direction of the message
	 */
	SSAPMessageDirection direction;
	
	/**
	 * The type of the message
	 */
	SSAPMessageTypes messageType;

	/**
	 * The persistence type of the message
	 */
	SSAPPersistenceType persistenceType;
	
	/**
	 * The message body. It stores the data.
	 */
	char* body;
} ssap_message;


typedef struct{
    char* key;
    char* value;    
} keyValuePair;

/**
 * This structure holds a fixed-size map.
 */
typedef struct{
  /**
   * The key-value pairs
   */
  keyValuePair *data;
  /**
   * The number of key-value pairs that are stored.
   */
  size_t size;
  /**
   * The maximum number of key-value pairs that can be stored.
   */
  size_t max_size;
} map_t;

typedef struct{
  /**
   * Messages of the request
   */
  ssap_message **payload;
  /**
   * The number of messages that are stored
   */
  size_t size;
  /**
   * The maximum number of messages that can be stored
   */
  size_t max_size;
} bulkRequest;

/**
 * Builds a join message
 * @param token The authentication token of the KP
 * @param instance The instance identifier of the KP. It must be unique among all the KPs connected
 * to the Sofia2 platform
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateJoinMessage(const char* token, const char* instance);


/**
 * Builds a join message to renovate a session.
 * @param token The authentication token of the KP
 * @param instance The instance identifier of the KP. It must be unique among all the KPs connected
 * to the Sofia2 platform
 * @param sessionKey The unique identifier of the session
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateJoinRenewSessionMessage(const char* token, const char* instance, const char* sessionKey);


/**
 * Builds a leave message
 * @param sessionKey The unique identifier of the session
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateLeaveMessage(const char* sessionKey);

/**
 * Builds an insert message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param data The data to insert in the target ontology
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateInsertMessage(const char* sessionKey, const char* ontology, const char* data);

/**
 * Builds an insert message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param data The data to insert in the target ontology
 * @param queryType The query tipe of the insert operation. It determines the data syntax. An example follows:
 * - Native inserts: they use the native syntax of MongoDB.
 * \code{.js}Ontology.insert({name : "foo"})\endcode
 * - SQLLIKE inserts: they use standard SQL syntax.
 * \code{.sql}INSERT INTO Ontology(name) VALUES ('foo');\endcode
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateInsertMessageWithQueryType(const char* sessionKey, const char* ontology, const char* data, SSAPQueryType queryType);

/**
 * Builds an update message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be updated.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateUpdateMessage(const char* sessionKey, const char* ontology, const char* query);

/**
 * Builds an update message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be updated.
 * @param queryType The query tipe of the update operation. It determines the data syntax.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateUpdateMessageWithQueryType(const char* sessionKey, const char* ontology, const char* query, SSAPQueryType queryType);

/**
 * Builds a delete message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be deleted.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateDeleteMessage(const char* sessionKey, const char* ontology, const char* query);

/**
 * Builds a delete message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be deleted.
 * @param queryType The query tipe of the delete operation.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateDeleteMessageWithQueryType(const char* sessionKey, const char* ontology, const char* query, SSAPQueryType queryType);

/**
 * Builds a query message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be returned.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateQueryMessage(const char* sessionKey, const char* ontology, const char* query);

/**
 * Builds a query message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be returned.
 * @param queryType The query tipe of the query operation.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateQueryMessageWithQueryType(const char* sessionKey, const char* ontology, const char* query, SSAPQueryType queryType);


/**
 * Builds a sib-defined query message
 * @param sessionKey The unique identifier of the session
 * @param query The query to execute
 * @param params A map_t structure containing the parameters of the query
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateSIBDefinedQueryMessageWithParams(const char* sessionKey, const char* query, map_t* params);


/**
 * Builds a subscribe message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be returned.
 * @param refreshMs The refresh period in milliseconds. An Indication message will be returned every refreshMs milliseconds
 * with the result of the query.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateSubscribeMessage(const char* sessionKey, const char* ontology, const char* query, int refreshMs);

/**
 * Builds a subscribe message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param query The query that selects the documents that will be returned.
 * @param queryType The query tipe of the query operation.
 * @param refreshMs The refresh period in milliseconds. An Indication message will be returned every refreshMs milliseconds
 * with the result of the query.
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateSubscribeMessageWithQueryType(const char* sessionKey, const char* ontology, const char* query, SSAPQueryType queryType, int refreshMs);


/**
 * Builds an unsubscribe message
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * @param subscriptionId The unique identifier of the subscription
 * @return The generated SSAP message.
 */
SSAPMSGGEN_API ssap_message* generateUnsubscribeMessage(const char* sessionKey, const char* ontology, const char* subscriptionId);

/**
 * Creates an empty map_t structure able to hold up to size key-value pairs.
 * @param size The maximum number of parameters to hold in the structure.
 * @return An empty map_t structure of the given size
 */
SSAPMSGGEN_API map_t* createEmptyMap(size_t size);

/**
 * Stores a key-value pair in a given map_t structure.
 * @param key The key of the pair. It will be copied.
 * @param value The value of the pair. It will be copied.
 * @param params A map_t structure
 * @return true if the key-value pair was added to the structure, and false otherwise.
 * <br><b>Notice</b></br> If the map_t structure is full, nothing will be done.
 */
SSAPMSGGEN_API int pushKeyValuePair(const char* key, const char *value, map_t* params);

/**
 * Frees the memory of a ssap_message structure. DO NOT USE THIS METHOD IN CLIENT CODE!
 */
void freeSsapMessage(ssap_message* message);

/**
 * Converts a ssap_message structure into a serialized JSON message. DO NOT USE THIS METHOD IN CLIENT CODE!
 * @param message The SSAP message structure to serialize
 * @return A serialized JSON message.
 */
char* ssap_messageToJson(ssap_message* message);

/**
 * Converts a serialized-JSON message into a ssap_message structure. DO NOT USE THIS METHOD IN CLIENT CODE!
 * @param source The serialized JSON data to parse
 * @return A ssap_message structure
 */
SSAPMSGGEN_API ssap_message* ssapMessageFromJson(const char* source);

/**
 * Creates an empty bulk request of a given size
 * @param max_size The number of SSAP messages of the bulk request
 * @return An empty bulk request of the specified size.
 */
SSAPMSGGEN_API bulkRequest* createBulkRequest(size_t max_size);

/**
 * Adds a SSAP message to a bulk request
 * @param message The SSAP message that will be added to the BULK request. It will not be copied,
 * and its memory will be freed after sending the SSAP BULK message.
 * @param request The BULK request that will be associated with the message
 * @return true if the message was successfully added to the request, and false otherwise.
 * <br><b>Notice</b></br> If the bulk request capacity has been exceeded, the given message will not
 * be added to the request.
 */
SSAPMSGGEN_API int addToBulkRequest(ssap_message* message, bulkRequest* request);

/**
 * Builds a SSAP BULK message. 
 * @param sessionKey The unique identifier of the session
 * @param ontology The unique identifier of the target ontology
 * <br><b>Notice</b></br> The memory of the request will be freed.
 * @param request A structure that holds the messages of the bulk request.
 * @return The SSAP bulk message.
 */
SSAPMSGGEN_API ssap_message* generateBulkMessage(const char* sessionKey, const char* ontology, bulkRequest* request);

/**
 * Builds a SSAP CONFIG message
 * @param kp a KP name
 * @param instance a KP instance
 * @param token the KP token
 * @param assetService the asset service. It may be null.
 * @param assetServiceParam a map_t structure holding the parameters of the asset service. It may be null
 * <br><b>Notice</b></br> The memory of the asset service parameters map will be freed
 * @return A SSAP CONFIG message.
 */
SSAPMSGGEN_API ssap_message* generateConfigMessage(const char* kp, const char* instance, const char* token, const char* assetService, map_t* assetServiceParam);

#ifdef __cplusplus
}
#endif
#endif /*SSAPMESSAGEGENERATOR_H*/