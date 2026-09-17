//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/TrackerHit.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_TrackerHit_h
#define Code4hep_G4Application_TrackerHit_h

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "edm4hep/SimTrackerHit.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Example tracker (sensitive) hit class.
 */

class TrackerHit : public G4VHit
{
  using SimTrackerHit = edm4hep::SimTrackerHit;

public:
  TrackerHit() : G4VHit() {}
  TrackerHit(SimTrackerHit hit);
  ~TrackerHit() override;

  TrackerHit(const TrackerHit&) = default;
  const TrackerHit& operator=(const TrackerHit& rhs);
  G4bool operator==(const TrackerHit& rhs) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  // Accessors
  inline const SimTrackerHit& hit() const { return hit_; };

private:
  SimTrackerHit hit_;
};

using TrackerHitsCollection = G4THitsCollection<TrackerHit>;
extern G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator;

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to allocate memory for a TrackerHit.
 */
inline void* TrackerHit::operator new(size_t)
{
  if (!TrackerHitAllocator)
  {
    TrackerHitAllocator = new G4Allocator<TrackerHit>;
  }
  return (void*)TrackerHitAllocator->MallocSingle();
}

//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to release memory for a TrackerHit.
 */
inline void TrackerHit::operator delete(void* hit)
{
  TrackerHitAllocator->FreeSingle((TrackerHit*)hit);
}

//---------------------------------------------------------------------------//
}  // namespace c4h      
#endif
