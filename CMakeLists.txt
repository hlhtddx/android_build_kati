cmake_minimum_required(VERSION 3.0)

project(ckati CXX)

option(DEBUG_SUPPORT "Debugging support" ON)
option(DEBUG_DBG_MSG "Debug message for debug feature" OFF)

if( DEBUG_SUPPORT )
    message("Debug support Enabled")
    add_definitions(-DDEBUG_SUPPORT)
    if(DEBUG_DBG_MSG)
        message("Output more debug message for debugging feature")
        add_definitions(-DDEBUG_DBG_MSG)
    endif(DEBUG_DBG_MSG)
endif( DEBUG_SUPPORT )

include_directories(.)

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

add_library(libckati ${libckati_SOURCES})

list(APPEND ckati_SOURCES
    main.cc
)

add_executable(ckati ${ckati_SOURCES} )

target_link_libraries( ckati
    libckati
)

if( DEBUG_SUPPORT )
    add_subdirectory(debug)
    target_link_libraries( ckati
        ckati-debug
    )
endif( DEBUG_SUPPORT )
