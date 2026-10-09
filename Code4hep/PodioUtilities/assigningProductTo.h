#ifndef Code4hep_PodioUtilities_assigningProductTo_h
#define Code4hep_PodioUtilities_assigningProductTo_h

/**

  Description:

    This is a customization point that modifies the behavior
    of calls to edm::Event::put and edm::Event::emplace if
    the product inherits from podio::CollectionBase.
    If this header is included before those functions are
    called, then the setID function will be called on the
    collection being put into the Event and it will insert
    a hash based on the ProductDescription into the collection.
    This is necessary for relations and links in podio
    to work properly in Code4hep.

    You MUST include this header in any producer of podio
    collections! You do not need to do anything else. You
    should not directly call the function.
 */
//
// Author:      W. David Dagenhart
// Created:     11 September 2026

#include "DataFormats/Provenance/interface/ProductDescriptionFwd.h"
#include "DataFormats/Provenance/interface/ProductID.h"

#include "podio/CollectionBase.h"

namespace edm {

  void assigningProductTo(podio::CollectionBase& collection,
                          edm::ProductDescription const& desc,
                          edm::ProductID const&);

}  // namespace edm

#endif
