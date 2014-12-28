Setup and compile instructions from terminal:

cmake (One time setup):
1) Make CMakeList.txt and add:

cmake_minimum_required(VERSION 2.8)
project( <YOUR PROJECT NAME> )
find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( <YOUR PROJECT NAME> <YOUR PROJECT NAME>.cpp )
target_link_libraries( <YOUR PROJECT NAME> ${OpenCV_LIBS} )

2) cd <path/to/your/program’s/folder/>
3) /Applications/CMake.app/Contents/bin/cmake .

Compile 
1) make
2) ./<YOUR PROJECT NAME>.cpp

Full instructions and opencv installation - http://blogs.wcode.org/2014/10/howto-install-build-and-use-opencv-macosx-10-10/