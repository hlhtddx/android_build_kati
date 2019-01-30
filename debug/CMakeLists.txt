cmake_minimum_required(VERSION 3.0)

project(ckati CXX)

list(APPEND libckatidebug_SOURCES
        breakpoint.cc
        breakpoint.h
        command_interface.cc
        command_interface.h
        session.cc
        session.h
)

add_library(ckati-debug ${libckatidebug_SOURCES} controller.cc controller.h handler.cc handler.h)
