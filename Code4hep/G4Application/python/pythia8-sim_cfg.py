import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

from FWCore.Modules.modules import EmptySource

process.source = EmptySource()

process.maxEvents.input = 10

#Setup FWK for multithreaded
process.options.numberOfThreads = 4
process.options.numberOfStreams = 0

from Code4hep.G4Application.modules import G4SimProducer
from Code4hep.Generators.modules import GenProducer

process.gen = GenProducer(
    generatorType = cms.string("Pythia8Generator"),
    generator = cms.InputTag("MCParticles"),
        Pythia8Parameters = cms.vstring(
            "Beams:idA = 2212",
            "Beams:idB = 2212",
            "Beams:eCM = 14000.",
            "HardQCD:all = on",
            "PhaseSpace:pTHatMin = 20."
        )
)

process.RandomNumberGeneratorService = cms.Service(
    "RandomNumberGeneratorService",
    gen = cms.PSet(
        initialSeed = cms.untracked.uint32(12345)
    )
)

process.sim = G4SimProducer(
    maxEvents = cms.int32(process.maxEvents.input.value()),
    generator = cms.InputTag("gen", "MCParticles"),
    Physics = cms.PSet(
        type = cms.string('FTFP_BERT')
    ),
    Detector = cms.PSet(
        gdml = cms.string('Code4hep/G4Application/test/simple-cms.gdml')
    )
)

process.generation_step = cms.Path(process.gen)
process.simulation_step = cms.Path(process.sim)

process.schedule = cms.Schedule(
    process.generation_step,
    process.simulation_step
)
