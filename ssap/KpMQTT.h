/**
  @file KpMQTT.h
  @author Indra Sistemas S.A.
  @author Carlo Gavazzi Automation S.p.A.
  @author Circutor S.A.
  @date Oct 20 2015
  @version 4.1
  @brief Functions that handle MQTT connections to the SIB
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

#include "../paho-mqtt-c/MQTTClient.h"
#include "SsapMessageGenerator.h"
#include "MqttConnection.h"

/**
 * \cond
 */
#ifndef KPMQTT_H
#define	KPMQTT_H
/**
 * \endcond
 */

#ifdef WIN32
#ifdef KPMQTTDLL_EXPORTS
#define KPMQTTDLL_API __declspec(dllexport) 
#else
#define KPMQTTDLL_API __declspec(dllimport) 
#endif
#else
#define KPMQTTDLL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * SSAP-related constants. They should not be changed.
 */

#define WATCHDOG_TOPIC "MQTT_CONN_WATCHDOG" ///< Topic to be used by the MQTT connection watchdog.
#define WATCHDOG_SEND_TIMEOUT 5000
#define SIB_GW_TOPIC "CLIENT_TOPIC" ///< Topic to use when sending data to the SIB
#define SSAP_RESPONSES_TOPIC "/TOPIC_MQTT_PUBLISH" ///< The topic that the SIB uses to answer us
#define NOTIFICATIONS_TOPIC_LENGTH 19 ///< The length of the notifications topic
#define SSAP_INDICATIONS_TOPIC "/TOPIC_MQTT_INDICATION" ///< The topic that the SIB uses to send subscription data
#define SEND_TO_SIB_QOS_LEVEL 1 ///< The ordinary SSAP QoS level
#define SUSCRIBE_QOS_LEVEL 2 ///< The subscription SSAP messages QoS level

/**
 * An enum type that describes the outcome of a connection process.
 */
KPMQTTDLL_API typedef enum { CONNECTED, ///< The MQTT connection was established
  FAILED_CallbacksNotRegistered, ///< The SSAP message callback could not be registered.
  FAILED_PhysicalConnection, ///< The TCP connection with the SIB could not be established.
  FAILED_PhysicalConnection_BadCredentials, ///< The MQTT credentials were not correct.
  FAILED_PhysicalConnection_BadClientID, ///< The MQTT ClientID was not valid.
  FAILED_SubscriptionToSIBTopic, ///< The subscriptions to the topics used by the SIB could not be created 
  FAILED_InternalError ///< An internal API error has occured
} ConnectionStatus;

/**
 * An enum type that describes the outcome of a disconnection process.
 *//**
 * \endcond
 */
KPMQTTDLL_API typedef enum {DISCONNECTED, ///< The connection was closed properly 
  FAILED_ClosePhysicalConnection, ///< The socket could not be closed
  FAILED_DisconnectionInternalError ///< An internal API error has occured
} DisconnectionStatus;

/**
 * An enum type that describes the outcome of a message delivery operation.
 */
KPMQTTDLL_API typedef enum {SENT, ///< The MQTT message was sent to the SIB
  FAILED_SendMessage, ///< The MQTT message could not be sent to the SIB 
  FAILED_SIBReceptionConfirmation ///< The MQTT delivery acknowledge was not received from the SIB
} SendStatus;

/**
 * This structure stores the result of a MQTT connection operation
 */
KPMQTTDLL_API typedef struct {
  ConnectionStatus op;
  mqtt_connection connection_obj; 
} mqtt_connection_result;

/**
 * Establishes a non-cyphered MQTT connection with the SIB.
 * @note If the connection is not established, all allocated the memory and the memory of the arguments will be freed.
 * @param server The SIB server name
 * @param port The SIB MQTT port
 * @param credentials The MQTT credentials to use. This pointer must be setted to NULL when no authentication credentials will
 * be used, and its memory will be freed from the API.
 * @param messageReceivedCallback A pointer to the function that will process all the incoming SSAP messages (except the INDICATION ones)
 * @param callbackContext A pointer to the context (a.k.a. "state") structure used by the messageReceivedCallback function.
 * @param connectionEventsCallback A pointer to the function that will process the MQTT connection events. So far, only
 * disconnection events are detected.
 * @param connectionEventsContext A pointer to the context (a.k.a. "state") structure used by the connectionEventsCallback function.
 * @param created_connection A pointer to an structure that stores the MQTT connection state, passed by reference. When no connection
 * 	has been established, it will be setted to NULL.
 * @return A ConnectionStatus enum value representing the result of the connection process.
 * @deprecated Use KpMqtt_connect() instead.
 */
KPMQTTDLL_API ConnectionStatus KpMqtt_connectd(const char* server, const char* port, mqtt_credentials* credentials,
                                genericSsapCallback* messageReceivedCallback, void* callbackContext,
                                void* connectionEventsCallback, void* connectionEventsContext,
                                mqtt_connection** created_connection);

/**
 * Establishes a non-cyphered MQTT connection with the SIB.
 * @note If the connection is not established, all allocated the memory and the memory of the arguments will be freed.
 * @param connection A configured MQTT connection structure.
 * @param server The SIB server name
 * @param port The SIB MQTT port
 * @return A ConnectionStatus enum value representing the result of the connection process.
 */
KPMQTTDLL_API ConnectionStatus KpMqtt_connect(mqtt_connection** connection, const char* server, const char* port);

