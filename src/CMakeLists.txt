CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SRC_LIST 
main.cc;
turl_reader.cc;
turl_counter.cc;
turl_map.cc;
turl_split.cc;
turl_define.cc;
turl_buf.cc;
turl_worker.cc)

FIND_PACKAGE(gflags REQUIRED)
INCLUDE_DIRECTORIES(${gflags_INCLUDE_DIR})

FIND_PACKAGE(Threads)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/..)

ADD_EXECUTABLE(top_url ${SRC_LIST})

TARGET_LINK_LIBRARIES(top_url ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(top_url gflags)