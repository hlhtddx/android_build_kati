cmake_minimum_required(VERSION 3.0)

project(ckati CXX)

list(APPEND libckati_SOURCES
    affinity.cc
    command.cc
    dep.cc
    eval.cc
    exec.cc
    expr.cc
    file.cc
    file_cache.cc
    fileutil.cc
    find.cc
    flags.cc
    func.cc
    io.cc
    log.cc
    ninja.cc
    parser.cc
    regen.cc
    rule.cc
    stats.cc
    stmt.cc
    string_piece.cc
    stringprintf.cc
    strutil.cc
    symtab.cc
    thread_pool.cc
    timeutil.cc
    var.cc
    version_unknown.cc
)

add_definitions(-std=c++14)

add_library(libckati ${libckati_SOURCES} debug.cpp debug.h)

list(APPEND ckati_SOURCES
    main.cc
)

add_executable(ckati ${ckati_SOURCES} )

target_link_libraries( ckati
    libckati
)