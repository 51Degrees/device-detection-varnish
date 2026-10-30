cmake_minimum_required(VERSION 3.24)
project(PerformanceTest NONE)

include(ExternalProject)

ExternalProject_Add(ApacheBench
    GIT_REPOSITORY https://github.com/51degrees/apachebench
    GIT_TAG main
    INSTALL_COMMAND ""
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE
)

add_custom_target(perf ALL
    COMMAND cp ${CMAKE_CURRENT_LIST_DIR}/runPerf.sh .
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/runPerf.sh
    VERBATIM
)

add_dependencies(perf ApacheBench-build)
