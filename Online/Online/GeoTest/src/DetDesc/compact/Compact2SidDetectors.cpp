#include "XML/lcdd/XMLLCDD.h"
#include "XML/compact/XMLDetector.h"
#include "XML/XMLTags.h"
#include "DetDesc/compact/Conversions.h"
#include "DetDesc/compact/Segmentations.h"
#include "DetDesc/compact/Detector.h"
#include "DetDesc/lcdd/Objects.h"
#include "DetDesc/lcdd/Shapes.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/LCDD.h"
#include "DetDesc/IDDescriptor.h"
#include "DetDesc/Layering.h"
#include <climits>
#include <iostream>
#include <set>

#define SHOW_ALL_DETECTORS 1

// To be seen where this goes....
namespace DetDesc { namespace Geometry {
  struct SensitiveDetector;
  struct PolyconeSupport;
  struct TubeSegment;
  struct MultiLayerTracker;
  struct DiskTracker;
  struct SiTrackerBarrel;
  struct SiTrackerEndcap;
  struct SiTrackerEndcap2;
  struct CylindricalBarrelCalorimeter;
  struct CylindricalEndcapCalorimeter;
  struct EcalBarrel;
  struct PolyhedraEndcapCalorimeter2 : public Subdetector {
  };
  struct PolyhedraBarrelCalorimeter2 : public Subdetector {
    /// Constructor for a new subdetector element
    PolyhedraBarrelCalorimeter2(const Document& doc, const std::string& name, const std::string& type, int id)
      : Subdetector(doc,name,type,id) {}
    void placeStaves(LCDD& lcdd, 
      const std::string& detName, 
      double rmin, 
      int numsides, 
      double total_thickness, 
      Volume envelopeVolume, 
      double innerAngle, 
      Volume sectVolume);

  };
  struct ForwardDetector;

  std::string _toString(const XMLCh* value)  
  {  return XML::_toString(value); }
  template <class T> std::string _toString(T value, const char* fmt)
  {  return XML::_toString(value, fmt); }
}}

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
typedef DetDesc::Geometry::Document   Doc_t;

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;
using XML::Tag_t;

template <> Ref_t Geometry::toObject<Geometry::PolyconeSupport,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  Document    doc     = lcdd.document();
  xml_det_t   x_det   = e;
  string      name    = x_det.nameStr();
  Subdetector sdet    (doc,name,x_det.typeStr(),x_det.id());
  Polycone    cone    (doc,name+"_envelope_polycone");
  Material    mat     (lcdd.material(x_det.materialStr()));
  Volume      volume  (doc,name+"_envelope_volume", cone, mat);

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
  lcdd.add(cone).add(volume);
  sdet.setEnvelope(cone).setVolume(volume);
  sdet.setVisAttributes(lcdd, x_det.visStr(), volume);
  lcdd.pickMotherVolume(sdet).addPhysVol(PhysVol(doc,volume,name),lcdd.identity());
  return sdet;
}

template <> Ref_t Geometry::toObject<Geometry::TubeSegment,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det   (e);
  xml_comp_t  x_tube  (x_det.child(_X(tubs)));
  xml_dim_t   x_pos   (x_det.child(_X(position)));
  xml_dim_t   x_rot   (x_det.child(_X(rotation)));
  string      name    = x_det.attr<string>(_A(name));
  Material    mat     = lcdd.material(x_det.materialStr());
  Volume      mother  = x_det.isInsideTrackingVolume() ? lcdd.trackingVolume() : lcdd.worldVolume();
  Document    doc     (lcdd.document());
  Subdetector sdet    (doc,name,x_det.typeStr(),x_det.id());
  Tube        tube    (doc,name+"_tube",x_tube.rmin(),x_tube.rmax(),x_tube.zhalf());
  Position    pos     (doc,name+"_position",x_pos.x(),x_pos.y(),x_pos.z());
  Rotation    rot     (doc,name+"_rotation",x_rot.x(),x_rot.y(),x_rot.z());
  Volume      vol     (doc,name,tube,mat);

  sdet.setVolume(vol).setEnvelope(tube);
  lcdd.add(tube).add(pos).add(rot).add(vol);
  sdet.setVisAttributes(lcdd, x_det.visStr(), vol);

  PhysVol     physvol (doc,vol,name);
  physvol.addPhysVolID(_A(id),x_det.id());
  mother.addPhysVol(physvol,pos,rot);
  return sdet;
}

template <> Ref_t Geometry::toObject<Geometry::MultiLayerTracker,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det    = e;
  Document    doc      = lcdd.document();
  string      det_name = x_det.nameStr();
  string      det_type = x_det.typeStr();
  Material    air      = lcdd.material(_X(Air));
  Subdetector sdet       (doc,det_name,det_type,x_det.id());
  Volume      motherVol= lcdd.pickMotherVolume(sdet);
  int n = 0;

  for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++n)  {
    xml_comp_t x_layer = i;
    string  layer_name = det_name+_toString(n,"_layer%d");
    Subdetector layer(doc,layer_name,"MultiLayerTracker/Layer",x_layer.id());
    Tube    layer_tub(doc,layer_name);
    Volume  layer_vol(doc,layer_name+"_volume",layer_tub,air);
    double  z    = x_layer.outer_z();
    double  rmin = x_layer.inner_r();
    double  r    = rmin;
    int m = 0;

    layer.setVolume(layer_vol).setEnvelope(layer_tub);
    for(xml_coll_t j(x_layer,_X(slice)); j; ++j, ++m)  {
      xml_comp_t x_slice = j;
      Material mat = lcdd.material(_toString(x_slice.material()));
      string slice_name = layer_name+_toString(m,"_slice%d");
      // Slices have no extra id. Take the ID of the layer!
      Subdetector slice(doc, slice_name,det_type+"/Layer/Slice",layer.id());
      Tube   slice_tub (doc,slice_name);
      Volume slice_vol (doc,slice_name+"_volume", slice_tub, mat);

      r += x_slice.thickness();
      slice_tub.setDimensions(r,r,2.*z,2.*M_PI);
      slice.setVolume(slice_vol).setEnvelope(slice_tub);
      lcdd.add(slice_tub).add(slice_vol);
      slice_vol.setSolid(slice_tub);
      if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
      // Set region of slice
      slice.setRegion(lcdd,x_slice.attr<string>(_A(region)), slice_vol);
      // Set limits of slice
      slice.setLimitSet(lcdd,x_slice.attr<string>(_A(limits)),slice_vol);
      // Set vis attributes of slice
      slice.setVisAttributes(lcdd, x_slice.visStr(), slice_vol);

      PhysVol spv(doc,slice_vol,slice_name);
      spv.addPhysVolID(_X(layer),n);
      layer_vol.addPhysVol(spv,lcdd.identity());
      layer.add(slice);
    }
    layer_tub.setDimensions(rmin,r,2.*z,2.*M_PI);
    sdet.setVisAttributes(lcdd, x_layer.visStr(), layer_vol);

    PhysVol lpv(doc,layer_vol,layer_name);
    lpv.addPhysVolID(_X(system),sdet.id())
       .addPhysVolID(_X(barrel),0);
    motherVol.addPhysVol(lpv,lcdd.identity());
    lcdd.add(layer_tub);
    lcdd.add(layer_vol);
    sdet.add(layer);
  }
  sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
  return sdet;
}

