# Standalone host tests; no MCU SDK required. Only the MCP9801 suite needs an
# external package (unitsnet-cpp, fetched through CPM.cmake).
cmake_minimum_required(VERSION 3.20)
project(LowLevelCPPClassesTests LANGUAGES CXX)
enable_testing()
add_executable(DDC114Tests DDC114Tests.cpp ../Devices/ADCs/DDC114.cpp)
target_include_directories(DDC114Tests PRIVATE ../Devices/ADCs)
target_compile_features(DDC114Tests PRIVATE cxx_std_20)
# Assertions are the test checks and must remain enabled in Release builds.
if(MSVC)
    target_compile_options(DDC114Tests PRIVATE /UNDEBUG)
else()
    target_compile_options(DDC114Tests PRIVATE -UNDEBUG -Wall -Wextra -Wpedantic)
endif()
add_test(NAME DDC114 COMMAND DDC114Tests)

add_executable(CD74HC4059Tests CD74HC4059Tests.cpp ../Devices/ClockDividers/CD74HC4059.cpp)
target_include_directories(CD74HC4059Tests PRIVATE ../Devices/ClockDividers)
target_compile_features(CD74HC4059Tests PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(CD74HC4059Tests PRIVATE /UNDEBUG)
else()
    target_compile_options(CD74HC4059Tests PRIVATE -UNDEBUG -Wall -Wextra -Wpedantic)
endif()
add_test(NAME CD74HC4059 COMMAND CD74HC4059Tests)

# MCP9801 uses unitsnet-cpp types at its API boundary. The header-only package is
# resolved through CPM.cmake, as in the main project, unless the parent already
# provides the unitsnet_cpp target.
if(NOT TARGET unitsnet_cpp)
    if(NOT COMMAND CPMAddPackage)
        set(CPM_VERSION 0.43.1)
        file(
            DOWNLOAD
            https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/CPM_${CPM_VERSION}.cmake
        )
        include("${CMAKE_CURRENT_BINARY_DIR}/CPM_${CPM_VERSION}.cmake")
    endif()
    CPMAddPackage("gh:JeroenVandezande/unitsnet-cpp@2.0.1")
    target_compile_definitions(unitsnet_cpp INTERFACE UNITSNET_CPP_USE_FLOAT=1)
endif()
add_executable(MCP9801Tests MCP9801Tests.cpp ../Devices/Sensors/MCP9801/MCP9801.cpp)
target_include_directories(MCP9801Tests PRIVATE ../Devices/Sensors/MCP9801 ../Base ../Utilities)
target_link_libraries(MCP9801Tests PRIVATE UnitsNet::UnitsNet)
target_compile_features(MCP9801Tests PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(MCP9801Tests PRIVATE /UNDEBUG)
else()
    target_compile_options(MCP9801Tests PRIVATE -UNDEBUG -Wall -Wextra -Wpedantic)
endif()
add_test(NAME MCP9801 COMMAND MCP9801Tests)
