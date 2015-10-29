/**
  @example bulk.c
  @author Indra Sistemas S.A.
  @date Oct 29 2015
  @version 4.2
  @brief Bulk (i.e. "multiple") requests to the SIB
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
#include "../ssap/MqttConnection.h"
#include "../ssap/sleeps.h"

#define ONTOLOGY "SensorHumedad"

#define NATIVE_INSERT_DATA "\"SensorHumedad\":{ \"identificador\":\"ST-TA3231\",\"timestamp\":{\"$date\": \"2014-01-30T17:14:00Z\"},\"medida\":28.6,\"unidad\":\"string\",\"geometry\":{\"type\":\"Point\", \"coordinates\":[9,19.3]}}}"
#define NATIVE_UPDATE "db.SensorHumedad.update({\"SensorHumedad.identificador\" : {$eq: \"ST-Jesus\"}}, {\"SensorHumedad.identificador\" : {$set : {\"kkk\"}}})"
#define NATIVE_DELETE "db.SensorHumedad.remove({\"SensorHumedad.identificador\":\"ST-TA3231-2\"});"

#define KP "SensorHumedadKP:SensorHumedadKPInstance01"
#define TOKEN "87d95afa2e87456e96a822e49495d1d1"

typedef struct {
  char* sessionKey;
  volatile int send_bulk;
  volatile int bulk_received;
  volatile int disconnect;
} context_t;

void messageReceivedHandler(ssap_message* response, void* context){
  context_t* typed_context = (context_t*) context;
  switch (response->messageType) {
    case JOIN:
      printf("A JOIN response was received!\n");
      printf("Session key: %s\n", response->sessionKey);
      typed_context->sessionKey = (char*) malloc((strlen(response->sessionKey) + 1) * sizeof(char));
      strcpy(typed_context->sessionKey, response->sessionKey);
	  typed_context->send_bulk = 1;
      break;
    case LEAVE:
      printf("A LEAVE response was received!\n");
	  typed_context->disconnect = 1;
      break;
    case BULK:
      printf("A BULK response was received!\n");
      printf("Body: %s\n", response->body);
	  typed_context->bulk_received = 1;
      break;
    default:
      break;
  }  
  return;  
}

int main(){
  context_t* context = malloc(sizeof(context_t));
  context->sessionKey = NULL;
  context->send_bulk = 0;
  context->bulk_received = 0;
  context->disconnect = 0;
  printf("Example: send bulk requests to the SIB.\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  ConnectionStatus status = KpMqtt_connect(&connection, "sofia2.com", "1880");
  if (status != CONNECTED){
    printf("Oops! Something went wrong...\n");    
  }
  
  ssap_message *joinMessage = generateJoinMessage(TOKEN, KP);
  printf("The JOIN message has been generated\n");
  
  KpMqtt_SendStatus send_status = KpMqtt_send(connection, joinMessage, 1000);
  if (send_status == MessageSent){
    printf("The JOIN message has been sent\n");   
  }
  
  while (!context->send_bulk)
	sleep(1);
  
  printf("Building insert, update and remove messages\n");
  
  ssap_message *insertMessage = generateInsertMessage(context->sessionKey,ONTOLOGY, NATIVE_INSERT_DATA);
  ssap_message *updateMessage = generateUpdateMessage(context->sessionKey, ONTOLOGY, NATIVE_UPDATE);  
  ssap_message *removeMessage = generateDeleteMessage(context->sessionKey, ONTOLOGY, NATIVE_DELETE);
  
  printf("Building BULK request\n");
  bulkRequest *br = createBulkRequest(3);
  addToBulkRequest(insertMessage, br);
  addToBulkRequest(updateMessage, br);
  addToBulkRequest(removeMessage, br);
  
  printf("Building SSAP BULK message\n");
  ssap_message *bulkMessage = generateBulkMessage(context->sessionKey, ONTOLOGY, br);
  
  printf("Sending BULK request to the SIB\n");
  KpMqtt_send(connection, bulkMessage, 1000);
  printf("The BULK request has been sent\n");

  while (!context->bulk_received)
	sleep(1);
  
  ssap_message *leaveMessage = generateLeaveMessage(context->sessionKey);
  
  send_status = KpMqtt_send(connection, leaveMessage, 1000);
  if (send_status == MessageSent){
    printf("The LEAVE message has been sent\n");   
  }

  while (!context->disconnect)
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