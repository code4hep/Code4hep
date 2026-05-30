file(GLOB PLUGIN_SOURCES plugins/*.cc)
stitched_generate_plugin(
    TARGET code4hep_IOPlugin
    SOURCES ${PLUGIN_SOURCES}
    LINK_LIBRARIES
    Stitched::stitched_FWCore_Framework
    code4hep_PodioUtilities
    EDM4HEP::edm4hep
    podio::podioIO
)

