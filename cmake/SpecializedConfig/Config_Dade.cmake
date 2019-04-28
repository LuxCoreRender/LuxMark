
###########################################################################
#
# Configuration
#
###########################################################################

#cmake -DLUXMARK_CUSTOM_CONFIG=cmake/SpecializedConfig/Config_Dade.cmake .

MESSAGE(STATUS "Using Dade's Linux Configuration settings")

SET(CMAKE_INCLUDE_PATH "/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2/include;/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")
SET(CMAKE_LIBRARY_PATH "/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2/lib;/home/david/projects/luxcorerender/LinuxCompile/target-64-sse2")
SET(Qt5_DIR "/home/david/tmp/Qt/5.12.2/gcc_64/lib/cmake/Qt5")

SET(LuxRays_HOME "/home/david/projects/luxcorerender/LuxCore")

#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")
