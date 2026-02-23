#include <string_view>
#include <typeinfo>
#include <vector>

#include "podio/CollectionBase.h"

#include "fillProductRegistry.h"

#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBaseFactory.h"
#include "DataFormats/Provenance/interface/ProductDescription.h"
#include "FWCore/Utilities/interface/BranchType.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/TypeID.h"

namespace c4h {

  std::unique_ptr<edm::ProductRegistry> fillProductRegistry(podio::Frame const& podioFrame,
                                                            std::string const& processName,
                                                            bool ignoreMissingOnFirstEvent) {
    auto productRegistry = std::make_unique<edm::ProductRegistry>();

    // Fill the process order first
    // The process order only contains the one process name for input products
    std::vector<std::string> processOrder = {processName};
    productRegistry->setProcessOrder(processOrder);

    edm::BranchType const branchType = edm::InEvent;
    std::string const productInstanceName = "";
    edm::TypeID typeID;
    bool const produced = false;

    auto factory = code4hep::CollectionWrapperConverterBaseFactory::get();

    // Iterate over all collections in the Podio Frame and
    // add a ProductDescription for each one to the ProductRegistry
    for (std::string const& collection : podioFrame.getAvailableCollections()) {
      const podio::CollectionBase* collectionBase = podioFrame.get(collection);

      // This pointer should never be null if there is a policy of writing
      // a collection for all events if it is written for any event.
      // Note that as currently implemented, PodioOutputModule will always
      // write all collections for all events. It will write an empty collection
      // if one is not found in the Event (so the pointer will never be null).
      // I believe it is possible using Podio directly to create a file where
      // we will encounter a nullptr because there is code in Podio to
      // handle that possibility, but I have not yet figured out how to create
      // such a file with Podio...
      // If null, Podio does not allow getting the collection type,
      // so this is a problem. I'm not completely happy with the following
      // behavior, but it is the best I could think of for now...
      // If we find this is inadequate, some possible alternatives for the future:
      // 1) Modify Podio to enable getting the type without getting the collection.
      // 2) Hardcode a mapping from collection name to type. This might just be for
      //    a subset of collection names where this is known to be a problem.
      // 3) Iterate through all events to try to find one where the collection
      //    is present.
      // 4) Drop problem collections on input identified via the configuration.
      //    This would require implementing a drop-on-input feature.
      // 5) Something else that I haven't thought of yet.
      if (!collectionBase) {
        if (ignoreMissingOnFirstEvent) {
          continue;
        } else {
          throw cms::Exception("MissingCollection")
              << "podio::Frame::get returned a nullptr for the collection "
              << "named\n'" << collection << "' on the first event of the file. If you know for\n"
              << "certain that this collection will not be needed for any events in\n"
              << "the file, you can set the ignoreMissingOnFirstEvent configuration\n"
              << "option to true to ignore this exception. However, if any module tries\n"
              << "to get such a collection a ProductNotFound exception will be thrown.\n";
        }
      }
      std::string type_name(collectionBase->getTypeName());
      try {
        auto converter = factory->create(type_name);
        std::type_info const& collection_type_info = converter->typeID();
        typeID = edm::TypeID(collection_type_info);
      } catch (cms::Exception const&) {
        throw cms::Exception("UnregisteredCollectionType")
            << "The data product type '" << collectionBase->getTypeName() << "' is not registered with the\n"
            << "C4H_COLLECTION macro so PodioSource cannot read it.";
      }

      // This line of code just emphasizes to the reader that a podio collection
      // name is used as the module label in the EDM ProductDescription
      std::string const& moduleLabel = collection;

      edm::ProductDescription productDescription(
          branchType, moduleLabel, processName, productInstanceName, typeID, produced);

      productRegistry->copyProduct(productDescription);
    }
    return productRegistry;
  }
}  // namespace c4h
