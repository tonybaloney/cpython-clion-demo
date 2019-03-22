cmake_minimum_required(VERSION 3.13)
project (cpython_eval C)

set(CMAKE_C_STANDARD 99)
# The version number.
set (Ceval_prof_VERSION_MAJOR 1)
set (Ceval_prof_VERSION_MINOR 0)

# Set this to where you
set(CPYTHON_SOURCE "/Users/anthonyshaw/cpython")

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${CPYTHON_SOURCE}")
include_directories("${CPYTHON_SOURCE}/Include")

# add the executable
add_executable(cpython_eval
        main.c
        ${CPYTHON_SOURCE}/pyconfig.h)

target_link_libraries(cpython_eval -L${CPYTHON_SOURCE}/Objects)
target_link_libraries(cpython_eval -L${CPYTHON_SOURCE}/Modules)
target_link_libraries(cpython_eval -L${CPYTHON_SOURCE}/Python)

# if you wanted to play with dtrace. Needs to run as root!
# target_link_libraries(cpython_eval /usr/lib/libdtrace.dylib)

# assumes you have compiled cpython beforehand --with-dtrace and --with-pydebug
target_link_libraries(cpython_eval ${CPYTHON_SOURCE}/libpython3.8dm.a)
