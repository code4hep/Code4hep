#include "podio/Writer.h"
#include "podio/Frame.h"
#include "podio/CollectionBase.h"
#include "edm4hep/edm4hep.h"

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
                                        << "' is not registered with the C4H_COLLECTION macro so cannot be stored. It "
                                           "will not be consumed by this module.";
      return false;
    }
    return true;
  }

  void PodioOutputModule::write(edm::EventForOutput const& e) {
    auto factory = code4hep::CollectionWrapperConverterBaseFactory::get();

    bool haveEventHeader = false;
    podio::Frame frame;
    for (auto const& product : keptProducts()[edm::InEvent]) {
      auto const& typeName = typeNameConversion(product.first->fullClassName());

      haveEventHeader = (product.first->moduleLabel() == "EventHeader");
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
        //Data product is missing, but we MUST include one
        auto empty = converter->createEmpty();
        frame.put(std::move(empty), collectionName);
      }
    }
    if (not haveEventHeader) {
      auto ehc = std::make_unique<edm4hep::EventHeaderCollection>();
      auto eh = ehc->create();
      eh.setEventNumber(e.id().event());
      eh.setRunNumber(e.id().run());
      eh.setTimeStamp(e.time().value());
      frame.put(std::move(ehc), "EventHeader");
    }
    writer_.writeEvent(frame);
  }
  void PodioOutputModule::writeLuminosityBlock(edm::LuminosityBlockForOutput const&) {}
  void PodioOutputModule::writeRun(edm::RunForOutput const&) {}
}  // namespace c4h

using c4h::PodioOutputModule;
DEFINE_FWK_MODULE(PodioOutputModule);
