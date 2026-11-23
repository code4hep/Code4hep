//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/plugins/HepMC3FileSource.cc
//---------------------------------------------------------------------------//
#include "Code4hep/Generators/HepMC3FileReader.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Sources/interface/ProducerSourceBase.h"

#include <edm4hep/MCParticleCollection.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
constexpr auto productInstance = "MCParticles";
}

//---------------------------------------------------------------------------//
/*!
 * Input source that reads HepMC3 files and produces edm4hep 
 * MCparticleCollection.
 *
 * This class is based on MCFileSource3 of CMSSW from
 * https://github.com/cms-sw/cmssw/blob/master/IOMC/Input
 */
class HepMC3FileSource final : public edm::ProducerSourceBase
{
public:
  HepMC3FileSource(const edm::ParameterSet&,
                   const edm::InputSourceDescription&);
  ~HepMC3FileSource() override = default;

  static void fillDescriptions(
      edm::ConfigurationDescriptions& descriptions);

private:
  bool setRunAndEventInfo(
      edm::EventID&,
      edm::TimeValue_t&,
      edm::EventAuxiliary::ExperimentType&) override;

  void produce(edm::Event&) override;

private:
  const bool printEvent_;

  std::unique_ptr<c4h::MCFileReader> reader_;
  c4h::MCFileReader::UPMCParticleColl mcParticles_;
};

//---------------------------------------------------------------------------//
HepMC3FileSource::HepMC3FileSource(
    const edm::ParameterSet& parameters,
    const edm::InputSourceDescription& description)
  : ProducerSourceBase(parameters, description, false)
  , printEvent_(parameters.getUntrackedParameter<bool>("printEvent", false))
  , reader_(std::make_unique<c4h::HepMC3FileReader>(
      parameters.getUntrackedParameter<std::vector<std::string>>("fileNames"),
      parameters.getUntrackedParameter<std::string>("fileFormat", "ascii")))
{
  produces<edm4hep::MCParticleCollection>(productInstance);
}

//---------------------------------------------------------------------------//
bool HepMC3FileSource::setRunAndEventInfo(
    edm::EventID&,
    edm::TimeValue_t&,
    edm::EventAuxiliary::ExperimentType&)
{
  // Read one HepMC3 event and return a MCParticleCollection
  mcParticles_ = (*reader_)();

  if (!mcParticles_)
  {
    return false;
  }

  if (printEvent_)
  {
    reader_->printCurrentEvent();
  }

  return true;
}

//---------------------------------------------------------------------------//
void HepMC3FileSource::produce(edm::Event& event)
{
  // setRunAndEventInfo() must successfully prepare an event before produce()
  // is called, so mcParticles_ is expected to be non-null here.
  event.put(std::move(mcParticles_), productInstance);
}

//---------------------------------------------------------------------------//
void HepMC3FileSource::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions)
{
  edm::ParameterSetDescription desc;
  desc.setComment("Reads HepMC3 files and produces EDM4hep MC particles.");

  edm::ProducerSourceBase::fillDescription(desc);

  desc.addUntracked<std::vector<std::string>>("fileNames");
  desc.addUntracked<std::string>("fileFormat", "ascii");
  desc.addUntracked<bool>("printEvent", false);
  descriptions.add("source", desc);
}

//---------------------------------------------------------------------------//
DEFINE_FWK_INPUT_SOURCE(HepMC3FileSource);
