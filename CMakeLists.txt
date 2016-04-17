cmake_minimum_required(VERSION 2.8.7)
project(line_finder)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_MODULE_PATH   ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core highgui imgproc calib3d)
find_package(Boost REQUIRED COMPONENTS thread system)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^x86_64" )
	message(STATUS "Architecture is x84_64, compiling with SSE")
	# SSE flags
	add_definitions("-DENABLE_SSE")

	# Also add some useful compiler flag
	set(CMAKE_CXX_FLAGS
	   "${CMAKE_CXX_FLAGS} ${SSE_FLAGS} -march=native  -std=c++0x -g"
	)
endif()
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
	message(STATUS "Architecture is arm, compiling with NEON")
	# NEON flags
	add_definitions("-DENABLE_NEON")

	# Also add some useful compiler flag
	set(CMAKE_CXX_FLAGS
	   "${CMAKE_CXX_FLAGS} -O2 -march=native -mfpu=neon -std=c++0x"
	)  
endif()

# Set source files
set(line_SOURCE_FILES
  ${PROJECT_SOURCE_DIR}/src/LineFinder.cpp
)
set(SOURCE_FILES
  ${line_SOURCE_FILES}
)

include_directories(
  ${EIGEN3_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
)


# build shared library.
add_library(line_finder_lib SHARED ${SOURCE_FILES})



target_link_libraries(line_finder_lib ${Boost_LIBRARIES} ${OpenCV_LIBRARIES})


# build test program
add_executable(line_test src/test/main_test.cpp)
target_link_libraries(line_test line_finder_lib ${OpenCV_LIBRARIES})

