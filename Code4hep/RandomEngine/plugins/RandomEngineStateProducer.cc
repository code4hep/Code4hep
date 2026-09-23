// -*- C++ -*-
//
// Package:    RandomEngine
// Class:      RandomEngineStateProducer
//
/** \class RandomEngineStateProducer

 Description: Gets the state of the random number engines from
the related service and stores it in the event and luminosity block.

 Implementation:  This simply copies from the cache in the
service and puts the product in the Event and LuminosityBlock.
The cache is filled at the beginning of processing for each
event or lumi by a call from the InputSource or EventProcessor
to the service. This module gets called later.

\author W. David Dagenhart, created October 4, 2006
  (originally in FWCore/Services)
*/

#include "FWCore/AbstractServices/interface/RandomNumberGenerator.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Code4hep/RandomEngine/RandomEngineStates.h"

#include <memory>

class RandomEngineStateProducer : public edm::global::EDProducer<edm::BeginLuminosityBlockProducer> {
public:
  explicit RandomEngineStateProducer(edm::ParameterSet const& pset);
  ~RandomEngineStateProducer() override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void globalBeginLuminosityBlockProduce(edm::LuminosityBlock&, edm::EventSetup const&) const override;
  void produce(edm::StreamID iID, edm::Event& ev, edm::EventSetup const& es) const override;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
RandomEngineStateProducer::RandomEngineStateProducer(edm::ParameterSet const&) {
  produces<edm::RandomEngineStates, edm::Transition::BeginLuminosityBlock>("beginLumi");
  produces<edm::RandomEngineStates>();
}

RandomEngineStateProducer::~RandomEngineStateProducer() {}

void RandomEngineStateProducer::produce(edm::StreamID iID, edm::Event& ev, edm::EventSetup const&) const {
  edm::Service<edm::RandomNumberGenerator>().and_then([&ev](auto const& randomService) {
    auto states = std::make_unique<edm::RandomEngineStates>();
    states->setRandomEngineStates(randomService.getEventCache(ev.streamID()));
    ev.put(std::move(states));
  });
}

void RandomEngineStateProducer::globalBeginLuminosityBlockProduce(edm::LuminosityBlock& lb,
                                                                  edm::EventSetup const&) const {
  edm::Service<edm::RandomNumberGenerator>().and_then([&lb](auto const& randomService) {
    auto states = std::make_unique<edm::RandomEngineStates>();
    states->setRandomEngineStates(randomService.getLumiCache(lb.index()));
    lb.put(std::move(states), "beginLumi");
  });
}

void RandomEngineStateProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  descriptions.add("randomEngineStateProducer", desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RandomEngineStateProducer);
