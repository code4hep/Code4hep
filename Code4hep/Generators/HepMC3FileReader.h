//---------------------------------------------------------------------------//
//! \file Code4hep/Generators/HepMC3FileReader.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_Generators_HepMC3FileReader_h
#define Code4hep_Generators_HepMC3FileReader_h

#include "Code4hep/Generators/MCFileReader.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace HepMC3
{
class GenEvent;
class Reader;
}  // namespace HepMC3

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * Read HepMC3 events and convert them to EDM4hep particles.
 *
 * Multiple files are read sequentially as one logical event stream. Supported
 * formats are HepMC3 ASCII and LHEF. The current native HepMC3 event is
 * retained until the next read so that it can be printed on request.
 *
 * This class is based on HepMC3FileReader of CMSSW from
 * https://github.com/cms-sw/cmssw/blob/master/IOMC/Input
 */
class HepMC3FileReader final : public MCFileReader
{
  //!@{
  //! \name Type aliases
  using VecString = std::vector<std::string>;
  //!@}  
public:
  explicit HepMC3FileReader(VecString fileNames, std::string fileFormat);
  ~HepMC3FileReader() override;

  UPMCParticleColl operator()() final;
  void printCurrentEvent() const final;

  std::size_t eventsRead() const noexcept final
  {
    return eventsRead_;
  }

private:
  // Construct a reader for one input file.
  std::unique_ptr<HepMC3::Reader>
  makeReader(const std::string& fileName) const;

  // Open the next configured file; return false when no files remain.
  bool openNextFile();

  // Convert one HepMC3 event into an EDM4hep particle collection.
  UPMCParticleColl
  convertToEDM4hep(const HepMC3::GenEvent& event) const;

private:
  const VecString fileNames_;
  const std::string fileFormat_;

  std::size_t nextFile_{0};
  std::size_t eventsRead_{0};

  std::unique_ptr<HepMC3::Reader> reader_;
  std::unique_ptr<HepMC3::GenEvent> currentEvent_;
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
