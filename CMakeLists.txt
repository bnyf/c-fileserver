cmake_minimum_required(VERSION 3.14)
project(c_fileserver)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)


set(INC_DIR /usr/local/libevent/include)
set(LINK_DIR /usr/local/libevent/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(libevent.a)

add_executable(c_fileserver
        src/http.c
        src/http.h
        src/main.c
        src/rio.c
        src/rio.h
        src/server.c
        src/server.h
        src/tools.c
        src/tools.h
        src/url_parser.c
        src/url_parser.h
        src/FilePresenter.h
        src/FilePresenter.c
        src/response.h
        src/response.c
        README.md
        )

target_link_libraries(c_fileserver libevent.a)