template <> Ref_t Geometry::toObject<Geometry::DiskTracker,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det     = e;
  Document    doc       = lcdd.document();
  Material    air       = lcdd.material(_X(Air));
  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  bool        reflect   = x_det.attr<bool>(_A(reflect));
  Rotation    refl_rot  = lcdd.rotation(_X(reflect_rot));
  Subdetector sdet        (doc,det_name,det_type,x_det.id());
  Volume      motherVol = lcdd.pickMotherVolume(sdet);
  int n = 0;

  for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++n)  {
    xml_comp_t x_layer = i;
    string  layer_name = det_name+_toString(n,"_layer%d");
    Tube    layer_tub(doc,layer_name);
    Volume  layer_vol(doc,layer_name+"_volume",layer_tub,air);
    Subdetector layer(doc,layer_name,det_type+"/Layer",x_layer.id());
    double  zmin = x_layer.inner_z();
    double  rmin = x_layer.inner_r();
    double  rmax = x_layer.outer_r();
    double  z = zmin, layerWidth = 0.;
    int m = 0;

    layer.setVolume(layer_vol).setEnvelope(layer_tub);
    for(xml_coll_t j(x_layer,_X(slice)); j; ++j)  {
      double w = xml_comp_t(j).thickness();
      layerWidth += w;
    }
    for(xml_coll_t j(x_layer,_X(slice)); j; ++j, ++m)  {
      xml_comp_t x_slice = j;
      double     w = x_slice.thickness();
      Material mat = lcdd.material(x_slice.materialStr());
      string slice_name = layer_name+_toString(m,"_slice%d");
      Tube   slice_tub(doc,slice_name);
      Volume slice_vol(doc,slice_name+"_volume", slice_tub, mat);
      // Slices have no extra id. Take the ID of the layer!
      Subdetector slice(doc,slice_name,det_type+"/Layer/Slice",layer.id());

      slice.setVolume(slice_vol).setEnvelope(slice_tub);
      slice_tub.setDimensions(rmin,rmax,w,2.*M_PI);
      lcdd.add(slice_tub).add(slice_vol);

      if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
      // Set region of slice
      slice.setRegion(lcdd, x_slice.attr<string>(_A(region)), slice_vol);
      // set limits of slice
      slice.setLimitSet(lcdd, x_slice.attr<string>(_A(limits)), slice_vol);
      // Set vis attributes of slice
      slice.setVisAttributes(lcdd, x_slice.visStr(), slice_vol);

      PhysVol spv(doc,slice_vol,slice_name);
      spv.addPhysVolID(_X(layer),n);
      layer_vol.addPhysVol(spv,Position(doc,slice_name+"_pos",0.,0.,z-zmin-layerWidth/2. + w/2.));
      layer.add(slice);
    }
    layer.setVisAttributes(lcdd, x_layer.visStr(), layer_vol);
    layer_tub.setDimensions(rmin,rmax,2.*z,2.*M_PI);
    PhysVol lpv(doc,layer_vol,layer_name);
    lpv.addPhysVolID(_X(system),sdet.id());
    lpv.addPhysVolID(_X(barrel),1);
    motherVol.addPhysVol(lpv,Position(doc,layer_name+"_pos",0,0,zmin+layerWidth/2.));
    if ( reflect )  {
      PhysVol lpvR(doc,layer_vol,layer_name+"_reflect");
      lpvR.addPhysVolID(_X(system),sdet.id());
      lpvR.addPhysVolID(_X(barrel),2);
      motherVol.addPhysVol(lpvR,Position(doc,layer_name+"_pos_reflect",0.,0.,-zmin-layerWidth/2.),refl_rot);
    }
    lcdd.add(layer_tub);
    lcdd.add(layer_vol);
    sdet.add(layer);
  }
  sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
  return sdet;
}

