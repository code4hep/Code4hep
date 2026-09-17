# top-level <use> + <export><lib name="1"/> — standard library package
c4h_add_library(
    DEPS
        FWCore/Framework
        FWCore/ParameterSet
	FWCore/Utilities
    EXT_DEPS
        CLHEP::CLHEP
)

# Shared dependencies inherited by all <library> elements in this file.
set(_c4h_shared
    DEPS
        FWCore/Framework
        FWCore/ParameterSet
	FWCore/Utilities
        FWCore/PluginManager
)

# <library name="Code4hepRandomEnginePlugins" file="*.cc"> (EDM_PLUGIN=1)
c4h_add_plugin(
    NAME Code4hepRandomEnginePlugins
    SOURCES plugins/*.cc
    DEPS Code4hep/RandomEngine
    ${_c4h_shared}
)
