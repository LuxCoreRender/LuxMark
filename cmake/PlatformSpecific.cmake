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

# Use relative paths
# This is mostly to reduce path size for command-line limits on windows
if(WIN32)
  set(CMAKE_USE_RELATIVE_PATHS true)
  set(CMAKE_SUPPRESS_REGENERATION true)
endif(WIN32)


###########################################################################
#
# Compiler Flags
#
###########################################################################

IF(MSVC)
	message(STATUS "MSVC")

	# Change warning level to something saner
	# Force to always compile with W3
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
	endif()

	# Minimizes Windows header files
	ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
	# Do not define MIN and MAX macros
	ADD_DEFINITIONS(-DNOMINMAX)
	# Do not warn about standard but insecure functions
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	# Enable Unicode
	ADD_DEFINITIONS(-D_UNICODE)
	# Use static FreeImage
	ADD_DEFINITIONS(-DFREEIMAGE_LIB)
	# Enable SSE2/SSE/MMX
	ADD_DEFINITIONS(-D__SSE2__ -D__SSE__ -D__MMX__)
	
	IF(MSVC90)
		message(STATUS "Version 9")
		# Whole Program Opt. gui display fixed in cmake 2.8.5
		# See http://public.kitware.com/Bug/view.php?id=6794
		# /GL will be used to build the code but the selection is not displayed in the menu

		set(MSVC_RELEASE_COMPILER_FLAGS "/WX- /MP /Ox /Ob2 /Oi /Oy /GT /GL /Gm- /EHsc /MD /GS /fp:precise /Zc:wchar_t /Zc:forScope /GR /Gd /TP /GL /GF /Ot")
		set(MSVC_RELEASE_WITH_DEBUG_COMPILER_FLAGS "/Zi")

		set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   ${MSVC_RELEASE_COMPILER_FLAGS} ${MSVC_RELEASE_WITH_DEBUG_COMPILER_FLAGS}")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_RELEASE_COMPILER_FLAGS} ${MSVC_RELEASE_WITH_DEBUG_COMPILER_FLAGS}")
		
		#set(MSVC_RELEASE_LINKER_FLAGS "/LTCG /OPT:REF /OPT:ICF")
		#set(MSVC_RELEASE_WITH_DEBUG_LINKER_FLAGS "/DEBUG")
		#set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} ${MSVC_RELEASE_LINKER_FLAGS} ${MSVC_RELEASE_WITH_DEBUG_LINKER_FLAGS}")

		# currently not in release version but should be soon - in meantime linker will inform you about switching this flag automatically because of /GL
		set(MSVC_RELEASE_LINKER_FLAGS "/LTCG")
		set(STATIC_LIBRARY_FLAGS_RELEASE "${STATIC_LIBRARY_FLAGS_RELEASE} ${MSVC_RELEASE_LINKER_FLAGS}")

	ENDIF(MSVC90)

	IF(MSVC10)
		message(STATUS "Version 10")
		# Whole Program Opt. gui display fixed in cmake 2.8.5
		# See http://public.kitware.com/Bug/view.php?id=6794
		# /GL will be used to build the code but the selection is not displayed in the menu

		set(MSVC_RELEASE_COMPILER_FLAGS "/WX- /MP /Ox /Ob2 /Oi /Oy /GT /GL /Gm- /EHsc /MD /GS /arch:SSE2 /fp:precise /Zc:wchar_t /Zc:forScope /GR /openmp /Gd /TP /GL /GF /Ot")
		set(MSVC_RELEASE_WITH_DEBUG_COMPILER_FLAGS "/Zi")

		set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   ${MSVC_RELEASE_COMPILER_FLAGS} ${MSVC_RELEASE_WITH_DEBUG_COMPILER_FLAGS}")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_RELEASE_COMPILER_FLAGS} ${MSVC_RELEASE_WITH_DEBUG_COMPILER_FLAGS}")
		
		#set(MSVC_RELEASE_LINKER_FLAGS "/LTCG /OPT:REF /OPT:ICF")
		#set(MSVC_RELEASE_WITH_DEBUG_LINKER_FLAGS "/DEBUG")
		#set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} ${MSVC_RELEASE_LINKER_FLAGS} ${MSVC_RELEASE_WITH_DEBUG_LINKER_FLAGS}")

		# currently not in release version but should be soon - in meantime linker will inform you about switching this flag automatically because of /GL
		set(MSVC_RELEASE_LINKER_FLAGS "/LTCG")
		set(STATIC_LIBRARY_FLAGS_RELEASE "${STATIC_LIBRARY_FLAGS_RELEASE} ${MSVC_RELEASE_LINKER_FLAGS}")

	ENDIF(MSVC10)

ENDIF(MSVC)



IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -mssse3")
  IF(NOT CYGWIN)
	  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  ENDIF(NOT CYGWIN)

  SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -ftree-vectorize -fvariable-expansion-in-unroller")
  
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_EXE_LINKER_FLAGS -Wl,--version-script='${CMAKE_SOURCE_DIR}/cmake/exportmaps/linux_symbol_exports.map')
ENDIF()
