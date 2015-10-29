/*
  @file MqttConnection.c
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

#include "MqttConnection.h"
#include <stdlib.h>
#include <string.h>
#include "Utils.h"

mqtt_connection* MqttConnection_allocated(const char* clientId, mqtt_credentials* credentials,
                genericSsapCallback* messageReceivedCallback, void* messageReceivedContext, 
                connectionEventsCallback* connectionEventsCallback, void* connectionEventsContext) {
    mqtt_connection* conn = MqttConnection_allocate();
    if (conn == NULL)
        return NULL;
    MqttConnection_setClientId(conn, clientId);
    MqttConnection_setCredentials(conn, credentials);
    MqttConnection_setSsapCallback(conn, messageReceivedCallback, messageReceivedContext);
    MqttConnection_setConnectionEventsCallback(conn, connectionEventsCallback, connectionEventsContext);
    return conn;
}

mqtt_connection* MqttConnection_allocate(){
    mqtt_connection* conn = (mqtt_connection*) malloc(sizeof(mqtt_connection));
    if (conn == NULL)
        return NULL;
    else {
        conn->indicationContext = NULL;
        conn->indicationCallback = NULL;
        conn->connectionEventsCallback = NULL;
        conn->connectionEventContext = NULL;
        conn->credentials = NULL;
        conn->keepAliveInterval = DEFAULT_KEEPALIVE_INTERVAL;
        conn->cleanSession = DEFAULT_CLEANSESSION_FLAG;
        conn->connectTimeout = DEFAULT_CONNECT_TIMEOUT;
        conn->retryInterval = DEFAULT_RETRY_INTERVAL;
        conn->lost = 0;
        conn->maxConsecutiveTimeoutErrors = DEFAULT_CONSECUTIVE_TIMEOUT_ERROR_THRESHOLD;
        conn->consecutiveTimeoutErrors = 0;
        conn->watchdogMutex = NULL;
        conn->timeoutErrorsMutex = Thread_create_mutex();
        return conn;
    }
}

char* CopyClientId(const char* clientId){
    // MQTT clientIDs must be between 1 and 23 bytes long.
    // We suppose that both interval limits ("1" and "23") are valid,
    // so an invalid clientId must be 24 chars or more
    // Allocate internal memory, +1 for string terminator
    char* clientId_copy = (char*) malloc((MAX_MQTT_CLIENT_ID_LEN + 1) * sizeof(char));
    memset(clientId_copy, 0x00, (MAX_MQTT_CLIENT_ID_LEN + 1) * sizeof(char));
    strncpy(clientId_copy, clientId, MAX_MQTT_CLIENT_ID_LEN);
    return clientId_copy;
}

void MqttConnection_setClientId(mqtt_connection* conn, const char* clientId){
    // A Server MAY allow a Client to supply a ClientId that has a length
    // of zero bytes, however if it does so the Server MUST treat this
    // as a special case and assign a unique ClientId to that Client.
    if ((clientId == NULL) || strlen(clientId)==0) {
        char* tmpId = generateUUID();
        conn->clientId = CopyClientId(tmpId);
        free(tmpId);
        return;
    } else {
        // Valid clientId, we make sure its length is lower or equal to MAX_MQTT_CLIENT_ID_LEN
        conn->clientId = CopyClientId(clientId);
    }
}

void MqttConnection_setRandomClientId(mqtt_connection* conn){
    MqttConnection_setClientId(conn, NULL);
}

void MqttConnection_setCleanSession(mqtt_connection* conn, int value){
    conn->cleanSession = value;
}

void MqttConnection_setKeepAliveInterval(mqtt_connection* conn, int seconds){
    conn->keepAliveInterval = seconds;
}

void MqttConnection_setRetryInterval(mqtt_connection* conn, int seconds){
    conn->retryInterval = seconds;
}

void MqttConnection_setConnectTimeout(mqtt_connection* conn, int seconds){
    conn->connectTimeout = seconds;
}

void MqttConnection_setCredentials(mqtt_connection* conn, mqtt_credentials* credentials){
    conn->credentials = credentials;
}

void MqttConnection_setSsapCallback(mqtt_connection* conn, genericSsapCallback* callback, 
                                     void* callbackContext){
    conn->messageReceivedCallback = callback;
    conn->messageReceivedContext = callbackContext;                                         
}

void MqttConnection_setConnectionEventsCallback(mqtt_connection* conn, connectionEventsCallback* callback, void* callbackContext){
    conn->connectionEventsCallback = callback;
    conn->connectionEventContext = callbackContext;
    if (callback != NULL){
        conn->watchdogMutex = Thread_create_mutex();        
    } else {
        conn->watchdogMutex = NULL;
    }
}

void MqttConnection_destroy(mqtt_connection* conn){
    free(conn->clientId);
    if (conn->credentials != NULL){
        mqtt_credentials* credentials = conn->credentials;
        free(credentials->username);
        free(credentials->password);
        free(credentials);
    }
    if (conn->connectionEventsCallback != NULL){
        Thread_destroy_mutex(conn->watchdogMutex);
    }
    Thread_destroy_mutex(conn->timeoutErrorsMutex);
    free(conn);    
}

void MqttConnection_setMaxConsecutiveTimeoutErrors(mqtt_connection* conn, int threshold){
    conn->maxConsecutiveTimeoutErrors = threshold;    
}