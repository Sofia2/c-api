/*
  @file KpMqtt.c
  @author Indra Sistemas S.A.
  @author Carlo Gavazzi Automation S.p.A.
  @author Circutor S.A.
  @date Oct 30 2015
  @version 4.3
  @brief Functions that handle MQTT connections to the SIB.

  This file implements a bunch of functions that handle MQTT connections to the SIB.
  The clients and the SIB communicate using a JSON-based protocol, called SSAP.  
 
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

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "KpMQTT.h"
#include "./paho-mqtt-c/Socket.h"
#include "sleeps.h"

/**
 * Auxiliary functions
 */

KpMqtt_SendStatus publish(mqtt_connection* connection, const char* topic, char* payload, int timeout){
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(payload);
    pubmsg.qos = SEND_TO_SIB_QOS_LEVEL;
    pubmsg.retained = 0;

    int prc = MQTTClient_publishMessage(connection->mqttClient, topic, &pubmsg, &token); // deadlock!
    if(prc != MQTTCLIENT_SUCCESS) {
        free((char*)payload);
        return DeliveryError_MalformedMqttMessage;
    }

    int wrc = MQTTClient_waitForCompletion(connection->mqttClient, token, timeout);
    if(wrc != MQTTCLIENT_SUCCESS) {
        free((char*)payload);
        switch (wrc){
            case MQTTCLIENT_DISCONNECTED:
                connection->lost = 1;
                return DeliveryError_ConnectionLost;
            default:
                return DeliveryError_TimeoutError;
        }
    }
    MQTTClient_message* aux=&pubmsg;
    MQTTClient_freeMessage(&aux);

    free((char*)payload);
    return Ssap_Message_Sent;
}

void onMqttConnectionEvent(void* context, char* cause){
    mqtt_connection *connection = (mqtt_connection*) context; 
    connection->lost = 1;
    if (connection->connectionEventsCallback != NULL){
        connection->connectionEventsCallback(CONNECTION_LOST, connection->connectionEventContext); 
    }
}

/*
thread_return_type monitorConnection(void* context){
    mqtt_connection* conn = (mqtt_connection*) context;
    Thread_lock_mutex(conn->watchdogMutex);
    while (!conn->lost){
        sleep(conn->keepAliveInterval);
        char* payload = malloc(1 * sizeof(char));
        if (payload == NULL)
            break;
        strncpy(payload, "", 1);
        publish(conn, WATCHDOG_TOPIC, payload, WATCHDOG_SEND_TIMEOUT);
    }
    Thread_unlock_mutex(conn->watchdogMutex);
    onMqttConnectionEvent(conn, NULL);
    return 0;
}*/

void deallocateMqttConnection(mqtt_connection* conn) {
    MQTTClient_destroy(&(conn->mqttClient));
    MqttConnection_destroy(conn);
}

/*
 * This function processes the RAW MQTT messages received from the SIB.
 */
int onMqttMessageReceived(void* context, char* topicName, int topicLen, MQTTClient_message* m) {
    mqtt_connection *connection = (mqtt_connection*) context; 
    
    char *serialized_json = (char*)malloc((m->payloadlen + 1) * sizeof(char));
    if (serialized_json == NULL)
        return 1;
    strncpy(serialized_json, m->payload, m->payloadlen);
    serialized_json[m->payloadlen] = '\0'; // The message payload is not null-terminated
    ssap_message *response = ssapMessageFromJson(serialized_json);
    free(serialized_json);	

    if((strncmp (topicName, SSAP_RESPONSES_TOPIC, NOTIFICATIONS_TOPIC_LENGTH))==0) {
        // Synchronous response  
        connection->messageReceivedCallback(response, connection->messageReceivedContext);
    } else if(((strncmp (topicName, SSAP_INDICATIONS_TOPIC, NOTIFICATIONS_TOPIC_LENGTH))==0)
              && connection->indicationCallback) {
        // INDICATION message.
        connection->indicationCallback(response, connection->indicationContext);
    }
    freeSsapMessage(response);
    MQTTClient_freeMessage(&m);
    MQTTClient_free(topicName);
    return 1;
}

