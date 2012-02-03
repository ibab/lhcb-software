// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/lcdd/Detector.h"

namespace DetDesc {

  typedef Geometry::RefHandle<TNamed> Ref_t;

  struct TPCData : public Geometry::DetElement::Object {
    Ref_t outerWall;
    Ref_t innerWall;
    Ref_t gas;
    double pressure;
  };

  struct ILDExTPC : public Geometry::DetElement {
    typedef TPCData Object;
    ILDExTPC(const Ref_t& e) : Geometry::DetElement(e) {}
    ILDExTPC(const Geometry::LCDD& lcdd, const std::string& name, const std::string& type, int id);
    void setInnerWall(Ref_t obj);
    void setOuterWall(Ref_t obj);
    void setGasVolume(Ref_t obj);
    double getVolume() const;
    double getWeight() const;
  };
}
