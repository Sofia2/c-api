/**
  @examples subscribes.c
  @author Indra Sistemas S.A.
  @date Oct 29 2015
  @version 4.2
  @brief An example on creating subscriptions
  In this example we establish a MQTT connection with the SIB, create a session, subscribe to the changes of an
  ontology that match a criterion and send an instance of that ontology to the SIB. 
  The ontology instance that we'll send to the SIB matches the subscription criterion. Therefore,after storing the instance, 
  the SIB will send us an indication message with its data.
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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "../cjson/cJSON.h"
#include "../ssap/KpMQTT.h"
#include "../ssap/sleeps.h"

#define ONTOLOGY "SensorHumedad"

#define NATIVE_INSERT_DATA "\"SensorHumedad\":{ \"identificador\":\"ST-TA3231\",\"timestamp\":{\"$date\": \"2014-01-30T17:14:00Z\"},\"medida\":28.6,\"unidad\":\"string\",\"geometry\":{\"type\":\"Point\", \"coordinates\":[9,19.3]}}}"
#define NATIVE_SUBSCRIPTION_CRITERION "{SensorHumedad.medida:{$gt:1}}"

#define KP "SensorHumedadKP:SensorHumedadKPInstance01"
#define TOKEN "87d95afa2e87456e96a822e49495d1d1"

typedef struct {
  char* sessionKey;
  char* subscriptionId;
  volatile int join_received;
  volatile int leave_received;
  volatile int indication_received;
  volatile int subscribe_received;
  volatile int unsubscribe_received;
} context_t;

void messageReceivedHandler(ssap_message* response, void* context){
  context_t* typed_context = (context_t*) context;
  switch (response->messageType) {
    case JOIN:
      printf("A JOIN response was received!\n");
      printf("Session key: %s\n", response->sessionKey);
      typed_context->sessionKey = (char*) malloc((strlen(response->sessionKey) + 1) * sizeof(char));
      strcpy(typed_context->sessionKey, response->sessionKey);
	  typed_context->join_received = 1;
      break;
    case LEAVE:
      printf("A LEAVE response was received!\n");
	  typed_context->leave_received = 1;
      break;
    case INSERT:
      printf("An INSERT response was received!\n");
      printf("Body: %s\n", response->body);
      break;
    case SUBSCRIBE:
      printf("An SUBSCRIBE response was received!\n");
      printf("Body: %s\n", response->body);
	  cJSON* parsed_body = cJSON_Parse(response->body);
	  cJSON* subscriptionId = cJSON_GetObjectItem(parsed_body, "data");
	  typed_context->subscriptionId = (char*)malloc((strlen(subscriptionId->valuestring) + 1) * sizeof(char));
	  strcpy(typed_context->subscriptionId, subscriptionId->valuestring);
	  cJSON_Delete(parsed_body);
	  typed_context->subscribe_received = 1;
      break;
    case UNSUBSCRIBE:
      printf("An UNSUBSCRIBE response was received!\n");
      printf("Body: %s\n", response->body);
	  typed_context->unsubscribe_received = 1;
      break;
    default:
      break;
  }  
  return;  
}

void indicationReceivedCallback(ssap_message* indicationMessage, void* context){
  printf("SSAP INDICATION MESSAGE RECEIVED\n");
  printf("Body: %s\n", indicationMessage->body);
  context_t* typed_context = (context_t*)context;
  typed_context->indication_received = 1;
}

int main(){
  context_t* context = malloc(sizeof(context_t));
  context->sessionKey = NULL;
  context->indication_received = 0;
  context->join_received = 0;
  context->leave_received = 0;
  context->subscribe_received = 0;
  context->unsubscribe_received = 0;

  printf("Example: subscriptions\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  ConnectionStatus status = KpMqtt_connect(&connection, "sofia2.com", "1880");
  if (status != CONNECTED){
    printf("Oops! Something went wrong...\n");    
  }
  
  ssap_message *joinMessage = generateJoinMessage(TOKEN, KP);
  printf("The JOIN message has been generated\n");
  
  SendStatus send_status = KpMqtt_send(connection, joinMessage, 1000);
  if (send_status == SENT){
    printf("The JOIN message has been sent\n");   
  }
  
  while (!context->join_received)
	sleep(1);
  
  ssap_message *subscribeMessage = generateSubscribeMessage(context->sessionKey, ONTOLOGY, NATIVE_SUBSCRIPTION_CRITERION, 1000);
  setIndicationListener(connection, indicationReceivedCallback, context);
  send_status = KpMqtt_send(connection, subscribeMessage, 1000);
  if (send_status == SENT){
	  printf("The SUBSCRIBE message has been sent\n");
  }
  
  while (!context->subscribe_received)
	sleep(1);
  
  printf("Sending data to the SIB\n");  
  ssap_message *insertMessage = generateInsertMessage(context->sessionKey,ONTOLOGY, NATIVE_INSERT_DATA);
  send_status = KpMqtt_send(connection, insertMessage, 1000);
  if (send_status == SENT){
	  printf("The INSERT message has been sent\n");
  }
  
  while (!context->indication_received){
	  sleep(1);
  }

  ssap_message *unsubscribeMessage = generateUnsubscribeMessage(context->sessionKey, ONTOLOGY, context->subscriptionId);
  send_status = KpMqtt_send(connection, unsubscribeMessage, 1000);

  if (send_status == SENT){
	  printf("The UNSUBSCRIBE MESSAGE has been sent\n");
  }
 
  ssap_message *leaveMessage = generateLeaveMessage(context->sessionKey);
  
  send_status = KpMqtt_send(connection, leaveMessage, 1000);
  if (send_status == SENT){
    printf("The LEAVE message has been sent\n");   
  }
  
  while (!context->leave_received)
	  sleep(1);

  DisconnectionStatus status1 = KpMqtt_disconnect(connection, 100);
  if (status1 != DISCONNECTED){
    printf("Oops! Something went wrong...\n");    
  }
  free(context->sessionKey);
  free(context->subscriptionId);
  free(context);
  printf("Exit!\n");
  KpMqtt_freeOpenSSLTables();
  return 0;
}