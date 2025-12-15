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

process.testAnalyzer = cms.EDAnalyzer("c4h::TestTracksAnalyzer",
    eventHeaders = cms.untracked.InputTag("EventHeader"),
    tracks = cms.untracked.InputTag("TrackCollection")
)

process.e = cms.EndPath(process.testAnalyzer + process.test)
