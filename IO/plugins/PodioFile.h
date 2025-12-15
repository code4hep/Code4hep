#ifndef Code4hep_IO_PodioFile_h
#define Code4hep_IO_PodioFile_h
/**\class c4h::PodioFile

 Description: Handles a single open Podio file

*/
//
// Author:      W. David Dagenhart
// Created:     14 November 2025

#include <cstddef>
#include <memory>
#include <string>

#include "podio/Frame.h"
#include "podio/Reader.h"

#include "DataFormats/Provenance/interface/ProcessHistoryID.h"
#include "DataFormats/Provenance/interface/ProcessHistoryRegistry.h"
#include "DataFormats/Provenance/interface/RunLumiEventNumber.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/InputSource.h"

namespace c4h {

  using namespace edm;

  class PodioFile {
  public:
    PodioFile(std::string const& fileName, ProcessHistoryRegistry& processHistoryRegistry);

    PodioFile(PodioFile const&) = delete;
    PodioFile& operator=(PodioFile const&) = delete;

    InputSource::ItemType getNextItemType();
    std::shared_ptr<RunAuxiliary> readRunAuxiliary_();
    std::shared_ptr<LuminosityBlockAuxiliary> readLuminosityBlockAuxiliary_();

    void readRun_(RunPrincipal& runPrincipal, ProcessHistoryRegistry const& processHistoryRegistry);
    void readLuminosityBlock_(LuminosityBlockPrincipal& lumiPrincipal,
                              ProcessHistoryRegistry const& processHistoryRegistry);
    void readEvent_(EventPrincipal& eventPrincipal, ProcessHistoryRegistry const& processHistoryRegistry);

    void logFileAction(char const* msg, std::string const& fileName) const;

    std::shared_ptr<ProductRegistry const> productRegistry() const { return productRegistry_; }

  private:
    std::unique_ptr<podio::Reader> podioReader_;
    InputSource::ItemType nextItemType_{InputSource::ItemType::IsInvalid};
    RunNumber_t nextRun_{invalidRunNumber};
    // Podio files don't have a concept of lumis, so we just use a dummy value
    // that is always 1
    LuminosityBlockNumber_t nextLumi_{1U};
    EventNumber_t nextEvent_{invalidEventNumber};
    ProcessHistoryID processHistoryID_;
    std::size_t numberOfEvents_{0};
    std::size_t nextEventIndex_{0};
    std::size_t eventIndexOfOpenFrame_{0};
    bool getNextItemTypeCalled_{false};
    podio::Frame frame_;
    std::shared_ptr<ProductRegistry const> productRegistry_;
  };
}  // namespace c4h
#endif
