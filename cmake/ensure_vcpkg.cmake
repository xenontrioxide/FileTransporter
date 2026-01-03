# -----------------------------------------------------------------------------
# VCPKG INTEGRATION
# This MUST run before project() to define the toolchain file.
# -----------------------------------------------------------------------------
function(ensure_vcpkg_used)
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(DEFINED ENV{VCPKG_ROOT})
            file(TO_CMAKE_PATH "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" VCPKG_PATH)
            message(STATUS "Found VCPKG_ROOT. Setting toolchain to: ${VCPKG_PATH}")
            set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}" CACHE STRING "Vcpkg toolchain file")
        else()
            message(FATAL_ERROR "VCPKG_ROOT environment variable is not set. You must set it or pass -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")
        endif()
    endif()
endfunction()