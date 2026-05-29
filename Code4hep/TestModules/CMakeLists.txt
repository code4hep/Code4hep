file(GLOB SOURCES plugins/*.cc)

file(GLOB PLUGIN_SOURCES plugins/*.cc)
stitched_generate_plugin(
    TARGET code4hep_TestModulesPlugin
    SOURCES ${PLUGIN_SOURCES}
    LINK_LIBRARIES
        Stitched::stitched_FWCore_Framework
        Stitched::stitched_DataFormats_Provenance
        Stitched::stitched_FWCore_PluginManager
        Stitched::stitched_FWCore_ParameterSet
        Stitched::stitched_FWCore_Utilities
        EDM4HEP::edm4hep
)

