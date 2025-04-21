cmake_minimum_required(VERSION 3.4.1)

set(CPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp)

set(PUGIXML_SOURCE_DIR ${CPP_SOURCE_DIR}/pugixml) 

include_directories(
        ${CPP_SOURCE_DIR}                   
        ${CPP_SOURCE_DIR}/extstdio          
        ${PUGIXML_SOURCE_DIR}               
        
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/includes 
)

set(WS_MSVC_NORETURN " ")


file(GLOB SRC_FILES
        ${CPP_SOURCE_DIR}/*.cpp
        ${CPP_SOURCE_DIR}/extstdio/*.cpp
        ${CPP_SOURCE_DIR}/pdml_parser.cpp
        ${PUGIXML_SOURCE_DIR}/pugixml.cpp
)

add_library( 
        diagRevealer
        SHARED
        ${SRC_FILES}
)


find_library( 
        log-lib
        log )


set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs)

add_library(LIBWIRESHARK SHARED IMPORTED)
set_target_properties(LIBWIRESHARK PROPERTIES
        IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libwireshark.so)

add_library(LIBWSUTIL SHARED IMPORTED)
set_target_properties(LIBWSUTIL PROPERTIES
        IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libwsutil.so)

add_library(LIBWIRETAP SHARED IMPORTED)
set_target_properties(LIBWIRETAP PROPERTIES
        IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libwiretap.so)

add_library(LIBGTHREAD SHARED IMPORTED)
set_target_properties(LIBGTHREAD PROPERTIES
        IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libgthread-2.0.so)

add_library(LIBGOBJECT SHARED IMPORTED)
set_target_properties(LIBGOBJECT PROPERTIES
        IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libgobject-2.0.so)

add_library(LIBGMODULE SHARED IMPORTED)
set_target_properties(LIBGMODULE PROPERTIES
        IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libgmodule-2.0.so)

add_library(LIBGLIB SHARED IMPORTED)
set_target_properties(LIBGLIB PROPERTIES
        IMPORTED_LOCATION ${LIBS_DIR}/${ANDROID_ABI}/libglib-2.0.so)

target_link_libraries( 
        diagRevealer
        ${log-lib}
        LIBWIRESHARK
        LIBWSUTIL
        LIBWIRETAP
        LIBGTHREAD
        LIBGOBJECT
        LIBGMODULE
        LIBGLIB
        atomic 
        m      
)