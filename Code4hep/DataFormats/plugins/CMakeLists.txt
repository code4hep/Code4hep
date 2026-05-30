file(GLOB PLUGIN_SOURCES *.cc)
stitched_generate_plugin(
    TARGET code4hep_DataFormatsPlugin
    SOURCES ${PLUGIN_SOURCES}
    LINK_LIBRARIES
    code4hep_DataFormats
    code4hep_PodioUtilities
)

