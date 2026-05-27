#include "Code4hep/Geometry/interface/Detector.h"
#include <DD4hep/DetectorTools.h>
#include <DD4hep/Printout.h>

namespace c4h {
  Detector::Detector(const std::string& tag, const std::string& fileName) : tag_(tag) {
    //We do not want to use any previously created TGeoManager but we do want to reset after we are done.
    auto oldGeoManager = gGeoManager;
    gGeoManager = nullptr;
    auto resetManager = [oldGeoManager](TGeoManager*) { gGeoManager = oldGeoManager; };
    std::unique_ptr<TGeoManager, decltype(resetManager)> sentry(oldGeoManager, resetManager);

    // Set DD4hep message level to ERROR. The default is INFO,
    // but those messages are not necessary for general use.
    dd4hep::setPrintLevel(dd4hep::ERROR);

    geometry_ = &dd4hep::Detector::getInstance(tag_);
    geometry_->setStdConditions("NTP");
    process(fileName);
  }

  void Detector::process(const std::string& fileName) {
    std::string name("DD4hep_CompactLoader");
    const char* files[] = {fileName.c_str(), nullptr};
    geometry_->apply(name.c_str(), 2, (char**)files);
  }

  TGeoManager& Detector::manager() const {
    assert(geometry_);
    return geometry_->manager();
  }

  dd4hep::DetElement Detector::findElement(const std::string& path) const {
    assert(geometry_);
    return dd4hep::detail::tools::findElement(*geometry_, path);
  }
}  // namespace c4h

#include "FWCore/Utilities/interface/typelookup.h"

TYPELOOKUP_DATA_REG(c4h::Detector);
