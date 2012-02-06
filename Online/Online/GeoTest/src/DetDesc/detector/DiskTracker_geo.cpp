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
   
  template <> Ref_t DetElementFactory<DiskTracker>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t  x_det     = e;
    Material   air       = lcdd.material(_X(Air));
    string     det_name  = x_det.nameStr();
    string     det_type  = x_det.typeStr();
    bool       reflect   = x_det.attr<bool>(_A(reflect));
    Rotation   refl_rot  = lcdd.rotation(_X(reflect_rot));
    DetElement sdet        (lcdd,det_name,det_type,x_det.id());
    Volume     motherVol = lcdd.pickMotherVolume(sdet);
    int n = 0;

    for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++n)  {
      xml_comp_t x_layer = i;
      string layer_name = det_name+_toString(n,"_layer%d");
      Tube   layer_tub(lcdd,layer_name);
      Volume layer_vol(lcdd,layer_name+"_volume",layer_tub,air);
      DetElement layer(lcdd,layer_name,det_type+"/Layer",x_layer.id());
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
	Tube   slice_tub(lcdd,slice_name);
	Volume slice_vol(lcdd,slice_name+"_volume", slice_tub, mat);
	// Slices have no extra id. Take the ID of the layer!
	DetElement slice(lcdd,slice_name,det_type+"/Layer/Slice",layer.id());

	slice.setVolume(slice_vol).setEnvelope(slice_tub);
	slice_tub.setDimensions(rmin,rmax,w,2.*M_PI);

	if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
	// Set region of slice
	slice.setRegion(lcdd, x_slice.attr<string>(_A(region)), slice_vol);
	// set limits of slice
	slice.setLimitSet(lcdd, x_slice.attr<string>(_A(limits)), slice_vol);
	// Set vis attributes of slice
	slice.setVisAttributes(lcdd, x_slice.visStr(), slice_vol);

	//PhysVol spv(lcdd,slice_vol,slice_name);
	PhysVol spv(slice_vol);
	spv.addPhysVolID(_X(layer),n);
	layer_vol.addPhysVol(spv,Position(lcdd,slice_name+"_pos",0.,0.,z-zmin-layerWidth/2. + w/2.));
	layer.add(slice);
      }
      layer.setVisAttributes(lcdd, x_layer.visStr(), layer_vol);
      layer_tub.setDimensions(rmin,rmax,2.*z,2.*M_PI);

      PhysVol lpv(layer_vol);
      lpv.addPhysVolID(_X(system),sdet.id());
      lpv.addPhysVolID(_X(barrel),1);
      motherVol.addPhysVol(lpv,Position(lcdd,layer_name+"_pos",0,0,zmin+layerWidth/2.));
      if ( reflect )  {
	PhysVol lpvR(layer_vol);
	lpvR.addPhysVolID(_X(system),sdet.id());
	lpvR.addPhysVolID(_X(barrel),2);
	motherVol.addPhysVol(lpvR,Position(lcdd,layer_name+"_pos_reflect",0.,0.,-zmin-layerWidth/2.),refl_rot);
      }
      sdet.add(layer);
    }
    sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,DiskTracker);
