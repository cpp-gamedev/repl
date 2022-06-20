cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # enable PIC
set(CMAKE_DEBUG_POSTFIX "-d")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug) # ensure CMAKE_BUILD_TYPE is not empty
endif()

# project parameters
set(project_name repl)
set(project_version "0.1.0")
set(project_version_name repl_version) # name of variable in generated header
set(cxx_standard cxx_std_20)

project(${project_name} VERSION "${project_version}")
set(is_root_project OFF) # indicate if this is the top-level project

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(is_root_project ON)
  message(STATUS "${PROJECT_NAME} is top level")
endif()

# header text
if(is_root_project)
  set(build_config ${CMAKE_BUILD_TYPE})

  if(CMAKE_CONFIGURATION_TYPES)
    set(build_config "Multi-config")
  endif()

  message(STATUS "[${CMAKE_SYSTEM_PROCESSOR}] [${CMAKE_SYSTEM_NAME}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}] [${build_config}]")
endif()

# version
function(configure_version)
  set(in "${CMAKE_CURRENT_SOURCE_DIR}/src/${project_version_name}.hpp.in")

  if(EXISTS "${in}")
    set(out "${CMAKE_CURRENT_BINARY_DIR}/${project_version_name}.hpp.in")
    message(STATUS "Configuring ${out}")
    configure_file("${in}" "${out}")
  endif()
endfunction()

configure_version()

# exe
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
  src/main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE src "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
get_target_property(sources ${PROJECT_NAME} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

if(GNU)
  target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror=return-type)
endif()
