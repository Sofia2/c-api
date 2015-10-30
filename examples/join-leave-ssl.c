/**
  @examples join-leave-ssh.c
  @author Indra Sistemas S.A.
  @date Oct 29 2015
  @version 4.2
  @brief An example of the simplest interaction with the Sofia2 platform.
  In this example we establish a MQTT connection over SSL with the SIB, and then create 
  and close a session using the JOIN and LEAVE SSAP messages.
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
  volatile int leave;
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
          typed_context->leave = 1;
      break;
    case LEAVE:
      printf("A LEAVE response was received!\n");
          typed_context->disconnect = 1;
      break;
    default:
      break;
  }  
  return;  
}

int main(){
  context_t* context = malloc(sizeof(context_t));
  context->sessionKey = NULL;
  context->leave = 0;
  context->disconnect = 1;

  printf("Example: establish and close a MQTT over SSL connection with the SIB.\n");
  mqtt_connection* connection = MqttConnection_allocate();
  MqttConnection_setRandomClientId(connection);
  MqttConnection_setSsapCallback(connection, messageReceivedHandler, (void*) context);
  KpMqtt_ConnectStatus status = KpMqtt_connectSSL(&connection, "sofia2.com", "8883", NULL, "/etc/ssl/certs");
  if (status != Connection_Established){
    printf("Oops! Something went wrong...\n");
  }  
  
  ssap_message *joinMessage = generateJoinMessage("2cbbe5655fcb4545be5039fc6fb511de", "KpName:KpName01");
  
  KpMqtt_SendStatus send_status = KpMqtt_send(connection, joinMessage, 1000);
  if (send_status == MessageSent){
    printf("The JOIN message was sent\n");   
  }   
  while (context->sessionKey == NULL)
        sleep(1);
  
  ssap_message *leaveMessage = generateLeaveMessage(context->sessionKey);
  
  send_status = KpMqtt_send(connection, leaveMessage, 1000);
  if (send_status == MessageSent){
    printf("The LEAVE message was sent\n");   
  } 
  
  while (!context->disconnect)
        sleep(1);
  
  KpMqtt_DisconnectStatus status1 = KpMqtt_disconnect(connection, 100);
  if (status1 != DISCONNECTED){
    printf("Oops! Something went wrong while disconnecting from the SIB...\n");    
  } 
  
  free(context->sessionKey);
  free(context); 
  printf("Exit!\n");  
  KpMqtt_freeOpenSSLTables();
  return 0;
}