/**
 * Establishes a MQTT over SSL connection with the SIB.
 * @note If the connection is not established, all allocated the memory and the memory of the arguments will be freed.
 * @param server The SIB server name
 * @param port The SIB MQTT port
 * @param credentials The MQTT credentials to use. This pointer must be setted to NULL when no authentication credentials will
 * be used, and its memory will be freed from the API.
 * @param ca_file The trusted PEM certificate. <strong>It must be a root certificate.</strong>
 * @param ca_path The directory that contains the trusted PEM certificates (usually, /etc/ssl/certs).
 * @warning You must pass either the ca_file or the ca_path parameter to this function.
 * @param messageReceivedCallback A pointer to the function that will process all the incoming SSAP messages (except the INDICATION ones)
 * @param callbackContext A pointer to the context (a.k.a. "state") structure used by the messageReceivedCallback function.
 * @param connectionEventsCallback A pointer to the function that will process the MQTT connection events. So far, only
 * disconnection events are detected.
 * @param connectionEventsContext A pointer to the context (a.k.a. "state") structure used by the connectionEventsCallback function.
 * @param created_connection A pointer to an structure that stores the MQTT connection state, passed by reference. When no connection
 * 	has been established, it will be setted to NULL.
 * @return A ConnectionStatus enum value representing the result of the connection process.
 * @deprecated Use KpMqtt_connectSSL() instead.
 */
KPMQTTDLL_API ConnectionStatus KpMqtt_connectSSLd(const char* server, const char* port, mqtt_credentials* credentials,
                                                  const char* ca_file, const char* ca_path,
                                   genericSsapCallback* messageReceivedCallback, void* callbackContext,
                                   void* connectionEventsCallback, void* connectionEventsContext,
                                   mqtt_connection** created_connection);

/**
 * Establishes a MQTT over SSL connection with the SIB.
 * @note If the connection is not established, all allocated the memory and the memory of the arguments will be freed.
 * @param connection A configured MQTT connection structure.
 * @param server The SIB server name
 * @param port The SIB MQTT port
 * @param ca_file The trusted PEM certificate. <strong>It must be a root certificate.</strong>
 * @param ca_path The directory that contains the trusted PEM certificates (usually, /etc/ssl/certs).
 * @warning You must pass either the ca_file or the ca_path parameter to this function.
 * @return A ConnectionStatus enum value representing the result of the connection process.
 */
KPMQTTDLL_API ConnectionStatus KpMqtt_connectSSL(mqtt_connection** connection, const char* server, const char* port,
                                                const char* ca_file, const char* ca_path);

/**
 * Closes an MQTT connection with the SIB.
 * @param connection A pointer to the status structure of the connection.
 * @param timeout A timeout to close the connection in milliseconds. If the
 * 	timeout elapses, the connection will be closed abruply.
 * @return A DisconnectionStatus enum value representing the result of the disconnection process.
 */
KPMQTTDLL_API DisconnectionStatus KpMqtt_disconnect(mqtt_connection* connection, int timeout);

/**
 * Sends a SSAP message to the SIB
 * @param connection A pointer to the status structure of the connection.
 * @param request A pointer to the SSAP message to send. Its memory will be freed
 * 	after sending the message.
 * @param timeout The timeout to send the message to the SIB in milliseconds.
 * @return A SendStatus enum value representing the status of the send operation.
 */
KPMQTTDLL_API SendStatus KpMqtt_send(mqtt_connection* connection, ssap_message* request, int timeout);

/**
 * Registers the indication listener on a MQTT connection.
 * @param connection A pointer to the status structure of the connection.
 * @param callback A pointer to the SSAP indication callback function.
 * @param callbackContext  A pointer to the context (a.k.a. "state") structure used by the ssapIndicationCallback function.
 */
KPMQTTDLL_API void KpMqtt_setIndicationListener(mqtt_connection* connection, ssapIndicationCallback* callback, void* callbackContext);

/**
 * Registers the indication listener on a MQTT connection.
 * @param connection A pointer to the status structure of the connection.
 * @param callback A pointer to the SSAP indication callback function.
 * @param callbackContext  A pointer to the context (a.k.a. "state") structure used by the ssapIndicationCallback function.
 * @deprecated Use KpMqtt_setIndicationListener() instead.
 */
KPMQTTDLL_API void setIndicationListener(mqtt_connection* connection, ssapIndicationCallback* callback, void* callbackContext);

/**
 * Builds an username and password-based MQTT credentials structure.
 * @param username The MQTT username to use.
 * @param password The MQTT password to use.
 * @return A MQTT credentials structure initialized with the given arguments. The arguments will be copied.
 */
KPMQTTDLL_API mqtt_credentials* KpMqtt_buildCredentials(const char* username, const char*password);

/**
 * Builds an username and password-based MQTT credentials structure.
 * @param username The MQTT username to use.
 * @param password The MQTT password to use.
 * @return A MQTT credentials structure initialized with the given arguments. The arguments will be copied.
 * @deprecated Use KpMqtt_buildCredentials() instead.
 */
KPMQTTDLL_API mqtt_credentials* buildCredentials(const char* username, const char*password);

/**
 * Frees the memory used by the OpenSSL library.
 * @note This function is not thread safe. Call it before exiting from the main thread.
 */
KPMQTTDLL_API void KpMqtt_freeOpenSSLTables();

#ifdef __cplusplus
}
#endif
#endif /* KPMQTT_H */