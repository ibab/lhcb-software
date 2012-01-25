#include "DetDesc/compact/Detector.h"


namespace DetDesc {
  
struct ILDExTPC : public Geometry::Subdetector {
  Geometry::RefElement outerWall;
  Geometry::RefElement innerWall;
  Geometry::RefElement gas;
  
  ILDExTPC(const Geometry::Document& doc, const std::string& name, const std::string& type, int id)
  : Geometry::Subdetector(doc,name,type,id) {}
  
  double getVolume() const;
  double getWeight() const;
};
  
}