/**
  @example inserts.c
  @author Indra Sistemas S.A.
  @date Oct 29 2015
  @version 4.2
  @brief An example on sending data to store at the RTDB
  In this example we establish a MQTT connection with the SIB, create a session and
  send data to the SIB.
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

#define NATIVE_INSERT_DATA "\"SensorHumedad\":{ \"identificador\":\"ST-TA3231\",\"timestamp\":{\"$date\": \"2014-01-30T17:14:00Z\"},\"medida\":28.6,\"unidad\":\"string\",\"geometry\":{\"type\":\"Point\", \"coordinates\":[9,19.3]}}}"
#define SQLLIKE_INSERT_DATA "INSERT INTO SensorHumedad (identificador, medida, unidad) VALUES (\"ST-TA3231-1HH\",50,\"C\");"
#define KP "SensorHumedadKP:SensorHumedadKPInstance01"
#define TOKEN "87d95afa2e87456e96a822e49495d1d1"

typedef struct {
  char* sessionKey;
  volatile int join_received;
  volatile int disconnect;
  volatile int insert_received;
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
	  typed_context->disconnect = 1;
      break;
    case INSERT:
      printf("An INSERT response was received!\n");
      printf("Body: %s\n", response->body);
	  typed_context->insert_received = 1;
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
  context->disconnect = 0;
  context->insert_received = 0;

  printf("Example: send data to the SIB.\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  KpMqtt_ConnectStatus status = KpMqtt_connect(&connection, "sofia2.com", "1880");
  if (status != Connection_Established){
    printf("Oops! Something went wrong...\n");    
  }
  
  ssap_message *joinMessage = generateJoinMessage(TOKEN, KP);
  printf("The JOIN message has been generated\n");
  
  KpMqtt_SendStatus send_status = KpMqtt_send(connection, joinMessage, 1000);
  if (send_status == MessageSent){
    printf("The JOIN message has been sent\n");   
  }
  
  while (!context->join_received)
	sleep(1);
  
  ssap_message *insertMessage = generateInsertMessage(context->sessionKey,ONTOLOGY, NATIVE_INSERT_DATA);
  send_status = KpMqtt_send(connection, insertMessage, 1000);

  if (send_status == MessageSent){
	  printf("The NATIVE INSERT message has been sent\n");
  }
  
  while (!context->insert_received)
	sleep(1);
  
  context->insert_received = 0;
  insertMessage = generateInsertMessageWithQueryType(context->sessionKey,ONTOLOGY, SQLLIKE_INSERT_DATA, SQLLIKE);
  send_status = KpMqtt_send(connection, insertMessage, 1000);

  if (send_status == MessageSent){
	  printf("The SQL-Like INSERT message has been sent\n");
  }
  
  while (!context->insert_received)
	sleep(1);
  
  ssap_message *leaveMessage = generateLeaveMessage(context->sessionKey);
  
  send_status = KpMqtt_send(connection, leaveMessage, 1000);
  if (send_status == MessageSent){
    printf("The LEAVE message has been sent\n");   
  }

  while (!context->disconnect)
	  sleep(1);
  
  KpMqtt_DisconnectStatus status1 = KpMqtt_disconnect(connection, 100);
  if (status1 != DISCONNECTED){
    printf("Oops! Something went wrong...\n");    
  }
  free(context->sessionKey);
  free(context);
  printf("Exit!\n"); 
  KpMqtt_freeOpenSSLTables();
  return 0;
}