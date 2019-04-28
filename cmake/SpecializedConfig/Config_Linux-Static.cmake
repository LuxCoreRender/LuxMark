
###########################################################################
#
# Configuration
#
###########################################################################

#cmake -DLUXRAYS_CUSTOM_CONFIG=cmake/SpecializedConfig/Config_Linux-Static.cmake .

MESSAGE(STATUS "Using Linux static own settings")

SET(CMAKE_INCLUDE_PATH "${LuxRays_SOURCE_DIR}/../target-64-sse2/include;${LuxRays_SOURCE_DIR}/../target-64-sse2")
SET(CMAKE_LIBRARY_PATH "${LuxRays_SOURCE_DIR}/../target-64-sse2/lib;${LuxRays_SOURCE_DIR}/../target-64-sse2")
SET(Qt5_DIR "${LuxRays_SOURCE_DIR}/../target-64-sse2/lib/cmake/Qt5")

#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")
