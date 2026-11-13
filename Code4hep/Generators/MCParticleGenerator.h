//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/MCParticleGenerator.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_Generators_MCParticleGenerator_h
#define Code4hep_Generators_MCParticleGenerator_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Code4hep/Generators/Generator.h"

#include <CLHEP/Random/RandomEngine.h>
#include <edm4hep/Vector3d.h>

#include <cstddef>

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Generic particle gun producing an edm4hep::MCParticleCollection.
 *
 * Each particle is sampled uniformly in (pt, eta, phi) and its production
 * vertex is smeared with an independent Gaussian along each axis.
 *
 * Units of the configuration parameters:
 *  - Time: ns
 *  - MinPt/MaxPt: GeV
 *  - MinPhi/MaxPhi: radians
 *  - Vertex/VertexSigma: cm (converted internally to mm, the EDM4hep unit)
 */
class MCParticleGenerator final : public Generator
{
public:
  //!@{
  //! \name Type aliases
  using Vector3d = edm4hep::Vector3d;
  using RandomEngine = CLHEP::HepRandomEngine;
  //!@}

public:
  explicit MCParticleGenerator(const edm::ParameterSet &pset);
  ~MCParticleGenerator() override = default;

  // Generate an edm4hep::MCParticleCollection.
  Generator::UPMCParticle operator()(RandomEngine* rng) override;

private:
  // Sample a vertex from independent Gaussians (mean/sigma in mm).
  static Vector3d
  smearVertex(RandomEngine* rng, const Vector3d& mean, const Vector3d& sigma);

  int verbose_{};
  int pdgcode_{};
  std::size_t multiplicity_{};

  double time_{};     //!< [ns]
  double min_pt_{};   //!< [GeV]
  double max_pt_{};   //!< [GeV]
  double min_eta_{};
  double max_eta_{};
  double min_phi_{};  //!< [rad]
  double max_phi_{};  //!< [rad]

  Vector3d vtx_mean_{};   //!< Mean vertex position [mm]
  Vector3d vtx_sigma_{};  //!< Vertex Gaussian sigma [mm]
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
