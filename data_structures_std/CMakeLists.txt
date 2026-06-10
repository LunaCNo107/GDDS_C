cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_COMPILER clang)

project(GDDS_Tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Default to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Put all test executables in build/test
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

# Common include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# ------------------------------------------------------------------
# Common compile warnings (applied per-target)
# ------------------------------------------------------------------
set(COMMON_WARNINGS
    -Wall
    -Wextra
)

# ------------------------------------------------------------------
# Common sources used by all tests
# ------------------------------------------------------------------
set(COMMON_SOURCES
    src/error.c
    test/point.c
)

# ------------------------------------------------------------------
# ARRAY TEST
# ------------------------------------------------------------------
add_executable(array_test
    test/array_test.c
    src/array.c
    ${COMMON_SOURCES}
)

# ------------------------------------------------------------------
# ARRAY MEMORY TEST ( array-based )
# ------------------------------------------------------------------
add_executable(array_memtest
    test/array_memtest.c
    src/array.c
    ${COMMON_SOURCES}
)

target_include_directories(array_memtest PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(array_memtest PRIVATE
    ${COMMON_WARNINGS}
    $<$<CONFIG:Debug>:-g -O0>
)

# Optional sanitizers (HIGHLY recommended)
target_compile_options(array_memtest PRIVATE -fsanitize=address,undefined)
target_link_options(array_memtest PRIVATE -fsanitize=address,undefined)

# ------------------------------------------------------------------
# STACK TEST ( array-based )
# ------------------------------------------------------------------
add_executable(stack_test
    test/stack_test.c
    src/stack.c
    ${COMMON_SOURCES}
)

# ------------------------------------------------------------------
# STACK MEMORY TEST ( array-based )
# ------------------------------------------------------------------
add_executable(stack_memtest
    test/stack_memtest.c
    src/stack.c
    ${COMMON_SOURCES}
)

target_include_directories(stack_memtest PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(stack_memtest PRIVATE
    ${COMMON_WARNINGS}
    $<$<CONFIG:Debug>:-g -O0>
)

# Optional sanitizers (HIGHLY recommended)
target_compile_options(stack_memtest PRIVATE -fsanitize=address,undefined)
target_link_options(stack_memtest PRIVATE -fsanitize=address,undefined)

# ------------------------------------------------------------------
# QUEUE TEST ( array-based )
# ------------------------------------------------------------------
add_executable(queue_test
    test/queue_test.c
    src/queue.c
    ${COMMON_SOURCES}
)

# ------------------------------------------------------------------
# QUEUE MEMORY TEST ( array-based )
# ------------------------------------------------------------------
add_executable(queue_memtest
    test/queue_memtest.c
    src/queue.c
    ${COMMON_SOURCES}
)

target_include_directories(queue_memtest PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(queue_memtest PRIVATE
    ${COMMON_WARNINGS}
    $<$<CONFIG:Debug>:-g -O0>
)

# Optional sanitizers (HIGHLY recommended)
target_compile_options(queue_memtest PRIVATE -fsanitize=address,undefined)
target_link_options(queue_memtest PRIVATE -fsanitize=address,undefined)