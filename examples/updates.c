/**
  @example updates.c
  @author Indra Sistemas S.A.
  @date Oct 30 2015
  @version 4.3
  @brief An example on updating data on the RTDB.
  In this example we establish a MQTT connection with the SIB, create a session and
  update some documents stored in the RTDB that match a criterion.
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
#include "../ssap/KpMQTT.h"
#include "../ssap/sleeps.h"

#define ONTOLOGY "SensorHumedad"

#define NATIVE_UPDATE "db.SensorHumedad.update({\"SensorHumedad.identificador\" : {$eq: \"ST-Jesus\"}}, {\"SensorHumedad.identificador\" : {$set : {\"kkk\"}}})"
#define SQLLIKE_UPDATE "update SensorHumedad set SensorHumedad.identificador=\"kkk\" where SensorHumedad.identificador = \"ST-Jesus\";"
#define KP "SensorHumedadKP:SensorHumedadKPInstance01"
#define TOKEN "87d95afa2e87456e96a822e49495d1d1"

typedef struct {
  char* sessionKey;
  volatile int join_received;
  volatile int leave_received;
  volatile int update_received;
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
    case UPDATE:
      printf("An UPDATE response was received!\n");
      printf("Body: %s\n", response->body);
	  typed_context->update_received = 1;
      break;
    default:
      break;
  }  
  return;  
}

int main(){
  context_t* context = malloc(sizeof(context_t));
  context->sessionKey = NULL;
  context->join_received = 0;
  context->leave_received = 0;
  context->update_received = 0;

  printf("Example: update data in the RTDB.\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  KpMqtt_ConnectStatus status = KpMqtt_connect(&connection, "sofia2.com", "1880");
  if (status != Connection_Established){
    printf("Oops! Something went wrong...\n");    
  }
  
  ssap_message *joinMessage = generateJoinMessage(TOKEN, KP);
  
  KpMqtt_SendStatus send_status = KpMqtt_send(connection, joinMessage, 1000);
  if (send_status == Ssap_Message_Sent){
    printf("The JOIN message has been sent\n");   
  }
  
  while (!context->join_received)
	sleep(1);
  
  ssap_message *updateMessage = generateUpdateMessage(context->sessionKey, ONTOLOGY, NATIVE_UPDATE);
  send_status = KpMqtt_send(connection, updateMessage, 1000);
  
  if (send_status == Ssap_Message_Sent){
	  printf("The NATIVE UPDATE message has been sent\n");
  }

  while (!context->update_received)
	sleep(1);
  
  context->update_received = 0;
  updateMessage = generateUpdateMessageWithQueryType(context->sessionKey, ONTOLOGY, SQLLIKE_UPDATE, SQLLIKE);
  KpMqtt_send(connection, updateMessage, 1000);
  
  if (send_status == Ssap_Message_Sent){
	  printf("The SQL-LIKE UPDATE message has been sent\n");
  }

  while (!context->update_received)
	sleep(1); 
  
  ssap_message *leaveMessage = generateLeaveMessage(context->sessionKey);
  
  send_status = KpMqtt_send(connection, leaveMessage, 1000);
  if (send_status == Ssap_Message_Sent){
    printf("The LEAVE message has been sent\n");   
  }

  while (!context->leave_received){
	  sleep(1);
  }
  
  KpMqtt_DisconnectStatus status1 = KpMqtt_disconnect(connection, 100);
  if (status1 != Connection_Closed){
    printf("Oops! Something went wrong...\n");    
  }
  free(context->sessionKey);
  free(context);
  printf("Exit!\n");  
  KpMqtt_freeOpenSSLTables();
  return 0;
}