# FWCore/ParameterSet library
file(GLOB SOURCES src/*.cc)
file(GLOB HEADERS interface/*.h)


add_library(code4hep_PodioUtilities SHARED ${SOURCES})

target_include_directories(code4hep_PodioUtilities
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_sources(code4hep_PodioUtilities
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_SOURCE_DIR}
        FILES ${HEADERS}
)

target_link_libraries(code4hep_PodioUtilities
  PUBLIC
  podio::podio
  Stitched::stitched_FWCore_PluginManager
  Stitched::stitched_FWCore_Utilities
)

set_target_properties(code4hep_PodioUtilities PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# Add bin directory for executables
#add_subdirectory(bin)
