###########################################################################
#
# Configuration ( Jens Verwiebe )
#
###########################################################################

MESSAGE(STATUS "Using OSX Configuration settings")

# Allow for the location of OSX_DEPENDENCY_ROOT to be set from the command line
IF( NOT OSX_DEPENDENCY_ROOT )
  set(OSX_DEPENDENCY_ROOT ${CMAKE_SOURCE_DIR}/macos) # can be macos or usr/local for example
ENDIF()

MESSAGE(STATUS "OSX_DEPENDENCY_ROOT : " ${OSX_DEPENDENCY_ROOT})
set(OSX_SEARCH_PATH     ${OSX_DEPENDENCY_ROOT})

# Libs present in system ( /usr )
SET(SYS_LIBRARIES z )

execute_process(COMMAND python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())" OUTPUT_VARIABLE PY3_INCLUDE)
execute_process(COMMAND python3 -c "import distutils.sysconfig as sysconfig; print(sysconfig.get_config_var('LIBDIR'))" OUTPUT_VARIABLE PY3_LIB)
IF(PY3_INCLUDE)
SET(PYTHON_LIBRARIES ${PY3_LIB})
SET(PYTHON_INCLUDE_DIRS ${PY3_INCLUDE})
SET(PYTHONLIBS_FOUND 1)
MESSAGE(STATUS "Python3 found !!!!")
ELSE()
    #SET(PYTHON_INCLUDE_DIRS "/usr/local/Cellar/python/3.7.5/Frameworks/Python.framework/Versions/3.7/include/python3.7m")
    message(STATUS "Python3 include not found !!!!!")
ENDIF()

#Find pyside2-uic so that .ui files rebuild pyside deps
execute_process(COMMAND python3 -c "import sys; print(sys.base_exec_prefix)" OUTPUT_VARIABLE PY_EXEC_PREFIX)
find_program(PYSIDE_UIC
             NAME pyside2-uic
             HINTS ${PY_EXEC_PREFIX}/bin/)

# Libs that have find_package modules

# Intel Oidn
set(OIDN_ROOT                "${OSX_SEARCH_PATH}")
find_package(Oidn REQUIRED)

if (OIDN_FOUND)
	include_directories(BEFORE SYSTEM ${OIDN_INCLUDE_PATH})
endif ()

set(OPENIMAGEIO_ROOT_DIR "${OSX_SEARCH_PATH}")
set(OPENIMAGEIO_LIBRARY_PATH "${OSX_SEARCH_PATH}/lib")

set(BOOST_SEARCH_PATH         "${OSX_SEARCH_PATH}")
set(BOOST_LIBRARYDIR          "${BOOST_SEARCH_PATH}/lib")
set(BOOST_FOUND 1)

set(OPENCL_SEARCH_PATH        "${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/opencl.framework")
set(OPENCL_INCLUDE_DIR       "${OSX_SEARCH_PATH}/include")
set(OPENCL_LIBRARYDIR         "${OPENCL_SEARCH_PATH}")
set(OPENCL_FOUND 1)

set(EMBREE_SEARCH_PATH			"${OSX_SEARCH_PATH}")

set(TBB_SEARCH_PATH "${OSX_SEARCH_PATH}")
set(TBB_INCLUDE_PATH "${OSX_SEARCH_PATH}/include")
set(TBB_LIBRARY "${OSX_SEARCH_PATH}/lib/libtbb.dylib")
set(TBB_FOUND 1)
#INSTALL(FILES ${OSX_SEARCH_PATH}/lib/libtbb.dylib DESTINATION lib)
#INSTALL(FILES ${OSX_SEARCH_PATH}/lib/libtbbmalloc.dylib DESTINATION lib)
set(BLOSC_SEARCH_PATH "${OSX_SEARCH_PATH}")
set(BLOSC_INCLUDE_PATH "${OSX_SEARCH_PATH}/include")
set(BLOSC_LIBRARY "${OSX_SEARCH_PATH}/lib/libblosc.a")
set(BLOSC_FOUND 1)

