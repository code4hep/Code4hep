#include "podio/Writer.h"
#include "podio/Frame.h"
#include "podio/CollectionBase.h"

#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBaseFactory.h"
#include "FWCore/Framework/interface/one/OutputModule.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventForOutput.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <memory>
namespace c4h {

  class PodioOutputModule : public edm::one::OutputModule<> {
  public:
    PodioOutputModule(edm::ParameterSet const&);
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    std::string const& typeNameConversion(std::string const&) const;
    void write(edm::EventForOutput const& e) final;
    void writeLuminosityBlock(edm::LuminosityBlockForOutput const&) final;
    void writeRun(edm::RunForOutput const&) final;
    bool finalSelection(edm::ProductDescription const&) const final;
    podio::Writer writer_;
  };

  PodioOutputModule::PodioOutputModule(edm::ParameterSet const& iPSet)
      : edm::one::OutputModuleBase(iPSet),
        edm::one::OutputModule<>(iPSet),
        writer_{podio::makeWriter(iPSet.getUntrackedParameter<std::string>("fileName"),
                                  iPSet.getUntrackedParameter<std::string>("type"))} {}

  void PodioOutputModule::fillDescriptions(edm::ConfigurationDescriptions& oDesc) {
    edm::ParameterSetDescription pset;
    edm::one::OutputModule<>::fillDescription(pset);
    pset.addUntracked<std::string>("fileName")->setComment("The name of the file to write.");
    pset.addUntracked<std::string>("type", "default")
        ->setComment(
            "The type of the file to write, e.g. 'root'. The value of 'default' says the type should be based on the "
            "file extension in the name.");
    oDesc.addDefault(pset);
  }

  std::string const& PodioOutputModule::typeNameConversion(std::string const& iRootTypeName) const {
    static std::unordered_map<std::string, std::string> s_conversion = {
        {"podio::UserDataCollection<float,void>", "podio::UserDataCollection<float>"},
        {"podio::UserDataCollection<int,void>", "podio::UserDataCollection<int32_t>"}};
    auto itFound = s_conversion.find(iRootTypeName);
    if (itFound != s_conversion.end()) {
      return itFound->second;
    }

    return iRootTypeName;
  }
  bool PodioOutputModule::finalSelection(edm::ProductDescription const& product) const {
    try {
      auto const& typeName = typeNameConversion(product.fullClassName());

      auto factory = code4hep::CollectionWrapperConverterBaseFactory::get();
      (void)factory->create(typeName);
    } catch (cms::Exception const&) {
      edm::LogWarning("UnstorableType") << "The type '" << product.fullClassName()
                                        << "' is not regiersted with the C4H_COLLECTION macro so can not be stored. It "
                                           "will not be consumed by this module.";
      return false;
    }
    return true;
  }

  void PodioOutputModule::write(edm::EventForOutput const& e) {
    auto factory = code4hep::CollectionWrapperConverterBaseFactory::get();

    podio::Frame frame;
    for (auto const& product : keptProducts()[edm::InEvent]) {
      auto const& typeName = typeNameConversion(product.first->fullClassName());

      auto converter = factory->create(typeName);

      edm::TypeID const& tid = product.first->unwrappedTypeID();
      edm::EDGetToken const& token = product.second;
      edm::BasicHandle bh = e.getByToken(token, tid);

      std::string collectionName = product.first->moduleLabel() + product.first->productInstanceName();
      if (bh.isValid()) {
        assert(bh.wrapper());
        auto collectionBase = converter->getCollection(*bh.wrapper());
        assert(collectionBase);
        auto copiedCollection = converter->copy(*const_cast<podio::CollectionBase*>(collectionBase));
        assert(copiedCollection);
        frame.put(std::move(copiedCollection), collectionName);
      } else {
        std::cout << "  product is missing" << std::endl;
        //Data product is missing, but we MUST include one
        auto empty = converter->createEmpty();
        frame.put(std::move(empty), collectionName);
      }
    }
    writer_.writeEvent(frame);
  }
  void PodioOutputModule::writeLuminosityBlock(edm::LuminosityBlockForOutput const&) {}
  void PodioOutputModule::writeRun(edm::RunForOutput const&) {}
}  // namespace c4h

using c4h::PodioOutputModule;
DEFINE_FWK_MODULE(PodioOutputModule);