template <> Ref_t Geometry::toObject<Geometry::CylindricalBarrelCalorimeter,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
#if defined(SHOW_ALL_DETECTORS)
  xml_det_t   x_det     = e;
  xml_dim_t   dim       = x_det.dimensions();
  Document    doc       = lcdd.document();
  Material    air       = lcdd.material(_X(Air));
  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  Tube        envelope   (doc,det_name+"_envelope");
  Volume      envelopeVol(doc,det_name+"_envelope_volume",envelope,air);
  Subdetector sdet       (doc,det_name,det_type,x_det.id());
  double      z    = dim.outer_z();
  double      rmin = dim.inner_r();
  double      r    = rmin;
  int         n    = 0;

  envelope.setDimensions(rmin,r,2.*z);
  sdet.setVolume(envelopeVol).setEnvelope(envelope);
  for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
      string layer_name = det_name + _toString(n,"_layer%d");
      Tube   layer_tub(doc,layer_name);
      Volume layer_vol(doc,layer_name+"_volume",layer_tub,air);
      Subdetector layer(doc,layer_name,"CylindricalBarrelCalorimeter/Layer",sdet.id());
      double rlayer = r;

      layer.setVolume(layer_vol).setEnvelope(layer_tub);
      for(xml_coll_t l(x_layer,_X(slice)); l; ++l, ++m)  {
        xml_comp_t x_slice = l;
        Material    slice_mat  = lcdd.material(x_slice.materialStr());
        string      slice_name = layer_name + _toString(m,"slice%d");
        Tube        slice_tube(doc,slice_name);
        Volume      slice_vol (doc,slice_name+"_volume",slice_tube,slice_mat);
        Subdetector slice(doc,slice_name,"CylindricalBarrelCalorimeter/Layer/Slice",sdet.id());
        double      router = r + x_slice.thickness();

        slice.setVolume(slice_vol).setEnvelope(slice_tube);
        if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
        slice_tube.setDimensions(r,router,z * 2);
        r = router;
        lcdd.add(slice_tube).add(slice_vol);
        // Set region of slice
        slice.setRegion(lcdd,x_slice.attr<string>(_A(region)),slice_vol);
        // set limits of slice
        slice.setLimitSet(lcdd,x_slice.attr<string>(_A(limits)),slice_vol);
        // Set vis attributes of slice
        slice.setVisAttributes(lcdd,x_slice.visStr(),slice_vol);
        // Instantiate physical volume
        layer_vol.addPhysVol(PhysVol(doc,slice_vol,slice_name),lcdd.identity());
        layer.add(slice);
      }
      layer.setVisAttributes(lcdd,x_layer.visStr(),layer_vol);
      layer_tub.setDimensions(rlayer,r,z * 2);

      PhysVol layer_physvol(doc,layer_vol,layer_name);
      layer_physvol.addPhysVolID(_A(layer),n);
      envelopeVol.addPhysVol(layer_physvol,lcdd.identity());
      lcdd.add(layer_vol).add(layer_tub);
      sdet.add(layer);
      ++n;
    }
  }
  lcdd.add(envelope).add(envelopeVol);
  // Set region of slice
  sdet.setRegion(lcdd,x_det.attr<string>(_A(region)),envelopeVol);
  // set limits of slice
  sdet.setLimitSet(lcdd,x_det.attr<string>(_A(limits)),envelopeVol);
  // Set vis attributes of slice
  sdet.setVisAttributes(lcdd,x_det.visStr(),envelopeVol);

  PhysVol physvol(doc,envelopeVol,det_name);
  physvol.addPhysVolID(_A(system),sdet.id())
         .addPhysVolID(_A(barrel),0);
  envelopeVol.addPhysVol(physvol,lcdd.identity());
  lcdd.pickMotherVolume(sdet).addPhysVol(physvol,lcdd.identity());
  return sdet;
#else
  return Ref_t(0);
#endif
}

template <> Ref_t Geometry::toObject<Geometry::CylindricalEndcapCalorimeter,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
#if defined(SHOW_ALL_DETECTORS)
  xml_det_t   x_det     = e;
  xml_dim_t   dim       = x_det.dimensions();
  Document    doc       = lcdd.document();
  Material    air       = lcdd.material(_X(Air));
  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  Tube        envelope   (doc,det_name+"_envelope");
  Volume      envelopeVol(doc,det_name+"_envelope_volume",envelope,air);
  Subdetector sdet       (doc,det_name,det_type,x_det.id());
  Volume      motherVol = lcdd.pickMotherVolume(sdet);
  bool        reflect   = dim.reflect();
  double      zmin      = dim.inner_z();
  double      rmin      = dim.inner_r();
  double      rmax      = dim.outer_r();
  double      totWidth  = Layering(x_det).totalThickness();
  double      z    = zmin;
  int         n    = 0;

  for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    double layerWidth = 0;
    for(xml_coll_t l(x_layer,_X(slice)); l; ++l)
      layerWidth += xml_comp_t(l).thickness();
    for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
      double zlayer = z;
      string  layer_name = det_name + _toString(n,"_layer%d");
      Tube    layer_tub(doc,layer_name);
      Volume  layer_vol(doc,layer_name+"_volume",layer_tub,air);
      Subdetector layer(doc,layer_name,"CylindricalEndcapCalorimeter/Layer",sdet.id());

      layer.setVolume(layer_vol).setEnvelope(layer_tub);
      for(xml_coll_t l(x_layer,_X(slice)); l; ++l, ++m)  {
        xml_comp_t  x_slice = l;
        double      w = x_slice.thickness();
        string      slice_name = layer_name + _toString(m,"slice%d");
        Material    slice_mat  = lcdd.material(x_slice.materialStr());
        Tube        slice_tube(doc,slice_name);
        Volume      slice_vol (doc,slice_name+"_volume", slice_tube, slice_mat);
        Subdetector slice(doc,slice_name,"CylindricalEndcapCalorimeter/Layer/Slice",sdet.id());

        slice.setVolume(slice_vol).setEnvelope(slice_tube);
        if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
        slice_tube.setDimensions(rmin,rmax,w);

        lcdd.add(slice_tube).add(slice_vol);
        // Set attributes of slice
        slice.setAttributes(lcdd, slice_vol, 
          x_slice.attr<string>(_A(region)), 
          x_slice.attr<string>(_A(limits)),
          x_slice.visStr());

        PhysVol  slice_physvol(doc,slice_vol,slice_name);
        Position slice_pos(doc,slice_name+"_pos",0.,0.,z-zlayer-layerWidth/2.+w/2.);
        layer_vol.addPhysVol(slice_physvol,slice_pos);
        z += w;
        layer.add(slice);
      }
      layer.setVisAttributes(lcdd,x_layer.visStr(),layer_vol);
      layer_tub.setDimensions(rmin,rmax,layerWidth);

      PhysVol  layer_phys(doc,layer_vol,layer_name);
      Position layer_pos(doc,layer_name+"_pos",0.,0.,zlayer-zmin-totWidth/2.+layerWidth/2.);
      layer_phys.addPhysVolID("layer",n);
      envelopeVol.addPhysVol(layer_phys,layer_pos);
      sdet.add(layer);
      lcdd.add(layer_vol).add(layer_tub);
      ++n;
    }
  }
  envelope.setDimensions(rmin,rmax,totWidth,2.*M_PI);
  lcdd.add(envelope).add(envelopeVol);
  // Set attributes of slice
  sdet.setAttributes(lcdd, envelopeVol,
                     x_det.attr<string>(_A(region)),
                     x_det.attr<string>(_A(limits)),
                     x_det.visStr());

  PhysVol  det_physvol(doc,envelopeVol,det_name);
  Position det_pos(doc,det_name+"_pos",0.,0.,zmin+totWidth/2.);
  det_physvol.addPhysVolID(_A(system),sdet.id());
  det_physvol.addPhysVolID(_A(barrel),1);
  motherVol.addPhysVol(det_physvol,det_pos);

  if ( reflect )   {
    Rotation rot = lcdd.rotation("reflect_rot");
    Position r_pos(doc,det_name+"_pos",0.,0.,-zmin-totWidth/2.);
    PhysVol  r_det_physvol(doc,envelopeVol,det_name+"_reflect");
    r_det_physvol.addPhysVolID(_A(system),sdet.id());
    r_det_physvol.addPhysVolID(_A(barrel),2);
    motherVol.addPhysVol(r_det_physvol,r_pos,rot);
  }
  return sdet;

