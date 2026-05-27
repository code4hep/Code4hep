#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Code4hep/Geometry/interface/Detector.h"
#include "Code4hep/GeometryRecords/interface/GeometryRecord.h"
#include "DD4hep/Detector.h"

class DetectorAnalyzer : public edm::one::EDAnalyzer<> {
public:
  explicit DetectorAnalyzer(const edm::ParameterSet& iConfig);

  void beginJob() override {}
  void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) override;
  void endJob() override {}

private:
  const edm::ESInputTag tag_;
  const edm::ESGetToken<c4h::Detector, GeometryRecord> token_;
};

DetectorAnalyzer::DetectorAnalyzer(const edm::ParameterSet& iConfig)
    : tag_(iConfig.getParameter<edm::ESInputTag>("detectorLabel")), token_(esConsumes(tag_)) {}

void DetectorAnalyzer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) {
  edm::ESTransientHandle<c4h::Detector> h_det = iSetup.getTransientHandle(token_);
  auto geo = h_det->geometry();

  edm::LogVerbatim("Geometry") << "Iterate over the detectors:\n";
  edm::LogVerbatim("Geometry").log([&](auto& log) {
    for (auto const& it : geo->detectors()) {
      dd4hep::DetElement det(it.second);
      log << it.first << ": " << det.path() << "\n";
    }
  });
  edm::LogVerbatim("Geometry") << "done!\n";

  edm::LogVerbatim("Geometry") << "Test the magnetic field:\n";
  double position[3] = {0.0, 0.0, 0.0};
  double bField[3] = {0.0, 0.0, 0.0};
  geo->field().magneticField(position, bField);
  edm::LogVerbatim("Geometry") << "B-field at origin: "
                               << "Bx = " << bField[0] << ", "
                               << "By = " << bField[1] << ", "
                               << "Bz = " << bField[2] << " T";
  edm::LogVerbatim("Geometry") << "done!";
}

DEFINE_FWK_MODULE(DetectorAnalyzer);
