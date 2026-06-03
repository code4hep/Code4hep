#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "podio/CollectionBase.h"

#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBaseFactory.h"
#include "DataFormats/Common/interface/WrapperBase.h"
#include "DataFormats/Provenance/interface/ProductDescription.h"
#include "DataFormats/Provenance/interface/ProductProvenance.h"

#include "putOnReadForAllProducts.h"
#include "TypeNameConversion.h"

namespace c4h {

  void putOnReadForAllProducts(podio::Frame const& podioFrame,
                               edm::ProductRegistry const& productRegistry,
                               edm::EventPrincipal& eventPrincipal) {
    auto factory = code4hep::CollectionWrapperConverterBaseFactory::get();

    // It is important that are two iterations, one to get the
    // collections into the Frame and then a second to move them
    // into the Event. The initialization of one-to-many relations
    // fails if these steps are done in the same iteration.
    for (auto const& iter : productRegistry.productList()) {
      auto const& productDescription = iter.second;
      std::string const& moduleLabel = productDescription.moduleLabel();
      podioFrame.get(moduleLabel);
    }

    for (auto const& iter : productRegistry.productList()) {
      auto const& productDescription = iter.second;
      std::string const& moduleLabel = productDescription.moduleLabel();
      std::optional<edm::ProductProvenance> productProvenance;
      std::unique_ptr<edm::WrapperBase> wrapperBase;

      const podio::CollectionBase* constCollection = podioFrame.get(moduleLabel);

      // We need to construct a Wrapper<T> object to put in the edm::Event which
      // contains the collection.

      if (constCollection) {
        // Most of the time the pointer is not null and we take this code path

        // We must use a const_cast here. It is not nice, but I have not yet thought
        // of a way to avoid it.
        // Note that if you trace this pointer's origin it comes from an internal
        // pointer in podio that is the result of converting a non-const pointer
        // to a const pointer, so this is not undefined behavior, but it is fragile.
        auto* collection = const_cast<podio::CollectionBase*>(constCollection);

        std::string type_name(collection->getTypeName());
        auto converter = factory->create(type_name);

        wrapperBase = converter->toWrapper(*collection);
      } else {
        // As currently implemented, PodioOutputModule will always write a collection.
        // It will write an empty collection if one is not found in the Event.
        // Therefore, when reading a file written by PodioOutputModule, we should never
        // encounter the case where constCollection is a null pointer.
        // But it is possible using Podio directly to create a file where the pointer
        // is null. In that case, the following code will do the correct thing. A
        // ProductNotFound exception will be thrown if a module tries to get such a collection.
        std::string type_name(productDescription.fullClassName());
        auto converter = factory->create(typeNameConversion(type_name));
        wrapperBase = converter->createEmptyWrapper();
      }

      eventPrincipal.putOnRead(productDescription, std::move(wrapperBase), productProvenance);
    }
  }
}  // namespace c4h
