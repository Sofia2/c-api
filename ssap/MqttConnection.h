/**
  @file MqttConnection.h
  @author Indra Sistemas S.A.
  @author Carlo Gavazzi Automation S.p.A.
  @author Circutor S.A.
  @date Oct 29 2015
  @version 4.2
  @brief Utility functions to build MQTT connection objects.
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
#ifndef MQTT_CONNECTION_H
#define MQTT_CONNECTION_H
#ifdef WIN32
#ifdef SSAP_MQTT_CONNECTION_EXPORTS
#define SSAP_MQTT_CONNECTION_API __declspec(dllexport) 
#else
#define SSAP_MQTT_CONNECTION_API __declspec(dllimport) 
#endif // WIN32
#else
#define SSAP_MQTT_CONNECTION_API
#endif // not WIN32

#ifdef __cplusplus
extern "C" {
#endif

#include "SsapMessageGenerator.h"
#include "../paho-mqtt-c/MQTTClient.h"
#include "../paho-mqtt-c/Thread.h"
    
#define DEFAULT_KEEPALIVE_INTERVAL 60 ///< The default MQTT keepAlive interval in seconds.
#define DEFAULT_CLEANSESSION_FLAG 1 ///< The default value of the MQTT cleanSession flag.
#define DEFAULT_CONNECT_TIMEOUT 30 ///< The default MQTT connect timeout in seconds.
#define DEFAULT_RETRY_INTERVAL 20 ///< The default message delivery retry interval in seconds.
#define MAX_MQTT_CLIENT_ID_LEN 23 ///< MQTT clientIDs must be between 1 and 23 bytes long.
    
    
SSAP_MQTT_CONNECTION_API typedef enum {CONNECTION_LOST ///< Event type of detected MQTT disconnections.
} ConnectionEvent;

/**
 * The type of every SSAP subscription callback.
 */
SSAP_MQTT_CONNECTION_API typedef void ssapIndicationCallback(ssap_message* indicationMessage, void* context);

/**
 * The type of the callback function that processes all the SSAP messages except the indication ones.
 */
SSAP_MQTT_CONNECTION_API typedef void genericSsapCallback(ssap_message* response, void* context);

/**
 * The type of the callback function that processes MQTT connection events.
 */
SSAP_MQTT_CONNECTION_API typedef void connectionEventsCallback(ConnectionEvent event, void* context);

/**
 * This structure contains the credentials that will be used when establishing a MQTT connection.
 */
SSAP_MQTT_CONNECTION_API typedef struct {
  /**
   * MQTT connection username
   */
  char* username;
  
  /**
   * MQTT connection password
   */
  char* password;
} mqtt_credentials;

/*
 * This structure represents an MQTT connection.
 */
SSAP_MQTT_CONNECTION_API typedef struct {
  
  /**
   * The SSAP indication callback function
   */
  ssapIndicationCallback* indicationCallback;
  
  /**
   * The generic SSAP message received callback
   */
  genericSsapCallback* messageReceivedCallback;
  
  /**
   * The MQTT connection events callback.
   */
  connectionEventsCallback* connectionEventsCallback;
  
  /**
   * The indication callback context (i.e. state)
   */
  void* indicationContext;
  
  /**
   * The message received context (i.e. state)
   */
  void* messageReceivedContext;
  
  /**
   * The connection events context (i.e. state)
   */
  void* connectionEventContext;
  
  /**
   * MQTT client library status
   */
  MQTTClient mqttClient;
  
  /**
   * MQTT connection clientId
   */
  char *clientId;
  
  /**
   * MQTT connection credentials
   */
  mqtt_credentials* credentials;
  
  /**
   * MQTT keepAlive interval in seconds. Defaults to 60 seconds.
   */
  int keepAliveInterval;
  
  /**
   * Sets the default behavior after a MQTT disconnection. Defaults to 1.
   */
  int cleanSession;
  
  /**
   * Connect timeout in seconds. Defaults to 30 seconds.
   */
  int connectTimeout;
  
  /**
   * Message delivery retry interval in seconds. Defaults to 20 seconds.
   */
  int retryInterval;
  
  /**
   * A flag that indicates if the connection was lost.
   */
  volatile char lost;  
  /**
   * The mutex used by the connection watchdog.
   * @warning This member must only be used by the C API.
   */
  mutex_type watchdogMutex;
  
} mqtt_connection;

