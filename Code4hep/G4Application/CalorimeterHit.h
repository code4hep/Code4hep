//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/CalorimeterHit.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_CalorimeterHit_h
#define Code4hep_G4Application_CalorimeterHit_h

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "edm4hep/CaloHitContribution.h"

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Example sensitive hit class.
 */
class CalorimeterHit : public G4VHit
{
    using CaloHitContribution = edm4hep::CaloHitContribution;

  public:
    CalorimeterHit() : G4VHit() {}
    CalorimeterHit(CaloHitContribution hit);
    ~CalorimeterHit() override;

    CalorimeterHit(const CalorimeterHit&) = default;
    const CalorimeterHit& operator=(const CalorimeterHit& rhs);
    G4bool operator==(const CalorimeterHit& rhs) const;

    inline void* operator new(size_t);
    inline void operator delete(void*);

    // Accessors
    inline const CaloHitContribution& hit() const { return hit_; };

  private:
    CaloHitContribution hit_;
};

using CalorimeterHitsCollection = G4THitsCollection<CalorimeterHit>;
extern G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator;

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to allocate memory for a CalorimeterHit.
 */
inline void* CalorimeterHit::operator new(size_t)
{
    if (!CalorimeterHitAllocator)
    {
        CalorimeterHitAllocator = new G4Allocator<CalorimeterHit>;
    }
    return (void*)CalorimeterHitAllocator->MallocSingle();
}

//---------------------------------------------------------------------------//
/*!
 * Use G4Allocator to release memory for a CalorimeterHit.
 */
inline void CalorimeterHit::operator delete(void* hit)
{
    CalorimeterHitAllocator->FreeSingle((CalorimeterHit*)hit);
}

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
