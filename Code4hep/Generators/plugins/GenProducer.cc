//---------------------------------------------------------------------------//
//! \file Code4hep/Generator/plugins/GenProducer.cc
//---------------------------------------------------------------------------//
#include "FWCore/AbstractServices/interface/RandomNumberGenerator.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "Code4hep/Generators/Generator.h"
#include "Code4hep/Generators/HepMC3Generator.h"
#include "Code4hep/Generators/Pythia8Generator.h"
#include "Code4hep/PodioUtilities/assigningProductTo.h"

#include "CLHEP/Random/RandFlat.h"
#include <edm4hep/MCParticleCollection.h>

using namespace edm;

//---------------------------------------------------------------------------//
/*! 
 * Code4hep producer that drives an edm4hep MC event generation.
 */
class GenProducer : public one::EDProducer<one::WatchRuns, EndRunProducer>
{
public:
  GenProducer(const ParameterSet& pset);

  void beginRun(const edm::Run&, const edm::EventSetup&) override {};
  void endRun(edm::Run const&, const edm::EventSetup&) override {};
  void endRunProduce(edm::Run&, const edm::EventSetup&) override {};

  void produce(edm::Event& event, const edm::EventSetup& es) override;

private:
  std::unique_ptr<c4h::Generator> generator_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
GenProducer::GenProducer(const ParameterSet& pset)
{
  auto type = pset.getParameter<std::string>("generatorType");

  if (type == "HepMC3Generator")
  {
    generator_ = std::make_unique<c4h::HepMC3Generator>(pset);
  }
  else if (type == "Pythia8Generator")
  {
    generator_ = std::make_unique<c4h::Pythia8Generator>(pset);
  }
  else
  {
      throw cms::Exception("Configuration")
          << "Unknown generatorType = " << type;
  }

  edm::Service<RandomNumberGenerator> rng;
  if (!rng.isAvailable()) {
    throw cms::Exception("Configuration")
      << "RandomNumberProducer requires RandomNumberGeneratorService";
  }
  
  produces<edm4hep::MCParticleCollection>("MCParticles");
}

void GenProducer::produce(edm::Event& event, const edm::EventSetup& es)
{
  edm::Service<edm::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine* engine = &rng->getEngine(event.streamID());

  // Generate a edm4hep::MCParticleCollections and store it into edm::Event
  auto mcParticles = (*generator_)(engine);

  event.put(std::move(mcParticles),"MCParticles");
}

//define this as a plug-in
DEFINE_FWK_MODULE(GenProducer);
