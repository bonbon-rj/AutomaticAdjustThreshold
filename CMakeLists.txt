cmake_minimum_required(VERSION 2.8)
project( AutomaticAdjustThreshold )
set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_CXX_FLAGS
-pthread)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

find_package( OpenCV REQUIRED )
include_directories(    
    ${OpenCV_INCLUDE_DIRS}
 )

set(PROJ_SOURCES
    main.cpp
    GetThreshold.cpp
)

add_executable(${PROJECT_NAME} ${PROJ_SOURCES})
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

