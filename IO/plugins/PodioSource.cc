/**\class c4h::PodioSource

  Description: An InputSource for reading Podio files

  Written to support serial processing of files (1 file open at a time).

*/
//
// Author:      W. David Dagenhart
// Created:     31 October 2025

#include <memory>
#include <string>
#include <vector>

#include "PodioFile.h"

#include "FWCore/Framework/interface/FileBlock.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/InputSource.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/EDMException.h"

namespace c4h {

  using namespace edm;

  class PodioSource : public InputSource {
  public:
    PodioSource(ParameterSet const& pset, InputSourceDescription const& desc);

    static void fillDescriptions(ConfigurationDescriptions& descriptions);

  private:
    ItemTypeInfo getNextItemType() override;
    std::shared_ptr<RunAuxiliary> readRunAuxiliary_() override;
    std::shared_ptr<LuminosityBlockAuxiliary> readLuminosityBlockAuxiliary_() override;
    std::shared_ptr<FileBlock> readFile_() override;
    void closeFile_() override;
    void readRun_(RunPrincipal& runPrincipal) override;
    void readLuminosityBlock_(LuminosityBlockPrincipal& lumiPrincipal) override;
    void readEvent_(EventPrincipal& eventPrincipal) override;

    std::vector<std::string> inputFiles_;
    std::vector<std::string>::const_iterator currentFileIter_;
    std::vector<std::string>::const_iterator lastFileIter_;
    bool firstFile_{true};
    std::unique_ptr<PodioFile> podioFile_;
  };

  PodioSource::PodioSource(ParameterSet const& pset, InputSourceDescription const& desc)
      : InputSource(pset, desc),
        inputFiles_(pset.getUntrackedParameter<std::vector<std::string>>("fileNames")),
        currentFileIter_(inputFiles_.begin()),
        lastFileIter_(inputFiles_.end() - 1) {
    if (processingMode() != RunsLumisAndEvents) {
      throw Exception(errors::Configuration) << "PodioSource constructor: PodioSource does not support\n"
                                             << "processing modes other than RunsLumisAndEvents\n";
    }
    if (currentFileIter_ != inputFiles_.end()) {
      podioFile_ = std::make_unique<PodioFile>(*currentFileIter_, processHistoryRegistryForUpdate());
    }

    std::vector<std::string> processOrder;
    processingOrderMerge(processHistoryRegistry(), processOrder);

    productRegistryUpdate().updateFromInput(podioFile_->productRegistry()->productList(), processOrder);
  }

  void PodioSource::fillDescriptions(ConfigurationDescriptions& descriptions) {
    ParameterSetDescription desc;
    desc.addUntracked<std::vector<std::string>>("fileNames")->setComment("Names of files to be processed.");
    InputSource::fillDescription(desc);
    descriptions.add("source", desc);
  }

  InputSource::ItemTypeInfo PodioSource::getNextItemType() {
    if (firstFile_) {
      if (currentFileIter_ == inputFiles_.end()) {
        return ItemType::IsStop;
      }
      return ItemTypeInfo(ItemType::IsFile);
    }
    ItemType nextItemType = podioFile_->getNextItemType();
    if (nextItemType != ItemType::IsFile) {
      return nextItemType;
    }
    if (currentFileIter_ == lastFileIter_) {
      return ItemTypeInfo(ItemType::IsStop);
    }
    return ItemTypeInfo(ItemType::IsFile);
  }

  std::shared_ptr<RunAuxiliary> PodioSource::readRunAuxiliary_() { return podioFile_->readRunAuxiliary_(); }

  std::shared_ptr<LuminosityBlockAuxiliary> PodioSource::readLuminosityBlockAuxiliary_() {
    return podioFile_->readLuminosityBlockAuxiliary_();
  }

  std::shared_ptr<FileBlock> PodioSource::readFile_() {
    if (firstFile_) {
      firstFile_ = false;
    } else {
      ++currentFileIter_;
      podioFile_ = std::make_unique<PodioFile>(*currentFileIter_, processHistoryRegistryForUpdate());
    }

    // make sure the new product registry is compatible with the main one
    std::string const& mergeInfo =
        productRegistryUpdate().merge(*podioFile_->productRegistry(), *currentFileIter_, ProductDescription::Strict);
    if (!mergeInfo.empty()) {
      throw Exception(errors::MismatchedInputFiles, "PodioSource::readFile_()") << mergeInfo;
    }

    return std::make_shared<FileBlock>();
  }

  void PodioSource::closeFile_() { podioFile_.reset(); }

  void PodioSource::readRun_(RunPrincipal& runPrincipal) {
    podioFile_->readRun_(runPrincipal, processHistoryRegistry());
  }

  void PodioSource::readLuminosityBlock_(LuminosityBlockPrincipal& lumiPrincipal) {
    podioFile_->readLuminosityBlock_(lumiPrincipal, processHistoryRegistry());
  }

  void PodioSource::readEvent_(EventPrincipal& eventPrincipal) {
    podioFile_->readEvent_(eventPrincipal, processHistoryRegistry());
  }

}  // namespace c4h
using c4h::PodioSource;
DEFINE_FWK_INPUT_SOURCE(PodioSource);