#else
  return Ref_t(0);
#endif
}

template <> Ref_t Geometry::toObject<Geometry::EcalBarrel,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  return Ref_t(0);
}
template <> Ref_t Geometry::toObject<Geometry::PolyhedraEndcapCalorimeter2,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  return Ref_t(0);
}
#include "TGeoVolume.h"
template <> Ref_t Geometry::toObject<Geometry::PolyhedraBarrelCalorimeter2,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
#if defined(SHOW_ALL_DETECTORS)
  xml_det_t   x_det     = e;
  xml_comp_t  staves    = x_det.child(_X(staves));
  xml_dim_t   dim       = x_det.dimensions();

  Document    doc       = lcdd.document();
  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  Material    air       = lcdd.material(_X(Air));
  Material    gapMat    = air;
  Layering    layering(x_det);
  double      totalThickness = layering.totalThickness();
  int         totalRepeat = 0;
  int         totalSlices = 0;
  double      gap         = xml_dim_t(x_det).gap();
  int         numSides    = dim.numsides();
  double      detZ        = dim.z();
  double      rmin        = dim.rmin();
  double      r           = rmin;
  double      zrot        = M_PI/numSides;
  Rotation    rot(doc,det_name+"_rotation",0,0,zrot);
  Position    pos_identity = lcdd.position("identity_pos");
  PolyhedraBarrelCalorimeter2 sdet(doc,det_name,det_type,x_det.id());
  Volume      motherVol = lcdd.pickMotherVolume(sdet);

  lcdd.add(rot);
  for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    int repeat = x_layer.repeat();
    totalRepeat += repeat;
    totalSlices += x_layer.numChildren(_X(slice));
  }

  PolyhedraRegular polyhedra(doc,det_name+"_polyhedra",numSides,rmin,rmin+totalThickness,detZ);
  Volume           envelopeVol(doc,det_name+"_envelope",polyhedra,air);

  // Add the subdetector envelope to the structure.
  lcdd.add(polyhedra).add(envelopeVol);

  double halfInnerAngle = M_PI/numSides;
  double innerAngle     = 2e0*halfInnerAngle;
  double tan_inner      = tan(halfInnerAngle) * 2e0;
  double rmax           = rmin + totalThickness;
  double innerFaceLen   = rmin * tan_inner;
  double outerFaceLen   = rmax * tan_inner;
  double staveThickness = totalThickness;

  Trapezoid staveTrdOuter(doc,det_name+"_stave_trapezoid_outer",
    innerFaceLen/2e0,outerFaceLen/2e0,detZ/2e0,detZ/2e0,staveThickness/2e0);
  Volume staveOuterVol(doc,det_name+"_stave_outer",staveTrdOuter,air);

  Trapezoid staveTrdInner(doc,det_name+"_stave_trapezoid_inner",
    innerFaceLen/2e0-gap,outerFaceLen/2e0-gap,detZ/2e0,detZ/2e0,staveThickness/2e0);
  Volume staveInnerVol(doc,det_name+"_stave_inner",staveTrdInner,air);

  // Add the stave inner and outer volume/shape to the structure.
  lcdd.add(staveTrdInner).add(staveInnerVol)
      .add(staveTrdOuter).add(staveOuterVol);

  double layerOuterAngle = (M_PI-innerAngle)/2e0;
  double layerInnerAngle = (M_PI/2e0 - layerOuterAngle);
  double layer_position_z = -(staveThickness / 2);                        
  double layer_dim_x = innerFaceLen - gap * 2;
  int layer_number = 0;

  for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
    xml_comp_t   x_layer_element = c;
    int          repeat = x_layer_element.repeat();     // Get number of times to repeat this layer.
    const Layer* lay    = layering.layer(layer_number); // Get the layer from the layering engine.

    // Loop over repeats for this layer.
    for (int j = 0; j < repeat; j++)    {                
      string layer_name = det_name+_toString(layer_number,"_stave_layer%d");
      double layer_thickness = lay->thickness();
      Subdetector  layer(doc,layer_name,det_name+"/Layer",x_det.id());

      // Layer position in Z within the stave.
      layer_position_z += layer_thickness / 2;
      // Position of layer.
      Position layer_position(doc,layer_name+"_position", 0, 0, layer_position_z);
      // Layer box.
      Box layer_box(doc,layer_name+"_box", layer_dim_x, detZ, layer_thickness);
      // Layer volume. 
      Volume layer_volume(doc,layer_name,layer_box,air);

      lcdd.add(layer_position).add(layer_box).add(layer_volume);

      // Create the slices (sublayers) within the layer.
      double slice_position_z = -(layer_thickness / 2);
      int slice_number = 0;
      for(xml_coll_t k(x_layer_element,_X(slice)); k; ++k)  {
        xml_comp_t x_slice = k;
        string   slice_name      = layer_name + _toString(slice_number,"_slice%d");
        double   slice_thickness = x_slice.thickness();
        Material slice_material  = lcdd.material(x_slice.materialStr());
        Subdetector  slice(doc,slice_name,det_name+"/Layer/Slice",x_det.id());

        slice_position_z += slice_thickness / 2;
        // Slice Position.
        Position slice_position(doc,slice_name+"_position",0,0,slice_position_z);
        // Slice box. 
        Box slice_box(doc,slice_name + "_box",layer_dim_x,detZ,slice_thickness);

        lcdd.add(slice_position).add(slice_box);

        // Slice volume.
        Volume slice_volume(doc,slice_name,slice_box,slice_material);
        if ( x_slice.isSensitive() ) slice_volume.setSensitiveDetector(sens);
        lcdd.add(slice_volume);
        // Set region, limitset, and vis.
        slice.setAttributes(lcdd, slice_volume,
          x_slice.attr<string>(_A(region)),x_slice.attr<string>(_A(limits)),x_slice.visStr());
        // slice PhysVol
        PhysVol slice_physvol(doc,slice_volume);
        slice_physvol.addPhysVolID(_X(slice),slice_number);
        layer_volume.addPhysVol(slice_physvol,slice_position);

        layer.add(slice);
        // Increment Z position for next slice.
        slice_position_z += slice_thickness / 2;
        // Increment slice number.
        ++slice_number;             
      }

      // Set region, limitset, and vis.
      layer.setAttributes(lcdd, layer_volume,
        x_layer_element.attr<string>(_A(region)),
        x_layer_element.attr<string>(_A(limits)),
        x_layer_element.visStr());

      // Layer physical volume.
      PhysVol layer_physvol(doc,layer_volume);
      layer_physvol.addPhysVolID(_X(layer), layer_number);
      staveInnerVol.addPhysVol(layer_physvol,layer_position);

      sdet.add(layer);

      // Increment the layer X dimension.
      layer_dim_x += layer_thickness * tan(layerInnerAngle) * 2;
      // Increment the layer Z position.
      layer_position_z += layer_thickness / 2;
      // Increment the layer number.
      ++layer_number;         
    }
  }

  // Make stave inner physical volume.
  PhysVol staveInnerPhysVol(doc,staveInnerVol);
  // Add stave inner physical volume to outer stave volume.
  staveOuterVol.addPhysVol(staveInnerPhysVol,pos_identity);

  // Set envelope volume attributes.
  sdet.setAttributes(lcdd, envelopeVol,
    x_det.attr<string>(_A(region)),
    x_det.attr<string>(_A(limits)),
    x_det.visStr());

  // Make stave inner physical volume.
  PhysVol staveOuterPhysVol(doc,staveOuterVol,det_name+"_staves");
  envelopeVol.addPhysVol(staveOuterPhysVol,pos_identity);

  // Place the staves.
  sdet.placeStaves(lcdd,det_name,rmin,numSides,totalThickness,envelopeVol,innerAngle,staveOuterVol);

  // Set the vis attributes of the outer stave section.
  sdet.setVisAttributes(lcdd,staves.visStr(),staveOuterVol);

  PhysVol envelopePhysVol(doc,envelopeVol);
  envelopePhysVol.addPhysVolID(_X(system), sdet.id());
  envelopePhysVol.addPhysVolID(_X(barrel), 0);
  motherVol.addPhysVol(envelopePhysVol,pos_identity);

  cout << "+++ Staves inner:" << endl;
  staveInnerVol.volume()->PrintNodes();

  cout << "+++ Staves outer:" << endl;
  staveOuterVol.volume()->PrintNodes();

  cout << "+++ Envelope:" << endl;
  envelopeVol.volume()->PrintNodes();

  return sdet;

