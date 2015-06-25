# Path Finding in Images
#
# Author: Bruno Alano Medina
# License: LGPL
cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# Setup the project
project( pather C )

# Where are the additional libraries installed? Note: provide includes
# path here, subsequent checks will resolve everything else
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/modules/" )

# Setup the include path of headers
include_directories( include )

# GCC Flags (also works for Clang)
if ( "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang" )
  message( STATUS "Setting the flags for ${CMAKE_C_COMPILER_ID} compiler" )
  add_definitions( --std=c99 )
endif()

# Setup the list of source files
set( PATHER_SOURCES 
  src/main.c
  src/imagem.c
  src/pather.c
)

# Output the sources that we will compile
message( STATUS "Will compile: ${PATHER_SOURCES}" )

# Create the executable
add_executable( ${PROJECT_NAME} ${PATHER_SOURCES} )

# Link the libraries
target_link_libraries( ${PROJECT_NAME} ${LIBS} m )
