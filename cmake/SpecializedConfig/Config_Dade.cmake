
###########################################################################
#
# Configuration
#
###########################################################################

#cmake -DLUXMARK_CUSTOM_CONFIG=cmake/SpecializedConfig/Config_Dade.cmake .

MESSAGE(STATUS "Using Dade's Linux Configuration settings")

#set(BOOST_SEARCH_PATH         "/home/david/projects/luxrender-dev/boost_1_47_0")
set(OPENIMAGEIO_ROOT_DIR        "/home/david/projects/luxrender-dev/oiio-RB-1.3/dist/linux64")
set(OPENEXR_ROOT                "/usr/local")

set(OPENCL_SEARCH_PATH        "$ENV{AMDAPPSDKROOT}")
set(OPENCL_INCLUDEPATH        "${OPENCL_SEARCH_PATH}/include")
#set(OPENCL_LIBRARYDIR         "${OPENCL_SEARCH_PATH}/lib/x86_64")

#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")
