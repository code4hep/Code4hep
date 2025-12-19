#include "Code4hep/PodioUtilities/interface/CollectionMacros.h"

#include "edm4hep/edm4hep.h"

C4H_COLLECTION(edm4hep::TrackCollection);

C4H_CONTAINED_CLASS(edm4hep::TrackCollection, std::int32_t);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, edm4hep::TrackState);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, edm4hep::TrackData);

C4H_COLLECTION(edm4hep::CaloHitContributionCollection);
C4H_CONTAINED_CLASS(edm4hep::CaloHitContributionCollection, edm4hep::CaloHitContributionData);

C4H_COLLECTION(edm4hep::CalorimeterHitCollection);
C4H_CONTAINED_CLASS(edm4hep::CalorimeterHitCollection, edm4hep::CalorimeterHitData);

C4H_COLLECTION(edm4hep::ClusterCollection);
C4H_CONTAINED_CLASS(edm4hep::ClusterCollection, edm4hep::ClusterData);
C4H_CONTAINED_CLASS(edm4hep::ClusterCollection, float);

C4H_COLLECTION(edm4hep::EventHeaderCollection);
C4H_CONTAINED_CLASS(edm4hep::EventHeaderCollection, edm4hep::EventHeaderData);
C4H_CONTAINED_CLASS(edm4hep::EventHeaderCollection, double);

C4H_COLLECTION(edm4hep::GeneratorEventParametersCollection);
C4H_CONTAINED_CLASS(edm4hep::GeneratorEventParametersCollection, edm4hep::GeneratorEventParametersData);
C4H_CONTAINED_CLASS(edm4hep::GeneratorEventParametersCollection, double);

C4H_COLLECTION(edm4hep::MCParticleCollection);
C4H_CONTAINED_CLASS(edm4hep::MCParticleCollection, edm4hep::MCParticleData);

C4H_COLLECTION(edm4hep::ParticleIDCollection);
C4H_CONTAINED_CLASS(edm4hep::ParticleIDCollection, edm4hep::ParticleIDData);
C4H_CONTAINED_CLASS(edm4hep::ParticleIDCollection, float);

C4H_COLLECTION(edm4hep::RawCalorimeterHitCollection);
C4H_CONTAINED_CLASS(edm4hep::RawCalorimeterHitCollection, edm4hep::RawCalorimeterHitData);

C4H_COLLECTION(edm4hep::RawTimeSeriesCollection);
C4H_CONTAINED_CLASS(edm4hep::RawTimeSeriesCollection, edm4hep::RawTimeSeriesData);
C4H_CONTAINED_CLASS(edm4hep::RawTimeSeriesCollection, std::int32_t);

C4H_COLLECTION(edm4hep::RecDqdxCollection);
C4H_CONTAINED_CLASS(edm4hep::RecDqdxCollection, edm4hep::RecDqdxData);

C4H_COLLECTION(edm4hep::ReconstructedParticleCollection);
C4H_CONTAINED_CLASS(edm4hep::ReconstructedParticleCollection, edm4hep::ReconstructedParticleData);

C4H_COLLECTION(edm4hep::SenseWireHitCollection);
C4H_CONTAINED_CLASS(edm4hep::SenseWireHitCollection, edm4hep::SenseWireHitData);
C4H_CONTAINED_CLASS(edm4hep::SenseWireHitCollection, std::uint16_t);

C4H_COLLECTION(edm4hep::SimCalorimeterHitCollection);
C4H_CONTAINED_CLASS(edm4hep::SimCalorimeterHitCollection, edm4hep::SimCalorimeterHitData);

C4H_COLLECTION(edm4hep::SimTrackerHitCollection);
C4H_CONTAINED_CLASS(edm4hep::SimTrackerHitCollection, edm4hep::SimTrackerHitData);

C4H_COLLECTION(edm4hep::TimeSeriesCollection);
C4H_CONTAINED_CLASS(edm4hep::TimeSeriesCollection, edm4hep::TimeSeriesData);
C4H_CONTAINED_CLASS(edm4hep::TimeSeriesCollection, float);

C4H_COLLECTION(edm4hep::TrackerHitPlaneCollection);
C4H_CONTAINED_CLASS(edm4hep::TrackerHitPlaneCollection, edm4hep::TrackerHitPlaneData);

C4H_COLLECTION(edm4hep::TrackerHit3DCollection);
C4H_CONTAINED_CLASS(edm4hep::TrackerHit3DCollection, edm4hep::TrackerHit3DData);

C4H_COLLECTION(edm4hep::VertexCollection);
C4H_CONTAINED_CLASS(edm4hep::VertexCollection, edm4hep::VertexData);
C4H_CONTAINED_CLASS(edm4hep::VertexCollection, float);
