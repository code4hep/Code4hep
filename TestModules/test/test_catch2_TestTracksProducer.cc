#include "catch2/catch_all.hpp"
#include "FWCore/TestProcessor/interface/TestProcessor.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "edm4hep/TrackCollection.h"

static constexpr auto s_tag = "[TestTracksProducer]";

TEST_CASE("Standard checks of TestTracksProducer", s_tag) {
  const std::string baseConfig{
      R"_(from FWCore.TestProcessor.TestProcess import *
process = TestProcess()
from Code4hep.TestModules.modules import c4h_TestTracksProducer
process.toTest = c4h_TestTracksProducer(
 )
process.moduleToTest(process.toTest)
)_"};

  edm::test::TestProcessor::Config config{baseConfig};
  SECTION("base configuration is OK") { REQUIRE_NOTHROW(edm::test::TestProcessor(config)); }

  SECTION("No event data") {
    edm::test::TestProcessor tester(config);

    REQUIRE_NOTHROW(tester.test());
    auto const& event = tester.test();
    REQUIRE(event.get<edm4hep::TrackCollection>()->size() == 1);
  }

  SECTION("beginJob and endJob only") {
    edm::test::TestProcessor tester(config);

    REQUIRE_NOTHROW(tester.testBeginAndEndJobOnly());
  }

  SECTION("Run with no LuminosityBlocks") {
    edm::test::TestProcessor tester(config);

    REQUIRE_NOTHROW(tester.testRunWithNoLuminosityBlocks());
  }

  SECTION("LuminosityBlock with no Events") {
    edm::test::TestProcessor tester(config);

    REQUIRE_NOTHROW(tester.testLuminosityBlockWithNoEvents());
  }
}

//Add additional TEST_CASEs to exercise the modules capabilities
