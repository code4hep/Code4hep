import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.source = cms.Source("PodioSource",
    fileNames = cms.untracked.vstring(
        "testPodioSource.root"
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.testTracksProducer2 = cms.EDProducer("c4h::TestTracksProducer")

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

process.readTracksFromProducer2 = cms.EDAnalyzer("c4h::TestTracksAnalyzer",
    tracks = cms.untracked.InputTag("testTracksProducer2")
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
process.testRelationsAndLinksProducer41 = cms.EDProducer("c4h::TestRelationsAndLinksProducer",
    tracks = cms.untracked.InputTag("TrackCollection"),
    mcParticles = cms.untracked.InputTag("MCParticleCollection"),
    testValue = cms.untracked.int32(41)
)

# Links and relations point into collections created by
# this process in the same module
process.testRelationsAndLinksProducer51 = cms.EDProducer("c4h::TestRelationsAndLinksProducer",
    tracks = cms.untracked.InputTag("NotUsed"),
    mcParticles = cms.untracked.InputTag("NotUsed"),
    linkToCollectionsMadeInModule = cms.untracked.bool(True),
    testValue = cms.untracked.int32(51)
)

# For tracks, links and relations point into collections
# created by this process in a different module
process.testRelationsAndLinksProducer61 = cms.EDProducer("c4h::TestRelationsAndLinksProducer",
    tracks = cms.untracked.InputTag("testTracksProducer2"),
    mcParticles = cms.untracked.InputTag("MCParticleCollection"),
    testValue = cms.untracked.int32(61)
)

# Links and relations point into collections created by
# the previous process
process.testRelationsAndLinksProducer71 = cms.EDProducer("c4h::TestRelationsAndLinksProducer",
    tracks = cms.untracked.InputTag("testRelationsAndLinksProducer11"),
    mcParticles = cms.untracked.InputTag("testRelationsAndLinksProducer11mcParticles"),
    testValue = cms.untracked.int32(71)
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
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer11recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer11reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer11links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(11),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(42),
    expectedTestValueInMCParticle = cms.untracked.int32(42)
)

process.testRelationsAndLinksRead21 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer21links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(21),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(21),
    expectedTestValueInMCParticle = cms.untracked.int32(21)
)

process.testRelationsAndLinksRead31 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer31links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(31),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(0),
    expectedTestValueInMCParticle = cms.untracked.int32(42)
)

process.testRelationsAndLinksRead41 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer41"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer41", "recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer41", "reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer41", "links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(41),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(42),
    expectedTestValueInMCParticle = cms.untracked.int32(42)
)

process.testRelationsAndLinksRead51 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer51"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer51", "recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer51", "reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer51", "links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(51),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(51),
    expectedTestValueInMCParticle = cms.untracked.int32(51)
)

process.testRelationsAndLinksRead61 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer61"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer61", "recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer61", "reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer61", "links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(61),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(0),
    expectedTestValueInMCParticle = cms.untracked.int32(42)
)

process.testRelationsAndLinksRead71 = cms.EDAnalyzer("c4h::TestRelationsAndLinksAnalyzer",
    trackCollection = cms.untracked.InputTag("testRelationsAndLinksProducer71"),
    recDqdxCollection = cms.untracked.InputTag("testRelationsAndLinksProducer71", "recDqdxs"),
    reconstructedParticleCollection = cms.untracked.InputTag("testRelationsAndLinksProducer71", "reconstructedParticles"),
    trackToMCParticleLinkCollection = cms.untracked.InputTag("testRelationsAndLinksProducer71", "links"),
    expectedTrackTypeInTrackCollection = cms.untracked.int32(71),
    expectedTrackTypeTrackInRelationOrLink = cms.untracked.int32(11),
    expectedTestValueInMCParticle = cms.untracked.int32(11)
)

process.p = cms.Path(process.testTracksProducer2 +
                     process.testRelationsAndLinksProducer41 +
                     process.testRelationsAndLinksProducer51 +
                     process.testRelationsAndLinksProducer61 +
                     process.testRelationsAndLinksProducer71
)

process.e = cms.EndPath(process.test +
                        process.readTrackCollection +
                        process.readTracksFromProducer1 +
                        process.readTracksFromProducer2 +
                        process.testEventHeaderAnalyzer +
                        process.testRelationsAndLinks +
                        process.testRelationsAndLinksRead11 +
                        process.testRelationsAndLinksRead21 +
                        process.testRelationsAndLinksRead31 +
                        process.testRelationsAndLinksRead41 +
                        process.testRelationsAndLinksRead51 +
                        process.testRelationsAndLinksRead61 +
                        process.testRelationsAndLinksRead71
)

process.out = cms.OutputModule("PodioOutputModule",
    fileName = cms.untracked.string('testPodioSource2.root')
)

process.outpath = cms.EndPath(process.out)
