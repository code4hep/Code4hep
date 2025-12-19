#include "Code4hep/PodioUtilities/interface/CollectionMacros.h"

#include "edm4hep/edm4hep.h"

//NOTE: the macro can not handle a comma in a type name

using CaloMCPartLink = podio::LinkCollection<edm4hep::CalorimeterHit, edm4hep::MCParticle>;
C4H_COLLECTION_NAMED(CaloMCPartLink, "podio::LinkCollection<edm4hep::CalorimeterHit,edm4hep::MCParticle>");
C4H_CONTAINED_CLASS(CaloMCPartLink, podio::LinkData);

using CaloSimLink = podio::LinkCollection<edm4hep::CalorimeterHit, edm4hep::SimCalorimeterHit>;
C4H_COLLECTION_NAMED(CaloSimLink, "podio::LinkCollection<edm4hep::CalorimeterHit,edm4hep::SimCalorimeterHit>");
C4H_CONTAINED_CLASS(CaloSimLink, podio::LinkData);

using ClusterMCPartLink = podio::LinkCollection<edm4hep::Cluster, edm4hep::MCParticle>;
C4H_COLLECTION_NAMED(ClusterMCPartLink, "podio::LinkCollection<edm4hep::Cluster,edm4hep::MCParticle>");
C4H_CONTAINED_CLASS(ClusterMCPartLink, podio::LinkData);

using RecoPartMCPartLink = podio::LinkCollection<edm4hep::ReconstructedParticle, edm4hep::MCParticle>;
C4H_COLLECTION_NAMED(RecoPartMCPartLink, "podio::LinkCollection<edm4hep::ReconstructedParticle,edm4hep::MCParticle>");
C4H_CONTAINED_CLASS(RecoPartMCPartLink, podio::LinkData);

using TrackMCPartLink = podio::LinkCollection<edm4hep::Track, edm4hep::MCParticle>;
C4H_COLLECTION_NAMED(TrackMCPartLink, "podio::LinkCollection<edm4hep::Track,edm4hep::MCParticle>");
C4H_CONTAINED_CLASS(TrackMCPartLink, podio::LinkData);

using TrackSimTrackLink = podio::LinkCollection<edm4hep::TrackerHit, edm4hep::SimTrackerHit>;
C4H_COLLECTION_NAMED(TrackSimTrackLink, "podio::LinkCollection<edm4hep::TrackerHit,edm4hep::SimTrackerHit>");
C4H_CONTAINED_CLASS(TrackSimTrackLink, podio::LinkData);

using VertexRecoPartLink = podio::LinkCollection<edm4hep::Vertex, edm4hep::ReconstructedParticle>;
C4H_COLLECTION_NAMED(VertexRecoPartLink, "podio::LinkCollection<edm4hep::Vertex,edm4hep::ReconstructedParticle>");
C4H_CONTAINED_CLASS(VertexRecoPartLink, podio::LinkData);
