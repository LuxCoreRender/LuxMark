###########################################################################
#   Copyright (C) 1998-2013 by authors (see AUTHORS.txt)                  #
#                                                                         #
#   This file is part of LuxMark.                                         #
#                                                                         #
#   LuxMark is free software; you can redistribute it and/or modify       #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 3 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   LuxMark is distributed in the hope that it will be useful,            #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program.  If not, see <http://www.gnu.org/licenses/>. #
#                                                                         #
#   LuxMark website: http://www.luxrender.net                             #
###########################################################################

MESSAGE(STATUS "CMAKE VERSION DETECTED " ${CMAKE_VERSION})


###########################################################################
#
# Check and configure cmake
#
###########################################################################
# Fresh start
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
#Remove the following when the version check is at least 2.8.4
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(LuxMark)

# Dont overconfigure
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limit configs" FORCE)


###########################################################################
#
# Include necessary submodules
#
###########################################################################

set(CMAKE_MODULE_PATH
	"${LuxMark_SOURCE_DIR}"
	"${LuxMark_SOURCE_DIR}/cmake"
	"${LuxMark_SOURCE_DIR}/cmake/Utils"
	"${LuxMark_SOURCE_DIR}/cmake/Packages"
	"${LuxMark_SOURCE_DIR}/cmake/SpecializedConfig"
)

INCLUDE(PlatformSpecific)
INCLUDE(Configuration)

# Install CMake modules
#add_subdirectory(CMake)

SET(LuxMark_INCLUDE_DIR "${LuxMark_SOURCE_DIR}/include")
include_directories("${LuxMark_INCLUDE_DIR}")

# When using single configuration generators like make
# cmake does need to know which of the possible configurations
# to generate
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "assure config" FORCE) # makes sure type is shown in cmake gui

message(STATUS "Building mode: " ${CMAKE_BUILD_TYPE})

# Use relative paths
# This is mostly to reduce path size for command-line limits on windows
if(WIN32)
  # This seems to break Xcode projects so definitely don't enable on Apple builds
  set(CMAKE_USE_RELATIVE_PATHS true)
  set(CMAKE_SUPPRESS_REGENERATION true)
endif(WIN32)

#
# Overwrite defaults with Custom Settings
#
IF(LUXMARK_CUSTOM_CONFIG)
	MESSAGE(STATUS "Using custom build config: ${LUXMARK_CUSTOM_CONFIG}")
	INCLUDE(${LUXMARK_CUSTOM_CONFIG})
ELSE(LUXMARK_CUSTOM_CONFIG)
  IF (WIN32)
    IF(MSVC)
      # Disable Boost automatic linking
      ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

      MESSAGE(STATUS "Using default WIN32 Configuration settings")
      IF(DEFINED ENV{LUX_WINDOWS_BUILD_ROOT} AND DEFINED ENV{LIB_DIR})
        MESSAGE(STATUS "Lux build environment variables found")
        MESSAGE(STATUS "  LUX_WINDOWS_BUILD_ROOT = $ENV{LUX_WINDOWS_BUILD_ROOT}")
        MESSAGE(STATUS "  INCLUDE_DIR = $ENV{INCLUDE_DIR}")
        MESSAGE(STATUS "  LIB_DIR = $ENV{LIB_DIR}")

        SET(OPENIMAGEIO_INCLUDE_DIR   "$ENV{INCLUDE_DIR}/OpenImageIO")
        SET(OPENEXR_ROOT              "$ENV{INCLUDE_DIR}/OpenEXR")
        #SET(OPENCL_SEARCH_PATH       "$ENV{LUX_WINDOWS_BUILD_ROOT}/include")
        SET(BOOST_SEARCH_PATH         "$ENV{INCLUDE_DIR}/Boost")
        SET(Boost_USE_STATIC_LIBS     ON)
        SET(BOOST_LIBRARYDIR          "$ENV{LIB_DIR}")
        SET(QT_HEADERS_DIR            "$ENV{INCLUDE_DIR}/Qt/include")
        SET(QT_QTCORE_INCLUDE_DIR     "$ENV{INCLUDE_DIR}/Qt/include/QtCore")
        SET(QT_QTCORE_LIBRARY_RELEASE "$ENV{LIB_DIR}/QtCore4.lib") 
        SET(QT_QTGUI_INCLUDE_DIR      "$ENV{INCLUDE_DIR}/Qt/include/QtGui")
        SET(QT_QMAKE_EXECUTABLE       "$ENV{LIB_DIR}/qmake.exe")
        SET(QT_BINARY_DIR             "$ENV{LIB_DIR}")

        ADD_DEFINITIONS(-DFREEIMAGE_LIB)
        ADD_DEFINITIONS(-DFREEGLUT_STATIC)
        ADD_DEFINITIONS(-DGLEW_STATIC)
      ENDIF()
    ENDIF(MSVC)
  ENDIF(WIN32)
ENDIF(LUXMARK_CUSTOM_CONFIG)

# Find dependencies
include(Dependencies)

if (NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required Boost files - Please check ${BOOST_SEARCH_PATH}")
endif()

if (NOT OPENIMAGEIO_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required OpenImageIO files - Please check ${OPENIMAGEIO_SEARCH_PATH}")
endif()

if (NOT OPENCL_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required OpenCL files - Please check ${OPENCL_SEARCH_PATH}")
endif()

if (NOT OPENGL_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required OpenGL files")
endif()

if (NOT GLEW_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required GLEW files, disabling samples build - Please check ${GLEW_SEARCH_PATH}")
endif()

if (NOT GLUT_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required GLUT files, disabling samples build - Please check ${GLUT_SEARCH_PATH}")
endif()

if(NOT WIN32)
	ADD_DEFINITIONS(-msse2 -mfpmath=sse -ftree-vectorize -funroll-loops -Wall -fPIC -DHAVE_PTHREAD_H)
endif()


###########################################################################
#
# LuxMark
#
###########################################################################

add_subdirectory(src)


###########################################################################
#
# For non win32 we'll have to copy everything to a single dir
#
###########################################################################

INCLUDE(AssembleBinDirs)
