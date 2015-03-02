
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
add_library(Analysis STATIC ${SOURCE} ${HEADER} )
add_library(gifcv STATIC gifcv.h gifcv.cpp )
target_link_libraries(gifcv ${OpenCV_LIBS} ${GIFLIB_LIBRARY})

# Local libraries
include_directories(${CLM_SOURCE_DIR}/include)

include_directories(../../lib/local/CLM/include)
			
target_link_libraries(Analysis CLM)
target_link_libraries(Analysis dlib)
target_link_libraries(Analysis ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TBB_LIBRARIES})

MESSAGE(“VOIO files:“)
MESSAGE(${TBB_INCLUDE_DIRS})
MESSAGE(${TBB_LIBRARIES})

include_directories(${TBB_INCLUDE_DIRS})
include_directories(${GIFLIB_INCLUDE_DIR})

install (TARGETS Analysis DESTINATION out)
install (FILES timestamp.h DESTINATION out)

add_executable(camDemo camDemo.cpp timestamp.h timestamp.cpp)
target_link_libraries(camDemo CLM)
target_link_libraries(camDemo dlib)
target_link_libraries(camDemo ${OpenCV_LIBS} ${Boost_LIBRARIES} ${TBB_LIBRARIES})
target_link_libraries(camDemo gifcv)

add_executable(AnalysisTest main.cpp timestamp.h analysis.h gifcv.h)
target_link_libraries(AnalysisTest gifcv)
target_link_libraries(AnalysisTest Analysis)
install (TARGETS AnalysisTest DESTINATION exe)