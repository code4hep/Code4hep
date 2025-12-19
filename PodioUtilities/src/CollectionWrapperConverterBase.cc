#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBase.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBase.h"
#include <cassert>

namespace code4hep {
  namespace {
    void fromVectorToVector(void const* iFrom,
                            void* iTo,
                            CollectionWrapperConverterBase::FromVectorToVector converter) {
      auto f = reinterpret_cast<void* const*>(iFrom);
      converter(*f, iTo);
    }
  }  // namespace

  std::unique_ptr<podio::CollectionBase> CollectionWrapperConverterBase::copy(podio::CollectionBase& iCollection) const {
    iCollection.prepareForWrite();
    auto writeBufs = iCollection.getBuffers();

    auto readBufs = podio::CollectionBufferFactory::instance().createBuffers(
        std::string(iCollection.getTypeName()), iCollection.getSchemaVersion(), false);
    assert(readBufs);

    if (writeBufs.data) {
      fromVectorToVector(writeBufs.data, readBufs->data, copier(iCollection.getDataTypeName()));
    }

    assert(readBufs->references != nullptr);
    assert(writeBufs.references != nullptr);
    readBufs->references->reserve(writeBufs.references->size());

    for (auto const& r : *(writeBufs.references)) {
      podio::UVecPtr<podio::ObjectID> entry;
      if (r) {
        entry = std::make_unique<std::vector<podio::ObjectID>>();
        entry->reserve(r->size());
        std::copy(r->begin(), r->end(), std::back_inserter(*entry));
      }
      readBufs->references->emplace_back(std::move(entry));
    }

    assert(readBufs->vectorMembers != nullptr);
    assert(writeBufs.vectorMembers != nullptr);

    assert(readBufs->vectorMembers->size() == writeBufs.vectorMembers->size());
    for (size_t index = 0; index < writeBufs.vectorMembers->size(); ++index) {
      assert((*writeBufs.vectorMembers)[index].first == (*(readBufs->vectorMembers))[index].first);
      fromVectorToVector((*writeBufs.vectorMembers)[index].second,
                         (*(readBufs->vectorMembers))[index].second,
                         copier((*writeBufs.vectorMembers)[index].first));
    }
    auto newCol = readBufs->createCollection(*readBufs, iCollection.isSubsetCollection());
    newCol->prepareAfterRead();
    newCol->setSubsetCollection(iCollection.isSubsetCollection());
    newCol->setID(iCollection.getID());
    return newCol;
  }

  void CollectionWrapperConverterBase::throwMissingConverter(std::string_view iCollection,
                                                             std::string_view iContainedType) {
    throw cms::Exception("MissingConverter")
        << "The collection '" << iCollection << "' contains type '" << iContainedType << "'. The type '"
        << iContainedType << "' needs to be registered using the C4H_CONTAINED_CLASS macro.";
  }
}  // namespace code4hep
