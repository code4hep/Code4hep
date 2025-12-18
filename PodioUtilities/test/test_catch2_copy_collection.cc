#include "catch2/catch_all.hpp"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/TrackData.h"
#include <unordered_map>

#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverter.h"

C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "std::int32_t", std::int32_t);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "edm4hep::TrackState", edm4hep::TrackState);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "edm4hep::TrackData", edm4hep::TrackData);

namespace {

  template <typename T>
  std::unique_ptr<T> copyCollection(T& iCollection) {
    code4hep::CollectionWrapperConverter<edm4hep::TrackCollection> converter;
    auto base = converter.copy(iCollection);
    assert(nullptr != dynamic_cast<T*>(base.get()));
    return std::unique_ptr<T>(static_cast<T*>(base.release()));
  }
};  // namespace

TEST_CASE("Copy a podio::Collection ", "[podio_copy]") {
  SECTION("TrackCollection") {
    edm4hep::TrackCollection tracks;
    auto track = tracks.create();
    track.setType(1);
    track.setNholes(1);

    edm4hep::TrackState state;
    track.addToTrackStates(state);

    auto newTrackCol = copyCollection(tracks);
    REQUIRE(tracks.getID() == newTrackCol->getID());
    REQUIRE(tracks.size() == newTrackCol->size());
    REQUIRE(tracks[0].getType() == (*newTrackCol)[0].getType());
    REQUIRE(tracks[0].getNholes() == (*newTrackCol)[0].getNholes());
    //TrivialCollectionProvider provider;
    //provider.add(newTrackCol->getID(), newTrackCol.get());
    //newTrackCol->setReferences(&provider);
  }
}
