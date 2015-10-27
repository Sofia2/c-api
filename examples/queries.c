/**
  @examples queries.h
  @author Indra Sistemas S.A.
  @date Oct 20 2015
  @version 4.1
  @brief An example on retrieving data from the RTDB
  In this example we establish a MQTT connection with the SIB, create a session and
  retrieve data from the RTDB.
  @see http://sofia2.com/desarrollador_en.html for further information.
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
#include "../ssap/SsapMessageGenerator.h"
#include "../ssap/sleeps.h"

#define ONTOLOGY "SensorHumedad"
#define NATIVE_QUERY "db.SensorHumedad.find().limit(10)"
#define SQLLIKE_QUERY "select * from SensorHumedad where SensorHumedad.medida = 25 limit 3;"
#define SIB_DEFINED_QUERY "MiConsulta"
#define SIB_DEFINED_QUERY_WITH_PARAMS "selectAllWithParam"
#define KP "KPvisualizacionHT:KPvisualizacionHTInstance01"
#define TOKEN "cbb9364c434543a18dc6efa30dc780eb"

typedef struct {
  char* sessionKey;
  volatile int join_received;
  volatile int leave_received;
  volatile int query_received;
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
    case QUERY:
      printf("A QUERY response was received!\n");
      printf("Body: %s\n", response->body);
	  typed_context->query_received = 1;
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
  context->query_received = 0;

  printf("Example: send queries to the SIB\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  ConnectionStatus status = KpMqtt_connect(&connection, "sofia2.com", "1880");
  if (status != CONNECTED){
    printf("Oops! Something went wrong...\n");    
  }
  
  ssap_message *joinMessage = generateJoinMessage(TOKEN, KP);
  
  SendStatus send_status = KpMqtt_send(connection, joinMessage, 1000);
  if (send_status == SENT){
    printf("JOIN message sent\n");   
  }
  
  while (!context->join_received)
	sleep(1);
  
  printf("SQL-like query\n");
  ssap_message *queryMessage = generateQueryMessageWithQueryType(context->sessionKey, ONTOLOGY, SQLLIKE_QUERY, SQLLIKE);
  send_status = KpMqtt_send(connection, queryMessage, 1000);
  
  if (send_status == SENT){
	  printf("NATIVE QUERY message sent\n");
  }

  while (!context->query_received)
	sleep(1);
  
  printf("Native query\n");
  context->query_received = 0;
  queryMessage = generateQueryMessage(context->sessionKey, ONTOLOGY, NATIVE_QUERY);
  send_status = KpMqtt_send(connection, queryMessage, 1000);

  if (send_status == SENT){
	  printf("SQL-Like QUERY message sent\n");
  }
  
  while (!context->query_received)
	sleep(1);
  
  printf("SIB-defined query\n");
  context->query_received = 0;
  queryMessage = generateQueryMessageWithQueryType(context->sessionKey, ONTOLOGY, SIB_DEFINED_QUERY, SIB_DEFINED);
  send_status = KpMqtt_send(connection, queryMessage, 1000);
  
  if (send_status == SENT){
	  printf("SIB-defined QUERY message sent\n");
  }

  while (!context->query_received)
	sleep(1);
  
  printf("SIB-defined query with params\n");
  context->query_received = 0;
  map_t *params = createEmptyMap(1);
  pushKeyValuePair("PARAM1", "S_Temperatura_00001", params);
  queryMessage = generateSIBDefinedQueryMessageWithParams(context->sessionKey, SIB_DEFINED_QUERY_WITH_PARAMS, params);
  send_status = KpMqtt_send(connection, queryMessage, 1000);
  
  if (send_status == SENT){
	  printf("SIB-defined QUERY with parameters message sent\n");
  }

  while (!context->query_received)
	sleep(1);
  
  ssap_message *leaveMessage = generateLeaveMessage(context->sessionKey);
  
  send_status = KpMqtt_send(connection, leaveMessage, 1000);
  if (send_status == SENT){
    printf("LEAVE message sent\n");   
  }

  while (!context->leave_received)
	  sleep(1);
  
  DisconnectionStatus status1 = KpMqtt_disconnect(connection, 100);
  if (status1 != DISCONNECTED){
    printf("Oops! Something went wrong...\n");    
  }
  free(context->sessionKey);
  free(context);
  printf("Exit!\n");
  KpMqtt_freeOpenSSLTables();
  return 0;
}