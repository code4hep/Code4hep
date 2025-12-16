#ifndef Code4hep_IO_fillProductRegistry_h
#define Code4hep_IO_fillProductRegistry_h
/**

  Description: A function that fills an edm::ProductRegistry
  from a Podio file for all available collections.

*/
// Author:      W. David Dagenhart
// Created:     11 December 2025

#include <memory>
#include <string>

#include "podio/Frame.h"

#include "DataFormats/Provenance/interface/ProductRegistry.h"

namespace c4h {

  std::unique_ptr<edm::ProductRegistry> fillProductRegistry(podio::Frame const& podioFrame,
                                                            std::string const& processName);
}  // namespace c4h
#endif
