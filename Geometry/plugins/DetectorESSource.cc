#include <memory>

#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Concurrency/interface/SharedResourceNames.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Code4hep/GeometryRecords/interface/GeometryRecord.h"
#include "Code4hep/GeometryRecords/interface/MagneticFieldRecord.h"
#include "Code4hep/Geometry/interface/Detector.h"

class DetectorESSource : public edm::ESProducer, public edm::EventSetupRecordIntervalFinder {
public:
  DetectorESSource(const edm::ParameterSet& iConfig);
  ~DetectorESSource() override;

  using ReturnType = std::unique_ptr<c4h::Detector>;

  ReturnType produce();
  ReturnType produceGeom(const GeometryRecord&);
  ReturnType produceMagField(const MagneticFieldRecord&);
  static void fillDescriptions(edm::ConfigurationDescriptions&);

protected:
  void setIntervalFor(const edm::eventsetup::EventSetupRecordKey&, const edm::IOVSyncValue&, edm::ValidityInterval&) override;

private:
  const bool isMagField_;
  const std::string appendToDataLabel_;
  const std::string confGeomXMLFiles_;
  const std::string label_;
};

DetectorESSource::DetectorESSource(const edm::ParameterSet& iConfig)
    : isMagField_(iConfig.getParameter<bool>("isMagField")),
      appendToDataLabel_(iConfig.getParameter<std::string>("appendToDataLabel")),
      confGeomXMLFiles_(iConfig.getParameter<edm::FileInPath>("confGeomXMLFiles").fullPath()),
      label_(iConfig.getParameter<std::string>("label")) {
  usesResources({edm::ESSharedResourceNames::kDD4hep});
  edm::LogVerbatim("Geometry") << "DetectorESSource::isMagField " << isMagField_ << " appendToDataLabel "
                               << appendToDataLabel_ << " label " << label_
                               << " confGeomXMLFiles " << confGeomXMLFiles_;
  if (isMagField_) {
    setWhatProduced(this, &DetectorESSource::produceMagField, edm::es::Label(iConfig.getParameter<std::string>("@module_label")));
    findingRecord<MagneticFieldRecord>();
  } else {
    setWhatProduced(this, &DetectorESSource::produceGeom);
    findingRecord<GeometryRecord>();
  }
}


DetectorESSource::~DetectorESSource() {}

void DetectorESSource::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<bool>("isMagField", false);
  desc.add<edm::FileInPath>("confGeomXMLFiles", edm::FileInPath());
  desc.add<std::string>("label", "");
  descriptions.add("DetectorESSource", desc);
}

void DetectorESSource::setIntervalFor(const edm::eventsetup::EventSetupRecordKey& iKey,
                                          const edm::IOVSyncValue& iTime,
                                          edm::ValidityInterval& oInterval) {
  oInterval = edm::ValidityInterval(edm::IOVSyncValue::beginOfTime(), edm::IOVSyncValue::endOfTime());  //infinite
}

DetectorESSource::ReturnType DetectorESSource::produce() {
  edm::LogVerbatim("Geometry") << "DetectorESSource::produce " << appendToDataLabel_;
  return std::make_unique<c4h::Detector>(appendToDataLabel_, confGeomXMLFiles_);
}

DetectorESSource::ReturnType DetectorESSource::produceGeom(const GeometryRecord&) {
  return produce();
}

DetectorESSource::ReturnType DetectorESSource::produceMagField(const MagneticFieldRecord&) {
  return produce();
}

DEFINE_FWK_EVENTSETUP_SOURCE(DetectorESSource);
