cmake_minimum_required( VERSION 3.5.1 )
project( ABugsLifeV2 )

set( CMAKE_CXX_STANDARD 20 )
set( OpenCV_DIR /Users/gabrielkosmacher/Documents/opencvDownload/build_opencv )

find_package( OpenCV REQUIRED )
find_package ( SQLite3 REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${SQLite3_INCLUDE_DIRS} )

add_executable( ABugsLifeV2 src/main.cpp tests/tests.cpp includes/AntProc.h
        src/AntProc.cpp)

target_link_libraries( ABugsLifeV2 ${OpenCV_LIBS} ${SQLite3_LIBRARIES} )