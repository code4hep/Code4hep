import FWCore.ParameterSet.Config as cms

process = cms.Process("GeometryTest")

process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)
process.MessageLogger.cerr.INFO.limit = -1

process.DetectorESSource = cms.ESSource("DetectorESSource",
    confGeomXMLFiles = cms.FileInPath('Code4hep/Geometry/test/testGeo.xml'),
    appendToDataLabel = cms.string('Test')
)

process.test = cms.EDAnalyzer("DetectorAnalyzer",
    detectorLabel = cms.ESInputTag('','Test')
)

process.p = cms.Path(process.test)
