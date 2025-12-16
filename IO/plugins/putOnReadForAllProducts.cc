#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "edm4hep/edm4hep.h"
#include "podio/CollectionBase.h"
#include "podio/UserDataCollection.h"

#include "putOnReadForAllProducts.h"

#include "DataFormats/Common/interface/Wrapper.h"
#include "DataFormats/Common/interface/WrapperBase.h"
#include "DataFormats/Provenance/interface/ProductDescription.h"
#include "DataFormats/Provenance/interface/ProductProvenance.h"
#include "FWCore/Utilities/interface/EDMException.h"

namespace c4h {

  namespace {

    template <typename CollectionType>
    void putOnReadForProductType(edm::EventPrincipal& eventPrincipal,
                                 edm::ProductDescription const& productDescription,
                                 const podio::CollectionBase* collectionBase) {
      std::optional<edm::ProductProvenance> productProvenance;

      const auto* collectionConstPtr = dynamic_cast<const CollectionType*>(collectionBase);
      if (!collectionConstPtr) {
        throw edm::Exception(edm::errors::LogicError, "putOnReadForProductType")
            << "Failed to get product with moduleLabel \"" << productDescription.moduleLabel()
            << "\" from podio::Frame\n";
      }

      // Not nice, but I have not yet thought of a way to avoid this const_cast.
      // Note that if you trace this pointer's origin it comes from an internal
      // pointer in podio that is the result of converting a non-const pointer
      // to a const pointer, so this is not undefined behavior, but it is fragile.
      auto* collection = const_cast<CollectionType*>(collectionConstPtr);

      auto wrappedCollection =
          std::make_unique<edm::Wrapper<CollectionType>>(edm::WrapperBase::Emplace(), std::move(*collection));
      eventPrincipal.putOnRead(productDescription, std::move(wrappedCollection), productProvenance);
    }
  }  // namespace

  void putOnReadForAllProducts(podio::Frame const& podioFrame,
                               edm::ProductRegistry const& productRegistry,
                               edm::EventPrincipal& eventPrincipal) {
    static const std::unordered_map<
        std::string,
        std::function<void(edm::EventPrincipal&, edm::ProductDescription const&, const podio::CollectionBase*)>>
        moduleLabelToHandler = {
            {"CalorimeterHitCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::CalorimeterHitCollection>(ep, pd, cb);
             }},
            {"CaloHitContributionCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::CaloHitContributionCollection>(ep, pd, cb);
             }},
            {"ClusterCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::ClusterCollection>(ep, pd, cb);
             }},
            {"EventHeader",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::EventHeaderCollection>(ep, pd, cb);
             }},
            {"GeneratorEventParametersCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::GeneratorEventParametersCollection>(ep, pd, cb);
             }},
            {"MCParticleCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::MCParticleCollection>(ep, pd, cb);
             }},
            {"ParticleIDCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::ParticleIDCollection>(ep, pd, cb);
             }},
            {"RawCalorimeterHitCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::RawCalorimeterHitCollection>(ep, pd, cb);
             }},
            {"RawTimeSeriesCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::RawTimeSeriesCollection>(ep, pd, cb);
             }},
            {"RecDqdxCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::RecDqdxCollection>(ep, pd, cb);
             }},
            {"ReconstructedParticleCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::ReconstructedParticleCollection>(ep, pd, cb);
             }},
            {"SenseWireHitCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::SenseWireHitCollection>(ep, pd, cb);
             }},
            {"SimCalorimeterHitCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::SimCalorimeterHitCollection>(ep, pd, cb);
             }},
            {"SimTrackerHitCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::SimTrackerHitCollection>(ep, pd, cb);
             }},
            {"TimeSeriesCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::TimeSeriesCollection>(ep, pd, cb);
             }},
            {"TrackCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::TrackCollection>(ep, pd, cb);
             }},
            {"TrackerHitPlaneCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::TrackerHitPlaneCollection>(ep, pd, cb);
             }},
            {"TrackerHit3DCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::TrackerHit3DCollection>(ep, pd, cb);
             }},
            {"UserDataCollectionFloat",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::UserDataCollection<float>>(ep, pd, cb);
             }},
            {"UserDataCollectionInt",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::UserDataCollection<int32_t>>(ep, pd, cb);
             }},
            {"VertexCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<edm4hep::VertexCollection>(ep, pd, cb);
             }},
            {"CaloHitMCParticleLinkCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::LinkCollection<edm4hep::CalorimeterHit, edm4hep::MCParticle>>(ep, pd, cb);
             }},
            {"CaloHitSimCaloHitLinkCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::LinkCollection<edm4hep::CalorimeterHit, edm4hep::SimCalorimeterHit>>(
                   ep, pd, cb);
             }},
            {"ClusterMCParticleLinkCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::LinkCollection<edm4hep::Cluster, edm4hep::MCParticle>>(ep, pd, cb);
             }},
            {"RecoMCParticleLinkCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::LinkCollection<edm4hep::ReconstructedParticle, edm4hep::MCParticle>>(
                   ep, pd, cb);
             }},
            {"TrackMCParticleLinkCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::LinkCollection<edm4hep::Track, edm4hep::MCParticle>>(ep, pd, cb);
             }},
            {"TrackerHitSimTrackerHitLinkCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::LinkCollection<edm4hep::TrackerHit, edm4hep::SimTrackerHit>>(ep, pd, cb);
             }},
            {"VertexRecoParticleLinkCollection",
             [](edm::EventPrincipal& ep, edm::ProductDescription const& pd, const podio::CollectionBase* cb) {
               putOnReadForProductType<podio::LinkCollection<edm4hep::Vertex, edm4hep::ReconstructedParticle>>(
                   ep, pd, cb);
             }},
        };

    for (auto const& iter : productRegistry.productList()) {
      auto const& productDescription = iter.second;
      std::string const& moduleLabel = productDescription.moduleLabel();

      const podio::CollectionBase* collectionBase = podioFrame.get(moduleLabel);

      auto it = moduleLabelToHandler.find(moduleLabel);
      if (it != moduleLabelToHandler.end()) {
        it->second(eventPrincipal, productDescription, collectionBase);
      } else {
        throw edm::Exception(edm::errors::LogicError, "putOnReadForAllProducts")
            << "Unknown moduleLabel \"" << moduleLabel << "\"\n";
      }
    }
  }
}  // namespace c4h
