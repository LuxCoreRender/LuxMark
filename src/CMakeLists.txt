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

set(LUXMARK_VERSION_MAJOR 3)
set(LUXMARK_VERSION_MINOR 0)

FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore QtGui QtNetwork QtMain)
IF(QT4_FOUND)
  include(${QT_USE_FILE})
  include_directories(${QT_INCLUDES} ${QT_QTNETWORK_INCLUDE_DIR})
ELSE(QT4_FOUND)
  message(STATUS "Is env variable QTDIR set correctly? Currently: $ENV{QTDIR}")
ENDIF(QT4_FOUND)


# Configure a header file to pass some of the CMake settings
# to the source code
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/luxmarkcfg.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/luxmarkcfg.h"
  )

#############################################################################
#
# LuxMark Docs
#
#############################################################################

find_package(Doxygen)

if(DOXYGEN_FOUND)
	message(STATUS "Found Doxygen and generating LuxMark documentation")

	# Generate doxygen.template
	set(DOXYGEN_LUXMARK_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/../doxygen/luxmark.template)
	configure_file(
	  "${DOXYGEN_LUXMARK_TEMPLATE}.in"
	  "${DOXYGEN_LUXMARK_TEMPLATE}"
	  )

	set(DOXYGEN_LUXMARK_INPUT ${CMAKE_CURRENT_BINARY_DIR}/../doxygen-luxmark.conf)
	set(DOXYGEN_LUXMARK_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/../doc/luxmark)
	set(DOXYGEN_LUXMARK_OUTPUT ${DOXYGEN_OUTPUT_DIR}/luxmark/html/index.html)

	message(STATUS "Doxygen LuxMark output: " ${DOXYGEN_LUXMARK_OUTPUT})

	if(DOXYGEN_DOT_FOUND)
			message(STATUS "Found dot")
			set(DOXYGEN_DOT_CONF "HAVE_DOT = YES")
	endif(DOXYGEN_DOT_FOUND)

	add_custom_command(
		OUTPUT ${DOXYGEN_LUXMARK_OUTPUT}
		# Creating custom doxygen-luxmark.conf
		COMMAND mkdir -p ${DOXYGEN_LUXMARK_OUTPUT_DIR}
		COMMAND cp ${DOXYGEN_LUXMARK_TEMPLATE} ${DOXYGEN_LUXMARK_INPUT}
		COMMAND echo "INPUT = " ${CMAKE_CURRENT_SOURCE_DIR} >> ${DOXYGEN_LUXMARK_INPUT}
		COMMAND echo "OUTPUT_DIRECTORY = " ${DOXYGEN_LUXMARK_OUTPUT_DIR} >> ${DOXYGEN_LUXMARK_INPUT}
		COMMAND echo ${DOXYGEN_DOT_CONF} >> ${DOXYGEN_LUXMARK_INPUT}
		# Launch doxygen
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_LUXMARK_INPUT}
		DEPENDS ${DOXYGEN_LUXMARK_TEMPLATE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)

	add_custom_target(apidoc_luxmark DEPENDS ${DOXYGEN_LUXMARK_OUTPUT})
endif(DOXYGEN_FOUND)

#############################################################################
#
# LuxMark binary
#
#############################################################################

set(QT_USE_QTOPENGL true)
include(${QT_USE_FILE})

set(LUXMARK_SRCS
	aboutdialog.cpp
    hardwaretree.cpp
	luxmarkapp.cpp
	main.cpp
	mainwindow.cpp
	luxvrdialog.cpp
    resultdialog.cpp
	submitdialog.cpp
	luxrendersession.cpp
	)
set(LUXMARK_MOC
	aboutdialog.h
	mainwindow.h
    hardwaretree.h
	luxmarkapp.h
	luxvrdialog.h
    resultdialog.h
	submitdialog.h
	)
set(LUXMARK_UIS
	aboutdialog.ui
	mainwindow.ui
	luxvrdialog.ui
    resultdialog.ui
	submitdialog.ui
	)
set(LUXMARK_RCS
	resources.qrc
	)

