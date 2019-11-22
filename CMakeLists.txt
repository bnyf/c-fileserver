cmake_minimum_required(VERSION 3.14)
project(c_fileserver)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(src/fileManager)

add_executable(c_fileserver
        src/fileManager/example.c
        src/fileManager/FileManager.c
        src/fileManager/FileManager.h
        src/fileManager/utility.c
        src/fileManager/utility.h
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
        README.md)