#else
  return Ref_t(0);
#endif
}

void PolyhedraBarrelCalorimeter2::placeStaves(LCDD& lcdd, 
                                              const string& detName, 
                                              double rmin, 
                                              int numsides, 
                                              double total_thickness, 
                                              Volume envelopeVolume, 
                                              double innerAngle, 
                                              Volume sectVolume)
{
  Document doc = lcdd.document();
  double innerRotation = innerAngle;
  double offsetRotation= -innerRotation / 2e0;
  double sectCenterRadius = rmin + total_thickness / 2e0;
  double rotY = -offsetRotation;
  double rotX = M_PI / 2e0;
  double posX = -sectCenterRadius * sin(rotY);
  double posY = sectCenterRadius * cos(rotY);
  string nam = sectVolume.name();
  for (int module = 0; module < numsides; ++module)  {
    Position position(doc, detName + _toString(module,"_stave0_module%d_position"),posX,posY,0);
    Rotation rotation(doc, detName + _toString(module,"_stave0_module%d_rotation"),rotX,rotY,0);
    lcdd.add(position).add(rotation);

    PhysVol sectPhysVol(doc,sectVolume,nam+_toString(module,"_module%d"));
    envelopeVolume.addPhysVol(sectPhysVol,position,rotation);
    sectPhysVol.addPhysVolID(_X(stave), 0);
    sectPhysVol.addPhysVolID(_X(module), module);

    rotY -= innerRotation;
    posX  = -sectCenterRadius * sin(rotY);
    posY  = sectCenterRadius * cos(rotY);            
  }
}

