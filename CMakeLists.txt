cmake_minimum_required(VERSION 3.0.0)
project(My_Hash)

include(CTest)
enable_testing()

set(SOURCES
    source/cache.c
    source/client.c
    source/conf.c
    source/my_db.c
    source/hash_func.c
    source/my_hash.c
    source/server.c
    source/request_response.c
)


add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include    
)

find_package(Threads)
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

