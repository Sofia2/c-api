/*
  @file MqttConnection.c
  @author Indra Sistemas S.A.
  @author Carlo Gavazzi Automation S.p.A.
  @author Circutor S.A.
  @date Oct 20 2015
  @version 4.1
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
#include "Utils.h"

mqtt_connection* MqttConnection_allocated(char* clientId, mqtt_credentials* credentials, 
                genericSsapCallback* messageReceivedCallback, void* messageReceivedContext, 
                connectionEventsCallback* connectionEventsCallback, void* connectionEventsContext) {
    mqtt_connection* conn = MqttConnection_allocate();
    if (conn == NULL)
        return NULL;
    if (clientId != NULL)
        MqttConnection_setClientId(conn, clientId);
    else
        MqttConnection_setClientId(conn, generateUUID());
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
        conn->keepAliveInterval = DEFAULT_KEEPALIVE_INTERVAL;
        conn->cleanSession = DEFAULT_CLEANSESSION_FLAG;
        conn->connectTimeout = DEFAULT_CONNECT_TIMEOUT;
        conn->retryInterval = DEFAULT_RETRY_INTERVAL;
        conn->lost = 0;
        conn->watchdogMutex = NULL;
        return conn;
    }
}

void MqttConnection_setClientId(mqtt_connection* conn, char* clientId){
    clientId[22] = '\0'; // MQTT clientIDs must be between 1 and 23 bytes long.
    conn->clientId = clientId;    
}

void MqttConnection_setRandomClientId(mqtt_connection* conn){
    MqttConnection_setClientId(conn, generateUUID());    
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
    free(conn);    
}