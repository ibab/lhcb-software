// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/detector/DetFactoryHelper.h"
#include "DetDesc/detector/CompactDetectors.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

namespace DetDesc { namespace Geometry {
  
  template <> Ref_t DetElementFactory<PolyconeSupport>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
    xml_det_t  x_det   = e;
    string     name    = x_det.nameStr();
    DetElement sdet   (lcdd,name,x_det.typeStr(),x_det.id());
    Polycone   cone   (lcdd,name+"_envelope_polycone");
    Material   mat    (lcdd.material(x_det.materialStr()));
    Volume     volume (lcdd,name+"_envelope_volume", cone, mat);

    int num = 0;
    vector<double> rmin,rmax,z;
    double v1,v2,v3;
    for(xml_coll_t c(e,_X(zplane)); c; ++c, ++num)  {
      v1 = c.attr<double>(_A(rmin));
      v2 = c.attr<double>(_A(rmax));
      v3 = c.attr<double>(_A(z));
      rmin.push_back(v1);
      rmax.push_back(v2);
      z.push_back(v3);
    }
    if ( num < 2 )  {
      throw runtime_error("PolyCone["+name+"]> Not enough Z planes. minimum is 2!");
    }
    cone.addZPlanes(rmin,rmax,z);
    sdet.setEnvelope(cone).setVolume(volume);
    sdet.setVisAttributes(lcdd, x_det.visStr(), volume);
    lcdd.pickMotherVolume(sdet).addPhysVol(PhysVol(volume),lcdd.identity());
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,PolyconeSupport);