/**
 * Initializes a MQTT connection.
 * @param clientId The MQTT clientId to be used.
 * @note The memory of the clientId will NOT be freed after the connection is closed.
 * @param credentials The MQTT credentials to be used.
 * @param messageReceivedCallback A pointer to the function that will process all the incoming SSAP messages (except the INDICATION ones)
 * @param messageReceivedContext A pointer to the context (a.k.a. "state") structure used by the messageReceivedCallback function.
 * @param connectionEventsCallback A pointer to the function that will process the MQTT connection events. So far, only
 * disconnection events are detected.
 * @param connectionEventsContext A pointer to the context (a.k.a. "state") structure used by the connectionEventsCallback function.
 * @note The memory of the connection and of all the given parameters will be freed when the connection is destroyed.
 * @deprecated Use MqttConnection_allocate(), MqttConnection_setClientId(), MqttConnection_setRandomClientId(), MqttConnection_setCredentials(), 
 * MqttConnection_setSsapCallback() and MqttConnection_setConnectionEventsCallback() methods instead.
 */
SSAP_MQTT_CONNECTION_API mqtt_connection* MqttConnection_allocated(const char* clientId, mqtt_credentials* credentials,
                genericSsapCallback* messageReceivedCallback, void* messageReceivedContext, 
                connectionEventsCallback* connectionEventsCallback, void* connectionEventsContext);

/**
 * Allocates the memory of a MQTT connection.
 * @note The connection 
 */
SSAP_MQTT_CONNECTION_API mqtt_connection* MqttConnection_allocate();

/**
 * Sets the MQTT clientId to be used.
 * @param conn The MQTT connection to be configured.
 * @param clientId The MQTT clientID to be used.
 * @note The memory of the clientId will NOT be freed after the connection is closed.
 * @see http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html, Section 3.1, before generating the 
 * clientId.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setClientId(mqtt_connection* conn, const char* clientId);

/**
 * Configures the MQTT connection to use a random clientID.
 * @param conn The MQTT connection to be configured.
 * @note The memory of the clientId will be freed after the connection is closed.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setRandomClientId(mqtt_connection* conn);

/**
 * Sets the MQTT credentials to be used. If this method is not invoked, an anonymous connection will be established.
 * @param conn The MQTT connection to be configured.
 * @param credentials credentials The MQTT credentials to be used.
 * @note The memory of the credentials will be freed after the connection is closed.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setCredentials(mqtt_connection* conn, mqtt_credentials* credentials);

/**
 * Sets the callback function that will handle the incoming SSAP messages.
 * @param conn The MQTT connection to be configured.
 * @param callback A pointer to the function that will process all the incoming SSAP messages (except the INDICATION ones)
 * @param callbackContext A pointer to the context (a.k.a. "state") structure used by the callback function.
 * @warning These callbacks must be configured before establishing the connection.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setSsapCallback(mqtt_connection* conn, genericSsapCallback* callback, 
                                     void* callbackContext);

/**
 * Sets the callback function that will handle the MQTT connection events.
 * @param conn The MQTT connection to be configured.
 * @param callback A pointer to the function that will process all the MQTT connection events.
 * @param callbackContext A pointer to the context (a.k.a. "state") structure used by the callback function.
 * @warning Due to the limitations of the underlying MQTT client (Eclipse Paho), only disconnection events will be reported.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setConnectionEventsCallback(
    mqtt_connection* conn, connectionEventsCallback* callback, void* callbackContext);

/**
 * Overrides the default MQTT cleansession value (1).
 * @param conn The MQTT connection to be configured.
 * @param value The MQTT cleansession value.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setCleanSession(mqtt_connection* conn, int value);

/**
 * Overrides the default MQTT keepAlive interval (60 seconds).
 * @param conn The MQTT connection to be configured.
 * @param seconds The MQTT keepAlive interval to be used.
 * @warning Setting a keepAlive interval too low can dramatically increase the number of disconnections.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setKeepAliveInterval(mqtt_connection* conn, int seconds);

/**
 * Overrides the default connect timeout (30 seconds).
 * @param conn The MQTT connection to be configured.
 * @param seconds The connect timeout to be used.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setConnectTimeout(mqtt_connection* conn, int seconds);

/**
 * Overrides the default message delivery retry interval (20 seconds).
 * @param conn The MQTT connection to be configured.
 * @param seconds The delivery retry interval to be used.
 */
SSAP_MQTT_CONNECTION_API void MqttConnection_setRetryInterval(mqtt_connection* conn, int seconds);

/**
 * Frees the memory of a MQTT connection.
 * @param conn The connection whose memory will be freed.
 * @warning This method is not intended to be used outside the SSAP API.
 */
void MqttConnection_destroy(mqtt_connection* conn);

#ifdef __cplusplus
}
#endif
#endif // MQTT_CONNECTION_H