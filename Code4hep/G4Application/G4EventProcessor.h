//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/G4EventProcessor.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_G4EventProcessor_h
#define Code4hep_G4Application_G4EventProcessor_h

#include "edm4hep/CaloHitContributionCollection.h"
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"

#include <functional>
#include <map>
#include <string>

class G4Event;
class G4VHitsCollection;

namespace c4h
{
// ----------------------------------------------------------------------------
/*!
 * Class that runs a G4Event and converts the resulting Geant4 hit collections
 * into EDM4hep collections.
 */
class G4EventProcessor
{
public:

  // Per-event output hit collections
  struct HitCollections
  {
    edm4hep::CaloHitContributionCollection calo;
    edm4hep::SimTrackerHitCollection       trk;
  };

  // Callable that processes one G4Event on the Geant4 worker
  using RunEvent = std::function<void(G4Event&)>;

  explicit G4EventProcessor(RunEvent runEvent);

  // Covert Geant4 hit collections to edm4hep hit collections
  HitCollections operator()(G4Event& g4evt) const;

private:

  // A filler converts Geant4 hits collection into a member of HitCollections
  using HitFiller = std::function<void(G4VHitsCollection*, HitCollections&)>;
  using HitCollectionMap = std::map<std::string, HitFiller>;

  // Build the "G4 hits-collection name -> filler" table 
  static HitCollectionMap buildHitCollectionMap();

  // Loop over HitFiller and fill HitCollections from G4Event hits
  void convertHits(const G4Event& g4evt, HitCollections& collection) const;

  // Bind a Geant4 hit class (HitT) to a destination HitCollections member
  template <typename HitT, typename CollT>
  static HitFiller makeFiller(CollT HitCollections::* member);

  // Copy all hits of a G4THitsCollection<HitT> into a EMD4hep collection
  template <typename HitT, typename CollT>
  static void copyHits(G4VHitsCollection* hc, CollT& out);

  RunEvent m_runEvent;  // run one event on the Geant4 worker
  const HitCollectionMap m_collectionMap; 
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
