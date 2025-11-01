// -*- C++ -*-
//
// Package:    Code4hep/TestTracksProducer
// Class:      TestTracksProducer
//
/**\class TestTracksProducer TestTracksProducer.cc Code4hep/TestTracksProducer/plugins/TestTracksProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Christopher Jones
//         Created:  Fri, 31 Oct 2025 20:41:51 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Utilities/interface/EDPutToken.h"

#include "edm4hep/TrackCollection.h"

//
// class declaration
//

namespace c4h {
  class TestTracksProducer : public edm::global::EDProducer<> {
  public:
    explicit TestTracksProducer(const edm::ParameterSet&);
    ~TestTracksProducer() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const final;

    const edm::EDPutTokenT<edm4hep::TrackCollection> putToken_;
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
TestTracksProducer::TestTracksProducer(const edm::ParameterSet& iConfig) : putToken_{produces()} {}

//
// member functions
//

// ------------ method called to produce the data  ------------
void TestTracksProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  using namespace edm;

  edm4hep::TrackCollection tracks;

  auto track = tracks.create();

  iEvent.emplace(putToken_, std::move(tracks));
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void TestTracksProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(c4h::TestTracksProducer);
