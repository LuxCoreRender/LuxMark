
###########################################################################
#
# Configuration
#
###########################################################################

#cmake -DLUXMARK_CUSTOM_CONFIG=cmake/SpecializedConfig/Config_Dade.cmake .

MESSAGE(STATUS "Using Dade's Linux Configuration settings")

SET(BOOST_SEARCH_PATH		"/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")
SET(OPENIMAGEIO_ROOT_DIR	"/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")
SET(EMBREE_SEARCH_PATH		"/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")
SET(OIDN_SEARCH_PATH		"/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")

SET(OPENCL_SEARCH_PATH		"$ENV{AMDAPPSDKROOT}")

SET(BLOSC_SEARCH_PATH       "/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")
SET(TBB_SEARCH_PATH         "/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")
SET(ENV{QTDIR}				"/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2/bin")

SET(LuxRays_HOME		"/home/david/projects/luxcorerender/LuxCore")

#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")

