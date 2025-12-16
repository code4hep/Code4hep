#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "edm4hep/edm4hep.h"
#include "podio/UserDataCollection.h"

#include "fillProductRegistry.h"

#include "DataFormats/Provenance/interface/ProductDescription.h"
#include "FWCore/Utilities/interface/BranchType.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Utilities/interface/TypeID.h"

namespace c4h {

  std::unique_ptr<edm::ProductRegistry> fillProductRegistry(podio::Frame const& podioFrame,
                                                            std::string const& processName) {
    auto productRegistry = std::make_unique<edm::ProductRegistry>();

    // Fill the process order first
    // The process order only contains the one process name for input products
    std::vector<std::string> processOrder = {processName};
    productRegistry->setProcessOrder(processOrder);

    edm::BranchType const branchType = edm::InEvent;
    std::string const productInstanceName = "";
    edm::TypeID typeID;
    bool const produced = false;

    static const std::unordered_map<std::string, const std::type_info*> collectionToTypeInfo = {
        {"CalorimeterHitCollection", &typeid(edm4hep::CalorimeterHitCollection)},
        {"CaloHitContributionCollection", &typeid(edm4hep::CaloHitContributionCollection)},
        {"ClusterCollection", &typeid(edm4hep::ClusterCollection)},
        {"EventHeader", &typeid(edm4hep::EventHeaderCollection)},
        {"GeneratorEventParametersCollection", &typeid(edm4hep::GeneratorEventParametersCollection)},
        {"MCParticleCollection", &typeid(edm4hep::MCParticleCollection)},
        {"ParticleIDCollection", &typeid(edm4hep::ParticleIDCollection)},
        {"RawCalorimeterHitCollection", &typeid(edm4hep::RawCalorimeterHitCollection)},
        {"RawTimeSeriesCollection", &typeid(edm4hep::RawTimeSeriesCollection)},
        {"RecDqdxCollection", &typeid(edm4hep::RecDqdxCollection)},
        {"ReconstructedParticleCollection", &typeid(edm4hep::ReconstructedParticleCollection)},
        {"SenseWireHitCollection", &typeid(edm4hep::SenseWireHitCollection)},
        {"SimCalorimeterHitCollection", &typeid(edm4hep::SimCalorimeterHitCollection)},
        {"SimTrackerHitCollection", &typeid(edm4hep::SimTrackerHitCollection)},
        {"TimeSeriesCollection", &typeid(edm4hep::TimeSeriesCollection)},
        {"TrackCollection", &typeid(edm4hep::TrackCollection)},
        {"TrackerHitPlaneCollection", &typeid(edm4hep::TrackerHitPlaneCollection)},
        {"TrackerHit3DCollection", &typeid(edm4hep::TrackerHit3DCollection)},
        {"UserDataCollectionFloat", &typeid(podio::UserDataCollection<float>)},
        {"UserDataCollectionInt", &typeid(podio::UserDataCollection<int32_t>)},
        {"VertexCollection", &typeid(edm4hep::VertexCollection)},
        {"CaloHitMCParticleLinkCollection",
         &typeid(podio::LinkCollection<edm4hep::CalorimeterHit, edm4hep::MCParticle>)},
        {"CaloHitSimCaloHitLinkCollection",
         &typeid(podio::LinkCollection<edm4hep::CalorimeterHit, edm4hep::SimCalorimeterHit>)},
        {"ClusterMCParticleLinkCollection", &typeid(podio::LinkCollection<edm4hep::Cluster, edm4hep::MCParticle>)},
        {"RecoMCParticleLinkCollection",
         &typeid(podio::LinkCollection<edm4hep::ReconstructedParticle, edm4hep::MCParticle>)},
        {"TrackMCParticleLinkCollection", &typeid(podio::LinkCollection<edm4hep::Track, edm4hep::MCParticle>)},
        {"TrackerHitSimTrackerHitLinkCollection",
         &typeid(podio::LinkCollection<edm4hep::TrackerHit, edm4hep::SimTrackerHit>)},
        {"VertexRecoParticleLinkCollection",
         &typeid(podio::LinkCollection<edm4hep::Vertex, edm4hep::ReconstructedParticle>)},
    };

    for (std::string const& collection : podioFrame.getAvailableCollections()) {
      // This line of code just emphasizes to the reader that a podio collection
      // name is used as the module label in the EDM ProductDescription
      std::string const& moduleLabel = collection;

      auto it = collectionToTypeInfo.find(collection);
      if (it != collectionToTypeInfo.end()) {
        typeID = edm::TypeID(*it->second);
      } else {
        throw edm::Exception(edm::errors::LogicError, "fillProductRegistry")
            << "Unknown podio collection name " << collection << "\n";
      }
      edm::ProductDescription productDescription(
          branchType, moduleLabel, processName, productInstanceName, typeID, produced);

      productRegistry->copyProduct(productDescription);
    }
    return productRegistry;
  }
}  // namespace c4h
