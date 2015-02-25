add_executable(FunctionalityTest main.cpp)

# Local libraries
include_directories(${CLM_SOURCE_DIR}/include)

include_directories(../../lib/local/CLM/include)
			
target_link_libraries(FunctionalityTest CLM)

target_link_libraries(FunctionalityTest dlib)


if(UNIX)
    target_link_libraries(FunctionalityTest ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TBB_LIBRARIES})
endif(UNIX)

install (TARGETS FunctionalityTest DESTINATION ${CMAKE_BINARY_DIR}/Voio)
