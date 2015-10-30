/*
  @file SsapMessageGenerator.c
  @author Indra Sistemas S.A.
  @author Carlo Gavazzi Automation S.p.A.
  @author Circutor S.A.
  @date Oct 30 2015
  @version 4.3
  @brief Functions that manipulate SSAP messages.
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
#include "SsapMessageGenerator.h"
#include <stdlib.h>
#include <string.h>
#include "../cjson/cJSON.h"

ssap_message* allocateSsapMessage();
void freeMap(map_t *params);

ssap_message* generateJoinMessage(const char* token, const char* instance){
    ssap_message* joinMessage = allocateSsapMessage();
    cJSON *body;
    
    body=cJSON_CreateObject();  
    
    cJSON_AddItemToObject(body, "token", cJSON_CreateString(token));
    cJSON_AddItemToObject(body, "instance", cJSON_CreateString(instance));    
    
    joinMessage->body=cJSON_PrintUnformatted(body);
    joinMessage->direction = REQUEST;
    joinMessage->messageId=NULL;
    joinMessage->messageType=JOIN;
    joinMessage->ontology=NULL;
    joinMessage->sessionKey=NULL;
    joinMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body); 
    return joinMessage;
}


ssap_message* generateJoinRenewSessionMessage(const char* token, const char* instance, const char* sessionKey){
    ssap_message* joinMessage = allocateSsapMessage();
    cJSON *body;
    
    body=cJSON_CreateObject();  
    
    cJSON_AddItemToObject(body, "token", cJSON_CreateString(token));
    cJSON_AddItemToObject(body, "instance", cJSON_CreateString(instance));    
    
    joinMessage->body=cJSON_PrintUnformatted(body);
    joinMessage->direction = REQUEST;
    joinMessage->messageId=NULL;
    joinMessage->messageType=JOIN;
    joinMessage->ontology=NULL;
    joinMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(joinMessage->sessionKey, sessionKey);
    joinMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body); 
    return joinMessage;
}

ssap_message* generateLeaveMessage(const char* sessionKey){
    
    ssap_message* leaveMessage = allocateSsapMessage();
  
    leaveMessage->body=NULL;
    leaveMessage->direction = REQUEST;
    leaveMessage->messageId=NULL;
    leaveMessage->messageType=LEAVE;
    leaveMessage->ontology=NULL;
    leaveMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(leaveMessage->sessionKey, sessionKey);    
    leaveMessage->persistenceType=MONGODB;
    return leaveMessage;
        
}

ssap_message* generateInsertMessage(const char* sessionKey, const char* ontology, const char* data){
    return generateInsertMessageWithQueryType(sessionKey, ontology, data, NATIVE);
}

ssap_message* generateInsertMessageWithQueryType(const char* sessionKey, const char* ontology, const char* data, SSAPQueryType queryType){
    ssap_message* insertMessage = allocateSsapMessage();
    cJSON *body;    
    body=cJSON_CreateObject();    
    switch(queryType){
        case NATIVE:            
            cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("NATIVE"));
            cJSON_AddItemToObject(body, "query", cJSON_CreateNull());
            cJSON_AddItemToObject(body, "data", cJSON_CreateString(data));
            break;
        case SQLLIKE:
            cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SQLLIKE"));
            cJSON_AddItemToObject(body, "query", cJSON_CreateString(data));
            cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
            break;                    
       case BDH:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("BDH"));
           cJSON_AddItemToObject(body, "query", cJSON_CreateString(data));
           cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
           break;                     
        default:
            cJSON_AddItemToObject(body, "queryType", cJSON_CreateNull());
            cJSON_AddItemToObject(body, "query", cJSON_CreateNull());
            cJSON_AddItemToObject(body, "data", cJSON_CreateString(data));
            break;
    }
    
    insertMessage->body=cJSON_PrintUnformatted(body);
    insertMessage->direction = REQUEST;
    insertMessage->messageId = NULL;
    insertMessage->messageType=INSERT;
    
    insertMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
    strcpy(insertMessage->ontology, ontology);
    
    insertMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(insertMessage->sessionKey, sessionKey);
    
    insertMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body);
    return insertMessage;
}

ssap_message* generateUpdateMessage(const char* sessionKey, const char* ontology, const char* query){
    return generateUpdateMessageWithQueryType(sessionKey, ontology, query, NATIVE);
}

ssap_message* generateUpdateMessageWithQueryType(const char* sessionKey, const char* ontology, const char* query, SSAPQueryType queryType){
    ssap_message* updateMessage = allocateSsapMessage();
    cJSON *body;
    
    //Crea la raiz del JSON
    body=cJSON_CreateObject();  
    
    //Añade las propiedades al body
    cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
    cJSON_AddItemToObject(body, "query", cJSON_CreateString(query));
    switch(queryType){
       case NATIVE:             
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("NATIVE"));
           break; 
       case SQLLIKE:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SQLLIKE"));
           break;
       case BDH:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("BDH"));
           break;                     
       default:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateNull());
           break;
    }
    
    updateMessage->body=cJSON_PrintUnformatted(body);
    updateMessage->direction = REQUEST;
    updateMessage->messageId = NULL;
    updateMessage->messageType=UPDATE;
    
    updateMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
    strcpy(updateMessage->ontology, ontology);
    
    updateMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(updateMessage->sessionKey, sessionKey);
    
    updateMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body); 
    return updateMessage;
}

ssap_message* generateDeleteMessage(const char* sessionKey, const char* ontology, const char* query){
    return generateDeleteMessageWithQueryType(sessionKey, ontology, query, NATIVE);
}

ssap_message* generateDeleteMessageWithQueryType(const char * sessionKey, const char* ontology, const char* query, SSAPQueryType queryType){
    ssap_message* removeMessage = allocateSsapMessage();
    cJSON *body;
    
    //Crea la raiz del JSON
    body=cJSON_CreateObject();  
    
    //Añade las propiedades al body
    cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
    cJSON_AddItemToObject(body, "query", cJSON_CreateString(query));
    
    switch(queryType){
       case NATIVE:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("NATIVE"));
           break; 
       case SQLLIKE:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SQLLIKE"));
           break;                     
       case BDH:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("BDH"));
           break;                     
       default:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateNull());
           break;
    }
    
    removeMessage->body=cJSON_PrintUnformatted(body);
    removeMessage->direction = REQUEST;
    removeMessage->messageId = NULL;
    removeMessage->messageType=DELETE;
    removeMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
    strcpy(removeMessage->ontology, ontology);
    removeMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(removeMessage->sessionKey, sessionKey);
    removeMessage->persistenceType=MONGODB;
    cJSON_Delete(body);
    return removeMessage;
}

ssap_message* generateQueryMessage(const char* sessionKey, const char* ontology, const char* query){
    return generateQueryMessageWithQueryType(sessionKey, ontology, query, NATIVE);
}

ssap_message* generateQueryMessageWithQueryType(const char* sessionKey, const char* ontology, const char* query, SSAPQueryType queryType){
    ssap_message* queryMessage = allocateSsapMessage();
    cJSON *body;
    
    //Crea la raiz del JSON
    body=cJSON_CreateObject();  
    
    //Añade las propiedades al body
    cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
    cJSON_AddItemToObject(body, "query", cJSON_CreateString(query));
    switch(queryType){
       case NATIVE:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("NATIVE"));
           break; 
       case SQLLIKE:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SQLLIKE"));
           break;
       case SIB_DEFINED:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SIB_DEFINED"));
           break;                     
       case BDH:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("BDH"));
           break;                     
       default:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateNull());
           break;
    }
    
    queryMessage->body=cJSON_PrintUnformatted(body);
    queryMessage->direction = REQUEST;
    queryMessage->messageId = NULL;
    queryMessage->messageType=QUERY;
    
    if(ontology!=NULL){
        queryMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
        strcpy(queryMessage->ontology, ontology);
    }else{
        queryMessage->ontology = NULL;
    }
    
    queryMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(queryMessage->sessionKey, sessionKey);
    
    queryMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body);
    return queryMessage;
}

ssap_message* generateSIBDefinedQueryMessageWithParams(const char* sessionKey, const char* query, map_t *params){
    ssap_message* queryMessage = allocateSsapMessage();
    cJSON *body;
    
    body=cJSON_CreateObject();  
    
    cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
    cJSON_AddItemToObject(body, "query", cJSON_CreateString(query));
    cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SIB_DEFINED"));
    
    cJSON *queryParams;
    queryParams=cJSON_CreateObject();
    
    size_t i;
    for(i=0;i<params->size;i++){
        cJSON_AddItemToObject(queryParams, params->data[i].key, cJSON_CreateString(params->data[i].value));
    }
    
    cJSON_AddItemToObject(body, "queryParams", queryParams);
    
    queryMessage->body=cJSON_PrintUnformatted(body);
    queryMessage->direction = REQUEST;
    queryMessage->messageId = NULL;
    queryMessage->messageType=QUERY;
    queryMessage->ontology=NULL;
   
    queryMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(queryMessage->sessionKey, sessionKey);
    
    queryMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body); //This method frees the memory of the key-value pairs
    freeMap(params);
    return queryMessage;
}

ssap_message* generateSubscribeMessage(const char* sessionKey, const char* ontology, const char* query, int msRefresh){
    ssap_message* subscribeMessage = allocateSsapMessage();
    cJSON *body;
    
    body=cJSON_CreateObject();  
    
    cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
    cJSON_AddItemToObject(body, "query", cJSON_CreateString(query));
    cJSON_AddItemToObject(body, "msRefresh", cJSON_CreateNumber(msRefresh));
    
    subscribeMessage->body=cJSON_PrintUnformatted(body);
    subscribeMessage->direction = REQUEST;
    subscribeMessage->messageId = NULL;
    subscribeMessage->messageType=SUBSCRIBE;
    
    subscribeMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
    strcpy(subscribeMessage->ontology, ontology);
    
    subscribeMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(subscribeMessage->sessionKey, sessionKey);
    
    subscribeMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body);
    return subscribeMessage;
}


ssap_message* generateSubscribeMessageWithQueryType(const char* sessionKey, const char* ontology, const char* query, SSAPQueryType queryType, int msRefresh){
    ssap_message* subscribeMessage = allocateSsapMessage();
    cJSON *body;
    
    body=cJSON_CreateObject();  
    
    cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
    cJSON_AddItemToObject(body, "query", cJSON_CreateString(query));
    cJSON_AddItemToObject(body, "msRefresh", cJSON_CreateNumber(msRefresh));
    switch(queryType){
       case NATIVE:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("NATIVE"));
           break; 
       case SQLLIKE:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SQLLIKE"));
           break;
       case SIB_DEFINED:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SIB_DEFINED"));
           break;                     
       case CEP:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("CEP"));
           break;
        case BDH:
            cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("BDH"));
            break;                    
       default:
           cJSON_AddItemToObject(body, "queryType", cJSON_CreateNull());
           break;
    }
    
    subscribeMessage->body=cJSON_PrintUnformatted(body);
    subscribeMessage->direction = REQUEST;
    subscribeMessage->messageId = NULL;
    subscribeMessage->messageType=SUBSCRIBE;
    
    if(ontology!=NULL){
        subscribeMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
        strcpy(subscribeMessage->ontology, ontology);
    }else{
        subscribeMessage->ontology = NULL;
    }
    
    subscribeMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(subscribeMessage->sessionKey, sessionKey);
    
    subscribeMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body);
    return subscribeMessage;
}


ssap_message* generateSIBDefinedSubscribeMessageWithParam(const char* sessionKey, const char* query, map_t *params){
    ssap_message* queryMessage = allocateSsapMessage();
    cJSON *body;
    
    body=cJSON_CreateObject();  
    
    cJSON_AddItemToObject(body, "data", cJSON_CreateNull());
    cJSON_AddItemToObject(body, "query", cJSON_CreateString(query));
    cJSON_AddItemToObject(body, "queryType", cJSON_CreateString("SIB_DEFINED"));
    
    cJSON *queryParams;
    queryParams=cJSON_CreateObject();
    
    size_t i;
    for(i=0;i<params->size;i++){
        cJSON_AddItemToObject(queryParams, params->data[i].key, cJSON_CreateString(params->data[i].value));
    }
    
    cJSON_AddItemToObject(body, "queryParams", queryParams);
    
    queryMessage->body=cJSON_PrintUnformatted(body);
    queryMessage->direction = REQUEST;
    queryMessage->messageId = NULL;
    queryMessage->messageType=QUERY;
    queryMessage->ontology=NULL;
   
    queryMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(queryMessage->sessionKey, sessionKey);
    
    queryMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body); //This method frees the memory of the key-value pairs
    freeMap(params);
    return queryMessage;
}

ssap_message* generateUnsubscribeMessage(const char* sessionKey, const char* ontology, const char* suscriptionId){
    ssap_message* unsubscribeMessage = allocateSsapMessage();
    cJSON *body;
    
    body=cJSON_CreateObject();  
    
    cJSON_AddItemToObject(body, "idSuscripcion", cJSON_CreateString(suscriptionId));
   
    
    unsubscribeMessage->body=cJSON_PrintUnformatted(body);
    unsubscribeMessage->direction = REQUEST;
    unsubscribeMessage->messageId = NULL;
    unsubscribeMessage->messageType=UNSUBSCRIBE;
    
    unsubscribeMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
    strcpy(unsubscribeMessage->ontology, ontology);
    
    unsubscribeMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(unsubscribeMessage->sessionKey, sessionKey);
    
    unsubscribeMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body);
    return unsubscribeMessage;
}

ssap_message* allocateSsapMessage(){
  ssap_message* msg = (ssap_message*) malloc(sizeof(ssap_message));
  if (msg == NULL)
          return NULL;
  msg->body = NULL;
  msg->messageId = NULL;
  msg->ontology = NULL;
  msg->sessionKey = NULL;
  return msg;
}

void freeSsapMessage(ssap_message* message){
     if(message == NULL){
         return;
     }
     if(message->body != NULL){
         free((char*)message->body);
         message->body = NULL;
     }
     if(message->messageId != NULL){
         free((char*)message->messageId);
         message->messageId = NULL;
     }
     if(message->ontology != NULL){
         free((char*)message->ontology);
         message->ontology = NULL;
     }
     if(message->sessionKey != NULL){
         free((char*)message->sessionKey);
         message->sessionKey = NULL;
     }
     free(message);
}

char* messageTypeToString(SSAPMessageTypes messageType){
  switch(messageType){
        case JOIN:              return "JOIN";
        case LEAVE:             return "LEAVE";
        case INSERT:            return "INSERT";
        case UPDATE:            return "UPDATE";
        case DELETE:            return "DELETE";
        case QUERY:             return "QUERY";
        case SUBSCRIBE:         return "SUBSCRIBE";
        case UNSUBSCRIBE:       return "UNSUBSCRIBE";
        case INDICATION:        return "INDICATION";
        case BULK:              return "BULK";
        case CONFIG:            return "CONFIG";
        default:                return NULL;
    }  
}

char* ssap_messageToJson(ssap_message* message){
    cJSON *root;
    
    root=cJSON_CreateObject();  
    
    if(message->body==NULL || strcmp(message->body, "")==0){
        cJSON_AddItemToObject(root, "body", cJSON_CreateNull());
    }else{
         cJSON_AddItemToObject(root, "body", cJSON_CreateString(message->body));
    }
    
    switch(message->direction){
        case REQUEST: cJSON_AddItemToObject(root, "direction", cJSON_CreateString("REQUEST"));
                      break;
        case RESPONSE: cJSON_AddItemToObject(root, "direction", cJSON_CreateString("RESPONSE"));
                      break;
        default:      cJSON_AddItemToObject(root, "direction", cJSON_CreateNull());
                      break;
    }
    
    cJSON_AddItemToObject(root, "messageType", cJSON_CreateString(messageTypeToString(message->messageType)));
    
    if(message->messageId==NULL || strcmp(message->messageId, "")==0){
        cJSON_AddItemToObject(root, "messageId", cJSON_CreateNull());
    }else{
         cJSON_AddItemToObject(root, "messageId", cJSON_CreateString(message->messageId));
    }
    
    if(message->ontology==NULL || strcmp(message->ontology, "")==0){
        cJSON_AddItemToObject(root, "ontology", cJSON_CreateNull());
    }else{
         cJSON_AddItemToObject(root, "ontology", cJSON_CreateString(message->ontology));
    }
    
    
    if(message->sessionKey==NULL || strcmp(message->sessionKey, "")==0){
        cJSON_AddItemToObject(root, "sessionKey", cJSON_CreateNull());
    }else{
         cJSON_AddItemToObject(root, "sessionKey", cJSON_CreateString(message->sessionKey));
    }  
    char* retorno = cJSON_PrintUnformatted(root);
    
    cJSON_Delete(root); 
    
    return retorno;
}

ssap_message* ssapMessageFromJson(const char* source){

    ssap_message* message = allocateSsapMessage();
    
    cJSON *json = NULL;

    cJSON* body=NULL;
    cJSON* direction=NULL;
    cJSON* messageId=NULL;
    cJSON* messageType=NULL;
    cJSON* ontology=NULL;
    cJSON* sessionKey=NULL;

    json = cJSON_Parse(source);
    body = cJSON_GetObjectItem(json, "body");
    direction = cJSON_GetObjectItem(json, "direction");
    messageId = cJSON_GetObjectItem(json, "messageId");
    messageType = cJSON_GetObjectItem(json, "messageType");
    ontology = cJSON_GetObjectItem(json, "ontology");
    sessionKey = cJSON_GetObjectItem(json, "sessionKey");
    
    if(body == NULL){
        message->body=NULL;
    }else{
    	message->body = (char*)malloc((strlen(body->valuestring) + 1)*sizeof(char));
    	strcpy(message->body, body->valuestring);
    }
    
    if ((direction != NULL) && (direction->valuestring != NULL)) {
        if (strcasecmp(direction->valuestring, "REQUEST") == 0){
            message->direction=REQUEST;
        } else if(strcasecmp(direction->valuestring, "RESPONSE") == 0){
                message->direction=RESPONSE;
        } else {
            message->direction=ERROR;                
        }
	} else {
		message->direction=ERROR;
    }
    
    
    if(messageId == NULL || messageId->valuestring == NULL){
        message->messageId=NULL;
    }else{
        message->messageId = (char*) malloc((strlen(messageId->valuestring)+1)*sizeof(char));
        strcpy(message->messageId, messageId->valuestring);
    }
    
    if ((messageType != NULL) && (messageType->valuestring != NULL)) {
        if(strcasecmp(messageType->valuestring, "JOIN") == 0){
            message->messageType=JOIN;
        }else if(strcasecmp(messageType->valuestring, "LEAVE") == 0){
            message->messageType=LEAVE;
        }else if(strcasecmp(messageType->valuestring, "INSERT") == 0){
            message->messageType=INSERT;
        }else if(strcasecmp(messageType->valuestring, "UPDATE") == 0){
            message->messageType=UPDATE;
        }else if(strcasecmp(messageType->valuestring, "DELETE") == 0){
            message->messageType=DELETE;
        }else if(strcasecmp(messageType->valuestring, "QUERY") == 0){
            message->messageType=QUERY;
        }else if(strcasecmp(messageType->valuestring, "SUBSCRIBE") == 0){
            message->messageType=SUBSCRIBE;
        }else if(strcasecmp(messageType->valuestring, "UNSUBSCRIBE") == 0){
            message->messageType=UNSUBSCRIBE;
        }else if(strcasecmp(messageType->valuestring, "INDICATION") == 0){
            message->messageType=INDICATION;
        }else if (strcasecmp(messageType->valuestring, "BULK") == 0){
           message->messageType=BULK;      
        } else if (strcasecmp(messageType->valuestring, "CONFIG") == 0){
           message->messageType=CONFIG;      
        }
	} else {
        message->messageType=LEAVE;
    }
    
    if(ontology == NULL || ontology->valuestring==NULL){
        message->ontology=NULL;
    }else{
        message->ontology = (char*) malloc((strlen(ontology->valuestring)+1)*sizeof(char));
        strcpy(message->ontology, ontology->valuestring);
    }
  
    if(sessionKey == NULL||sessionKey->valuestring==NULL){
        message->sessionKey=NULL;
    }else{
        message->sessionKey = (char*) malloc((strlen(sessionKey->valuestring)+1)*sizeof(char));
        strcpy(message->sessionKey, sessionKey->valuestring);
    }
    
	cJSON_Delete(json);

    return message;
}

map_t* createEmptyMap(size_t size){
  map_t *result = (map_t*) malloc(sizeof(map_t));
  if (result == NULL)
	  return NULL;
  result->data = malloc(sizeof(keyValuePair) * size);
  if (result->data == NULL)
	  return NULL;
  result->size = 0;
  result->max_size = size;
  return result;
}

int pushKeyValuePair(const char* key, const char *value, map_t* params){
  if (params->size == params->max_size)
    return 0;
  keyValuePair *tmp = &(params->data[params->size]);
  tmp->key = (char*) malloc((strlen(key) + 1) * sizeof(char));
  if (tmp->key == NULL)
	  return 1;
  strcpy(tmp->key, key);
  tmp->value = (char*) malloc((strlen(value) + 3) * sizeof(char));
  if (tmp->value == NULL)
	  return 1;
  tmp->value[0] = '\0';
  strcat(tmp->value, "\"");
  strcat(tmp->value, value);
  strcat(tmp->value, "\"");
  params->size++;
  return 1;
}

void freeMap(map_t* params){
    size_t i;
    for (i = 0; i < params->size; i++){
      free(params->data[i].key);
      free(params->data[i].value);      
    }
    free(params->data);
    free(params);
}

bulkRequest* createBulkRequest(size_t max_size){
  bulkRequest* request = (bulkRequest*) malloc(sizeof(bulkRequest));
  if (request == NULL)
	  return NULL;
  request->size = 0;
  request->max_size = max_size;
  request->payload = (ssap_message**) malloc(max_size * sizeof(ssap_message*));
  return request;
}

int addToBulkRequest(ssap_message* message, bulkRequest* request){
   if (request->size == request->max_size)
     return 0;
   request->payload[request->size] = message;
   request->size++;  
   return 1;
}

void freeBulkRequest(bulkRequest* request){
  size_t i;
  for (i = 0; i < request->size; i++){
    freeSsapMessage(request->payload[i]);    
  }
  free(request->payload);
  free(request);  
}



ssap_message* generateBulkMessage(const char* sessionKey, const char* ontology, bulkRequest* request){
    ssap_message* bulkMessage = allocateSsapMessage();
    cJSON *body;
    
    body = cJSON_CreateArray();
    size_t i;
    for (i = 0; i < request->size; i++){
      cJSON *ssapBulkItem = cJSON_CreateObject();
      ssap_message* msg = request->payload[i];
      cJSON_AddItemToObject(ssapBulkItem, "type", cJSON_CreateString(messageTypeToString(msg->messageType)));
      cJSON_AddItemToObject(ssapBulkItem, "body", cJSON_CreateString(msg->body));
      cJSON_AddItemToObject(ssapBulkItem, "ontology", cJSON_CreateString(msg->ontology));
      cJSON_AddItemToArray(body, ssapBulkItem);
    }    
    
    bulkMessage->body=cJSON_PrintUnformatted(body);
    bulkMessage->direction = REQUEST;
    bulkMessage->messageId=NULL;
    bulkMessage->messageType=BULK;
    bulkMessage->ontology=(char*) malloc((strlen(ontology)+1)*sizeof(char));
    strcpy(bulkMessage->ontology, ontology);
    bulkMessage->sessionKey=(char*) malloc((strlen(sessionKey)+1)*sizeof(char));
    strcpy(bulkMessage->sessionKey, sessionKey);
    bulkMessage->persistenceType=MONGODB;
 
    cJSON_Delete(body);
    freeBulkRequest(request);
    return bulkMessage;
}

ssap_message* generateConfigMessage(const char* kp, const char* instance, const char* token, const char* assetService, map_t* assetServiceParam) {
    ssap_message* configMessage = allocateSsapMessage();
    cJSON *body;
    cJSON* assetServiceParam_json = NULL;
    
    body = cJSON_CreateObject();	
    
    cJSON_AddItemToObject(body, "token", cJSON_CreateString(token));
    cJSON_AddItemToObject(body, "instanciaKp", cJSON_CreateString(instance));
    cJSON_AddItemToObject(body, "kp", cJSON_CreateString(kp));
    
    if (assetService != NULL){
        cJSON_AddItemToObject(body, "assetService", cJSON_CreateString(assetService));
    } else {
        cJSON_AddItemToObject(body, "assetService", cJSON_CreateNull());
    }
    
    if (assetServiceParam != NULL){
        assetServiceParam_json = cJSON_CreateObject();
        size_t i;
        for(i = 0; i < assetServiceParam->size; i++){
            cJSON_AddItemToObject(assetServiceParam_json, assetServiceParam->data[i].key, cJSON_CreateString(assetServiceParam->data[i].value));
        }    
        
        cJSON_AddItemToObject(body, "assetServiceParam", cJSON_CreateString(cJSON_PrintUnformatted(assetServiceParam_json)));
    } else {
        cJSON_AddItemToObject(body, "assetServiceParam", cJSON_CreateNull());     
    }
    configMessage->body=cJSON_PrintUnformatted(body);
    configMessage->direction = REQUEST;
    configMessage->messageId=NULL;
    configMessage->messageType=CONFIG;
    configMessage->ontology=NULL;
    configMessage->sessionKey=NULL;
    configMessage->persistenceType=MONGODB;
    
    cJSON_Delete(body);
    if (assetServiceParam_json != NULL){
        cJSON_Delete(assetServiceParam_json);
        freeMap(assetServiceParam);
    }

    return configMessage;  
}