//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/CalorimeterSD.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/CalorimeterSD.h"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "edm4hep/CaloHitContribution.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Construct with sensitive detector name.
 */
CalorimeterSD::CalorimeterSD(G4String name)
    : G4VSensitiveDetector(name), hcid_(-1), collection_(nullptr)
{
  G4String HCname = name + "_HC";
  collectionName.insert(HCname);
}

//---------------------------------------------------------------------------//
/*!
 * Set up hit collections for a new event.
 */  
void CalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  collection_ = new CalorimeterHitsCollection(SensitiveDetectorName,
                                              collectionName[0]);
  if (hcid_ < 0)
  {
      hcid_ = G4SDManager::GetSDMpointer()->GetCollectionID(collection_);
  }
  hce->AddHitsCollection(hcid_, collection_);
}

G4bool CalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto edep = step->GetTotalEnergyDeposit();

  if (edep == 0.)
  {
      return false;
  }

  // Get hit data for this sensitive detector
  auto touchable = step->GetPreStepPoint()->GetTouchable();

  std::int32_t id = touchable->GetVolume()->GetCopyNo();

  auto d2f = [](G4double value) -> float
  {
    return static_cast<float>(value);
  };

  float energy = d2f(edep/CLHEP::GeV); // [GeV]
  float time = d2f(step->GetPreStepPoint()->GetGlobalTime()); // [ns]

  auto pos = touchable->GetTranslation();
  edm4hep::Vector3f step_pos{d2f(pos.x()), d2f(pos.y()), d2f(pos.z())}; // [mm]
  float stepLength = d2f(step->GetStepLength()); // [mm]

  // Create a new hit contribution and insert to the collection:
  edm4hep::CaloHitContribution hit(id, energy, time, step_pos, stepLength);
  collection_->insert(new CalorimeterHit(hit));

  return true;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
