#ifndef Code4Hep_PodioUtilities_CollectionWrapperConverter_h
#define Code4Hep_PodioUtilities_CollectionWrapperConverter_h

#include <string_view>
#include <typeinfo>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBase.h"
#include "DataFormats/Common/interface/Wrapper.h"
#include "FWCore/Utilities/interface/concatenate.h"

namespace podio {
  class CollectionBase;
}

namespace code4hep {
  template <typename T>
  class CollectionWrapperConverter : public CollectionWrapperConverterBase {
  public:
    std::unique_ptr<edm::WrapperBase> toWrapper(podio::CollectionBase& iCollection) const final {
      auto& collection = dynamic_cast<T&>(iCollection);
      return std::make_unique<edm::Wrapper<T>>(edm::WrapperBase::Emplace(), std::move(collection));
    }
    podio::CollectionBase const* getCollection(edm::WrapperBase const& iWrapper) const final {
      auto const& wrapper = dynamic_cast<edm::Wrapper<T> const&>(iWrapper);
      return wrapper.product();
    }
    std::string_view typeName() const final { return T::typeName; }
    std::type_info const& typeID() const final { return typeid(T); };

    static std::unordered_map<std::string_view, FromVectorToVector>& copiers() {
      static std::unordered_map<std::string_view, FromVectorToVector> s_copiers;
      return s_copiers;
    }

    std::unique_ptr<podio::CollectionBase> createEmpty() const {
      return std::make_unique<T>();
    }

  private:
    CollectionWrapperConverterBase::FromVectorToVector copier(std::string_view iName) const final {
      auto itFound = copiers().find(iName);
      if (itFound == copiers().end()) {
        throwMissingConverter(T::typeName, iName);
      }
      return itFound->second;
    }
  };

  namespace cwc {
    //Used when writing code for converter
    template <typename T>
    static void copyVector(void const* iFrom, void* iTo) {
      std::vector<T> const& f = *reinterpret_cast<std::vector<T> const*>(iFrom);
      std::vector<T>& t = *reinterpret_cast<std::vector<T>*>(iTo);
      std::copy(f.begin(), f.end(), std::back_inserter(t));
    }
    template <typename C, typename T>
    struct AddEntry {
      using FromVectorToVector = CollectionWrapperConverterBase::FromVectorToVector;
      AddEntry(std::string_view iName) { CollectionWrapperConverter<C>::copiers().emplace(iName, copyVector<T>); }
    };
  }  // namespace cwc
}  // namespace code4hep

#define C4H_CONTAINED_CLASS_SYM(x, y) C4H_CONTAINED_CLASS_SYM2(x, y)
#define C4H_CONTAINED_CLASS_SYM2(x, y) x##y
#define C4H_CONTAINED_CLASS(collection, name, type) \
  static const code4hep::cwc::AddEntry<collection, type> C4H_CONTAINED_CLASS_SYM(s_entry, __LINE__)(name)

#endif
