// -*- C++ -*-
//
// Package:    Code4hep/TestModules
//
/**\class c4h::TestRelationsAndLinksProducer

 Description: This module produces collections that are
 used in tests of Podio relations and links.
 One can configure whether the relations and links
 point at objects from input files, from other
 producers in this process, or that this module
 itself produces. It produces a set of products
 that include one-to-one relations, one-to-many
 relations, links, and also one that is circular
 (tracks pointing at tracks).

*/
//
// Original Author:  W. David Dagenhart
//         Created:  16 July 2026
//

#include <utility>

#include "podio/LinkCollection.h"

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/RecDqdxCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/TrackCollection.h"

#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/EDPutToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "Code4hep/PodioUtilities/assigningProductTo.h"

namespace c4h {
  class TestRelationsAndLinksProducer : public edm::global::EDProducer<> {
  public:
    explicit TestRelationsAndLinksProducer(const edm::ParameterSet&);
    ~TestRelationsAndLinksProducer() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const final;

    const edm::EDPutTokenT<edm4hep::TrackCollection> tracksPutToken_;
    const edm::EDPutTokenT<edm4hep::RecDqdxCollection> recDqdxsPutToken_;
    const edm::EDPutTokenT<edm4hep::ReconstructedParticleCollection> reconstructedParticlesPutToken_;
    const edm::EDPutTokenT<edm4hep::MCParticleCollection> mcParticlesPutToken_;
    const edm::EDPutTokenT<podio::LinkCollection<edm4hep::Track, edm4hep::MCParticle>> linksPutToken_;

    const edm::EDGetTokenT<edm4hep::TrackCollection> tracksGetToken_;
    const edm::EDGetTokenT<edm4hep::MCParticleCollection> mcParticlesGetToken_;
    const bool linkToCollectionsMadeInModule_;
    const int testValue_;
  };

  TestRelationsAndLinksProducer::TestRelationsAndLinksProducer(const edm::ParameterSet& iConfig)
    : tracksPutToken_{produces()},
      // Note we need to give instance names for the other produced
      // items to keep the collection names in Podio unique.
      recDqdxsPutToken_{produces("recDqdxs")},
      reconstructedParticlesPutToken_{produces("reconstructedParticles")},
      mcParticlesPutToken_{produces("mcParticles")},
      linksPutToken_{produces("links")},
      tracksGetToken_{consumes(iConfig.getUntrackedParameter<edm::InputTag>("tracks"))},
      mcParticlesGetToken_{consumes(iConfig.getUntrackedParameter<edm::InputTag>("mcParticles"))},
      linkToCollectionsMadeInModule_{iConfig.getUntrackedParameter<bool>("linkToCollectionsMadeInModule")},
      testValue_{iConfig.getUntrackedParameter<int>("testValue")} {}

  void TestRelationsAndLinksProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const {
    // First create all the collections and objects
    edm4hep::TrackCollection tracks;
    auto track = tracks.create();
    track.setType(testValue_);

    edm4hep::RecDqdxCollection recDqdxs;
    auto recDqdx = recDqdxs.create();

    edm4hep::ReconstructedParticleCollection reconstructedParticles;
    auto reconstructedParticle = reconstructedParticles.create();

    edm4hep::MCParticleCollection mcParticles;
    auto mcParticle = mcParticles.create();
    mcParticle.setPDG(testValue_);

    podio::LinkCollection<edm4hep::Track, edm4hep::MCParticle> trackToMCParticleLinks;
    auto link = trackToMCParticleLinks.create();

    // Next set the relations and links between objects
    // Includes a one-to-one relation, a one-to-many relation,
    // a circular case (a product pointing to itself), and a
    // link.
    if (linkToCollectionsMadeInModule_) {
      track.addToTracks(track);
      recDqdx.setTrack(track);
      reconstructedParticle.addToTracks(track);
      link.set<edm4hep::Track>(track);
      link.set<edm4hep::MCParticle>(mcParticle);
    } else {
      const auto& inputTracks = iEvent.get(tracksGetToken_);
      if (inputTracks.empty()) {
        throw cms::Exception("TestFailure") << "Expected non-empty input track collection";
      }
      const auto& inputTrack = inputTracks[0];
      track.addToTracks(inputTrack);
      recDqdx.setTrack(inputTrack);
      reconstructedParticle.addToTracks(inputTrack);
      link.set<edm4hep::Track>(inputTrack);

      const auto& inputMCParticles = iEvent.get(mcParticlesGetToken_);
      if (inputMCParticles.empty()) {
        throw cms::Exception("TestFailure") << "Expected non-empty input MCParticles collection";
      }
      const auto& inputMCParticle = inputMCParticles[0];
      link.set<edm4hep::MCParticle>(inputMCParticle);
    }

    // Put the collections into the Event
    iEvent.emplace(tracksPutToken_, std::move(tracks));
    iEvent.emplace(recDqdxsPutToken_, std::move(recDqdxs));
    iEvent.emplace(reconstructedParticlesPutToken_, std::move(reconstructedParticles));
    iEvent.emplace(mcParticlesPutToken_, std::move(mcParticles));
    iEvent.emplace(linksPutToken_, std::move(trackToMCParticleLinks));
  }

  void TestRelationsAndLinksProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.addUntracked<edm::InputTag>("tracks");
    desc.addUntracked<edm::InputTag>("mcParticles");
    desc.addUntracked<bool>("linkToCollectionsMadeInModule", false);
    desc.addUntracked<int>("testValue");
    descriptions.addDefault(desc);
  }
}  // namespace c4h

DEFINE_FWK_MODULE(c4h::TestRelationsAndLinksProducer);
