//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/G4WorkerInterface.h
//---------------------------------------------------------------------------//
#ifndef Code4hep_G4Application_G4WorkerInterface_h
#define Code4hep_G4Application_G4WorkerInterface_h

class G4Event;
class G4MTRunManager;
class G4Run;
class G4WorkerRunManager;

#include <memory>

namespace c4h
{
//---------------------------------------------------------------------------//
/*!
 * This encapsulates the per-thread Geant4 worker run manager.
 *
 * This interface initializes a Geant4 worker using a master thread's
 * configuration, processes individual G4 events on the worker thread, and
 * forwards run lifecycle events to the worker's user actions.
 */
class G4WorkerInterface
{
public:
  explicit G4WorkerInterface(int maxEvent);
  ~G4WorkerInterface();

  // Process one G4 event using the worker run manager
  void produce(G4Event& g4evt);

  // Worker thread index assigned at construction
  inline int getThreadIndex() const { return threadIndex_; }

  // Initialize the worker using the master Geant4 run manager
  void initializeG4(G4MTRunManager* runManagerMaster);  

  // Finalize the current run and call end-of-run actions
  void endRun();

private:
  const int threadIndex_{-1};
  int maxEvents_{};
  bool threadInitialized_{false};
  std::unique_ptr<G4WorkerRunManager> workerRM_; 
  G4Run* currentRun_{nullptr}; 
};

//---------------------------------------------------------------------------//
}  // namespace c4h
#endif
