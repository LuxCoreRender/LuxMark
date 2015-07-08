
###########################################################################
#
# Configuration
#
###########################################################################

#cmake -DLUXMARK_CUSTOM_CONFIG=cmake/SpecializedConfig/Config_Dade.cmake .

MESSAGE(STATUS "Using Dade's Linux Configuration settings")

#SET(BOOST_SEARCH_PATH		"/home/david/projects/luxrender-dev/boost_1_47_0")
#SET(OPENIMAGEIO_ROOT_DIR	"/home/david/projects/luxrender-dev/oiio-RB-1.3/dist/linux64")
#SET(OPENEXR_ROOT			"/usr/local")

SET(OPENCL_SEARCH_PATH	"$ENV{AMDAPPSDKROOT}")
SET(OPENCL_INCLUDEPATH	"${OPENCL_SEARCH_PATH}/include")
#SET(OPENCL_LIBRARYDIR	"${OPENCL_SEARCH_PATH}/lib/x86_64")
SET(EMBREE_SEARCH_PATH	"/home/david/src/embree-bin-2.3.3_linux")

#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")
