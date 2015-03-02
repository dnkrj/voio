cmake_minimum_required (VERSION 2.8)

project (VOIO-CLM-framework)

if(UNIX)
	MESSAGE(“UNIX    detected“)
else()
	MESSAGE(“+———————————————————————————————————————+“)	
	MESSAGE(“|                                       |“)
	MESSAGE(“|   A non-UNIX file system detected     |“)
	MESSAGE(“|   CMAKE will fail                     |“)
	MESSAGE(“|   Ignore results                      |“)
	MESSAGE(“|                                       |“)
	MESSAGE(“+———————————————————————————————————————+“)
endif(UNIX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_BUILD_TYPE “Release”)

set(CMAKE_EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/exe/)

set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/)

set(CMAKE_INSTALL_PREFIX ../../CLM/)

# OpenCV

	find_package( OpenCV 2.4.6 REQUIRED )
	

# TBB
	find_package(TBB REQUIRED)

# Boost
	find_package( Boost REQUIRED COMPONENTS filesystem system)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}/boost)
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})	
	INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})


# Eigen
	find_package(Eigen2 REQUIRED)  
	INCLUDE_DIRECTORIES(${EIGEN2_INCLUDE_DIR})	

# GIFLIB

	find_package(GIFLIB REQUIRED)
	INCLUDE_DIRECTORIES(${GIFLIB_INCLUDE_DIR})
	LINK_DIRECTORIES(${GIF_LIBRARY})

# Compiler flags

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_LESS 4.7)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -msse -msse2 -msse3")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -msse -msse2 -msse3")
    endif ()
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -msse -msse2 -msse3")
endif ()


# dlib
include_directories(lib/dlib/include)

# dlib library
add_subdirectory(lib/dlib)

# CLM library 
add_subdirectory(lib/CLM)

# VOIO
add_subdirectory(Analysis)

# move required files
file(COPY "lib/CLM/classifiers" DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY "lib/CLM/model" DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY "lib/CLM/classifiers" DESTINATION ${CMAKE_INSTALL_PREFIX}/exe)
file(COPY "lib/CLM/model" DESTINATION ${CMAKE_INSTALL_PREFIX}/exe)

#file(GLOB filesa "lib/CLM/model/*.txt")
#file(GLOB filesb "lib/CLM/model/detection_validation/*.txt")
#file(GLOB filesc "lib/CLM/model/patch_experts/*.txt")
#file(GLOB filesd "lib/CLM/model/pdms/*.txt")
#file(GLOB filese "lib/CLM/classifiers/*.xml")

#install(FILES &{filesa} DESTINATION exe/model)
#install(FILES &{filesb} DESTINATION exe/model/detection_validation)
#install(FILES &{filesc} DESTINATION exe/model/patch_experts)
#install(FILES &{filesd} DESTINATION exe/model/pdms)
#install(FILES &{filese} DESTINATION exe/classifiers)

#file(COPY "lib/CLM/classifiers" DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
#file(COPY "lib/CLM/classifiers" DESTINATION ${CMAKE_BINARY_DIR}/bin/Release)
#file(COPY "lib/CLM/model" DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug)
#file(COPY "lib/CLM/model" DESTINATION ${CMAKE_BINARY_DIR}/bin/Release)
