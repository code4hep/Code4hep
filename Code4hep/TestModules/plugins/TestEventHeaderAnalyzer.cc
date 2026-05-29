// -*- C++ -*-
//
// Package:    Code4hep/TestEventHeaderAnalyzer
// Class:      TestEventHeaderAnalyzer
//
/**\class TestEventHeaderAnalyzer TestEventHeaderAnalyzer.cc Code4hep/TestEventHeaderAnalyzer/plugins/TestEventHeaderAnalyzer.cc

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

namespace c4h {
  class TestEventHeaderAnalyzer : public edm::one::EDAnalyzer<> {
  public:
    explicit TestEventHeaderAnalyzer(const edm::ParameterSet&);
    ~TestEventHeaderAnalyzer() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    void beginJob() final;
    void analyze(const edm::Event&, const edm::EventSetup&) final;
    void endJob() final;

    // ----------member data ---------------------------
    edm::EDGetTokenT<edm4hep::EventHeaderCollection>
        eventHeaderToken_;  //used to select what event headers to read from configuration file
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
TestEventHeaderAnalyzer::TestEventHeaderAnalyzer(const edm::ParameterSet& iConfig)
    : eventHeaderToken_(consumes(iConfig.getUntrackedParameter<edm::InputTag>("eventHeaders"))) {}

//
// member functions
//

// ------------ method called for each event  ------------
void TestEventHeaderAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  edm::LogInfo("EventHeaders").log([this, &iEvent](auto& log) {
    for (const auto& eventHeader : iEvent.get(eventHeaderToken_)) {
      log << eventHeader;
    }
  });
}

// ------------ method called once each job just before starting event loop  ------------
void TestEventHeaderAnalyzer::beginJob() {
  // please remove this method if not needed
}

// ------------ method called once each job just after ending the event loop  ------------
void TestEventHeaderAnalyzer::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void TestEventHeaderAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.addUntracked<edm::InputTag>("eventHeaders");

  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(c4h::TestEventHeaderAnalyzer);
