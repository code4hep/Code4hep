// -*- C++ -*-
//
// Package:    Code4hep/TestTracksAnalyzer
// Class:      TestTracksAnalyzer
//
/**\class TestTracksAnalyzer TestTracksAnalyzer.cc Code4hep/TestTracksAnalyzer/plugins/TestTracksAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Christopher Jones
//         Created:  Fri, 31 Oct 2025 20:42:18 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "edm4hep/EventHeaderCollection.h"
#include "edm4hep/TrackCollection.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

namespace c4h {
  class TestTracksAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
  public:
    explicit TestTracksAnalyzer(const edm::ParameterSet&);
    ~TestTracksAnalyzer() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    void beginJob() final;
    void analyze(const edm::Event&, const edm::EventSetup&) final;
    void endJob() final;

    // ----------member data ---------------------------
    edm::EDGetTokenT<edm4hep::EventHeaderCollection>
        eventHeaderToken_;  //used to select what event headers to read from configuration file
    edm::EDGetTokenT<edm4hep::TrackCollection> tracksToken_;  //used to select what tracks to read from configuration file
  };
}  // namespace c4h

using namespace c4h;
//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TestTracksAnalyzer::TestTracksAnalyzer(const edm::ParameterSet& iConfig)
    : eventHeaderToken_(consumes(iConfig.getUntrackedParameter<edm::InputTag>("eventHeaders"))),
      tracksToken_(consumes(iConfig.getUntrackedParameter<edm::InputTag>("tracks"))) {
  //now do what ever initialization is needed
}

//
// member functions
//

// ------------ method called for each event  ------------
void TestTracksAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  edm::LogInfo("EventHeaders").log([this, &iEvent](auto& log) {
    for (const auto& eventHeader : iEvent.get(eventHeaderToken_)) {
      log << eventHeader;
    }
  });
  edm::LogInfo("Tracks").log([this, &iEvent](auto& log) {
    for (const auto& track : iEvent.get(tracksToken_)) {
      log << track << "\n";
    }
  });
}

// ------------ method called once each job just before starting event loop  ------------
void TestTracksAnalyzer::beginJob() {
  // please remove this method if not needed
}

// ------------ method called once each job just after ending the event loop  ------------
void TestTracksAnalyzer::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void TestTracksAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.addUntracked<edm::InputTag>("eventHeaders");
  desc.addUntracked<edm::InputTag>("tracks");
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //edm::ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks", edm::InputTag("ctfWithMaterialTracks"));
  //descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(c4h::TestTracksAnalyzer);
