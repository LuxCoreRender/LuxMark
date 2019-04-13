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
#   LuxMark website: https://www.luxcorerender.org                        #
###########################################################################

###########################################################################
#
# LuxMark binary directory
#
###########################################################################

IF (WIN32)

ELSE (WIN32)

	set(LUXMARK_LINUX64_BIN_DIR "luxmark-linux64-v2.1beta2")
	set(LUXMARK_WIN32_BIN_DIR "luxmark-win32-v2.1beta2")

	# Win32
	add_custom_command(
	    OUTPUT "${LUXMARK_WIN32_BIN_DIR}"
	    COMMAND rm -rf ${LUXMARK_WIN32_BIN_DIR}
	    COMMAND mkdir ${LUXMARK_WIN32_BIN_DIR}
	    COMMAND cp -r samples/luxmark/scenes ${LUXMARK_WIN32_BIN_DIR}
		COMMAND rm -rf ${LUXMARK_WIN32_BIN_DIR}/scenes/room
	    COMMAND cp samples/luxmark/exe-32bit/*.* ${LUXMARK_WIN32_BIN_DIR}
		COMMAND mv ${LUXMARK_WIN32_BIN_DIR}/LuxMark.exe ${LUXMARK_WIN32_BIN_DIR}/LuxMark-win32.exe
	    COMMAND cp AUTHORS.txt COPYING.txt README.txt ${LUXMARK_WIN32_BIN_DIR}
	    COMMENT "Building ${LUXMARK_WIN32_BIN_DIR}")
	
	add_custom_command(
	    OUTPUT "${LUXMARK_WIN32_BIN_DIR}.zip"
	    COMMAND zip -r ${LUXMARK_WIN32_BIN_DIR}.zip ${LUXMARK_WIN32_BIN_DIR}
	    COMMAND rm -rf ${LUXMARK_WIN32_BIN_DIR}
	    DEPENDS ${LUXMARK_WIN32_BIN_DIR}
	    COMMENT "Building ${LUXMARK_WIN32_BIN_DIR}.zip")
	
	# Linux64
	add_custom_command(
	    OUTPUT "${LUXMARK_LINUX64_BIN_DIR}"
	    COMMAND rm -rf ${LUXMARK_LINUX64_BIN_DIR}
	    COMMAND mkdir ${LUXMARK_LINUX64_BIN_DIR}
	    COMMAND cp -r samples/luxmark/scenes ${LUXMARK_LINUX64_BIN_DIR}
	    COMMAND cp bin/luxmark ${LUXMARK_LINUX64_BIN_DIR}/luxmark-linux64
	    COMMAND cp AUTHORS.txt COPYING.txt README.txt ${LUXMARK_LINUX64_BIN_DIR}
		COMMAND cp samples/luxmark/linux-64bit/* ${LUXMARK_LINUX64_BIN_DIR}
	    COMMENT "Building ${LUXMARK_LINUX64_BIN_DIR}")

	add_custom_command(
	    OUTPUT "${LUXMARK_LINUX64_BIN_DIR}.zip"
	    COMMAND zip -r ${LUXMARK_LINUX64_BIN_DIR}.zip ${LUXMARK_LINUX64_BIN_DIR}
	    DEPENDS ${LUXMARK_LINUX64_BIN_DIR}
	    COMMENT "Building ${LUXMARK_LINUX64_BIN_DIR}.zip")
	
	add_custom_target(luxmark_all_zip DEPENDS "${LUXMARK_LINUX64_BIN_DIR}.zip" "${LUXMARK_WIN32_BIN_DIR}.zip")
ENDIF(WIN32)
