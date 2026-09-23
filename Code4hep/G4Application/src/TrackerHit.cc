//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/TrackerHit.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/TrackerHit.h"

namespace c4h
{
G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator = nullptr;

//---------------------------------------------------------------------------//
/*!
 * Construct with hit data.
 */
TrackerHit::TrackerHit(SimTrackerHit hit) : G4VHit(), hit_(hit)
{
}

TrackerHit::~TrackerHit() {}

const TrackerHit& TrackerHit::operator=(const TrackerHit& rhs)
{
  hit_  = rhs.hit_;

  return *this;
}

G4bool TrackerHit::operator==(const TrackerHit& rhs) const
{
  return (this == &rhs) ? true : false;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
