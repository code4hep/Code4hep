# DataFormats/TestObjects library
file(GLOB SOURCES src/*.h)
file(GLOB HEADERS interface/*.h)

add_library(code4hep_DataFormats SHARED ${SOURCES})

# Generate ROOT dictionary if classes_def.xml exists
stitched_generate_dictionary(code4hep_DataFormats_dict
    ${CMAKE_CURRENT_SOURCE_DIR}/src/classes.h
    LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/classes_def.xml
    MODULE code4hep_DataFormats
    OPTIONS --reflex
)

target_sources(code4hep_DataFormats
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_SOURCE_DIR}
        FILES ${HEADERS}
)

target_link_libraries(code4hep_DataFormats
    PUBLIC
        Stitched::stitched_DataFormats_Common
        ROOT::Core
        EDM4HEP::edm4hep
)

set_target_properties(code4hep_DataFormats PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

add_subdirectory(plugins)
