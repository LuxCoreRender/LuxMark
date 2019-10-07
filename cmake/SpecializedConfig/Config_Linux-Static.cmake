
###########################################################################
#
# Configuration
#
###########################################################################

#cmake -DLUXMARK_CUSTOM_CONFIG=cmake/SpecializedConfig/Config_Linux-Static.cmake .

MESSAGE(STATUS "Using Linux static own settings")

SET(CMAKE_INCLUDE_PATH "${LuxMark_SOURCE_DIR}/../target-64-sse2/include;${LuxMark_SOURCE_DIR}/../target-64-sse2")
SET(CMAKE_LIBRARY_PATH "${LuxMark_SOURCE_DIR}/../target-64-sse2/lib;${LuxMark_SOURCE_DIR}/../target-64-sse2")
#SET(Qt5_DIR "${LuxMark_SOURCE_DIR}/../target-64-sse2/lib/cmake/Qt5")

#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")
