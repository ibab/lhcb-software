#define _USE_MATH_DEFINES
#include "XML/compact/XMLTracker.h"
#include "XML/lcdd/XMLShapes.h"
#include "XML/lcdd/XMLVolumes.h"
#include "XML/lcdd/XMLObjects.h"
#include "XML/lcdd/XMLLCDD.h"

// C+_+ include files
#include <cmath>

using namespace std;
using namespace DetDesc::XML;

namespace DetDesc {  namespace XML  {
  extern const Tag_t Tag_Air;
  extern const Tag_t Tag_layer;
  extern const Tag_t Tag_slice;
  extern const Tag_t Tag_reflect_rot;
  extern const Tag_t Tag_system;
  extern const Tag_t Tag_barrel;

  extern const Tag_t Attr_repeat;
  extern const Tag_t Attr_system;
  extern const Tag_t Attr_barrel;
  extern const Tag_t Attr_reflect;
}}

/// Constructor to be used when creating a new DOM tree
Subdetector& MultiLayerTracker::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)
{
  Document    doc       = lcdd.document();
  Handle_t    compact   = handle();
  Tag_t       det_name  = name();
  Material    air       = lcdd.material(Tag_Air);
  Volume      motherVol = lcdd.pickMotherVolume(*this);
  int n = 0, id = this->id();

  for(Collection_t i(compact,Tag_layer); i; ++i, ++n)  {
    Component layer = i;
    Tag_t   layer_name = det_name+_toString(n,"_layer%d");
    Tube    layer_tub(doc,layer_name);
    Volume  layer_vol(doc,layer_name+"_volume",layer_tub,air);
    double  z    = layer.outer_z();
    double  rmin = layer.inner_r();
    double  r    = rmin;
    int m = 0;
    for(Collection_t j(layer,Tag_slice); j; ++j, ++m)  {
      Component slice = j;
      Material mat = lcdd.material(slice.material());
      Tag_t  slice_name = layer_name+_toString(m,"_slice%d");
      Tube   slice_tub(doc,slice_name);
      Volume slice_vol(doc,slice_name+"_volume", slice_tub, mat);

      r += slice.thickness();
      slice_tub.setDimensions(r,r,2.*z,2.*M_PI);
      lcdd.solids().append(slice_tub);
      slice_vol.setSolid(slice_tub);
      if ( slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
      // Set region of slice
      setRegion(lcdd, slice, slice_vol);
      // set limits of slice
      setLimitSet(lcdd, slice, slice_vol);
      // Set vis attributes of slice
      setVisAttributes(lcdd, slice, slice_vol);

      lcdd.structure().append(slice_vol);
      PhysVol spv(doc,slice_vol);
      spv.addPhysVolID(Tag_layer,n);
      layer_vol.addPhysVol(spv);
    }
    layer_tub.setDimensions(rmin,r,2.*z,2.*M_PI);
    setVisAttributes(lcdd, layer, layer_vol);
    PhysVol lpv(doc,layer_vol);
    lpv.addPhysVolID(Tag_system,id)
       .addPhysVolID(Tag_barrel,0);
    motherVol.addPhysVol(lpv);
    lcdd.solids().append(layer_tub);
    lcdd.structure().append(layer_vol);
  }
  setCombineHits(compact,sens);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Subdetector& DiskTracker::fromCompact(LCDD& lcdd, const SensitiveDetector& sens)    {
  Document    doc       = lcdd.document();
  Handle_t    compact   = handle();
  Tag_t       det_name  = name();
  Material    air       = lcdd.material(Tag_Air);
  bool        reflect   = attr<bool>(Attr_reflect);
  Rotation    refl_rot  = lcdd.rotation(Tag_reflect_rot);
  Volume      motherVol = lcdd.pickMotherVolume(*this);
  int n = 0, id = this->id();

  for(Collection_t i(compact,Tag_layer); i; ++i, ++n)  {
    Component layer = i;
    Tag_t   layer_name = det_name+_toString(n,"_layer%d");
    Tube    layer_tub(doc,layer_name);
    Volume  layer_vol(doc,layer_name+"_volume",layer_tub,air);
    double  zmin = layer.inner_z();
    double  rmin = layer.inner_r();
    double  rmax = layer.outer_r();
    double  z = zmin, layerWidth = 0.;
    int m = 0;

    for(Collection_t j(layer,Tag_slice); j; ++j)  {
      double w = Component(j).thickness();
      layerWidth += w;
    }
    for(Collection_t j(layer,Tag_slice); j; ++j, ++m)  {
      Component slice = j;
      double     w = slice.thickness();
      Material mat = lcdd.material(slice.material());
      Tag_t  slice_name = layer_name+_toString(m,"_slice%d");
      Tube   slice_tub(doc,slice_name);
      Volume slice_vol(doc,slice_name+"_volume", slice_tub, mat);
      slice_tub.setDimensions(rmin,rmax,w,2.*M_PI);

      lcdd.add(slice_tub);
      slice_vol.setSolid(slice_tub);
      if ( slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
      // Set region of slice
      setRegion(lcdd, slice, slice_vol);
      // set limits of slice
      setLimitSet(lcdd, slice, slice_vol);
      // Set vis attributes of slice
      setVisAttributes(lcdd, slice, slice_vol);

      lcdd.add(slice_vol);
      PhysVol spv(doc,slice_vol);
      spv.setZ(z-zmin-layerWidth/2. + w/2.);
      spv.addPhysVolID(Tag_layer,n);
      layer_vol.addPhysVol(spv);
    }
    setVisAttributes(lcdd, layer, layer_vol);
    layer_tub.setDimensions(rmin,rmax,2.*z,2.*M_PI);
    PhysVol lpv(doc,layer_vol);
    lpv.setZ(zmin+layerWidth/2.);
    lpv.addPhysVolID(Tag_system,id);
    lpv.addPhysVolID(Tag_barrel,1);
    motherVol.addPhysVol(lpv);
    if ( reflect )  {
      PhysVol lpvR(doc,layer_vol);
      lpvR.setZ(-zmin-layerWidth/2.);
      lpvR.setRotation(refl_rot);
      lpvR.addPhysVolID(Tag_system,id);
      lpvR.addPhysVolID(Tag_barrel,2);
      motherVol.addPhysVol(lpvR);
    }
    lcdd.add(layer_tub);
    lcdd.structure().append(layer_vol);
  }
  setCombineHits(compact,sens);
  return *this;
}
