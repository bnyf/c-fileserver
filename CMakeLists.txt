cmake_minimum_required(VERSION 3.14)
project(c_fileserver)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

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
        src/response_demo.c
        src/FilePresenter.h
        src/FilePresenter.c
        src/response.h
        src/response.c
        README.md
        )