find_library(OPENMP_LIB libiomp5.dylib HINTS ${OSX_SEARCH_PATH}/lib)

if(NOT OPENMP_LIB)
    message(FATAL_ERROR "OpenMP library not found")
else()
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${OSX_SEARCH_PATH}/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${OSX_SEARCH_PATH}/lib -liomp5")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${OSX_SEARCH_PATH}/lib -liomp5")
    #INSTALL(FILES ${OSX_SEARCH_PATH}/lib/libomp.dylib DESTINATION lib)
    #INSTALL(FILES ${OSX_SEARCH_PATH}/lib/libiomp5.dylib DESTINATION lib)
    set(OPENMP_FOUND 1)
    MESSAGE(STATUS "OpenMP found !!!!")
endif()

set(GLEW_SEARCH_PATH          "${OSX_SEARCH_PATH}")
find_path(GLEW_INCLUDE_DIR glew.h PATHS ${OSX_SEARCH_PATH}/include/GL )
find_library(GLEW_LIBRARY libglfw3.a PATHS ${OSX_SEARCH_PATH}/lib )
set(GLEW_FOUND 1)

set(GLUT_SEARCH_PATH          "${CMAKE_OSX_SYSROOT}/System/Library/Frameworks/glut.framework")
set(GLUT_INCLUDE_PATH 		"${GLUT_SEARCH_PATH}/Headers")

SET(OPENEXR_ROOT "${OSX_SEARCH_PATH}")

SET(TIFF_ROOT "${OSX_SEARCH_PATH}")

# Libs with hardcoded pathes ( macos repo )

SET(TIFF_LIBRARIES ${OSX_DEPENDENCY_ROOT}/lib/libtiff.dylib ${OSX_DEPENDENCY_ROOT}/lib/liblzma.a)
SET(TIFF_INCLUDE_DIR ${OSX_DEPENDENCY_ROOT}/include/tiff)
SET(TIFF_FOUND ON)
SET(JPEG_LIBRARIES ${OSX_DEPENDENCY_ROOT}/lib/libjpeg.a)
SET(JPEG_INCLUDE_DIR ${OSX_DEPENDENCY_ROOT}/include/jpeg)
SET(JPEG_FOUND ON)
SET(PNG_LIBRARIES ${OSX_DEPENDENCY_ROOT}/lib/libpng.a) #${SYS_LIBRARIES}
SET(PNG_INCLUDE_DIR ${OSX_DEPENDENCY_ROOT}/include/png)
SET(PNG_FOUND ON)
SET(EMBREE_LIBRARY ${OSX_DEPENDENCY_ROOT}/lib/libembree3.dylib)
SET(EMBREE_INCLUDE_PATH ${OSX_DEPENDENCY_ROOT}/include/embree3)
SET(EMBREE_FOUND ON)
#INSTALL(FILES ${OSX_DEPENDENCY_ROOT}/lib/libembree3.dylib DESTINATION lib)
#INSTALL(FILES ${OSX_DEPENDENCY_ROOT}/lib/libembree3.3.dylib DESTINATION lib)

SET(OPENIMAGEIO_LIBRARY ${OSX_DEPENDENCY_ROOT}/lib/libOpenImageIO.dylib)
SET(OPENIMAGEIO_INCLUDE_PATH ${OSX_DEPENDENCY_ROOT}/include/OpenImageIO)
SET(OPENIMAGEIO_FOUND ON)

SET(OIDN_LIBRARY ${OSX_DEPENDENCY_ROOT}/lib/libOpenImageDenoise.dylib)
SET(OIDN_INCLUDE_PATH ${OSX_DEPENDENCY_ROOT}/include/OpenImageDenoise)
SET(Qt5_DIR ${OSX_DEPENDENCY_ROOT}/lib/cmake/Qt5)
SET(ZLIB_LIBRARY ${OSX_DEPENDENCY_ROOT}/lib/libz.a)

#############################################################################
#############################################################################
##########################      Find LuxRays       ##########################
#############################################################################
#############################################################################

set(LuxRays_HOME ${OSX_DEPENDENCY_ROOT})

