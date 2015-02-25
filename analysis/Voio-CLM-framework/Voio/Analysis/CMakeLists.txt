add_executable(Analysis analysis.cpp)

# Local libraries
include_directories(${CLM_SOURCE_DIR}/include)

include_directories(../../lib/local/CLM/include)
			
target_link_libraries(Analysis CLM)

target_link_libraries(Analysis dlib)

MESSAGE(“VOIO files:“)
MESSAGE(${TBB_INCLUDE_DIRS})
MESSAGE(${TBB_LIBRARIES})

include_directories(${TBB_INCLUDE_DIRS})
if(UNIX)
    target_link_libraries(Analysis ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TBB_LIBRARIES})
endif(UNIX)

install (TARGETS Analysis DESTINATION ${CMAKE_BINARY_DIR}/Voio)