template <> Ref_t Geometry::toObject<Geometry::ForwardDetector,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
#if defined(SHOW_ALL_DETECTORS)
  xml_det_t   x_det      = e;
  xml_dim_t   dim        = x_det.dimensions();
  bool        reflect    = xml_comp_t(x_det).reflect();
  xml_comp_t  beam       = x_det.child(XML::Tag_beampipe);
  string      det_name   = x_det.nameStr();
  string      det_type   = x_det.typeStr();
  int         id         = x_det.id();
  Layering    layering(x_det);

  Document    doc        = lcdd.document();
  Material    air        = lcdd.material(_X(Air));
  Subdetector sdet       (doc,det_name,det_type,x_det.id());

  Volume      motherVol  = lcdd.pickMotherVolume(sdet);

  double      rmax       = dim.outer_r();
  double      rmin       = dim.inner_r();
  double      zinner     = dim.inner_z();
  double      outgoingR  = beam.outgoing_r();
  double      incomingR  = beam.incoming_r();
  double      xangle     = beam.crossing_angle();
  double      xangleHalf = xangle / 2;
  double      thickness  = layering.totalThickness();
  double      zpos       = zinner + (thickness / 2);
  double      zouter     = zinner + thickness;
  // Beampipe position in envelope.
  double      beamPosX   = std::tan(xangleHalf) * zpos;

  // Detector envelope solid. 
  Tube envelopeTube(doc,det_name+"_tube",rmin,rmax,thickness);

  // First envelope bool subtracion of outgoing beampipe.
  // Incoming beampipe solid.
  Tube beamInTube(doc,det_name + "_beampipe_incoming_tube",0,outgoingR,thickness * 2);
  // Position of incoming beampipe.
  Position beamInPos(doc,det_name + "_subtraction1_tube_pos",beamPosX,0,0);
  /// Rotation of incoming beampipe.
  Rotation beamInRot(doc,det_name + "_subtraction1_tube_rot",0,xangleHalf,0);

  // Second envelope bool subtracion of outgoing beampipe.
  // Outgoing beampipe solid.
  Tube     beamOutTube(doc,det_name + "_beampipe_outgoing_tube",0,incomingR,thickness * 2);
  // Position of outgoing beampipe.
  Position beamOutPos(doc,det_name + "_subtraction2_tube_pos",-beamPosX,0,0);
  // Rotation of outgoing beampipe.
  Rotation beamOutRot(doc,det_name + "_subtraction2_tube_rot",0,-xangleHalf,0);

  lcdd.add(envelopeTube);
  lcdd.add(beamInTube).add(beamInPos).add(beamInRot);
  lcdd.add(beamOutTube).add(beamOutPos).add(beamOutRot);

  // First envelope bool subtraction of incoming beampipe.

  //SubtractionSolid envelopeSubtraction2(doc,det_name+"_subtraction1_tube",envelopeTube,beamInTube);
  //SubtractionSolid envelopeSubtraction2(doc,det_name+"_subtraction1_tube",
  //  string(envelopeTube.name())+"-("+beamInTube.name()+"+"+beamOutTube.name()+")");
  SubtractionSolid envelopeSubtraction1(doc,det_name+"_subtraction1_tube",
    envelopeTube,beamInTube,beamInPos,beamInRot);
  lcdd.add(envelopeSubtraction1);

  SubtractionSolid envelopeSubtraction2(doc,det_name+"_subtraction2_tube",
    envelopeSubtraction1,beamOutTube,beamInPos,beamOutRot);
  lcdd.add(envelopeSubtraction2);


  // Final envelope bool volume.
  Volume envelopeVol(doc,det_name + "_envelope_volume", envelopeSubtraction2, air);

  // Process each layer element.
  double layerPosZ   = -thickness / 2;
  double layerDisplZ = 0;
  for(xml_coll_t c(x_det,XML::Tag_layer); c; ++c)  {
    xml_comp_t x_layer = c;
    double layerThickness = layering.singleLayerThickness(x_layer);

    // Create tube envelope for this layer, which can be reused in bool definition
    // in the repeat loop below.
    Tube layerTube(doc, det_name + "_layer_tube",rmin,rmax,layerThickness);
    lcdd.add(layerTube);

    for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
      string layer_nam = det_name + _toString(i,"_layer%d");
      // Increment to new layer position.
      layerDisplZ += layerThickness / 2;
      layerPosZ   += layerThickness / 2;

      // First layer subtraction solid.
      Subdetector layer(doc,layer_nam,"ForwardDetector/Layer",sdet.id());
      double layerGlobalZ = zinner + layerDisplZ;
      double layerPosX    = tan(xangleHalf) * layerGlobalZ;
      Position layerSubtraction1Pos(doc,layer_nam + "_subtraction1_pos", layerPosX,0,0);
      Position layerSubtraction2Pos(doc,layer_nam + "_subtraction2_pos",-layerPosX,0,0);

      lcdd.add(layerSubtraction1Pos).add(layerSubtraction2Pos);

      SubtractionSolid layerSubtraction1(doc,layer_nam + "_subtraction1",
        layerTube,beamInTube,layerSubtraction1Pos,beamInRot);
      lcdd.add(layerSubtraction1);

      // Second layer subtraction solid.
      SubtractionSolid layerSubtraction2(doc,layer_nam + "_subtraction2",
        layerSubtraction1,beamOutTube,layerSubtraction2Pos,beamOutRot);
      lcdd.add(layerSubtraction2);
      
      // Layer LV.
      Volume layerVol(doc,layer_nam + "_volume", layerSubtraction2, air);
      
      // Slice loop.
      int sliceCount = 0;
      double slicePosZ = -layerThickness / 2;
      double sliceDisplZ = 0;
      for(xml_coll_t l(x_layer,XML::Tag_slice); l; ++l, ++m)  {
        xml_comp_t x_slice = l;
        string slice_nam = layer_nam + _toString(sliceCount,"_slice%d");
        /** Get slice parameters. */
        double sliceThickness = x_slice.thickness();
        Material slice_mat = lcdd.material(x_slice.materialStr());

        // Go to mid of this slice.
        sliceDisplZ += sliceThickness / 2;
        slicePosZ   += sliceThickness / 2;

        // Slice's basic tube.
        Tube sliceTube(doc, slice_nam + "_tube", rmin,rmax,sliceThickness);
        Subdetector slice(doc,slice_nam,"ForwardDetector/Layer/Slice",sdet.id());
        double sliceGlobalZ = zinner + (layerDisplZ - layerThickness / 2) + sliceDisplZ;
        double slicePosX    = tan(xangleHalf) * sliceGlobalZ;
        Position sliceSubtraction1Pos(doc,slice_nam + "_subtraction1_pos",slicePosX,0,0);
        Position sliceSubtraction2Pos(doc,slice_nam + "_subtraction2_pos",-slicePosX,0,0);

        lcdd.add(sliceTube).add(sliceSubtraction1Pos).add(sliceSubtraction2Pos);

        // First slice subtraction solid.
        SubtractionSolid sliceSubtraction1(doc,slice_nam + "_subtraction1",
          sliceTube,beamInTube,sliceSubtraction1Pos,beamInRot);
        lcdd.add(sliceSubtraction1);

        // Second slice subtraction solid.
        SubtractionSolid sliceSubtraction2(doc,slice_nam + "_subtraction2",
          sliceSubtraction1,beamOutTube,sliceSubtraction2Pos,beamOutRot); 
        lcdd.add(sliceSubtraction2);
       
        // Slice LV.
        Volume sliceVol(doc,slice_nam + "_volume", sliceSubtraction2, slice_mat);
        
        if ( x_slice.isSensitive() ) sliceVol.setSensitiveDetector(sens);
        // Set attributes of slice
        slice.setAttributes(lcdd, sliceVol, 
          x_slice.attr<string>(_A(region)),
          x_slice.attr<string>(_A(limits)),
          x_slice.visStr());

        lcdd.add(sliceVol);
        // Slice PV.
        PhysVol slicePV(doc,sliceVol);
        layerVol.addPhysVol(slicePV,Position(doc,slice_nam+"_pos",0,0,slicePosZ));
        layer.add(slice);

        // Start of next slice.
        sliceDisplZ += sliceThickness / 2;
        slicePosZ   += sliceThickness / 2;
        ++sliceCount;
      }
      // Set attributes of slice
      layer.setAttributes(lcdd, layerVol, 
        x_layer.attr<string>(_A(region)),
        x_layer.attr<string>(_A(limits)),
        x_layer.visStr());

      lcdd.add(layerVol);

      // Layer PV.
      PhysVol layerPV(doc,layerVol);
      layerPV.addPhysVolID(_X(layer), i);
      envelopeVol.addPhysVol(layerPV,Position(doc,layer_nam+"_pos",0,0,layerPosZ));
      sdet.add(layer);

      // Increment to start of next layer.
      layerDisplZ += layerThickness / 2;
      layerPosZ   += layerThickness / 2;
    }
  }
  sdet.setVisAttributes(lcdd, x_det.visStr(), envelopeVol);
  
  lcdd.add(envelopeVol);                // Add envelope LV.
  PhysVol envelopePV(doc,envelopeVol);  // Add envelope PV.
  envelopePV.addPhysVolID(_X(system), id);
  envelopePV.addPhysVolID(_X(barrel), 1);
  motherVol.addPhysVol(envelopePV,Position(doc,det_name+"_pos",0,0,zpos));
  
  // Reflect it.
  if ( reflect )  {
    Rotation reflectRot(lcdd.rotation(_X(reflect_rot)));
    PhysVol envelopePV2(doc,envelopeVol);
    envelopePV2.addPhysVolID(_X(system), id);
    envelopePV2.addPhysVolID(_X(barrel), 2);
    motherVol.addPhysVol(envelopePV2,Position(doc,det_name+"_pos_reflect",0,0,-zpos),reflectRot);
  }
  return sdet;

