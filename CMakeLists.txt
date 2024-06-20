cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_COMPILER "C:\\msys64\\ucrt64\\bin\\g++.exe")
set(CMAKE_C_COMPILER "C:\\msys64\\ucrt64\\bin\\gcc.exe")
project(ReadExcelOpenXLSX)

set(CMAKE_CXX_STANDARD 17)

# Set the build output location to a common directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# Add OpenXLSX library
add_subdirectory(OpenXLSX)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Your executable
add_executable(ReadExcel main.cpp)
target_link_libraries(ReadExcel OpenXLSX::OpenXLSX)
