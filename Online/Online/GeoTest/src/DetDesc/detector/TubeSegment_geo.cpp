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
  
  template <> Ref_t DetElementFactory<TubeSegment>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
    xml_det_t  x_det  (e);
    xml_comp_t x_tube (x_det.child(_X(tubs)));
    xml_dim_t  x_pos  (x_det.child(_X(position)));
    xml_dim_t  x_rot  (x_det.child(_X(rotation)));
    string     name   = x_det.attr<string>(_A(name));
    Material   mat    = lcdd.material(x_det.materialStr());
    Volume     mother = x_det.isInsideTrackingVolume() ? lcdd.trackingVolume() : lcdd.worldVolume();
    DetElement sdet   (lcdd,name,x_det.typeStr(),x_det.id());
    Tube       tub    (lcdd,name+"_tube",x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
    Position   pos    (lcdd,name+"_position",x_pos.x(),x_pos.y(),x_pos.z());
    Rotation   rot    (lcdd,name+"_rotation",x_rot.x(),x_rot.y(),x_rot.z());
    Volume     vol    (lcdd,name,tub,mat);

    sdet.setVolume(vol).setEnvelope(tub);
    sdet.setVisAttributes(lcdd, x_det.visStr(), vol);
    //sdet.setVisAttributes(lcdd, "GreenVis", vol);

    PhysVol physvol(vol);
    physvol.addPhysVolID(_A(id),x_det.id());
    mother.addPhysVol(physvol,pos,rot);
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,TubeSegment);
