#ifndef Code4Hep_PodioUtilities_CollectionWrapperConverterBaseFactory_h
#define Code4Hep_PodioUtilities_CollectionWrapperConverterBaseFactory_h
#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBase.h"

namespace code4hep {
  using CollectionWrapperConverterBaseFactory = edmplugin::PluginFactory<CollectionWrapperConverterBase*()>;
}  // namespace code4hep

#endif
