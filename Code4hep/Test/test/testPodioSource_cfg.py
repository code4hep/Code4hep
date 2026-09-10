import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.source = cms.Source("PodioSource",
    fileNames = cms.untracked.vstring(
        "edm4hep.root",
        "edm4hep_copy.root"
    ),
    ignoreMissingOnFirstEvent = cms.untracked.bool(False)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.testTracksProducer1 = cms.EDProducer("c4h::TestTracksProducer")

from FWCore.TestModules.modules import RunLumiEventAnalyzer
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

process.testEventHeaderAnalyzer = cms.EDAnalyzer("c4h::TestEventHeaderAnalyzer",
    eventHeaders = cms.untracked.InputTag("EventHeader"),
)

# The next part tests relations and links. It exercises
# one-to-one relations, one-to-many relations (including a
# circular case and one that isn't), and links. It includes
# cases where the target of the link was in the input file,
# created in another module in the same process, and created
# by the same module. It also includes cases where the collection
# containing the links was created in the current process or
# read from an input file. There are several producers and
# analyzers to hit the various combinations of those cases.

# The initial input files are created by this script:
#   https://github.com/key4hep/EDM4hep/blob/main/scripts/createEDM4hepFile.py
# Note this script is in the EDM4hep repository, not in the
# Code4hep repository. If the people who manage that repository
# ever change it, then this test may fail and need to be updated.

# Links and relations point into collections in the initial file
# from the EDM4hep script.
process.testRelationsAndLinksProducer11 = cms.EDProducer("c4h::TestRelationsAndLinksProducer",
    tracks = cms.untracked.InputTag("TrackCollection"),
    mcParticles = cms.untracked.InputTag("MCParticleCollection"),
    testValue = cms.untracked.int32(11)
)

# Links and relations point into collections created by
# this process in the same module
process.testRelationsAndLinksProducer21 = cms.EDProducer("c4h::TestRelationsAndLinksProducer",
    tracks = cms.untracked.InputTag("NotUsed"),
    mcParticles = cms.untracked.InputTag("NotUsed"),
    linkToCollectionsMadeInModule = cms.untracked.bool(True),
    testValue = cms.untracked.int32(21)
)

# For tracks, links and relations point into collections
# created by this process in a different module
process.testRelationsAndLinksProducer31 = cms.EDProducer("c4h::TestRelationsAndLinksProducer",
    tracks = cms.untracked.InputTag("testTracksProducer1"),
    mcParticles = cms.untracked.InputTag("MCParticleCollection"),
    testValue = cms.untracked.int32(31)
)

process.testRelationsAndLinks = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("TrackCollection"),
    recDqdxCollection = cms.untracked.InputTag("RecDqdxCollection"),
    reconstructedParticleCollection = cms.untracked.InputTag("ReconstructedParticleCollection"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("TrackMCParticleLinkCollection"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(42),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(42),
    expectedTestValueInMCParticle = cms.untracked.int32(42)
)

process.testRelationsAndLinksRead11 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer11"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer11", "recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer11", "reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer11", "links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(11),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(42),
    expectedTestValueInMCParticle = cms.untracked.int32(42)
)

process.testRelationsAndLinksRead21 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21", "recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21", "reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21", "links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(21),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(21),
    expectedTestValueInMCParticle = cms.untracked.int32(21)
)

process.testRelationsAndLinksRead31 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31", "recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31", "reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31", "links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(31),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(0),
    expectedTestValueInMCParticle = cms.untracked.int32(42)
)

process.p = cms.Path(process.testTracksProducer1 +
                     process.testRelationsAndLinksProducer11 +
                     process.testRelationsAndLinksProducer21 +
                     process.testRelationsAndLinksProducer31
)

process.e = cms.EndPath(process.test +
                        process.readTrackCollection +
                        process.readTracksFromProducer1 +
                        process.testEventHeaderAnalyzer +
                        process.testRelationsAndLinks +
                        process.testRelationsAndLinksRead11 +
                        process.testRelationsAndLinksRead21 +
                        process.testRelationsAndLinksRead31
)

process.out = cms.OutputModule("PodioOutputModule",
    fileName = cms.untracked.string('testPodioSource.root')
)

process.outpath = cms.EndPath(process.out)
