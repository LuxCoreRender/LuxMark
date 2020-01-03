###########################################################################
#   Copyright (C) 1998-2019 by authors (see AUTHORS.txt)                  #
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
#   LuxMark website: https://www.luxcorerender.org                        #
###########################################################################

include(FindPkgMacros)
getenv_path(LuxMark_DEPENDENCIES_DIR)

#######################################################################
# Core dependencies
#######################################################################

# Find threading library
find_package(Threads REQUIRED)

find_package(OpenImageIO REQUIRED)
include_directories(BEFORE SYSTEM ${OPENIMAGEIO_INCLUDE_DIR})
find_package(OpenEXR REQUIRED)

if(NOT APPLE)
    # Apple has these available hardcoded and matched in macos repo, see Config_OSX.cmake

    include_directories(BEFORE SYSTEM ${OPENEXR_INCLUDE_DIRS})
    find_package(TIFF REQUIRED)
    include_directories(BEFORE SYSTEM ${TIFF_INCLUDE_DIR})
    find_package(JPEG REQUIRED)
    include_directories(BEFORE SYSTEM ${JPEG_INCLUDE_DIR})
    find_package(PNG REQUIRED)
    include_directories(BEFORE SYSTEM ${PNG_PNG_INCLUDE_DIR})
	# Find Python Libraries
    if("${PYTHON_V}" EQUAL "27")
        find_package(PythonLibs 2.7)
    else()
        find_package(PythonLibs 3.4)
    endif()
endif()

include_directories(${PYTHON_INCLUDE_DIRS})

# Find Boost
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT                  "${BOOST_SEARCH_PATH}")
#set(Boost_DEBUG                 ON)
set(Boost_MINIMUM_VERSION       "1.56.0")

# For Windows builds, PYTHON_V must be defined as "3x" (x=Python minor version, e.g. "35")
# For other platforms, specifying python minor version is not needed
set(LUXRAYS_BOOST_COMPONENTS thread program_options filesystem serialization iostreams regex system python${PYTHON_V} chrono serialization numpy${PYTHON_V})
find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${LUXRAYS_BOOST_COMPONENTS})
if (NOT Boost_FOUND)
        # Try again with the other type of libs
        if(Boost_USE_STATIC_LIBS)
                set(Boost_USE_STATIC_LIBS OFF)
        else()
                set(Boost_USE_STATIC_LIBS ON)
        endif()
		message(STATUS "Re-trying with link static = ${Boost_USE_STATIC_LIBS}")
        find_package(Boost ${Boost_MINIMUM_VERSION} COMPONENTS ${LUXRAYS_BOOST_COMPONENTS})
endif()

if (Boost_FOUND)
	include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	# Don't use old boost versions interfaces
	ADD_DEFINITIONS(-DBOOST_FILESYSTEM_NO_DEPRECATED)
	if (Boost_USE_STATIC_LIBS)
		ADD_DEFINITIONS(-DBOOST_STATIC_LIB)
		ADD_DEFINITIONS(-DBOOST_PYTHON_STATIC_LIB)
	endif()
endif ()


# OpenGL
find_package(OpenGL)

if (OPENGL_FOUND)
	include_directories(BEFORE SYSTEM ${OPENGL_INCLUDE_PATH})
endif()

# OpenCL
set(OPENCL_ROOT                "${OPENCL_SEARCH_PATH}")
find_package(OpenCL)

if (OPENCL_FOUND)
	include_directories(BEFORE SYSTEM ${OPENCL_INCLUDE_DIR} ${OPENCL_C_INCLUDE_DIR})
endif ()

# Intel Embree
set(EMBREE_ROOT                "${EMBREE_SEARCH_PATH}")
find_package(Embree REQUIRED)

if (EMBREE_FOUND)
	include_directories(BEFORE SYSTEM ${EMBREE_INCLUDE_PATH})
endif ()

# Intel Oidn
set(OIDN_ROOT                "${OIDN_SEARCH_PATH}")
find_package(Oidn REQUIRED)

if (OIDN_FOUND)
	include_directories(BEFORE SYSTEM ${ODIN_INCLUDE_PATH})
endif ()

# Intel TBB
set(TBB_ROOT                   "${TBB_SEARCH_PATH}")
find_package(TBB REQUIRED)

if (TBB_FOUND)
	include_directories(BEFORE SYSTEM ${TBB_INCLUDE_PATH})
endif ()

# Blosc
set(BLOSC_ROOT                   "${BLOSC_SEARCH_PATH}")
find_package(Blosc REQUIRED)

if (BLOSC_FOUND)
	include_directories(BEFORE SYSTEM ${BLOSC_INCLUDE_PATH})
endif ()

# OpenMP
if(NOT APPLE)
	find_package(OpenMP)
	if (OPENMP_FOUND)
		MESSAGE(STATUS "OpenMP found - compiling with")
   		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
   		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	else()
		MESSAGE(WARNING "OpenMP not found - compiling without")
	endif()