KpMqtt_ConnectStatus establishConnection(mqtt_connection* connection, const char* server, const char* port,
                                     const char* ca_file, const char* ca_path) {
    
    char* address = (char*) malloc((strlen("tcp://")+strlen(server)+strlen(":")+strlen(port)+1)*sizeof(char));
    
    if (address == NULL)
        return ConnectError_InternalError;
    if (ca_file != NULL || ca_path != NULL) {
        strcpy(address, "ssl://");
    } else {
        strcpy(address, "tcp://");
    }
    strcat(address, server);
    strcat(address, ":");
    strcat(address, port);

    int rc = MQTTClient_create(&(connection->mqttClient), (char*)address, connection->clientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS){
        free(address);
        return ConnectError_InvalidArguments;
    }
    
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;   

    conn_opts.connectTimeout = connection->connectTimeout;
    conn_opts.cleansession = connection->cleanSession;
    conn_opts.keepAliveInterval = connection->keepAliveInterval;    
    conn_opts.retryInterval = connection->retryInterval;    
    
    if (connection->credentials != NULL){
      conn_opts.username = connection->credentials->username;
      conn_opts.password = connection->credentials->password;
    }

    if (ca_file != NULL || ca_path != NULL) {
      MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
      conn_opts.ssl = &ssl_opts;      
      conn_opts.ssl->cafile = ca_file;
      conn_opts.ssl->capath = ca_path;
      conn_opts.ssl->enableServerCertAuth = 1;
    }

    rc = MQTTClient_setCallbacks(connection->mqttClient, (void*) connection, onMqttConnectionEvent, 
                               onMqttMessageReceived, NULL);
    if(rc == MQTTCLIENT_FAILURE) {
        free(address);
        return ConnectError_CallbacksNotRegistered;
    }

    if ((rc = MQTTClient_connect(connection->mqttClient, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        deallocateMqttConnection(connection);
        free(address);
        switch(rc){
          case 2:
              return ConnectError_BadClientID;
          case 4:
          case 5:
              return ConnectError_BadCredentials;
          default:
              return ConnectError_SocketError;
        }
    }

    // Create & subscribe to the topic of this KP.
    char* topic= (char*) malloc((strlen(SSAP_RESPONSES_TOPIC)+strlen(connection->clientId)+1)*sizeof(char));
    if (topic == NULL)
        return ConnectError_InternalError;
    strcpy(topic,SSAP_RESPONSES_TOPIC);
    strcat(topic, connection->clientId);

    rc = MQTTClient_subscribe(connection->mqttClient, topic, SUSCRIBE_QOS_LEVEL);
    if(rc != MQTTCLIENT_SUCCESS) {
        deallocateMqttConnection(connection);
        free(address);
        free(topic);
        return ConnectError_SubscriptionToSIBTopic;
    }


    // Create & subscribe to the indications topic of this KP.
    char* topicIndication=(char*) malloc((strlen(SSAP_INDICATIONS_TOPIC)+strlen(connection->clientId)+1)*sizeof(char));
    if (topicIndication == NULL)
        return ConnectError_InternalError;
    strcpy(topicIndication, SSAP_INDICATIONS_TOPIC);
    strcat(topicIndication, connection->clientId);

    rc = MQTTClient_subscribe(connection->mqttClient, topicIndication, SUSCRIBE_QOS_LEVEL);
    if(rc != MQTTCLIENT_SUCCESS) {
        deallocateMqttConnection(connection);
        free(address);
        free(topic);
        free(topicIndication);
        return ConnectError_SubscriptionToSIBTopic;
    }

    free(address);
    free(topic);
    free(topicIndication);
    
    if (connection->connectionEventsCallback != NULL){
        //Thread_start(monitorConnection, (void*) connection);
    }

    return Connection_Established;
}

KpMqtt_ConnectStatus KpMqtt_connectd(const char* server, const char* port, mqtt_credentials* credentials,
                                genericSsapCallback* messageReceivedCallback, void* callbackContext,
                                void* connectionEventsCallback, void* connectionEventsContext,
                                mqtt_connection** created_connection) {
    *created_connection = MqttConnection_allocated(NULL, credentials, messageReceivedCallback, callbackContext,
        connectionEventsCallback, connectionEventsContext);
    return KpMqtt_connectSSL(created_connection, server, port, NULL, NULL);
}

KpMqtt_ConnectStatus KpMqtt_connect(mqtt_connection** connection, const char* server, const char* port){
    return KpMqtt_connectSSL(connection, server, port, NULL, NULL);
}

KpMqtt_ConnectStatus KpMqtt_connectSSLd(const char* server, const char* port, mqtt_credentials* credentials, const char* ca_file, const char* ca_path,
                                   genericSsapCallback* messageReceivedCallback, void* callbackContext,
                                   void* connectionEventsCallback, void* connectionEventsContext,
                                   mqtt_connection** created_connection){
   *created_connection = MqttConnection_allocate(NULL, credentials, messageReceivedCallback, callbackContext,
        connectionEventsCallback, connectionEventsContext);
    return KpMqtt_connectSSL(created_connection, server, port, ca_file, ca_path);
}

KpMqtt_ConnectStatus KpMqtt_connectSSL(mqtt_connection** connection, const char* server, const char* port,
                                                const char* ca_file, const char* ca_path){
    KpMqtt_ConnectStatus retval = establishConnection(*connection, server, port, ca_file, ca_path);
    if (retval != Connection_Established)
        *connection = NULL;
    return retval;
}

KpMqtt_DisconnectStatus KpMqtt_disconnect(mqtt_connection* connection, int timeout) {
    
    int timeoutToUse = timeout;
    int unsubscribe_rc = MQTTCLIENT_SUCCESS;
    
    if(!connection->lost && MQTTClient_isConnected(connection->mqttClient)){
        // Unsubscribe from the KP and indication topics
        size_t topic_length = strlen(SSAP_RESPONSES_TOPIC);
        size_t clientId_length = strlen(connection->clientId);

        char* topic=(char*) malloc((topic_length + clientId_length +1)*sizeof(char));
        if (topic == NULL)
            return DisconnectError_InternalError;
        topic[0] = '\0';
        strcat(topic, SSAP_RESPONSES_TOPIC);
        strcat(topic, connection->clientId);

        int rc = MQTTClient_unsubscribe(connection->mqttClient, topic);
        free((char*)topic);
        if (rc != MQTTCLIENT_SUCCESS && rc != MQTTCLIENT_DISCONNECTED && rc != SOCKET_ERROR){
            unsubscribe_rc = DisconnectError_InternalError;
        }
        
        topic_length = strlen(SSAP_INDICATIONS_TOPIC);
        char* topicIndication=(char*) malloc((topic_length + clientId_length +1)*sizeof(char));
        if (topicIndication == NULL)
            return DisconnectError_InternalError;
        topicIndication[0] = '\0';
        strcat(topicIndication, SSAP_INDICATIONS_TOPIC);
        strcat(topicIndication, connection->clientId);

        rc = MQTTClient_unsubscribe(connection->mqttClient, topicIndication);
        free((char*)topicIndication);
        if (rc != MQTTCLIENT_SUCCESS && rc != MQTTCLIENT_DISCONNECTED && rc != SOCKET_ERROR){
            unsubscribe_rc = DisconnectError_InternalError;
        }        
       
    } else {
        timeoutToUse = 0;
    }
    
    if (connection->connectionEventsCallback != NULL){
        // Wait for the connection watchdog to terminate
        Thread_lock_mutex(connection->watchdogMutex);
        Thread_unlock_mutex(connection->watchdogMutex);        
    }
    
    
    int crd = MQTTCLIENT_SUCCESS;
    
    if (MQTTClient_isConnected(connection->mqttClient)){
        crd = MQTTClient_disconnect(connection->mqttClient, timeoutToUse);
    }
    
    deallocateMqttConnection(connection);
    
    if(crd != MQTTCLIENT_SUCCESS) {
        return DisconnectError_SocketError;
    } else if (unsubscribe_rc != MQTTCLIENT_SUCCESS){
        return DisconnectWarning_MqttSubscriptionErrors;
    } else {
        return Connection_Closed;
    }
}

KpMqtt_SendStatus KpMqtt_send(mqtt_connection* connection, ssap_message* request, int timeout) {
    //Pasa el mensaje a JSON
    char* payload = ssap_messageToJson(request);
    freeSsapMessage(request);
    return publish(connection, SIB_GW_TOPIC, payload,timeout);
}

void KpMqtt_setIndicationListener(mqtt_connection* connection, ssapIndicationCallback* callback, void* callbackContext) {
    connection->indicationContext = callbackContext;
    connection->indicationCallback = callback;
}

void setIndicationListener(mqtt_connection* connection, ssapIndicationCallback* callback, void* callbackContext) {
    KpMqtt_setIndicationListener(connection, callback, callbackContext);
}

mqtt_credentials* KpMqtt_buildCredentials(const char* username, const char* password){
  mqtt_credentials* result = (mqtt_credentials*) malloc(sizeof(mqtt_credentials));
  result->username = malloc((strlen(username) + 1) * sizeof(char));
  strcpy(result->username, username);
  result->password = malloc((strlen(password) + 1) * sizeof(char));
  strcpy(result->password, password);
  return result;
}

mqtt_credentials* buildCredentials(const char* username, const char* password){
    return KpMqtt_buildCredentials(username, password);
}

void KpMqtt_freeOpenSSLTables(){
    MQTTClient_freeOpenSSLTables();
}