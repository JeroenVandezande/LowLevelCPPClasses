cmake_minimum_required(VERSION 3.31.6)
project(LowLevelEmbedded CXX)

set(CMAKE_CXX_STANDARD 17)
enable_language(C)

file(GLOB_RECURSE LIB_INCLUDES "Base/*.h" "Base/*.hpp" "Devices/*.h" "Devices/*.hpp" "Logging/*.h" "Logging/*.hpp" "Segger_RTT/*.h" "Segger_RTT/*.hpp")
file(GLOB_RECURSE LIB_SOURCES "Base/*.c" "Base/*.cpp" "Devices/*.c" "Devices/*.cpp" "Logging/*.c" "Logging/*.cpp" "Segger_RTT/*.c" "Segger_RTT/*.cpp")

add_library(${PROJECT_NAME} ${LIB_INCLUDES} ${LIB_SOURCES})

# Collect and de-duplicate parent directories
set(LIB_INCLUDE_DIRS "")
foreach(hdr IN LISTS LIB_INCLUDES)
    get_filename_component(dir "${hdr}" DIRECTORY)
    list(APPEND LIB_INCLUDE_DIRS "${dir}")
endforeach()
list(REMOVE_DUPLICATES LIB_INCLUDE_DIRS)

include(cpm/CPM.cmake)
CPMAddPackage("gh:an-dr/microlog@6.4.10")

target_compile_definitions( microlog
        INTERFACE
        ULOG_EXTRA_OUTPUTS=1
        FEATURE_COLOR=1)

target_link_libraries(${PROJECT_NAME} PUBLIC microlog)

target_include_directories (${PROJECT_NAME} PUBLIC ${LIB_INCLUDE_DIRS})

