Setup and compile instructions from terminal:

Install OpenCV on Debian Tutorial - http://indranilsinharoy.com/2012/11/01/installing-opencv-on-linux/

> Directory used - home/debian/Desktop/

> Installing OpenCV on Debian:
1) sudo apt-get install build-essential
2) sudo apt-get install cmake
3) sudo apt-get install pkg-config
4) sudo apt-get install libgtk2.0-dev
5) sudo apt-get install libpng12-0 libpng12-dev libpng++-dev libpng3 libpnglite-dev zlib1g-dbg zlib1g zlib1g-dev pngtools libjasper-dev libjasper-runtime libjasper1 libjpeg8 libjpeg8-dbg libjpeg62 libjpeg62-dev libjpeg-progs libtiff4-dev libtiff4 libtiffxx0c2 libtiff-tools ffmpeg libavcodec-dev libavformat-dev libswscale-dev openexr libopenexr6 libopenexr-dev
6) sudo apt-get install libgstreamer0.10-0-dbg libgstreamer0.10-0 libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libxine1-ffmpeg libxine-dev libxine1-bin libunicap2 libunicap2-dev libdc1394-22-dev libdc1394-22 libdc1394-utils libv4l-0 libv4l-dev
7) git clone https://github.com/Itseez/opencv.git
8) cd home/debian/Desktop/opencv
9) mkdir release
10) cd release
11) cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_ZLIB=ON -D BUILD_PYTHON_SUPPORT=ON home/debian/Desktop/opencv
12) cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_ZLIB=ON -D WITH_V4L=ON -D WITH_GSTREAMER=ON -D WITH_OPENEXR=ON -D WITH_UNICAP=ON -D BUILD_PYTHON_SUPPORT=ON -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=ON ..

> cmake (One program time setup):
1) Make CMakeList.txt and add:

cmake_minimum_required(VERSION 2.8)
project( <YOUR PROJECT NAME> )
find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( <YOUR PROJECT NAME> <YOUR PROJECT NAME>.cpp )
target_link_libraries( <YOUR PROJECT NAME> ${OpenCV_LIBS} )

2) cd home/debian/Desktop/recognize
3) cmake .

> git code:
git pull https://github.com/mfonken/recognize

>compile 
1) make
2) ./<YOUR PROJECT NAME>.cpp

Instructions and opencv installation on Mac - http://blogs.wcode.org/2014/10/howto-install-build-and-use-opencv-macosx-10-10/

