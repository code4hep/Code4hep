#include "Code4hep/PodioUtilities/interface/CollectionMacros.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/TrackData.h"

C4H_COLLECTION(edm4hep::TrackCollection);

C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "std::int32_t", std::int32_t);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "edm4hep::TrackState", edm4hep::TrackState);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "edm4hep::TrackData", edm4hep::TrackData);
