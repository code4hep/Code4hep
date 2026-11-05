//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/G4WorkerInterface.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/G4WorkerInterface.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "G4Event.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4Threading.hh"
#include "G4StateManager.hh"
#include "G4MTRunManager.hh"
#include "G4WorkerRunManager.hh"
#include "G4WorkerRunManagerKernel.hh"
#include "G4WorkerThread.hh"
#include "G4UserWorkerInitialization.hh"
#include "G4UserWorkerThreadInitialization.hh"
#include "G4VUserActionInitialization.hh"

#include <atomic>
#include <thread>

namespace
{
  // Global atomic counter to assign unique thread IDs to worker interfaces
  std::atomic<int> thread_counter{0};
  int get_new_thread_index()
  {
    return thread_counter.fetch_add(1, std::memory_order_relaxed);
  }
}

namespace c4h
{
G4WorkerInterface::G4WorkerInterface(int maxEvents)
  : threadIndex_{(get_new_thread_index())}
  , maxEvents_(maxEvents)
{
  // Assign Geant4 thread id for thread-local bookkeeping
  G4Threading::G4SetThreadId(threadIndex_);
}

G4WorkerInterface::~G4WorkerInterface()
{
  workerRM_.reset();
}

void G4WorkerInterface::initializeG4(G4MTRunManager* masterRM)
{
  if (threadInitialized_)
  {
    return;
  }

  // Configure random engine for this worker using master's engine
  const CLHEP::HepRandomEngine* masterEngine 
    = masterRM->getMasterRandomEngine();
  masterRM->GetUserWorkerThreadInitialization()->SetupRNGEngine(masterEngine);
  masterRM->InitializeEventSeeds(maxEvents_);

  // Create the worker run manager 
  workerRM_.reset(
    masterRM->GetUserWorkerThreadInitialization()->CreateWorkerRunManager());
  
  // Initialize worker part of shared resources (geometry, physics)
  G4WorkerThread::BuildGeometryAndPhysicsVector();

  // Share detector geometry and physics list from master to worker
  const G4VUserDetectorConstruction* detector 
      = masterRM->GetUserDetectorConstruction();
  const G4VUserPhysicsList* physList = masterRM->GetUserPhysicsList();
  
  workerRM_->G4RunManager::SetUserInitialization(const_cast<G4VUserDetectorConstruction*>(detector));
  workerRM_->SetUserInitialization(const_cast<G4VUserPhysicsList*>(physList));

  // Build per-thread user actions if provided by the master.
  if (masterRM->GetUserActionInitialization() != nullptr)
  {
    masterRM->GetNonConstUserActionInitialization()->Build();
  }
  if (masterRM->GetUserWorkerInitialization() != nullptr)
  {
    masterRM->GetUserWorkerInitialization()->WorkerStart();
  }
  
  // Invokes InitializeGeometry and InitializePhysics
  workerRM_->Initialize();

  G4StateManager::GetStateManager()->SetNewState(G4State_Idle);

  // Initialize the worker kernel and invoke any BeginOfRun actions
  if(workerRM_->GetWorkerRunManagerKernel())
  {
    workerRM_->GetWorkerRunManagerKernel()->RunInitialization(true);

    // NOTE: Ideally BeginOfRunAction should be invoked by RunInitialization
    // itself. For now call it manually if a user run action exists.

    G4UserRunAction* userRunAction
      = const_cast<G4UserRunAction*>(workerRM_->GetUserRunAction());
    if (userRunAction != nullptr)
    {
      currentRun_ = userRunAction->GenerateRun();
      userRunAction->BeginOfRunAction(currentRun_);
    }
  }
  else
  {
    throw cms::Exception("Configuration")
      << "Illegal G4WorkerRunManagerKernel state";
  }

  G4StateManager::GetStateManager()->SetNewState(G4State_GeomClosed);
  threadInitialized_ = true;
}

void G4WorkerInterface::produce(G4Event& g4evt)
{
  auto kernel = workerRM_->GetWorkerRunManagerKernel();
  if (!kernel)
  {
    throw cms::Exception("Configuration")
      << "WorkerRunManagerKernel not available";
  }

  kernel->GetEventManager()->ProcessOneEvent(&g4evt);
}

void G4WorkerInterface::endRun()
{
  G4UserRunAction* userRunAction =
    const_cast<G4UserRunAction*>(workerRM_->GetUserRunAction());

  if (userRunAction != nullptr && currentRun_ != nullptr)
  {
    userRunAction->EndOfRunAction(currentRun_);
  }

  workerRM_->GetWorkerRunManagerKernel()->RunTermination();
}
  
//---------------------------------------------------------------------------//
}  // namespace c4h
