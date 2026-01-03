cmake_minimum_required(VERSION 3.20)

# Search for custom cmake functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ensure_vcpkg)

# Ensure VCPKG is installed and working before setting up project
ensure_vcpkg_used()

project(FileTransporter VERSION 1.0.0 )

find_package(nlohmann_json REQUIRED)

add_library(${PROJECT_NAME} 
    SHARED
    src/Actions.cpp
    src/Actions.hpp
    src/ClassFactory.cpp
    src/ClassFactory.hpp
    src/ComUtils.cpp
    src/ComUtils.hpp
    src/ContextMenuHandler.cpp
    src/ContextMenuHandler.hpp
    src/DllMain.cpp
    src/FileOperations.cpp
    src/FileOperations.hpp
    src/MenuItem.cpp
    src/MenuItem.hpp
    src/Reg.cpp
    src/Reg.hpp
    src/Shared.hpp
)

# Add the function export definitions
target_sources(${PROJECT_NAME} PRIVATE src/FileTransporter.def)

# Ensure using C++23
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
)