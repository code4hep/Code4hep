#ifndef Code4hep_PodioUtilities_CollectionMacros_h
#define Code4hep_PodioUtilities_CollectionMacros_h

#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBaseFactory.h"
#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverter.h"

#define C4H_COLLECTION(type) \
  DEFINE_EDM_PLUGIN(code4hep::CollectionWrapperConverterBaseFactory, code4hep::CollectionWrapperConverter<type>, #type)

#endif
