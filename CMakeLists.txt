#------------------------------------------------------------------------------#
# OpenMOC CMakeLists.txt
#------------------------------------------------------------------------------#

project(OpenMOC)
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(OPENMOC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
LIST( APPEND CMAKE_MODULE_PATH "${OPENMOC_DIR}" )

#------------------------------------------------------------------------------#
# OPTIONS
#------------------------------------------------------------------------------#

# Set the default build type (debug)
if( NOT CMAKE_BUILD_TYPE)
  set( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: Debug Release."
       FORCE)
endif()

# Optional libraries, etc.
SET( USE_SILO YES CACHE BOOL
  "Enable SILO."
)
SET( USE_IMAGEMAGICK YES CACHE BOOL
  "Enable IMAGEMAGICK."
)
SET( USE_OPENMC false CACHE BOOL
  "Enable OPENMC."
)
# Switches
SET( STORE_PREFACTORS true CACHE BOOL
  "Number of energy groups."
)
SET( NUM_ENERGY_GROUPS 7 CACHE INTEGER
  "Number of energy groups."
)
SET( NUM_ENERGY_GROUPS 7 CACHE INTEGER
  "Number of energy groups."
)
# Constants
SET( NUM_POLAR_ANGLES 3 CACHE INTEGER
  "Number of polar angles."
)
SET( NUM_ENERGY_GROUPS 7 CACHE INTEGER
  "Number of energy groups."
)
SET( NUM_KEFFS_TRACKED 3 CACHE INTEGER
  "Number of energy groups."
)
SET( MAX_ITERATIONS 3000 CACHE INTEGER
  "Maximum number of fixed source iterations allowed."
)
SET( FSR_HASHMAP_PRECISION 5 CACHE INTEGER
  "Number of significant digits for computing hashmap exponential prefactors."
)
SET( SIGMA_T_THRESH 1E-3 CACHE DOUBLE
  "Error threshold for determining how close the sum of sigma_a and sigma_s must match that of sigma_t for each energy group."
)
SET( ON_SURFACE_THRESH 1E-12 CACHE DOUBLE
  "Error threshold for determining how close a point needs to be to a surface to be considered on it."
)
SET( ON_LATTICE_CELL_THRESH 1E-12 CACHE DOUBLE
  "Error threshold for determining how close to the boundary of a lattice cell a point needs to be to be considered on it."
)
SET( TINY_MOVE 1E-12 CACHE DOUBLE
  "Distance a point is moved to cross over a surface into a new cell during track segmentation."
)

# Write config file, install it, and include that directory for all source
set(OPENMOC_CONFIG_IN  "${CMAKE_CURRENT_SOURCE_DIR}/src/configurations.h.in")
set(OPENMOC_CONFIG_OUT "${PROJECT_BINARY_DIR}/configurations.h")
message("OpenMOC CONFIGURATION = ${OPENMOC_CONFIG_IN}")
configure_file(${OPENMOC_CONFIG_IN} ${OPENMOC_CONFIG_OUT})
include_directories("${PROJECT_BINARY_DIR}/config")

#------------------------------------------------------------------------------#
# EXTERNAL LIBRARIES
#------------------------------------------------------------------------------#

# XML parsing
find_package(Expat REQUIRED)
include_directories(${EXPAT_INCLUDE_DIRS} )

if(USE_SILO)
    find_package(Silo)
    if(SILO_FOUND)
        include_directories(${Silo_INCLUDE_DIR})
    else()
        set( SERMENT_ENABLE_SILO OFF )
        message( STATUS "Disabling Silo since it was not found." )
    endif()
endif()
if(USE_IMAGEMAGICK)
    FIND_PACKAGE(ImageMagick COMPONENTS Magick++)
    if(ImageMagick_FOUND)
        include_directories(${ImageMagick_INCLUDE_DIRS})
        message(" imagemagick include dir = ${ImageMagick_INCLUDE_DIRS}")
        message(" imagemagick include dir = ${ImageMagick_LIBRARIES}")
    else()
        set(USE_IMAGEMAGICK OFF)
        message( STATUS "Disabling ImageMagick since it was not found." )
    endif()
endif()
if(USE_OPENMC)
    find_package(OpenMC)
    if(OPENMP_FOUND)
        include_directories(${OpenMP_CXX_FLAGS})
    else()
        set(USE_OPENMC false)
        message( STATUS "Disabling OpenMP since it was not found." )
    endif()
endif()

#------------------------------------------------------------------------------#
# SOURCE
#------------------------------------------------------------------------------#

include_directories(${OPENMOC_DIR}/src)
add_subdirectory(${OPENMOC_DIR}/src)

#-------- OUTPUT ------------------------------------------------------#

message("++++ System:      ${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR}")
message("++++ CXX:         ${CMAKE_CXX_COMPILER}")
message("++++ Build type:  ${CMAKE_BUILD_TYPE}")
if( USE_SILO )
message("++++ Silo enabled")
endif()
if( USE_IMAGEMAGICK )
message("++++ ImageMagick enabled")
endif()
if( USE_OPENMP)
message("++++ OpenMP enabled")
endif()