endif()

set(Qt5_MODULES Widgets Network)
FIND_PACKAGE(Qt5 5.9.5 COMPONENTS ${Qt5_MODULES})
IF(NOT Qt5_FOUND)
  message(FATAL_ERROR "Is env variable Qt5_DIR set correctly? Currently: $Qt5_DIR")
ENDIF()
foreach(comp ${Qt5_MODULES})
    set(Qt5_LIBRARIES ${Qt5_LIBRARIES} Qt5::${comp})
endforeach()
MESSAGE(STATUS "Qt5 includes: ${Qt5Widgets_INCLUDE_DIRS}")
MESSAGE(STATUS "Qt5 libraries: ${Qt5Widgets_LIBRARIES}")

#############################################################################
#############################################################################
##########################      Find LuxRays       ##########################
#############################################################################
#############################################################################

FIND_PATH(LUXRAYS_INCLUDE_DIRS NAMES luxrays/luxrays.h PATHS ../luxrays/include ${LuxRays_HOME}/include )
FIND_PATH(LUXRAYS_INCLUDE_CFG_DIRS NAMES luxrays/cfg.h PATHS ../luxrays/include ${LuxRays_HOME}/generated/include )
FIND_LIBRARY(LUXRAYS_LIBRARY luxrays PATHS ../luxrays/lib ${LuxRays_HOME}/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist )

IF (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)
	MESSAGE(STATUS "LuxRays include directory: " ${LUXRAYS_INCLUDE_DIRS})
	MESSAGE(STATUS "LuxRays config include directory: " ${LUXRAYS_INCLUDE_CFG_DIRS})
	MESSAGE(STATUS "LuxRays library directory: " ${LUXRAYS_LIBRARY})
	INCLUDE_DIRECTORIES(SYSTEM ${LUXRAYS_INCLUDE_DIRS} ${LUXRAYS_INCLUDE_CFG_DIRS})
ELSE (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)
	MESSAGE(FATAL_ERROR "LuxRays not found.")
ENDIF (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)


#############################################################################
#############################################################################
########################        Find LuxCore         ########################
#############################################################################
#############################################################################

FIND_PATH(LUXCORE_INCLUDE_DIRS NAMES luxcore/luxcore.h PATHS ../luxrays/include ${LuxRays_HOME}/include)
FIND_PATH(LUXCORE_INCLUDE_CFG_DIRS NAMES luxcore/cfg.h PATHS ../luxrays/include ${LuxRays_HOME}/generated/include)
FIND_LIBRARY(OPENVDB_LIBRARY openvdb PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(OPENSUBDIV_LIBRARY opensubdiv PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(BCD_LIBRARY bcd PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(SLG_LIBRARY_CORE slg-core PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(SLG_LIBRARY_FILM slg-film PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(SLG_LIBRARY_KERNELS slg-kernels PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(LUXCORE_LIBRARY luxcore PATHS ../luxrays/lib ${LuxRays_HOME}/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)

IF(APPLE)
SET(OPENSUBDIV_LIBRARY ${OPENSUBDIV_LIBRARY} ${ZLIB_LIBRARY})
ENDIF(APPLE)

IF (LUXCORE_INCLUDE_DIRS AND LUXCORE_LIBRARY)
	SET(ALL_LUXCORE_LIBRARIES ${LUXCORE_LIBRARY} ${SLG_LIBRARY_CORE} ${SLG_LIBRARY_FILM} ${SLG_LIBRARY_KERNELS} ${LUXRAYS_LIBRARY}
		${BCD_LIBRARY} ${OPENVDB_LIBRARY} ${OPENSUBDIV_LIBRARY} ${OPENIMAGEIO_LIBRARIES}
		${BLOSC_LIBRARY} ${EMBREE_LIBRARY} ${OIDN_LIBRARY} ${TBB_LIBRARY} ${TIFF_LIBRARIES}
		${OPENEXR_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES})

	MESSAGE(STATUS "LuxCore include directory: " ${LUXCORE_INCLUDE_DIRS})
	MESSAGE(STATUS "LuxCore config include directory: " ${LUXCORE_INCLUDE_CFG_DIRS})
	MESSAGE(STATUS "LuxCore library directory: " ${LUXCORE_LIBRARY})
	MESSAGE(STATUS "LuxCore all libraries: " ${ALL_LUXCORE_LIBRARIES})
	INCLUDE_DIRECTORIES(SYSTEM ${LUXCORE_INCLUDE_DIRS} ${LUXCORE_INCLUDE_CFG_DIRS})
ELSE (LUXCORE_INCLUDE_DIRS AND LUXCORE_LIBRARY)
	MESSAGE(FATAL_ERROR "LuxCore Library not found.")
ENDIF (LUXCORE_INCLUDE_DIRS AND LUXCORE_LIBRARY)