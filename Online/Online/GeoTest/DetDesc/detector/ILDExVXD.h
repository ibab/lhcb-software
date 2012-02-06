// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DETDESC_ILDEXVXD_H
#define DETDESC_ILDEXVXD_H

#include "DetDesc/lcdd/Detector.h"

namespace DetDesc {
  struct ILDExVXD : public Geometry::DetElement {
    ILDExVXD(const Geometry::LCDD& lcdd, const std::string& name, const std::string& type, int id)
      : Geometry::DetElement(lcdd,name,type,id) {}
  };
}
#endif // DETDESC_ILDEXVXD_H
