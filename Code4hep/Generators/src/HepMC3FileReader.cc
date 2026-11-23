//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/src/HepMC3FileReader.cc
//---------------------------------------------------------------------------//
#include "Code4hep/Generators/HepMC3FileReader.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Print.h"
#include "HepMC3/Reader.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/ReaderLHEF.h"
#include "HepMC3/Units.h"

#include <sstream>
#include <unordered_map>
#include <utility>


namespace c4h
{
namespace
{
constexpr auto loggerCategory = "HepMC3FileReader";
}

//---------------------------------------------------------------------------//
HepMC3FileReader::HepMC3FileReader(std::vector<std::string> fileNames,
                                   std::string fileFormat)
  : fileNames_(std::move(fileNames))
  , fileFormat_(std::move(fileFormat))
{
  if (fileNames_.empty())
  {
    throw cms::Exception("Configuration", loggerCategory)
        << "At least one input file must be specified.";
  }

  if (!openNextFile())
  {
    throw cms::Exception("FileOpenError", loggerCategory)
        << "No input file could be opened.";
  }
}

//---------------------------------------------------------------------------//
HepMC3FileReader::~HepMC3FileReader()
{
  if (reader_)
  {
    reader_->close();
  }
}

//---------------------------------------------------------------------------//
std::unique_ptr<HepMC3::Reader>
HepMC3FileReader::makeReader(const std::string& fileName) const
{
  if (fileFormat_ == "ascii" || fileFormat_ == "hepmc3")
  {
    return std::make_unique<HepMC3::ReaderAscii>(fileName);
  }

  if (fileFormat_ == "lhe" || fileFormat_ == "lhef")
  {
    return std::make_unique<HepMC3::ReaderLHEF>(fileName);
  }

  throw cms::Exception("Configuration", loggerCategory)
      << "Unsupported HepMC3 file format '" << fileFormat_
      << "'. Supported formats are 'ascii', 'hepmc3', 'lhe', and 'lhef'.";
}

//---------------------------------------------------------------------------//
bool HepMC3FileReader::openNextFile()
{
  if (reader_)
  {
    reader_->close();
    reader_.reset();
  }

  if (nextFile_ >= fileNames_.size())
  {
    return false;
  }

  const auto& fileName = fileNames_[nextFile_++];

  edm::LogInfo(loggerCategory)
      << "Opening input file '" << fileName
      << "' using format '" << fileFormat_ << "'.";

  reader_ = makeReader(fileName);

  if (!reader_ || reader_->failed())
  {
    throw cms::Exception("FileOpenError",
                         "HepMC3FileReader::openNextFile")
        << "File '" << fileName << "' could not be opened as format '"
        << fileFormat_ << "'.";
  }

  return true;
}

//---------------------------------------------------------------------------//
auto HepMC3FileReader::operator()() -> UPMCParticleColl
{
  while (reader_)
  {
    // Read the next HepMC3 event.
    auto hepmc3_event = std::make_unique<HepMC3::GenEvent>();
    const bool readSucceeded = reader_->read_event(*hepmc3_event);

    if (readSucceeded && !reader_->failed())
    {
      // Convert all inputs to the units expected by the simulation.
      hepmc3_event->set_units(HepMC3::Units::GEV, HepMC3::Units::MM);

      auto particles = convertToEDM4hep(*hepmc3_event);

      currentEvent_ = std::move(hepmc3_event);
      ++eventsRead_;

      return particles;
    }

    // The current file is exhausted. Continue with the next file, if any.
    if (!openNextFile())
    {
      break;
    }
  }

  currentEvent_.reset();
  return nullptr;
}

//---------------------------------------------------------------------------//
void HepMC3FileReader::printCurrentEvent() const
{
  if (!currentEvent_)
  {
    edm::LogWarning(loggerCategory)
        << "No current HepMC3 event is available to print.";
    return;
  }

  std::ostringstream output;
  HepMC3::Print::content(output, *currentEvent_);
  edm::LogVerbatim(loggerCategory) << output.str();
}

//---------------------------------------------------------------------------//
auto HepMC3FileReader::convertToEDM4hep(const HepMC3::GenEvent& event) const
  -> UPMCParticleColl
{
  auto result = std::make_unique<edm4hep::MCParticleCollection>();

  using ParticleMap
      = std::unordered_map<const HepMC3::GenParticle*,
                           edm4hep::MutableMCParticle>;

  ParticleMap particleMap;
  particleMap.reserve(event.particles().size());

  // Create all particles and copy their intrinsic properties.
  for (const auto& particle : event.particles())
  {
    if (!particle)
    {
      continue;
    }

    auto mcParticle = result->create();

    mcParticle.setPDG(particle->pid());
    mcParticle.setGeneratorStatus(particle->status());

    const auto& momentum = particle->momentum();
    mcParticle.setMomentum({momentum.px(), momentum.py(), momentum.pz()});
    mcParticle.setMass(particle->generated_mass());

    if (const auto& productionVertex = particle->production_vertex())
    {
      const auto& position = productionVertex->position();
      mcParticle.setVertex({position.x(), position.y(), position.z()});
      mcParticle.setTime(static_cast<float>(position.t()));
    }

    particleMap.emplace(particle.get(), mcParticle);
  }

  // Establish parent/daughter relations after every particle has been created.
  for (const auto& particle : event.particles())
  {
    if (!particle)
    {
      continue;
    }

    const auto& productionVertex = particle->production_vertex();
    if (!productionVertex)
    {
      continue;
    }

    const auto childIt = particleMap.find(particle.get());
    if (childIt == particleMap.end())
    {
      continue;
    }

    auto child = childIt->second;

    for (const auto& inputParent : productionVertex->particles_in())
    {
      if (!inputParent)
      {
        continue;
      }

      const auto parentIt = particleMap.find(inputParent.get());
      if (parentIt == particleMap.end())
      {
        edm::LogWarning(loggerCategory)
            << "A HepMC3 parent particle is not part of the event's "
               "particle collection; the relation will be skipped.";
        continue;
      }

      auto parent = parentIt->second;
      child.addToParents(parent);
      parent.addToDaughters(child);
    }
  }

  return result;
}

//---------------------------------------------------------------------------//
}  // namespace c4h
