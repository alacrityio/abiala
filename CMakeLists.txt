# copyright defined in abiala/LICENSE.txt

cmake_minimum_required (VERSION 3.8)
project(abiala VERSION 0.1 LANGUAGES CXX C)

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

option(ABIALA_NO_INT128 "disable use of __int128" OFF)
option(ABIALA_ONLY_LIBRARY "define and build the ABIALA library" OFF)

if(NOT DEFINED SKIP_SUBMODULE_CHECK)
  execute_process(COMMAND git submodule status --recursive
                  COMMAND grep -c "^[+\-]"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  OUTPUT_VARIABLE submodule_status
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(submodule_status GREATER 0)
     message(FATAL_ERROR "git submodules are not up to date.
Please run the command 'git submodule update --init --recursive'.")
  endif()
endif()

find_package(Threads)
include(GNUInstallDirs)

add_library(abiala STATIC src/abi.cpp src/crypto.cpp include/alaio/fpconv.c)
target_include_directories(abiala PUBLIC 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include" 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/external/rapidjson/include"
                          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(ABIALA_NO_INT128)
target_compile_definitions(abiala PUBLIC ABIALA_NO_INT128)
endif()

add_library(abiala_module MODULE src/abiala.cpp src/abi.cpp src/crypto.cpp include/alaio/fpconv.c)
target_include_directories(abiala_module PUBLIC 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include;" 
                          "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/external/rapidjson/include" 
                          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(abiala_module ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(abiala_module PROPERTIES OUTPUT_NAME "abiala")

enable_testing()

add_executable(test_abiala src/test.cpp src/abiala.cpp src/ship.abi.cpp)
target_link_libraries(test_abiala abiala ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME test_abiala COMMAND test_abiala)

if(NOT ABIALA_NO_INT128)
    add_executable(test_abiala_template src/template_test.cpp src/abiala.cpp)
    target_link_libraries(test_abiala_template abiala ${CMAKE_THREAD_LIBS_INIT})
    add_test(NAME test_abiala_template COMMAND test_abiala_template)
endif()

add_executable(test_abiala_key src/key_test.cpp src/abiala.cpp)
target_link_libraries(test_abiala_key abiala ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME test_abiala_key COMMAND test_abiala_key)

add_executable(test_abiala_reflect src/reflect_test.cpp)
target_include_directories(test_abiala_reflect PRIVATE include)
add_test(NAME test_abiala_reflect COMMAND test_abiala_reflect)

# Causes build issues on some platforms
# add_executable(test_abiala_sanitize src/test.cpp src/abiala.cpp src/abi.cpp src/crypto.cpp include/alaio/fpconv.c)
# target_include_directories(test_abiala_sanitize PRIVATE include external/outcome/single-header external/rapidjson/include external/date/include)
# target_link_libraries(test_abiala_sanitize abiala -fno-omit-frame-pointer -fsanitize=address,undefined ${CMAKE_THREAD_LIBS_INIT})
# target_compile_options(test_abiala_sanitize PUBLIC -fno-omit-frame-pointer -fsanitize=address,undefined)
# add_test(NAME test_abiala_sanitize COMMAND test_abiala_sanitize)

# add_executable(fuzzer src/fuzzer.cpp src/abiala.cpp)
# target_include_directories(fuzzer PRIVATE external/outcome/single-header external/rapidjson/include external/date/include)
# target_link_libraries(fuzzer -fsanitize=fuzzer,address,undefined,signed-integer-overflow -fstandalone-debug ${CMAKE_THREAD_LIBS_INIT})
# target_compile_options(fuzzer PRIVATE -fsanitize=fuzzer,address,undefined,signed-integer-overflow -fstandalone-debug)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang|AppleClang)
    target_compile_options(abiala PRIVATE -Wall -Wextra -Wno-unused-parameter -fcolor-diagnostics)
    target_compile_options(test_abiala PRIVATE -Wall -Wextra -Wno-unused-parameter -fcolor-diagnostics)
endif()

if (NOT ABIALA_ONLY_LIBRARY)
add_subdirectory(tools)
endif()

if(ABIALA_INSTALL_COMPONENT)
   set(INSTALL_COMPONENT_ARGS COMPONENT ${ABIALA_INSTALL_COMPONENT} EXCLUDE_FROM_ALL)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/alaio DESTINATION include ${INSTALL_COMPONENT_ARGS})

install(TARGETS abiala
  EXPORT abiala
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} ${INSTALL_COMPONENT_ARGS}
	ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR} ${INSTALL_COMPONENT_ARGS}
)

install(EXPORT abiala
  DESTINATION "share/abiala"
  FILE abiala-targets.cmake
  ${INSTALL_COMPONENT_ARGS}
)

export(TARGETS abiala 
       FILE ${CMAKE_CURRENT_BINARY_DIR}/share/abiala/abiala-targets.cmake)
configure_file(abiala-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/share/abiala/abiala-config.cmake COPYONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/abiala/abiala-config.cmake
        DESTINATION "share/abiala"
        ${INSTALL_COMPONENT_ARGS})