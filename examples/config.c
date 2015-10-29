/**
  @examples config.c
  @author Indra Sistemas S.A.
  @date Oct 29 2015
  @version 4.2
  @brief An example on retrieving KP configuration from the Sofia2 platform.
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

typedef struct {
  char* sessionKey;
  volatile int config_received;
} context_t;

void messageReceivedHandler(ssap_message* response, void* context){
  context_t* typed_context = (context_t*) context;
  switch (response->messageType) {
    case CONFIG:
      printf("A CONFIG response was received!\n");
      printf("Response body: %s\n", response->body);
	  typed_context->config_received = 1;
      break;
	default:
		break;
  }  
  return;  
}

int main(){
  context_t* context = malloc(sizeof(context_t));
  context->sessionKey = NULL;
  context->config_received = 0;

  printf("Example: send a CONFIG message to the SIB.\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  ConnectionStatus status = KpMqtt_connect(&connection, "sofia2.com", "1880");
  if (status != CONNECTED){
    printf("Oops! Something went wrong...\n");    
  }

  ssap_message *configMessage = generateConfigMessage("KPvisualizacionHT", "KPvisualizacionHT01", "cbb9364c434543a18dc6efa30dc780eb", NULL, NULL);
  
  SendStatus send_status = KpMqtt_send(connection, configMessage, 1000);
  if (send_status == SENT){
    printf("The CONFIG message was sent\n");   
  }
  
  while (!context->config_received)
	sleep(1);
  
  KpMqtt_disconnect(connection, 1000);  
  free(context); 
  printf("Exit!\n");
  KpMqtt_freeOpenSSLTables();  
  return 0;
}