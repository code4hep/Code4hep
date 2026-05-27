#ifndef Code4hep_Geometry_Detector_h
#define Code4hep_Geometry_Detector_h

#include <DD4hep/Detector.h>
#include <DD4hep/SpecParRegistry.h>
#include <string>

class TGeoManager;

namespace c4h {
  class Detector {
  public:
    explicit Detector(const std::string& tag, const std::string& fileName);
    Detector() = delete;

    //accessors
    TGeoManager& manager() const;
    dd4hep::DetElement findElement(const std::string& el) const;
    dd4hep::Detector const* geometry() const { return geometry_; }

  private:
    void process(const std::string&);

    dd4hep::Detector* geometry_ = nullptr;
    const std::string tag_;
  };
}  // namespace c4h

#endif
