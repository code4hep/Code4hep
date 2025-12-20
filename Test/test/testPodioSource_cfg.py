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
        "edm4hep.root",
        "edm4hep_copy.root"
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

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

process.testTrackAnalyzer = cms.EDAnalyzer("c4h::TestTracksAnalyzer",
    tracks = cms.untracked.InputTag("TrackCollection")
)

process.testEventHeaderAnalyzer = cms.EDAnalyzer("c4h::TestEventHeaderAnalyzer",
    eventHeaders = cms.untracked.InputTag("EventHeader"),
)


process.e = cms.EndPath(process.testTrackAnalyzer + process.testEventHeaderAnalyzer + process.test)