QT4_ADD_RESOURCES(LUXMARK_RC_SRCS ${LUXMARK_RCS})
QT4_WRAP_UI(LUXMARK_UI_HDRS ${LUXMARK_UIS})
# The next OPTIONS directive prevent the moc to include some boost files
# because qt 4 moc parser fails on some complexes macro definiton in boost >=
# 1.53.
QT4_WRAP_CPP(LUXMARK_MOC_SRCS ${LUXMARK_MOC} OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED -DBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

include_directories(".")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

set(LUXMARK_SRCS
	${LUXMARK_SRCS}
	${LUXMARK_UI_HDRS}
	${LUXMARK_RC_SRCS}
	${LUXMARK_MOC_SRCS}
	)

#############################################################################
#
#  APPLE CUSTOM GUI_TYPE MACOSX_BUNDLE 
#
#############################################################################

IF(APPLE)
	SET(GUI_TYPE MACOSX_BUNDLE)
	# SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${OPENSCENEGRAPH_MAJOR_VERSION}.${OPENSCENEGRAPH_MINOR_VERSION}.${OPENSCENEGRAPH_PATCH_VERSION}")
	# Short Version is the "marketing version". It is the version
	# the user sees in an information panel.
	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "v${LUXMARK_VERSION_MAJOR}.${LUXMARK_VERSION_MINOR}")
	# Bundle version is the version the OS looks at.
	SET(MACOSX_BUNDLE_BUNDLE_VERSION "v${LUXMARK_VERSION_MAJOR}.${LUXMARK_VERSION_MINOR}")
	SET(MACOSX_BUNDLE_GUI_IDENTIFIER "org.luxrenderk.luxrender" )
	SET(MACOSX_BUNDLE_BUNDLE_NAME "LuxMark" )
	SET(MACOSX_BUNDLE_ICON_FILE "LuxMark.icns")
	# SET(MACOSX_BUNDLE_COPYRIGHT "")
	# SET(MACOSX_BUNDLE_INFO_STRING "Info string, localized?")
ENDIF(APPLE)

#############################################################################

IF(APPLE)
	add_executable(LuxMark  ${GUI_TYPE} ${LUXMARK_SRCS})
	target_link_libraries(LuxMark ${LUXCORE_LIBRARY} ${SLG_LIBRARY} ${LUXRAYS_LIBRARY} ${EMBREE_LIBRARY} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} ${GLEW_LIBRARY} ${GLUT_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENCL_LIBRARIES} ${OPENIMAGEIO_LIBRARIES} ${TIFF_LIBRARIES} ${OPENEXR_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES})

	set_target_properties(LuxMark PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/osx-bundle/Info.plist)
	ADD_CUSTOM_COMMAND(
		TARGET LuxMark POST_BUILD
			COMMAND rm -rf ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/scenes
			COMMAND rm -rf ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/Resources
			COMMAND rm -rf ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/SmallluxGPU
			COMMAND mkdir ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/Resources
			COMMAND mkdir ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/SmallluxGPU
			COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/osx-bundle/LuxMark.icns ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/Resources/
			COMMAND cp ${OSX_DEPENDENCY_ROOT}/bin/slg4 ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/SmallluxGPU/ # bundle slg
			COMMAND install_name_tool -change @loader_path/libembree.2.4.0.dylib @loader_path/../MacOS/libembree.2.4.0.dylib ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/SmallluxGPU/slg4
			COMMAND cp ${OSX_DEPENDENCY_ROOT}/lib/embree2/libembree.2.4.0.dylib ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents/MacOS # bundle embree
			COMMAND cp -R ${CMAKE_SOURCE_DIR}/scenes ${CMAKE_BINARY_DIR}/bin/$(CONFIGURATION)/LuxMark.app/Contents
)
ELSE(APPLE)
	ADD_EXECUTABLE(luxmark ${LUXMARK_SRCS})

	TARGET_LINK_LIBRARIES(luxmark ${LUXCORE_LIBRARY} ${SLG_LIBRARY} ${LUXRAYS_LIBRARY} ${EMBREE_LIBRARY} ${Boost_LIBRARIES} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES} ${GLEW_LIBRARY} ${GLUT_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENCL_LIBRARIES} ${OPENIMAGEIO_LIBRARIES} ${TIFF_LIBRARIES} ${OPENEXR_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES})
	
	# This instructs FREEGLUT to emit a pragma for the static version
	SET_TARGET_PROPERTIES(luxmark PROPERTIES COMPILE_DEFINITIONS FREEGLUT_STATIC)

	if(WIN32)
		set_target_properties(luxmark PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
		set_target_properties(luxmark PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
		set_target_properties(luxmark PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
		set_target_properties(luxmark PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
		set_target_properties(luxmark PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
		set_target_properties(luxmark PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
	endif(WIN32)
ENDIF(APPLE)
