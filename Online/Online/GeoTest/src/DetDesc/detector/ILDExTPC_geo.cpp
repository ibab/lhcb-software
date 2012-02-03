// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "XML/XMLTags.h"
#include "XML/XMLElements.h"
#include "XML/lcdd/XMLDetector.h"
#include "DetDesc/lcdd/Segmentations.h"
#include "DetDesc/lcdd/Detector.h"
#include "DetDesc/lcdd/Objects.h"
#include "DetDesc/lcdd/Shapes.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/LCDD.h"

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _X(a) DetDesc::XML::Tag_##a
#define _A(a) DetDesc::XML::Attr_##a

// Shortcuts to elements of the XML namespace
typedef DetDesc::XML::Collection_t          xml_coll_t;
typedef DetDesc::XML::Handle_t              xml_h;
typedef DetDesc::XML::DetElement::Component xml_comp_t;
typedef DetDesc::XML::DetElement            xml_det_t;
typedef DetDesc::XML::Dimension             xml_dim_t;
typedef DetDesc::Geometry::LCDD             lcdd_t;

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

#include "DetDesc/detector/ILDExTPC.h"

namespace DetDesc { namespace Geometry {
  
  template <typename T, typename Q> Ref_t toRefObject(LCDD& lcdd, const Q& e, SensitiveDetector&)  {
    return Ref_t(0);
  }
  
  template <> Ref_t toRefObject<DetDesc::ILDExTPC,xml_h>(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
    xml_det_t   x_det = e;
    string      name  = x_det.nameStr();
    xml_comp_t  x_tube (x_det.child(_X(tubs)));
    Material    mat    (lcdd.material(x_det.materialStr()));
    ILDExTPC    tpc    (lcdd,name,x_det.typeStr(),x_det.id());
    Tube        tpc_tub(lcdd,name+"_envelope",x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
    Volume      tpc_vol(lcdd,name+"_envelope_volume", tpc_tub, mat);

    tpc.setEnvelope(tpc_tub).setVolume(tpc_vol);
    lcdd.pickMotherVolume(tpc).addPhysVol(PhysVol(tpc_vol),lcdd.identity());

    for(xml_coll_t c(e,_X(detector)); c; ++c)  {
      xml_det_t   px_det  (c);
      xml_comp_t  px_tube (px_det.child(_X(tubs)));
      xml_dim_t   px_pos  (px_det.child(_X(position)));
      xml_dim_t   px_rot  (px_det.child(_X(rotation)));
      string      part_nam(px_det.nameStr());
      Material    part_mat(lcdd.material(px_det.materialStr()));
      DetElement  part_det(lcdd,part_nam,px_det.typeStr(),px_det.id());
      Tube        part_tub(lcdd,part_nam+"_tube",px_tube.rmin(),px_tube.rmax(),px_tube.zhalf());
      Position    part_pos(lcdd,part_nam+"_position",px_pos.x(),px_pos.y(),px_pos.z());
      Rotation    part_rot(lcdd,part_nam+"_rotation",px_rot.x(),px_rot.y(),px_rot.z());
      Volume      part_vol(lcdd,part_nam,part_tub,part_mat);

      part_det.setVolume(part_vol).setEnvelope(part_tub);
      part_det.setVisAttributes(lcdd,px_det.visStr(),part_vol);

      PhysVol     part_physvol (part_vol);
      part_physvol.addPhysVolID(_A(id),px_det.id());
      tpc_vol.addPhysVol(part_physvol,part_pos,part_rot);

      switch(part_det.id()) {
      case 0:	tpc.setInnerWall(part_det);  break;
      case 1:	tpc.setOuterWall(part_det);  break;
      case 2:	tpc.setGasVolume(part_det);  break;
      }
      tpc.add(part_det);
    }
    tpc.setVisAttributes(lcdd, x_det.visStr(), tpc_vol);
    return tpc;
  }
}}
