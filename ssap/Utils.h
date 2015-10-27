/**
  @file Utils.h
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
#ifndef SSAP_UTILS_H
#define SSAP_UTILS_H
#ifdef WIN32
#ifdef SSAPUTILS_EXPORTS
#define SSAPUTILS_API __declspec(dllexport) 
#else
#define SSAPUTILS_API __declspec(dllimport) 
#endif // WIN32
#else
#define SSAPUTILS_API
#endif // not WIN32

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Replaces a substring in a source string with another.
 * @param str: the where the replacements will be made
 * @param oldstr: the string to be replaced
 * @param newstr: the replacement string
 * @return The result string
 */
SSAPUTILS_API char* replace_str(const char *str, const char *oldstr, const char *newstr);

/**
 * Extracts a token from a SSAP CONFIG message body.
 * @param configMessageBody: the body of a SSAP CONFIG message
 * @return The extracted token
*/
SSAPUTILS_API char* extractToken(const char *configMessageBody);

/**
 * Builds an Universally Unique Identifier (UUID)
 * @return The generated UUID
*/
char* generateUUID();

#ifdef __cplusplus
}
#endif
#endif // SSAP_UTILS_H