//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/MCFileReader.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_Generators_MCFileReader_h
#define Code4hep_Generators_MCFileReader_h

#include <edm4hep/MCParticleCollection.h>

#include <cstddef>
#include <memory>

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Abstract interface for Monte Carlo event readers.
 *
 * A concrete reader reads one event from its input source, converts it to an
 * edm4hep::MCParticleCollection, and returns ownership of the collection to
 * the producer. A null pointer indicates that the input stream is exhausted.
 */
class MCFileReader
{
public:
  using UPMCParticleColl = std::unique_ptr<edm4hep::MCParticleCollection>;

public:
  MCFileReader() = default;
  virtual ~MCFileReader() = default;

  MCFileReader(const MCFileReader&) = delete;
  MCFileReader& operator=(const MCFileReader&) = delete;
  MCFileReader(MCFileReader&&) = delete;
  MCFileReader& operator=(MCFileReader&&) = delete;

  // Read and convert the next event; return nullptr at end of input.
  virtual UPMCParticleColl operator()() = 0;

  // Print the most recently read event, if one is available.
  virtual void printCurrentEvent() const = 0;

  // Return the number of successfully read events.
  virtual std::size_t eventsRead() const noexcept = 0;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
