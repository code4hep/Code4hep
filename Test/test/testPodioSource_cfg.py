import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

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

process.e = cms.EndPath(process.test)
