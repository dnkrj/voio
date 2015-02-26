
set(SOURCE
    analysis.cpp
#gifcv.cpp
    timestamp.cpp
#    main.cpp
)
set(HEADER
    analysis.h
    timestamp.h
#gifcv.h
    Strategy.h
)
add_library(Analysis STATIC ${SOURCE} main.cpp ${HEADER} )

#target_link_libraries(gifcv ${OpenCV_LIBS} ${GIFLIB_LIBRARY})

add_executable(Demo ${SOURCE} main.cpp ${HEADER})

# Local libraries
include_directories(${CLM_SOURCE_DIR}/include)

include_directories(../../lib/local/CLM/include)
			
target_link_libraries(Demo CLM)
target_link_libraries(Analysis CLM)
target_link_libraries(Demo dlib)
target_link_libraries(Analysis dlib)

MESSAGE(“VOIO files:“)
MESSAGE(${TBB_INCLUDE_DIRS})
MESSAGE(${TBB_LIBRARIES})

include_directories(${TBB_INCLUDE_DIRS})
include_directories(${GIFLIB_INCLUDE_DIR})
if(UNIX)
    target_link_libraries(Demo ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TBB_LIBRARIES})
    target_link_libraries(Analysis ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TBB_LIBRARIES})
endif(UNIX)

install (TARGETS Demo DESTINATION exe)
install (TARGETS Analysis DESTINATION out)
install (FILES timestamp.h DESTINATION out)