#else
  return Ref_t(0);
#endif
}

template <> Ref_t Geometry::toObject<Geometry::SiTrackerBarrel,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  return Ref_t(0);
}

template <> Ref_t Geometry::toObject<Geometry::SiTrackerEndcap,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  return Ref_t(0);
}

template <> Ref_t Geometry::toObject<Geometry::SiTrackerEndcap2,xml_h>(lcdd_t& lcdd, const xml_h& e, SensitiveDetector& sens)  {
#if defined(SHOW_ALL_DETECTORS)
  xml_det_t   x_det     = e;
  Document    doc       = lcdd.document();
  Material    air       = lcdd.material(_X(Air));
  Material    vacuum    = lcdd.material(_X(Vacuum));
  int         sysID     = x_det.id();
  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  Tube        envelope   (doc,det_name+"_envelope");
  Volume      envelopeVol(doc,det_name+"_envelope_volume",envelope,air);
  Subdetector sdet       (doc,det_name,det_type,sysID);
  Volume      motherVol = lcdd.pickMotherVolume(sdet);
  int         m_id=0, c_id=0, n_sensor=0;
  double      posY;

  sdet.setVolume(envelopeVol).setEnvelope(envelope);
  for(xml_coll_t mi(x_det,_X(module)); mi; ++mi, ++m_id)  {
    xml_comp_t x_mod   = mi;
    string     m_nam   = x_mod.nameStr();
    string     vol_nam = m_nam+"Volume";
    xml_comp_t trd     = x_mod.child(_X(trd));
  	double     x1      = trd.x1();
    double     x2      = trd.x2();
    double     z       = trd.z();
    double     y1, y2, total_thickness=0.;
    xml_coll_t ci(x_mod,_X(module_component));
    Subdetector module (doc,m_nam,det_type+"/Module",sysID);

    /* Analyse these entries:
    <module name="SiVertexEndcapModule3">
        <trd x1="VertexEndcap_rmin3 * tan(pi/(VertexEndcapModules-0.1))" x2="VertexEndcap_rmax * sin(pi/(VertexEndcapModules-0.1))" z="(VertexEndcap_rmax - VertexEndcap_rmin3) / 2" />
        <module_component thickness="0.005*cm" material="Silicon" sensitive="true" />
        <module_component thickness="0.013*cm"   material="Carbon" />
    </module>
    */
    for(ci.reset(), total_thickness=0.0; ci; ++ci)
      total_thickness += xml_comp_t(ci).thickness();

    y1 = y2 = total_thickness / 2;
    Trapezoid m_envelope(doc, m_nam+"Trd", x1, x2, y1, y2, z);
	  Volume    m_volume(doc, vol_nam, m_envelope, vacuum);

    module.setVolume(m_volume).setEnvelope(m_envelope);
    lcdd.add(m_volume).add(m_envelope);
    m_volume.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));

    for(ci.reset(), n_sensor=0, c_id=0, posY=-y1; ci; ++ci, ++c_id)  {
      xml_comp_t c       = ci;
      double     c_thick = c.thickness();
      Material   c_mat   = lcdd.material(c.materialStr());
      string     c_name  = m_nam + _toString(c_id,"_component%d");
      Subdetector component(doc,c_name,det_type+"/Module/Component",sysID);
      Trapezoid trd(doc, c_name+"_trd", x1, x2, c_thick/2e0, c_thick/2e0, z);
      Volume    vol(doc, c_name, trd, c_mat);
      Position  pos(doc, c_name+"_position", 0e0, posY + c_thick/2e0, 0e0);
      Rotation  rot(doc, c_name+"_rotation");

      component.setVolume(vol).setEnvelope(trd);
      lcdd.add(trd).add(vol).add(pos).add(rot);

      vol.setVisAttributes(lcdd.visAttributes(c.visStr()));

      PhysVol   phv(doc, vol, c_name);
      phv.addPhysVolID(_X(component),c_id);
      vol.addPhysVol(phv,pos, rot);
      if ( c.isSensitive() ) {
        sdet.check(n_sensor > 1,"SiTrackerEndcap2::fromCompact: "+c_name+" Max of 2 modules allowed!");
        phv.addPhysVolID(_X(sensor),c_id);
        vol.setSensitiveDetector(sens);
        ++n_sensor;
      }

      posY += c_thick;
      module.add(component);
    }
    sdet.add(module);
  }
  return sdet;
