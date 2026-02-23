import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cout = cms.untracked.PSet(
    threshold = cms.untracked.string('INFO'),
    enable = cms.untracked.bool(True)
)

process.Tracer = cms.Service("Tracer")

process.source = cms.Source("PodioSource",
    fileNames = cms.untracked.vstring(
        "testPodioSource.root"
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.testTracksProducer2 = cms.EDProducer("c4h::TestTracksProducer")

from FWCore.Framework.modules import RunLumiEventAnalyzer
process.test = RunLumiEventAnalyzer(
    verbose = False,
    expectedRunLumiEvents = [
      43, 0, 0,
      43, 1, 0,
      43, 1, 42,
      43, 1, 42,
      43, 1, 42,
      43, 1, 42,
      43, 1, 42,
      43, 1, 42,
      43, 1, 0,
      43, 0, 0
    ],
    expectedEndingIndex = 30
)

process.readTrackCollection = cms.EDAnalyzer("c4h::TestTracksAnalyzer",
    tracks = cms.untracked.InputTag("TrackCollection")
)

process.readTracksFromProducer1 = cms.EDAnalyzer("c4h::TestTracksAnalyzer",
    tracks = cms.untracked.InputTag("testTracksProducer1")
)

process.readTracksFromProducer2 = cms.EDAnalyzer("c4h::TestTracksAnalyzer",
    tracks = cms.untracked.InputTag("testTracksProducer2")
)

process.testEventHeaderAnalyzer = cms.EDAnalyzer("c4h::TestEventHeaderAnalyzer",
    eventHeaders = cms.untracked.InputTag("EventHeader"),
)

process.p = cms.Path(process.testTracksProducer2)

process.e = cms.EndPath(process.test +
                        process.readTrackCollection +
                        process.readTracksFromProducer1 +
                        process.readTracksFromProducer2 +
                        process.testEventHeaderAnalyzer
)

process.out = cms.OutputModule("PodioOutputModule",
    fileName = cms.untracked.string('testPodioSource2.root')
)

process.outpath = cms.EndPath(process.out)
