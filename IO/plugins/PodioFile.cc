#include <cassert>
#include <exception>
#include <iomanip>

#include "edm4hep/EventHeaderCollection.h"
#include "podio/CollectionBase.h"

#include "fillProductRegistry.h"
#include "putOnReadForAllProducts.h"
#include "PodioFile.h"

#include "DataFormats/Provenance/interface/EventAuxiliary.h"
#include "DataFormats/Provenance/interface/EventID.h"
#include "DataFormats/Provenance/interface/HardwareResourcesDescription.h"
#include "DataFormats/Provenance/interface/LuminosityBlockAuxiliary.h"
#include "DataFormats/Provenance/interface/ProcessConfiguration.h"
#include "DataFormats/Provenance/interface/ProcessHistory.h"
#include "DataFormats/Provenance/interface/RunAuxiliary.h"

#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/LuminosityBlockPrincipal.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/RunPrincipal.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Utilities/interface/TimeOfDay.h"
#include "DataFormats/Provenance/interface/Timestamp.h"

namespace c4h {

  using namespace edm;

  PodioFile::PodioFile(std::string const& fileName, ProcessHistoryRegistry& processHistoryRegistry) {
    try {
      podioReader_ = std::make_unique<podio::Reader>(podio::makeReader(fileName));
      logFileAction("  Successfully opened file ", fileName);
    } catch (std::exception const& ex) {
      // Don't print out ex.what() as its info is currently misleading
      throw Exception(errors::FileOpenError, "PodioSource")
          << "Failed to open input file \"" << fileName << "\"\nor failed to create podio::Reader\n";
    }
    numberOfEvents_ = podioReader_->getEntries("events");

    std::string const processNameForInputProducts = "PODIO";

    ParameterSet processParameterSet;
    processParameterSet.registerIt();
    ProcessConfiguration processConfiguration(
        processNameForInputProducts, processParameterSet.id(), "", HardwareResourcesDescription());
    ProcessHistory processHistory;
    processHistory.push_back(processConfiguration);
    processHistory.setProcessHistoryID();
    processHistoryRegistry.registerProcessHistory(processHistory);
    processHistoryID_ = processHistory.id();

    // Fill the ProductRegistry with ProductDescriptions
    // from the collections available in the first event
    std::size_t eventIndex = 0;
    frame_ = podioReader_->readFrame("events", eventIndex);
    eventIndexOfOpenFrame_ = eventIndex;

    auto productRegistry = c4h::fillProductRegistry(frame_, processNameForInputProducts);
    productRegistry_.reset(productRegistry.release());
  }

  InputSource::ItemType PodioFile::getNextItemType() {
    assert(!getNextItemTypeCalled_);
    getNextItemTypeCalled_ = true;

    if (numberOfEvents_ == 0) {
      return InputSource::ItemType::IsFile;
    }
    if (nextItemType_ == InputSource::ItemType::IsEvent || nextItemType_ == InputSource::ItemType::IsInvalid) {
      if (nextItemType_ != InputSource::ItemType::IsInvalid) {
        ++nextEventIndex_;
      }

      // Already processed the last event in the file
      if (nextEventIndex_ >= numberOfEvents_) {
        return InputSource::ItemType::IsFile;
      }

      if (nextEventIndex_ != eventIndexOfOpenFrame_) {
        frame_ = podioReader_->readFrame("events", nextEventIndex_);
        eventIndexOfOpenFrame_ = nextEventIndex_;
      }
      auto const& eventHeaderCollection = frame_.get<edm4hep::EventHeaderCollection>("EventHeader");
      auto const& eventHeader = eventHeaderCollection.at(0);
      RunNumber_t run = eventHeader.getRunNumber();
      nextEvent_ = eventHeader.getEventNumber();
      if (nextEvent_ == invalidEventNumber) {
        throw Exception(errors::LogicError, "PodioSource") << "Invalid event number in file: " << nextEvent_ << "\n";
      }
      if (run == invalidRunNumber) {
        throw Exception(errors::LogicError, "PodioSource") << "Invalid run number in file: " << run << "\n";
      }
      if (nextItemType_ == InputSource::ItemType::IsInvalid || run != nextRun_) {
        nextItemType_ = InputSource::ItemType::IsRun;
        nextRun_ = run;
      } else {
        nextItemType_ = InputSource::ItemType::IsEvent;
      }
    } else {
      if (nextItemType_ == InputSource::ItemType::IsLumi) {
        nextItemType_ = InputSource::ItemType::IsEvent;
      } else if (nextItemType_ == InputSource::ItemType::IsRun) {
        nextItemType_ = InputSource::ItemType::IsLumi;
      } else {
        // It should be impossible to get here
        // There is a bug we need to fix if we do
        assert(false);
      }
    }
    return nextItemType_;
  }

  std::shared_ptr<RunAuxiliary> PodioFile::readRunAuxiliary_() {
    auto runAuxiliary =
        std::make_shared<RunAuxiliary>(nextRun_, Timestamp::invalidTimestamp(), Timestamp::invalidTimestamp());
    runAuxiliary->setProcessHistoryID(processHistoryID_);
    return runAuxiliary;
  }

  std::shared_ptr<LuminosityBlockAuxiliary> PodioFile::readLuminosityBlockAuxiliary_() {
    auto lumiAuxiliary = std::make_shared<LuminosityBlockAuxiliary>(
        nextRun_, nextLumi_, Timestamp::invalidTimestamp(), Timestamp::invalidTimestamp());
    lumiAuxiliary->setProcessHistoryID(processHistoryID_);
    return lumiAuxiliary;
  }

  void PodioFile::readRun_(RunPrincipal& runPrincipal, ProcessHistoryRegistry const& processHistoryRegistry) {
    assert(getNextItemTypeCalled_);
    getNextItemTypeCalled_ = false;
    runPrincipal.fillRunPrincipal(processHistoryRegistry);
  }

  void PodioFile::readLuminosityBlock_(LuminosityBlockPrincipal& lumiPrincipal,
                                       ProcessHistoryRegistry const& processHistoryRegistry) {
    assert(getNextItemTypeCalled_);
    getNextItemTypeCalled_ = false;

    auto history = processHistoryRegistry.getMapped(lumiPrincipal.aux().processHistoryID());
    lumiPrincipal.fillLuminosityBlockPrincipal(history);
  }

  void PodioFile::readEvent_(EventPrincipal& eventPrincipal, ProcessHistoryRegistry const& processHistoryRegistry) {
    assert(getNextItemTypeCalled_);
    getNextItemTypeCalled_ = false;

    EventAuxiliary eventAuxiliary;
    eventAuxiliary.id() = EventID(nextRun_, nextLumi_, nextEvent_);
    eventAuxiliary.setProcessHistoryID(processHistoryID_);

    auto history = processHistoryRegistry.getMapped(processHistoryID_);
    eventPrincipal.fillEventPrincipal(eventAuxiliary, history);

    putOnReadForAllProducts(frame_, *productRegistry_, eventPrincipal);
  }

  void PodioFile::logFileAction(char const* msg, std::string const& fileName) const {
    LogAbsolute("fileAction") << std::setprecision(0) << TimeOfDay() << msg << fileName;
    FlushMessageLog();
  }

}  // namespace c4h
