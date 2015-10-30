/**
  @examples disconnections.c
  @author Indra Sistemas S.A.
  @date Oct 29 2015
  @version 4.2
  @brief An example on detecting disconnections from the SIB server asynchronously.
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
  @see http://sofia2.com/desarrollador_en.html
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "../ssap/KpMQTT.h"
#include "../ssap/sleeps.h"

typedef struct {
  char* sessionKey;
  volatile int disconnect;
} context_t;

void messageReceivedHandler(ssap_message* response, void* context){
  context_t* typed_context = (context_t*) context;
  switch (response->messageType) {
    case JOIN:
      printf("A JOIN response was received\n");
      printf("Session key: %s\n", response->sessionKey);
      typed_context->sessionKey = (char*) malloc((strlen(response->sessionKey) + 1) * sizeof(char));
      strcpy(typed_context->sessionKey, response->sessionKey);
      break;
    default:
      break;
  }  
  return;  
}


void connectionEventsHandler(ConnectionEvent event, void* context){
    printf("Handling MQTT disconnection event...\n");
    context_t* typed_context = (context_t*) context;
    typed_context->disconnect = 1;
}

int main(){
  context_t* context = malloc(sizeof(context_t));
  context->sessionKey = NULL;
  context->disconnect = 0;

  printf("Example: detecting and handling disconnections.\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setConnectTimeout(connection, 20);
  MqttConnection_setKeepAliveInterval(connection, 10);
  MqttConnection_setRetryInterval(connection, 2);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  MqttConnection_setConnectionEventsCallback(connection, connectionEventsHandler, (void*) context);
  KpMqtt_ConnectStatus status = KpMqtt_connect(&connection, "sofia2.com", "1880");
  if (status != Connection_Established){
    printf("Unable to connect to the SIB\n");
    goto exit;
  }  
  
  ssap_message *joinMessage = generateJoinMessage("859ff38deae8442d90d3a525a0d837dd", "KP_Drone_Joystick:KP_Drone_Joystick01");
  
  KpMqtt_SendStatus send_status = KpMqtt_send(connection, joinMessage, 1000);
  if (send_status == Ssap_Message_Sent){
    printf("The JOIN message was sent\n");   
  }   
  while (context->sessionKey == NULL)
        sleep(1);
  
  
  printf("Release your IP address now!\n");
  while (!context->disconnect){
     printf("The connection with the SIB hasn't been lost yet. Sleeping...\n");
     sleep(5);
  }
  
  KpMqtt_DisconnectStatus status1 = KpMqtt_disconnect(connection, 0);
  if (status1 != DISCONNECTED){
    printf("Oops! Something went wrong while disconnecting from the SIB...\n");    
  } 
exit:
  if (context->sessionKey != NULL)
    free(context->sessionKey);
  free(context);
  printf("Exit!\n");  
  KpMqtt_freeOpenSSLTables();
  return 0;
}