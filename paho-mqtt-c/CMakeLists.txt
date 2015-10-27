# Disable all warnings for this shared library
set(CMAKE_C_FLAGS ${${PROJECT_NAME}_LIBS_C_FLAGS})
set(CMAKE_CXX_FLAGS ${${PROJECT_NAME}_LIBS_CXX_FLAGS})

# Uncomment the following line to enable SSL support
add_definitions(-DOPENSSL)

file(GLOB source_files *.c)
add_library(paho-mqtt-c SHARED ${source_files})