FIND_PATH(LUXRAYS_INCLUDE_DIRS NAMES luxrays/luxrays.h PATHS ../luxrays/include ${LuxRays_HOME}/include )
FIND_PATH(LUXRAYS_INCLUDE_CFG_DIRS NAMES luxrays/cfg.h PATHS ../luxrays/include ${LuxRays_HOME}/generated/include )
FIND_LIBRARY(LUXRAYS_LIBRARY luxrays PATHS ../luxrays/lib ${LuxRays_HOME}/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist )

IF (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)
	MESSAGE(STATUS "LuxRays include directory: " ${LUXRAYS_INCLUDE_DIRS})
	MESSAGE(STATUS "LuxRays config include directory: " ${LUXRAYS_INCLUDE_CFG_DIRS})
	MESSAGE(STATUS "LuxRays library directory: " ${LUXRAYS_LIBRARY})
	INCLUDE_DIRECTORIES(SYSTEM ${LUXRAYS_INCLUDE_DIRS} ${LUXRAYS_INCLUDE_CFG_DIRS})
ELSE (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)
	MESSAGE(FATAL_ERROR "LuxRays not found.")
ENDIF (LUXRAYS_INCLUDE_DIRS AND LUXRAYS_LIBRARY)


#############################################################################
#############################################################################
########################        Find LuxCore         ########################
#############################################################################
#############################################################################

FIND_PATH(LUXCORE_INCLUDE_DIRS NAMES luxcore/luxcore.h PATHS ../luxrays/include ${LuxRays_HOME}/include)
FIND_PATH(LUXCORE_INCLUDE_CFG_DIRS NAMES luxcore/cfg.h PATHS ../luxrays/include ${LuxRays_HOME}/generated/include)
FIND_LIBRARY(OPENVDB_LIBRARY openvdb PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(OPENSUBDIV_LIBRARY opensubdiv PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(BCD_LIBRARY bcd PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(SLG_LIBRARY_CORE slg-core PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(SLG_LIBRARY_FILM slg-film PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(SLG_LIBRARY_KERNELS slg-kernels PATHS ${LuxRays_HOME}/lib ../luxrays/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)
FIND_LIBRARY(LUXCORE_LIBRARY luxcore PATHS ../luxrays/lib ${LuxRays_HOME}/lib PATH_SUFFIXES "" release relwithdebinfo minsizerel dist)

IF (LUXCORE_INCLUDE_DIRS AND LUXCORE_LIBRARY)
	SET(ALL_LUXCORE_LIBRARIES ${LUXCORE_LIBRARY} ${SLG_LIBRARY_CORE} ${SLG_LIBRARY_FILM} ${SLG_LIBRARY_KERNELS} ${LUXRAYS_LIBRARY}
		${BCD_LIBRARY} ${OPENVDB_LIBRARY} ${OPENSUBDIV_LIBRARY} ${OPENIMAGEIO_LIBRARY}
		${BLOSC_LIBRARY} ${EMBREE_LIBRARY} ${OIDN_LIBRARY} ${TBB_LIBRARY} ${TIFF_LIBRARY}
		${OPENEXR_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES} ${ZLIB_LIBRARY})

	MESSAGE(STATUS "LuxCore include directory: " ${LUXCORE_INCLUDE_DIRS})
	MESSAGE(STATUS "LuxCore config include directory: " ${LUXCORE_INCLUDE_CFG_DIRS})
	MESSAGE(STATUS "LuxCore library directory: " ${LUXCORE_LIBRARY})
	MESSAGE(STATUS "LuxCore all libraries: " ${ALL_LUXCORE_LIBRARIES})
	INCLUDE_DIRECTORIES(SYSTEM ${LUXCORE_INCLUDE_DIRS} ${LUXCORE_INCLUDE_CFG_DIRS})
ELSE (LUXCORE_INCLUDE_DIRS AND LUXCORE_LIBRARY)
	MESSAGE(FATAL_ERROR "LuxCore Library not found.")
ENDIF (LUXCORE_INCLUDE_DIRS AND LUXCORE_LIBRARY)

