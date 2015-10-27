# Copyright 2013-15 Indra Sistemas S.A.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License. 

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Get rid of Cygwin warnings

project(api_ssap C)

# Initialize warning flags. By default, we'll complain about anything.
set(all_warnings_and_debug "-g -Wall -Wextra")
set(no_warnings "-w -g")

# Define default compiler warnings for the project
set(${PROJECT_NAME}_C_FLAGS ${all_warnings_and_debug} CACHE STRING "Default project C FLAGS" FORCE)

# Define default compiler warnings for shared libraries
set(${PROJECT_NAME}_LIBS_C_FLAGS ${no_warnings} CACHE STRING "Default project C FLAGS" FORCE)

# Set the compiler flags
set(CMAKE_C_FLAGS ${${PROJECT_NAME}_C_FLAGS})

# Link the executable with all the libraries
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(cjson)
add_subdirectory(paho-mqtt-c)
add_subdirectory(ssap)

file(GLOB EXAMPLES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} examples/*.c)
foreach(example ${EXAMPLES})
  string(REPLACE ".c" "" executable_name ${example})
  string(REPLACE "examples/" "" executable_name ${executable_name})
  add_executable(${executable_name} ${example})
  target_link_libraries(${executable_name} ssap paho-mqtt-c cjson) 
  target_link_libraries(${executable_name} pthread m uuid) 
  target_link_libraries(${executable_name} dl ssl crypto) 
endforeach(example ${EXAMPLES})
