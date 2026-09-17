//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/CalorimeterHit.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/CalorimeterHit.h"

namespace c4h
{
G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator = nullptr;

//---------------------------------------------------------------------------//
/*!
 * Construct with hit data.
 */
CalorimeterHit::CalorimeterHit(CaloHitContribution hit) : G4VHit(), hit_(hit)
{
}

CalorimeterHit::~CalorimeterHit() {}

const CalorimeterHit& CalorimeterHit::operator=(const CalorimeterHit& rhs)
{
  hit_ = rhs.hit_;

  return *this;
}

G4bool CalorimeterHit::operator==(const CalorimeterHit& rhs) const
{
  return (this == &rhs) ? true : false;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
