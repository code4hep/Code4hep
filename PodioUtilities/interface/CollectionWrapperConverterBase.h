#ifndef Code4Hep_PodioUtilities_CollectionWrapperConverterBase_h
#define Code4Hep_PodioUtilities_CollectionWrapperConverterBase_h

#include <string_view>
#include <typeinfo>
#include <functional>
#include <memory>

namespace podio {
  class CollectionBase;
}

namespace edm {
  class WrapperBase;
}

namespace code4hep {
  class CollectionWrapperConverterBase {
  public:
    virtual ~CollectionWrapperConverterBase() = default;

    //The CollectionBase will be moved
    virtual std::unique_ptr<edm::WrapperBase> toWrapper(podio::CollectionBase&) const = 0;
    virtual podio::CollectionBase const* getCollection(edm::WrapperBase const&) const = 0;
    virtual std::string_view typeName() const = 0;
    virtual std::type_info const& typeID() const = 0;

    //Internally the buffer will be requested which is a non-const interface call
    std::unique_ptr<podio::CollectionBase> copy(podio::CollectionBase&) const;

    using FromVectorToVector = std::function<void(void const*, void*)>;

  protected:
    static void throwMissingConverter(std::string_view iCollection, std::string_view iContainedType);

  private:
    virtual FromVectorToVector copier(std::string_view) const = 0;
  };
}  // namespace code4hep

#endif
