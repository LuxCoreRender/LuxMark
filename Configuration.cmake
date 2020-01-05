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

###########################################################################
#
# Configuration
#
# Use cmake "-DLUXMARK_CUSTOM_CONFIG=YouFileCName" To define your personal settings
# where YouFileCName.cname must exist in one of the cmake include directories
# best to use cmake/SpecializedConfig/
# 
# To not load defaults before loading custom values define
# -DLUXMARK_NO_DEFAULT_CONFIG=true
#
# WARNING: These variables will be cached like any other
#
###########################################################################

IF (NOT LUXMARK_NO_DEFAULT_CONFIG)

    # Disable Boost automatic linking
    ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

ELSE(NOT LUXMARK_NO_DEFAULT_CONFIG)

    MESSAGE(STATUS "LUXMARK_NO_DEFAULT_CONFIG defined - not using default configuration values.")

ENDIF(NOT LUXMARK_NO_DEFAULT_CONFIG)

# Setup libraries output directory
SET (LIBRARY_OUTPUT_PATH
   ${PROJECT_BINARY_DIR}/lib
   CACHE PATH
   "Single Directory for all Libraries"
   )

# Setup binaries output directory
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/bin
   CACHE PATH
   "Single Directory for all binaries"
	)

#
# Overwrite defaults with Custom Settings
#
#IF (LUXMARK_CUSTOM_CONFIG)
#	MESSAGE(STATUS "Using custom build config: ${LUXMARK_CUSTOM_CONFIG}")
#	INCLUDE(${LUXMARK_CUSTOM_CONFIG})
#ENDIF (LUXMARK_CUSTOM_CONFIG)

