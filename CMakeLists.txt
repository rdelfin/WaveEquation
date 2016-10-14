cmake_minimum_required(VERSION 3.6)
project(WaveEquation)

FILE(GLOB cmakes ${CMAKE_SOURCE_DIR}/cmake/*.cmake)
FOREACH(cmake ${cmakes})
    INCLUDE(${cmake})
ENDFOREACH(cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

ADD_SUBDIRECTORY(src)

IF (EXISTS ${CMAKE_SOURCE_DIR}/sln/CMakeLists.txt)
    ADD_SUBDIRECTORY(sln)
ENDIF()