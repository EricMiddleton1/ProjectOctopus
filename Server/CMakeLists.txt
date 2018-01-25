PROJECT(server)

CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)  # Required by CMake 2.7+
endif()
set(CMAKE_CXX_STANDARD 14)

# --------------------------------------------------------------------------
#   The list of "libs" which can be included can be found in:
#     http://www.mrpt.org/Libraries
#
#   The dependencies of a library are automatically added, so you only
#    need to specify the top-most libraries your code depend on.
# --------------------------------------------------------------------------

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED graphics window system)

# Add OpenCV
find_package( OpenCV REQUIRED )

FIND_PACKAGE( MRPT REQUIRED base slam comms)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Declare the target (an executable)
ADD_EXECUTABLE(server
	src/main.cpp
	src/server.cpp
	src/SLAMmer.cpp
	)
TARGET_LINK_LIBRARIES(server ${MRPT_LIBS} jsoncpp Threads::Threads ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${OpenCV_LIBS} )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

# Set optimized building:
IF(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
ENDIF()
