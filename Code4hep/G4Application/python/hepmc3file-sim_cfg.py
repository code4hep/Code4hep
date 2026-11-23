import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.maxEvents.input = 10

#Setup FWK for multithreaded
process.options.numberOfThreads = 4
process.options.numberOfStreams = 0

from Code4hep.G4Application.modules import G4SimProducer

process.source = cms.Source("HepMC3FileSource",
    fileNames = cms.untracked.vstring("Code4hep/Generators/test/test.hepmc3"),
    fileFormat = cms.untracked.string("ascii"),
    printEvent = cms.untracked.bool(True)
)

process.RandomNumberGeneratorService = cms.Service(
    "RandomNumberGeneratorService",
    gen = cms.PSet(
        initialSeed = cms.untracked.uint32(12345)
    )
)

process.sim = G4SimProducer(
    maxEvents = cms.int32(process.maxEvents.input.value()),
    generator = cms.InputTag("source", "MCParticles"),
    Physics = cms.PSet(
        type = cms.string('FTFP_BERT')
    ),
    Detector = cms.PSet(
        gdml = cms.string('Code4hep/G4Application/test/simple-cms.gdml')
    )
)

process.simulation_step = cms.Path(process.sim)

process.out = cms.OutputModule("PodioOutputModule",
    fileName = cms.untracked.string('HepMC3-simple-cms.root')
)

process.outpath = cms.EndPath(process.out)
