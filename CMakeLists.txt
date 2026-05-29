cmake_minimum_required(VERSION 3.16)
project(Code4Hep)

# Set C++ standard (C++20 required for concepts)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Stitched REQUIRED)
find_package(EDM4HEP REQUIRED)
find_package(podio REQUIRED)

# Include GNUInstallDirs early so variables are available for environment scripts
include(GNUInstallDirs)

# Add compile options
add_compile_options(-Wall -Werror=return-type -Wno-attributes)
add_link_options(LINKER:-z,defs)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(Code4hep)

stitched_generate_plugincache(
  PLUGIN_TARGETS
  code4hep_TestModulesPlugin
  code4hep_IOPlugin
  code4hep_DataFormatsPlugin
)

# ============================================================================
# Installation and package configuration
# ============================================================================

include(CMakePackageConfigHelpers)

# Set installation directories
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
set(CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/Code4Hep)

# Install all targets with EXPORT
# Note: This assumes all libraries are built as SHARED libraries
# and should be exported together

# Get all library targets that were created
set(Code4Hep_EXPORT_TARGETS
  # libraries
  code4hep_PodioUtilities
  # plugins
  code4hep_TestModulesPlugin
  code4hep_IOPlugin
  code4hep_DataFormatsPlugin
  # DataFormats libraries
  code4hep_DataFormats
)

# Install libraries and executables
foreach(target ${Code4Hep_EXPORT_TARGETS})
    if(TARGET ${target})
        install(TARGETS ${target}
            EXPORT Code4HepTargets
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}
            ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
            RUNTIME DESTINATION ${BIN_INSTALL_DIR}
            FILE_SET HEADERS
        )
    endif()
endforeach()

# Print installation information
message(STATUS "Code4Hep will be installed to: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Libraries: ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
message(STATUS "  Headers: ${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}")
message(STATUS "  CMake config: ${CMAKE_INSTALL_PREFIX}/${CONFIG_INSTALL_DIR}")