#else
  return Ref_t(0);
#endif
}

template <> Ref_t Geometry::toObject<Geometry::Subdetector,xml_h>(lcdd_t& lcdd, const xml_h& e)  {
  Subdetector      det(0);
  string           type = e.attr<string>(_A(type));
  string           name = e.attr<string>(_A(name));
  SensitiveDetector  sd = toObject<SensitiveDetector>(lcdd,e);

  if ( type == "PolyconeSupport" )
    det = toObject<PolyconeSupport>(lcdd,e,sd);
  else if ( type == "TubeSegment" )
    det = toObject<TubeSegment>(lcdd,e,sd);
  else if ( type == "MultiLayerTracker" )
    det = toObject<MultiLayerTracker>(lcdd,e,sd);
  else if ( type == "DiskTracker" )
    det = toObject<DiskTracker>(lcdd,e,sd);
  else if ( type == "SiTrackerBarrel" )
    det = toObject<SiTrackerBarrel>(lcdd,e,sd);
  else if ( type == "SiTrackerEndcap" )
    det = toObject<SiTrackerBarrel>(lcdd,e,sd);
  else if ( type == "SiTrackerEndcap2" )
    det = toObject<SiTrackerEndcap2>(lcdd,e,sd);
  else if ( type == "CylindricalBarrelCalorimeter" )
    det = toObject<CylindricalBarrelCalorimeter>(lcdd,e,sd);
  else if ( type == "CylindricalEndcapCalorimeter" )
    det = toObject<CylindricalEndcapCalorimeter>(lcdd,e,sd);
  else if ( type == "EcalBarrel" )
    det = toObject<EcalBarrel>(lcdd,e,sd);
  else if ( type == "PolyhedraEndcapCalorimeter2" )
    det = toObject<PolyhedraEndcapCalorimeter2>(lcdd,e,sd);
  else if ( type == "PolyhedraBarrelCalorimeter2" )
    det = toObject<PolyhedraBarrelCalorimeter2>(lcdd,e,sd);
  else if ( type == "ForwardDetector" )
    det = toObject<ForwardDetector>(lcdd,e,sd);
  else  {
    string err = "UNKNOWN detector:" + name + " of type " + type;
    cout << err << endl;
    throw runtime_error(err);
  }
  /*
  if ( type == "ForwardDetector" )
    //det = Subdetector(0);
    det = toObject<ForwardDetector>(lcdd,e,sd);
  else  {
    string err = "UNKNOWN detector:" + name + " of type " + type;
    cout << err << endl;
    //throw runtime_error(err);
  }
    */
  if ( det != 0 && e.hasAttr(_A(readout)) )  {
    string rdo = e.attr<string>(_A(readout));
    det.setReadout(lcdd.readout(rdo));
  }
  if ( det )
    cout << "Converted subdetector:" << name << " of type " << type << endl;
  else
    cout << "FAILED    subdetector:" << name << " of type " << type << endl;
  return det;
}
#if 0
struct TPC : RefElement {
  double outerRadius() const   {
    Tube env = volume().shape();
    return env.outer_r();
  }
};

Subdetector LCDD::detector(string&);

void foo(Subdetector& det)   {
  TPC tpc = lcdd.detector("TPC");
  tpc.outerRadius();


  tpc->outerRadius();
}
#endif
