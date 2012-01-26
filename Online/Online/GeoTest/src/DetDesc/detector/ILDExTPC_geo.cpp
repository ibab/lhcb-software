#include "XML/lcdd/XMLLCDD.h"
#include "XML/compact/XMLDetector.h"
#include "XML/XMLTags.h"
#include "DetDesc/compact/Segmentations.h"
#include "DetDesc/compact/Detector.h"
#include "DetDesc/lcdd/Objects.h"
#include "DetDesc/lcdd/Shapes.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/LCDD.h"

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _X(a) DetDesc::XML::Tag_##a
#define _A(a) DetDesc::XML::Attr_##a

// Shortcuts to elements of the XML namespace
typedef DetDesc::XML::Collection_t    xml_coll_t;
typedef DetDesc::XML::Handle_t        xml_h;
typedef DetDesc::XML::Attribute       xml_attr_t;
typedef DetDesc::XML::RefElement      xml_ref_t;
typedef DetDesc::XML::Element         xml_elem_t;
typedef DetDesc::XML::Subdetector::Component       xml_comp_t;
typedef DetDesc::XML::Subdetector     xml_det_t;
typedef DetDesc::XML::Dimension       xml_dim_t;
typedef DetDesc::Geometry::Handle_t   geo_h;
typedef DetDesc::Geometry::LCDD       lcdd_t;
typedef DetDesc::Geometry::RefElement Ref_t;
typedef DetDesc::Geometry::Element    Elt_t;
typedef DetDesc::Geometry::Document   Doc_t;

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;
using XML::Tag_t;

#include "DetDesc/detector/ILDExTPC.h"

namespace DetDesc { namespace Geometry {
  
  template <class T, class Q> Ref_t toRefObject(lcdd_t& lcdd, const Q& e, SensitiveDetector& /* sens */)  {
    return Ref_t(0);
  }

  template <> Ref_t toRefObject<DetDesc::ILDExTPC,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& /* sens */)  {
    Document    doc   = lcdd.document();
    xml_det_t   x_det = e;
    string      name  = x_det.nameStr();
    xml_comp_t  x_tube (x_det.child(_X(tubs)));
    Material    mat    (lcdd.material(x_det.materialStr()));
    ILDExTPC    tpc    (doc,name,x_det.typeStr(),x_det.id());
    Tube        tpc_tub(doc,name+"_envelope",x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
    Volume      tpc_vol(doc,name+"_envelope_volume", tpc_tub, mat);

    lcdd.add(tpc_tub).add(tpc_vol);
    tpc.setEnvelope(tpc_tub).setVolume(tpc_vol);
    lcdd.pickMotherVolume(tpc).addPhysVol(PhysVol(tpc_vol),lcdd.identity());

    for(xml_coll_t c(e,_X(detector)); c; ++c)  {
      xml_det_t   px_det  (c);
      xml_comp_t  px_tube (px_det.child(_X(tubs)));
      xml_dim_t   px_pos  (px_det.child(_X(position)));
      xml_dim_t   px_rot  (px_det.child(_X(rotation)));
      string      part_nam(px_det.nameStr());
      Material    part_mat(lcdd.material(px_det.materialStr()));
      Subdetector part_det(doc,part_nam,px_det.typeStr(),px_det.id());
      Tube        part_tub(doc,part_nam+"_tube",px_tube.rmin(),px_tube.rmax(),px_tube.zhalf());
      Position    part_pos(doc,part_nam+"_position",px_pos.x(),px_pos.y(),px_pos.z());
      Rotation    part_rot(doc,part_nam+"_rotation",px_rot.x(),px_rot.y(),px_rot.z());
      Volume      part_vol(doc,part_nam,part_tub,part_mat);

      lcdd.add(part_tub).add(part_vol).add(part_pos).add(part_rot);

      part_det.setVolume(part_vol).setEnvelope(part_tub);
      part_det.setVisAttributes(lcdd,px_det.visStr(),part_vol);

      //PhysVol     part_physvol (doc,part_vol,part_nam);
      PhysVol     part_physvol (part_vol);
      part_physvol.addPhysVolID(_A(id),px_det.id());
      tpc_vol.addPhysVol(part_physvol,part_pos,part_rot);
      //tpc_vol.addPhysVol(part_vol,part_pos,part_rot);

      switch(part_det.id()) {
      case 0:	tpc.setInnerWall(part_det);  break;
      case 1:	tpc.setOuterWall(part_det);  break;
      case 2:	tpc.setGasVolume(part_det);  break;
      }
      tpc.add(part_det);
    }
    tpc.setVisAttributes(lcdd, x_det.visStr(), tpc_vol);
    //lcdd.pickMotherVolume(tpc).addPhysVol(PhysVol(doc,tpc_vol,name),lcdd.identity());
    //lcdd.pickMotherVolume(tpc).addPhysVol(PhysVol(tpc_vol),lcdd.identity());
    //lcdd.pickMotherVolume(tpc).addPhysVol(tpc_vol,lcdd.identity());
    return tpc;
  }
}}
