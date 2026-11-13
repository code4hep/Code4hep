//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/src/MCParticleGenerator.cc
//---------------------------------------------------------------------------//
#include "Code4hep/Generators/MCParticleGenerator.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "G4ParticleTable.hh"
#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <edm4hep/MCParticleCollection.h>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace c4h
{
namespace
{
//---------------------------------------------------------------------------//
/*!
 * Convert a 3-element parameter vector into a Vector3d, scaled by \c unit.
 *
 * Fails early on a malformed configuration instead of reading out of bounds.
 */
edm4hep::Vector3d toVector3d(const std::vector<double>& v,
                             const std::string& name,
                             double unit)
{
  if (v.size() != 3)
  {
    throw cms::Exception("IllegalParameter")
      << "MCParticleGenerator: '" << name << "' must have exactly 3 entries";
  }
  return {v[0] * unit, v[1] * unit, v[2] * unit};
}

//---------------------------------------------------------------------------//
}  // namespace

//---------------------------------------------------------------------------//
/*!
 * Construct from parameter set.
 */
MCParticleGenerator::MCParticleGenerator(const edm::ParameterSet &p)
    : verbose_(p.getUntrackedParameter<int>("Verbosity", 0)),
      pdgcode_(p.getUntrackedParameter<int>("PartID")),
      time_(p.getParameter<double>("Time")),    // [ns]
      min_pt_(p.getParameter<double>("MinPt")), // [GeV]
      max_pt_(p.getParameter<double>("MaxPt")),
      min_eta_(p.getParameter<double>("MinEta")),
      max_eta_(p.getParameter<double>("MaxEta")),
      min_phi_(p.getParameter<double>("MinPhi")),  // [rad]
      max_phi_(p.getParameter<double>("MaxPhi")),
      // Configured in cm; stored in mm (EDM4hep convention)
      vtx_mean_(toVector3d(
          p.getParameter<std::vector<double>>("Vertex"), "Vertex", CLHEP::cm)),
      vtx_sigma_(toVector3d(p.getParameter<std::vector<double>>("VertexSigma"),
                            "VertexSigma",
                            CLHEP::cm))
{
  // Read as signed so that a negative value is caught rather than wrapped
  const int multiplicity = p.getParameter<int>("Multiplicity");
  if (multiplicity < 0)
  {
    throw cms::Exception("IllegalParameter")
      << "MCParticleGenerator: 'Multiplicity' must be non-negative";
  }
  multiplicity_ = static_cast<std::size_t>(multiplicity);
}

//---------------------------------------------------------------------------//
/*!
 * Generate an edm4hep::MCParticleCollection.
 */
auto MCParticleGenerator::operator()(RandomEngine* rng) -> UPMCParticle
{
  auto mcParticles = std::make_unique<edm4hep::MCParticleCollection>();

  const auto* particle = G4ParticleTable::GetParticleTable()->FindParticle(pdgcode_);
  if (!particle)
  {
    throw cms::Exception("PDGCodeError")
      << "MCParticleGenerator: unknown PDG code " << std::to_string(pdgcode_);
  }
  const double mass = particle->GetPDGMass() / CLHEP::GeV;  // EDM4hep: GeV

  // Uniform sampling in [lo, hi)
  auto flat = [rng](double lo, double hi) {
    return CLHEP::RandFlat::shoot(rng, lo, hi);
  };

  for (std::size_t i = 0; i < multiplicity_; ++i)
  {
    auto mcp = mcParticles->create();
    mcp.setPDG(pdgcode_);
    mcp.setGeneratorStatus(1);  // Stable final-state particle
    mcp.setMass(static_cast<float>(mass));
    mcp.setTime(static_cast<float>(time_));
    mcp.setVertex(smearVertex(rng, vtx_mean_, vtx_sigma_));

    const double pt  = flat(min_pt_, max_pt_);
    const double eta = flat(min_eta_, max_eta_);
    const double phi = flat(min_phi_, max_phi_);

    mcp.setMomentum(Vector3d{pt * std::cos(phi),
			     pt * std::sin(phi),
			     pt * std::sinh(eta)});
  }

  return mcParticles;
}

//---------------------------------------------------------------------------//
/*!
 * Sample each vertex coordinate from a Gaussian; a zero sigma returns the mean
 * exactly and consumes no random numbers.
 */
auto MCParticleGenerator::smearVertex(RandomEngine* rng,
                                      const Vector3d& mean,
                                      const Vector3d& sigma) -> Vector3d
{
  auto gauss = [rng](double m, double s) {
    return (s == 0.0) ? m : CLHEP::RandGauss::shoot(rng, m, s);
  };

  return {gauss(mean[0], sigma[0]),
          gauss(mean[1], sigma[1]),
          gauss(mean[2], sigma[2])};
}

//---------------------------------------------------------------------------//
}  // namespace c4h
