cmake_minimum_required(VERSION 3.0)

project(ckati CXX)

list(APPEND libckati_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/affinity.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/dep.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/eval.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/exec.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/expr.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/file_cache.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/fileutil.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/find.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/flags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/func.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/io.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/log.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ninja.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/parser.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/regen.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/rule.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stats.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stmt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/string_piece.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stringprintf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/strutil.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/symtab.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/timeutil.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/var.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/version_unknown.cc
)

add_definitions(-std=c++14)

add_library(libckati ${libckati_SOURCES})

list(APPEND ckati_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
)

add_executable(ckati ${ckati_SOURCES} )

target_link_libraries( ckati
    libckati
)