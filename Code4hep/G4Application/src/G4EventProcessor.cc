//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/G4EventProcessor.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/G4EventProcessor.h"

#include "Code4hep/G4Application/CalorimeterHit.h"
#include "Code4hep/G4Application/TrackerHit.h"
#include "Code4hep/Generators/MCParticlesToG4.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4THitsCollection.hh"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace c4h
{
// 
//---------------------------------------------------------------------------//
/*!  
 * Constructor with a callable function
 */
G4EventProcessor::G4EventProcessor(RunEvent runEvent)
  : m_runEvent(std::move(runEvent))
  , m_collectionMap(buildHitCollectionMap())
{
  if (!m_runEvent)
  {
    throw cms::Exception("IllegalArgument") << "Empty RunEvent callable";
  }
}

//---------------------------------------------------------------------------//
/*!  
 * Run a G4Event on the Geant4 worker thread and return hit collections.
 */
G4EventProcessor::HitCollections
G4EventProcessor::operator()(G4Event& g4evt) const
{
  // Process g4evt on the Geant4 worker 
  m_runEvent(g4evt);

  // Convert the resulting Geant4 hits into EDM4hep collections.
  HitCollections coll;

  convertHits(g4evt, coll);

  return coll;
}

//---------------------------------------------------------------------------//
/*!  
 * Hit collection map: add new sensitive detectors.
 * TODO: Move this to DetectorConstruction.
 */
G4EventProcessor::HitCollectionMap G4EventProcessor::buildHitCollectionMap()
{
  return {
    {"em_calorimeter_HC", makeFiller<CalorimeterHit>(&HitCollections::calo)},
    {"si_tracker_HC", makeFiller<TrackerHit>(&HitCollections::trk)},
  };
}

//---------------------------------------------------------------------------//
/*!  
 * Covert Geant4 hit collections into EDM4hep collections.
 */
void G4EventProcessor::convertHits(const G4Event& g4evt, HitCollections& coll) const
{
  G4HCofThisEvent* hce = g4evt.GetHCofThisEvent();
  if (hce == nullptr) return;  // no hits this event

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();

  for (auto const& [name, fill] : m_collectionMap)
  {
    const G4int hcid = sdManager->GetCollectionID(name);
    if (hcid < 0) continue;  // unknown collection name

    fill(hce->GetHC(hcid), coll);  // copyHits handles nullptr
  }
}
  
//---------------------------------------------------------------------------//
/*!  
 * Copy every hit of a G4THitsCollection<HitT> into the EDM4hep collection.
 */
template <typename HitT, typename CollT>
void G4EventProcessor::copyHits(G4VHitsCollection* hc, CollT& coll)
{
  auto* hits = dynamic_cast<G4THitsCollection<HitT>*>(hc);
  if (hits == nullptr) return;

  for (std::size_t i = 0, n = hits->entries(); i < n; ++i)
  {
    HitT* hit = (*hits)[i];
    if (hit == nullptr) continue;

    coll.push_back(hit->hit().clone());
  }
}

//---------------------------------------------------------------------------//
/*!  
 * Create a filler that writes HitT hits into the HitCollections member.
 */
template <typename HitT, typename CollT>
G4EventProcessor::HitFiller
G4EventProcessor::makeFiller(CollT HitCollections::* member)
{
  // Captures only the reusable member pointer
  return [member](G4VHitsCollection* hc, HitCollections& coll)
  {
    copyHits<HitT>(hc, coll.*member);  // coll.*member selects calo / trk
  };
}

//---------------------------------------------------------------------------//
}  // namespace c4h
