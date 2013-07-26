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

include(FindPkgMacros)
getenv_path(LuxMark_DEPENDENCIES_DIR)

#######################################################################
# Core dependencies
#######################################################################

# Find threading library
FIND_PACKAGE(Threads REQUIRED)

# Find FreeImage
find_package(FreeImage)

if (FreeImage_FOUND)
	include_directories(SYSTEM ${FreeImage_INCLUDE_DIRS})
endif ()

# Find Boost
set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT                  "${BOOST_SEARCH_PATH}")
#set(Boost_DEBUG                 ON)
set(Boost_MINIMUM_VERSION       "1.44.0")

set(Boost_ADDITIONAL_VERSIONS "1.47.0" "1.46.1" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0")

set(LUXMARK_BOOST_COMPONENTS thread filesystem system)
find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${LUXMARK_BOOST_COMPONENTS})
if (NOT Boost_FOUND)
        # Try again with the other type of libs
        if(Boost_USE_STATIC_LIBS)
                set(Boost_USE_STATIC_LIBS)
        else()
                set(Boost_USE_STATIC_LIBS OFF)
        endif()
        find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${LUXMARK_BOOST_COMPONENTS})
endif()

if (Boost_FOUND)
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	# Don't use old boost versions interfaces
	ADD_DEFINITIONS(-DBOOST_FILESYSTEM_NO_DEPRECATED)
endif ()


find_package(OpenGL)

if (OPENGL_FOUND)
	include_directories(SYSTEM ${OPENGL_INCLUDE_PATH})
endif()

set(GLEW_ROOT                  "${GLEW_SEARCH_PATH}")
if(NOT APPLE)
	find_package(GLEW)
endif()

# GLEW
if (GLEW_FOUND)
	include_directories(SYSTEM ${GLEW_INCLUDE_PATH})
endif ()


set(GLUT_ROOT                  "${GLUT_SEARCH_PATH}")
find_package(GLUT)

# GLUT
if (GLUT_FOUND)
	include_directories(SYSTEM ${GLUT_INCLUDE_PATH})
endif ()

set(OPENCL_ROOT                  "${OPENCL_SEARCH_PATH}")
find_package(OpenCL)
# OpenCL
if (OPENCL_FOUND)
	include_directories(SYSTEM ${OPENCL_INCLUDE_DIR} ${OPENCL_C_INCLUDE_DIR})
endif ()

#############################################################################
#############################################################################
##########################      Find LuxRays       ##########################
#############################################################################
#############################################################################

IF(APPLE)
	FIND_PATH(LUXRAYS_INCLUDE_DIRS NAMES luxrays/luxrays.h PATHS ${OSX_DEPENDENCY_ROOT}/include/LuxRays)
	FIND_LIBRARY(LUXRAYS_LIBRARY libluxrays.a ${OSX_DEPENDENCY_ROOT}/lib/LuxRays)
ELSE(APPLE)
	FIND_PATH(LUXRAYS_INCLUDE_DIRS NAMES luxrays/luxrays.h PATHS ../luxrays/include ${LuxRays_HOME}/include )
	FIND_LIBRARY(LUXRAYS_LIBRARY luxrays PATHS ../luxrays/lib ${LuxRays_HOME}/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist )
ENDIF(APPLE)

IF (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)
	MESSAGE(STATUS "LuxRays include directory: " ${LUXRAYS_INCLUDE_DIRS})
	MESSAGE(STATUS "LuxRays library directory: " ${LUXRAYS_LIBRARY})
	INCLUDE_DIRECTORIES(SYSTEM ${LUXRAYS_INCLUDE_DIRS})
ELSE (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)
	MESSAGE(FATAL_ERROR "LuxRays not found.")
ENDIF (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)


#############################################################################
#############################################################################
##########################        Find SLG         ##########################
#############################################################################
#############################################################################

IF(APPLE)
	FIND_PATH(SLG_INCLUDE_DIRS NAMES slg/slg.h PATHS ${OSX_DEPENDENCY_ROOT}/include/LuxRays)
	FIND_LIBRARY(SLG_LIBRARY libsmallluxgpu.a ${OSX_DEPENDENCY_ROOT}/lib/LuxRays)
ELSE(APPLE)
	FIND_PATH(SLG_INCLUDE_DIRS NAMES slg/slg.h PATHS ../luxrays/include)
	FIND_LIBRARY(SLG_LIBRARY smallluxgpu PATHS ../luxrays/lib ${LuxRays_HOME}/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist )
ENDIF(APPLE)

IF (SLG_INCLUDE_DIRS AND SLG_LIBRARY)
	MESSAGE(STATUS "SLG include directory: " ${SLG_INCLUDE_DIRS})
	MESSAGE(STATUS "SLG library directory: " ${SLG_LIBRARY})
	INCLUDE_DIRECTORIES(SYSTEM ${SLG_INCLUDE_DIRS})
ELSE (SLG_INCLUDE_DIRS AND SLG_LIBRARY)
	MESSAGE(FATAL_ERROR "SLG Library not found.")
ENDIF (SLG_INCLUDE_DIRS AND SLG_LIBRARY)


#############################################################################
#############################################################################
#######################        Find LuxRender         #######################
#############################################################################
#############################################################################

IF(APPLE)
	FIND_PATH(LUXRENDER_INCLUDE_DIRS NAMES core/lux.h PATHS ${LuxRender_SOURCE})
	FIND_LIBRARY(LUXRENDER_LIBRARY liblux.dylib ${LuxRender_BUILD}/${CMAKE_BUILD_TYPE})
ELSE(APPLE)
	FIND_PATH(LUXRENDER_INCLUDE_DIRS NAMES core/lux.h PATHS ../lux)
	FIND_LIBRARY(LUXRENDER_LIBRARY lux PATHS ../lux ${LuxRender_HOME}/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist )
ENDIF(APPLE)

IF (LUXRENDER_INCLUDE_DIRS AND LUXRENDER_LIBRARY)
	MESSAGE(STATUS "LuxRender include directory: " ${LUXRENDER_INCLUDE_DIRS})
	MESSAGE(STATUS "LuxRender library directory: " ${LUXRENDER_LIBRARY})
	INCLUDE_DIRECTORIES(SYSTEM ${LUXRENDER_INCLUDE_DIRS}/core)
	INCLUDE_DIRECTORIES(SYSTEM ${LUXRENDER_INCLUDE_DIRS})
ELSE (LUXRENDER_INCLUDE_DIRS AND LUXRENDER_LIBRARY)
	MESSAGE(FATAL_ERROR "LuxRender Library not found.")
ENDIF (LUXRENDER_INCLUDE_DIRS AND LUXRENDER_LIBRARY)
