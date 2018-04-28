cmake_minimum_required(VERSION 2.8)

project(proj2cmake)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-std=c++11)
   ADD_DEFINITIONS(-Wall)
endif()

#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#   ADD_DEFINITIONS(-stdlib=libc++)
#   SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -stdlib=libc++")
#endif()

FIND_PACKAGE (Boost COMPONENTS system filesystem REQUIRED)

if (Boost_FOUND)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    )
endif()

AUX_SOURCE_DIRECTORY(src/ vcparser_SRC)
AUX_SOURCE_DIRECTORY(src/vcx vcparser_SRC)
AUX_SOURCE_DIRECTORY(src/cmake vcparser_SRC)

ADD_EXECUTABLE(proj2cmake
               ${vcparser_SRC})

TARGET_LINK_LIBRARIES(proj2cmake
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY}
                     )
