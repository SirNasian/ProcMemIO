cmake_minimum_required(VERSION 3.27.1)
project(ProcMemIO)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(include)

if (UNIX)
	add_library(procmemio src/linux.c)
elseif (WIN32)
	add_library(procmemio src/windows.c)
endif()
