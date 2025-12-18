#ifndef Code4Hep_PodioUtilities_CollectionWrapperConverterBaseFactory_h
#define Code4Hep_PodioUtilities_CollectionWrapperConverterBaseFactory_h
#include "FWCore/PluginManager/interface/PluginFactory.h"

namespace code4hep {
  class CollectionWrapperConverterBase;
  using CollectionWrapperConverterBaseFactory = edmplugin::PluginFactory<CollectionWrapperConverterBase*()>;
}  // namespace code4hep

#endif
