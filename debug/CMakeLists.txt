cmake_minimum_required(VERSION 3.0)

project(ckati CXX)

list(APPEND libckatidebug_SOURCES
        breakpoint.cc
        breakpoint.h
        connector.cc
        connector.h
        session.cc
        session.h
)

add_library(ckati-debug ${libckatidebug_SOURCES}  )
