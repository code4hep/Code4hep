//---------------------------------------------------------------------------//
//! \file Code4hep/G4Application/plugins/G4SimProducer.cc
//---------------------------------------------------------------------------//
#include <memory>
#include <iostream>
#include <thread>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ServiceRegistry.h"

#include "Code4hep/Generators/MCParticlesToG4.h"
#include "Code4hep/G4Application/ThreadHandoff.h"
#include "Code4hep/G4Application/G4MasterInterface.h"
#include "Code4hep/G4Application/G4WorkerInterface.h"

#include "G4Event.hh"
#include <edm4hep/MCParticleCollection.h>

//---------------------------------------------------------------------------//
/*! 
 * Stream-based CMS producer that drives Geant4 event processing.
 *
 * The producer owns a thread-local Geant4 worker interface and uses a
 * master interface in the global cache for shared configuration, geometry,
 * and physics setup. Event processing is forwarded into the worker thread
 * via ThreadHandoff to avoid concurrent Geant4 initialization on the wrong
 * thread.
 */
class G4SimProducer : public edm::stream::EDProducer<edm::GlobalCache<c4h::G4MasterInterface>, edm::RunCache<int>, edm::stream::WatchRuns>
{
public:
  explicit G4SimProducer(const edm::ParameterSet&, const c4h::G4MasterInterface*);
  ~G4SimProducer() override;

  static std::unique_ptr<c4h::G4MasterInterface>
  initializeGlobalCache(const edm::ParameterSet& iConfig);

  static std::shared_ptr<int>
  globalBeginRun(const edm::Run& iRun,
		 const edm::EventSetup& iSetup,
		 const c4h::G4MasterInterface* masterThread);

  static void globalEndRun(const edm::Run& iRun,
			   const edm::EventSetup& iSetup,
			   const RunContext* iContext);

  static void globalEndJob(c4h::G4MasterInterface* masterThread);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;
  void beginRun(edm::Run const&, edm::EventSetup const&) override;
  void endRun(edm::Run const&, edm::EventSetup const&) override;
  void endStream() override;

private:
  int m_maxEvents{};
  omt::ThreadHandoff m_handoff;
  const c4h::G4MasterInterface* m_masterThread;
  std::unique_ptr<c4h::G4WorkerInterface> m_workerInterface;

  edm::EDGetTokenT<edm4hep::MCParticleCollection> mcToken_;
};

//---------------------------------------------------------------------------//
// INLINE DEFINITIONS
//---------------------------------------------------------------------------//
std::unique_ptr<c4h::G4MasterInterface>
G4SimProducer::initializeGlobalCache(const edm::ParameterSet& iConfig)
{
  return std::make_unique<c4h::G4MasterInterface>(iConfig);
}

G4SimProducer::G4SimProducer(const edm::ParameterSet& p,
			     const c4h::G4MasterInterface* masterThread)
  : m_maxEvents{p.getParameter<int>("maxEvents")}
  , m_handoff{p.getUntrackedParameter<int>("workerThreadStackSize", 10 * 1024*1024)}
  , m_masterThread(masterThread)
  , mcToken_(consumes(p.getParameter<edm::InputTag>("generator")))
{
  // Construct the worker interface on the worker thread using the same
  // thread stack semantics as later event processing.
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, &p, token]() {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface = std::make_unique<c4h::G4WorkerInterface>(m_maxEvents);
  });
}

G4SimProducer::~G4SimProducer() {}

//---------------------------------------------------------------------------//
// MEMBER FUNCTIONS
//---------------------------------------------------------------------------//
std::shared_ptr<int>
G4SimProducer::globalBeginRun(const edm::Run& iRun,
			      const edm::EventSetup& iSetup,
			      const c4h::G4MasterInterface* masterThread)
{
  if (masterThread)
  {
    masterThread->beginRun();
  }

  return std::shared_ptr<int>();
}

void G4SimProducer::globalEndRun(const edm::Run& iRun,
				 const edm::EventSetup& iSetup,
				 const RunContext* iContext)
{
  if (nullptr != iContext->global())
  {
    iContext->global()->endRun();
  }
}

void G4SimProducer::globalEndJob(c4h::G4MasterInterface* masterThread)
{
  if (masterThread)
  {
    masterThread->stopThread();
  }
}

// Method called to produce the data
void G4SimProducer::produce(edm::Event& e, const edm::EventSetup& es)
{
  using namespace edm;
  
  auto token = edm::ServiceRegistry::instance().presentToken();

  G4Event* evt = nullptr;
  m_handoff.runAndWait([this, &e, &evt, token]()
  {
    edm::ServiceRegistry::Operate guard{token};

    G4int evtid = static_cast<G4int>(e.id().event());
    auto const& genEvent = e.get(mcToken_);
    auto g4evt = c4h::MCParticlesToG4(genEvent, evtid);

    // Forward the converted EDM event into the Geant4 worker
    evt = m_workerInterface->produce(g4evt.get());
  });

  // Note: Do post-processing of processed G4 events
}

// Method called when starting to processes a run
void G4SimProducer::beginRun(edm::Run const&, edm::EventSetup const& es)
{
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, &es, token]()
  {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface->initializeG4(m_masterThread->runManagerMasterPtr());
  });
} 

// Method called when ending the processing of a run
void G4SimProducer::endRun(edm::Run const&, edm::EventSetup const& es)
{
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, &es, token]()
  {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface->endRun();
  });
}

// Method called when ending the processing of a stream
void G4SimProducer::endStream()
{
  // Reset the worker interface on the worker thread to keep all Geant4
  // lifecycle operations bound to the same thread context - Ensure that
  // all Geant4 worker threads are deleted before the master thread.
  auto token = edm::ServiceRegistry::instance().presentToken();
  m_handoff.runAndWait([this, token]() {
    edm::ServiceRegistry::Operate guard{token};
    m_workerInterface.reset();
  });
}

//define this as a plug-in
DEFINE_FWK_MODULE(G4SimProducer);
