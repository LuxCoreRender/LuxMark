
###########################################################################
#
# Configuration
#
###########################################################################

#cmake -DLUXMARK_CUSTOM_CONFIG=cmake/SpecializedConfig/Config_Dade.cmake .

MESSAGE(STATUS "Using Dade's Linux Configuration settings")

SET(BOOST_SEARCH_PATH		"/home/david/projects/luxrender-dev/boost_1_56_0-bin")
SET(OPENIMAGEIO_ROOT_DIR	"/home/david/projects/luxrender-dev/oiio/dist/linux64")

SET(OPENCL_SEARCH_PATH	"$ENV{AMDAPPSDKROOT}")
SET(OPENCL_INCLUDEPATH	"${OPENCL_SEARCH_PATH}/include")

SET(EMBREE_SEARCH_PATH		"/home/david/src/embree-bin-2.4_linux")

#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")
