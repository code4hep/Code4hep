#ifndef Code4hep_IO_putOnReadForAllProducts_h
#define Code4hep_IO_putOnReadForAllProducts_h
/**

  Description: A function that puts all data products read
  from a Podio file into the EventPrincipal using the function
  edm::EventPrincipal::putOnRead

*/
// Author:      W. David Dagenhart
// Created:     11 December 2025

#include "podio/Frame.h"

#include "DataFormats/Provenance/interface/ProductRegistry.h"
#include "FWCore/Framework/interface/EventPrincipal.h"

namespace c4h {
  void putOnReadForAllProducts(podio::Frame const& podioFrame,
                               edm::ProductRegistry const& productRegistry,
                               edm::EventPrincipal& eventPrincipal);
}  // namespace c4h
#endif
