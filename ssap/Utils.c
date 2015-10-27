/*
  @file Utils.c
  @author Indra Sistemas S.A.
  @author Carlo Gavazzi Automation S.p.A.
  @author Circutor S.A.
  @date Oct 20 2015
  @version 4.1
  @brief Utility functions that are called from the C API.
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
#include "Utils.h"
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <stddef.h>
#include <uuid/uuid.h>
#endif

char* replace_str(const char *str, const char *old, const char *new)
{
    char *ret, *r;
    const char *p, *q;
    size_t oldlen = strlen(old);
    size_t count, retlen, newlen = strlen(new);

    if (oldlen != newlen) {
        for (count = 0, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen)
            count++;
            /* this is undefined if p - str > PTRDIFF_MAX */
            retlen = p - str + strlen(p) + count * (newlen - oldlen);
    } else
        retlen = strlen(str);

    if ((ret = malloc(retlen + 1)) == NULL)
        return NULL;

    for (r = ret, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen) {
        /* this is undefined if q - p > PTRDIFF_MAX */
        ptrdiff_t l = q - p;
        memcpy(r, p, l);
        r += l;
        memcpy(r, new, newlen);
        r += newlen;
    }
    strcpy(r, p);

    return ret;
}
char* extractToken(const char *configMessageBody){
    char* token_heading = "\"token\":\"";
    char *start = strstr(configMessageBody, token_heading);
    char* token = malloc(33 * sizeof(char));
    strncpy(token, start + strlen(token_heading), 32);
    token[32] = '\0';
    return token;
}

char* generateUUID(){
    char* result = (char*) malloc(37 * sizeof(char));
#ifdef WIN32
    GUID guid;
    CoCreateGuid(&guid);
    sprintf(result, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
#else
        uuid_t uuid;
        uuid_generate(uuid);        
        uuid_unparse(uuid, result);
#endif
        return result;
}