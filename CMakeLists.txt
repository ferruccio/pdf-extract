# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

project(pdfex)

#find_package(FreeType REQUIRED)
find_library(FREETYPE_LIBRARIES NAMES freetype)

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PNG REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

find_path(PODOFO_INCLUDE NAMES "podofo/podofo.h")
find_library(PODOFO_LIBRARY NAMES podofo)

include_directories(${PODOFO_INCLUDE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#find_path(SYSTEM_INCLUDE_DIR clara.hpp)
#include_directories(${SYSTEM_INCLUDE_DIR})

add_executable(${PROJECT_NAME} src/main.cpp src/PdfPageParser.cpp)

target_link_libraries(${PROJECT_NAME} ${PODOFO_LIBRARY} ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${FREETYPE_LIBRARIES} Threads::Threads ${OPENSSL_LIBRARIES})
