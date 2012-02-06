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

  struct TPCData;

  struct MarkusTPC : public Geometry::DetElement {
    typedef TPCData Object;
    MarkusTPC(const Geometry::Handle<TNamed>& e) : Geometry::DetElement(e) {}
    double feature() const;
  };
}
