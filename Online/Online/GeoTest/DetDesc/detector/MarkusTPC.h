// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/compact/Detector.h"

namespace DetDesc {

  struct TPCData;

  struct MarkusTPC : public Geometry::Subdetector {
    typedef TPCData Object;
    MarkusTPC(const Geometry::RefHandle<TNamed>& e) : Geometry::Subdetector(e) {}
    double feature() const;
  };
}
