//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/TrackerSD.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/TrackerSD.h"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "edm4hep/SimTrackerHit.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct with sensitive detector name.
 */
TrackerSD::TrackerSD(G4String name)
  : G4VSensitiveDetector(name), hcid_(-1), collection_(nullptr)
{
  G4String HCname = name + "_HC";
  collectionName.insert(HCname);
}

//---------------------------------------------------------------------------//
/*!
 * Set up hit collections for a new event.
 */
void TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  collection_
    = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
  if (hcid_ < 0)
  {
    hcid_ = G4SDManager::GetSDMpointer()->GetCollectionID(collection_);
  }
  hce->AddHitsCollection(hcid_, collection_);
}

//---------------------------------------------------------------------------//
/*!
 * Add hits to the current hit collection.
 */
G4bool TrackerSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  // Get hit data for this sensitive detector
  auto touchable = step->GetPreStepPoint()->GetTouchable();
  std::uint64_t id = touchable->GetVolume()->GetCopyNo();

  auto d2f = [](G4double value) -> float
  {
    return static_cast<float>(value);
  };

  float energy = d2f(step->GetTotalEnergyDeposit()/CLHEP::GeV); // [GeV]
  float time = d2f(step->GetPreStepPoint()->GetGlobalTime()); // [ns]
  float stepLength = d2f(step->GetStepLength()); // [mm]
  std::int32_t quality{};
  const auto pos = touchable->GetTranslation();
  const auto mom = step->GetPreStepPoint()->GetMomentum()/CLHEP::GeV; // [GeV]

  edm4hep::Vector3d step_pos{pos.x(), pos.y(), pos.z()}; // [mm]
  edm4hep::Vector3f step_mom{d2f(mom.x()), d2f(mom.y()), d2f(mom.z())};

  edm4hep::SimTrackerHit hit(id, energy, time, stepLength, quality, step_pos, step_mom);

  // Create a new hit contribution and insert it to the collection::
  collection_->insert(new TrackerHit(hit));

  return true;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
