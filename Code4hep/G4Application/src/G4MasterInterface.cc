//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/src/G4MasterInterface.cc
//---------------------------------------------------------------------------//
#include "Code4hep/G4Application/G4MasterInterface.h"
#include "Code4hep/G4Application/ActionInitialization.h"
#include "Code4hep/G4Application/DetectorConstruction.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "G4MTRunManager.hh"
#include "G4MTRunManagerKernel.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4StateManager.hh"
#include "G4UserRunAction.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"

#include <thread>

namespace c4h
{
G4MasterInterface::G4MasterInterface(edm::ParameterSet const& p)
  : gdmlFile_(p.getParameter<edm::ParameterSet>("Detector").getParameter<std::string>("gdml"))
  , physName_(p.getParameter<edm::ParameterSet>("Physics").getParameter<std::string>("type"))
{
  // Lock the mutex
  std::unique_lock<std::mutex> lk(threadMutex_);

  // Create the Geant4 master thread
  masterThread_ = std::thread([&]() 
  {
    // Lock the mutex (i.e. wait until the creating thread has called cv.wait()
    std::unique_lock<std::mutex> lk2(threadMutex_);

    // Create the master run manager, and share it to the CMSSW thread
    runManagerMaster_ = std::make_shared<G4MTRunManager>();

    // State loop
    bool isG4Alive = false;
    while (true)
    {
      // Signal the main thread that it can proceed
      mainCanProceed_ = true;
      notifyMainCv_.notify_one();

      // Wait until the main thread sends signal
      masterCanProceed_ = false;
      notifyMasterCv_.wait(lk2, [&] { return masterCanProceed_; });

      // Act according to the state
      if (masterThreadState_ == ThreadState::BeginRun)
      {
        // Set mandatory Geant4 user initialization classes

	// Add a pysics list
        G4PhysListFactory factory;
        auto physics = std::unique_ptr<G4VModularPhysicsList>(
          factory.GetReferencePhysList(physName_));
        if (!physics)
        {
           G4Exception("main", "InvalidPhysicsList", FatalException,
                        ("Unknown physics list: " + physName_).c_str());
        }
        runManagerMaster_->SetUserInitialization(physics.release());

	// Add a user detector construction
        auto det = std::make_unique<DetectorConstruction>(gdmlFile_);
	
        runManagerMaster_->SetUserInitialization(det.release());
        runManagerMaster_->SetUserInitialization(new ActionInitialization());

	// Turned off parallelizing geometry initialization 
        G4GeometryManager::GetInstance()->RequestParallelOptimisation(false,
								      false);
        // Initialize the Geant4 G4MTRunManager
        runManagerMaster_->Initialize();
        runManagerMaster_->RunInitialization();

        isG4Alive = true;
      }
      else if (masterThreadState_ == ThreadState::EndRun)
      {
        // Stop Geant4
        G4GeometryManager::GetInstance()->OpenGeometry();
        G4StateManager::GetStateManager()->SetNewState(G4State_Quit);
	
        G4UserRunAction* userRunAction =
        const_cast<G4UserRunAction*>(runManagerMaster_->GetUserRunAction());

        if (userRunAction != nullptr)
        {
	  // End of run actions
	  userRunAction->EndOfRunAction(runManagerMaster_->GetCurrentRun());
        }

	runManagerMaster_->GetMTMasterRunManagerKernel()->RunTermination();

        G4PhysicalVolumeStore::Clean();
        isG4Alive = false;
      }
      else if (masterThreadState_ == ThreadState::Destruct)
      {
        // Stop the master thread started - breaking out of state loop
        if (isG4Alive)
	{
          throw cms::Exception("LogicError") << "Geant4 is still alive";
	}
	break;
      }
      else
      {
        throw cms::Exception("LogicError") << "Illegal master thread state";
      }
    }

    // Clean up must be done in this thread, otherwise will be segfault
    runManagerMaster_.reset();
    lk2.unlock();
  });

  // Start waiting a signal from the cv (releases the lock temporarily) 
  mainCanProceed_ = false;
  notifyMainCv_.wait(lk, [&]() { return mainCanProceed_; });
  lk.unlock();
}

G4MasterInterface::~G4MasterInterface()
{
  if (!stopped_)
  {
    stopThread();
  }
}

void G4MasterInterface::beginRun() const 
{
  std::lock_guard<std::mutex> lk(protectMutex_);
  std::unique_lock<std::mutex> lk2(threadMutex_);

  if (firstRun_)
  {
    // Do neccessary actions at the first run, if any
    firstRun_ = false;
  }

  masterThreadState_ = ThreadState::BeginRun;
  masterCanProceed_ = true;
  mainCanProceed_ = false;

  // Signal the master for BeginRun
  notifyMasterCv_.notify_one();
  notifyMainCv_.wait(lk2, [&]() { return mainCanProceed_; });

  lk2.unlock();
}

void G4MasterInterface::endRun() const
{
  std::lock_guard<std::mutex> lk(protectMutex_);
  std::unique_lock<std::mutex> lk2(threadMutex_);

  masterThreadState_ = ThreadState::EndRun;
  mainCanProceed_ = false;
  masterCanProceed_ = true;

  // Signal the master for EndRun;
  notifyMasterCv_.notify_one();
  notifyMainCv_.wait(lk2, [&]() { return mainCanProceed_; });

  lk2.unlock();
}

void G4MasterInterface::stopThread()
{
  if (stopped_)
  {
    return;
  }

  // Stop the main thread;
  std::unique_lock<std::mutex> lk2(threadMutex_);
  masterThreadState_ = ThreadState::Destruct;
  masterCanProceed_ = true;

  // Notify stopping the thread
  notifyMasterCv_.notify_one();
  lk2.unlock();

  masterThread_.join();
  stopped_ = true;
}

}  // namespace c4h
