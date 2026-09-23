#include <string>

#include "podio/CollectionIDTable.h"

#include "DataFormats/Provenance/interface/ProductDescription.h"

#include "Code4hep/PodioUtilities/assigningProductTo.h"

namespace edm {

  void assigningProductTo(podio::CollectionBase& collection,
                          edm::ProductDescription const& desc,
                          edm::ProductID const&) {
    std::string collectionName = desc.moduleLabel() + desc.productInstanceName();

    podio::CollectionIDTable collectionIDTable;
    auto id = collectionIDTable.add(collectionName);
    collection.setID(id);
  }

}  // namespace edm
