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
  
  template <> Ref_t DetElementFactory<MultiLayerTracker>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t  x_det    = e;
    string     det_name = x_det.nameStr();
    string     det_type = x_det.typeStr();
    Material   air      = lcdd.material(_X(Air));
    DetElement sdet    (lcdd,det_name,det_type,x_det.id());
    Volume     motherVol= lcdd.pickMotherVolume(sdet);
    int n = 0;

    for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++n)  {
      xml_comp_t x_layer = i;
      string  layer_name = det_name+_toString(n,"_layer%d");
      DetElement layer(lcdd,layer_name,"MultiLayerTracker/Layer",x_layer.id());
      Tube    layer_tub(lcdd,layer_name);
      Volume  layer_vol(lcdd,layer_name+"_volume",layer_tub,air);
      double  z    = x_layer.outer_z();
      double  rmin = x_layer.inner_r();
      double  r    = rmin;
      int m = 0;

      layer.setVolume(layer_vol).setEnvelope(layer_tub);
      for(xml_coll_t j(x_layer,_X(slice)); j; ++j, ++m)  {
	xml_comp_t x_slice = j;
	Material mat = lcdd.material(_toString(x_slice.material()));
	string slice_name= layer_name+_toString(m,"_slice%d");
	// Slices have no extra id. Take the ID of the layer!
	DetElement slice(lcdd, slice_name,det_type+"/Layer/Slice",layer.id());
	Tube   slice_tub(lcdd,slice_name);
	Volume slice_vol(lcdd,slice_name+"_volume", slice_tub, mat);

	r += x_slice.thickness();
	slice_tub.setDimensions(r,r,2.*z,2.*M_PI);
	slice.setVolume(slice_vol).setEnvelope(slice_tub);
	slice_vol.setSolid(slice_tub);
	if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
	// Set region of slice
	slice.setRegion(lcdd,x_slice.attr<string>(_A(region)), slice_vol);
	// Set limits of slice
	slice.setLimitSet(lcdd,x_slice.attr<string>(_A(limits)),slice_vol);
	// Set vis attributes of slice
	slice.setVisAttributes(lcdd, x_slice.visStr(), slice_vol);

	//PhysVol spv(lcdd,slice_vol,slice_name);
	PhysVol spv(slice_vol);
	spv.addPhysVolID(_X(layer),n);
	layer_vol.addPhysVol(spv,lcdd.identity());
	layer.add(slice);
      }
      layer_tub.setDimensions(rmin,r,2.*z,2.*M_PI);
      sdet.setVisAttributes(lcdd, x_layer.visStr(), layer_vol);

      //PhysVol lpv(lcdd,layer_vol,layer_name);
      PhysVol lpv(layer_vol);
      lpv.addPhysVolID(_X(system),sdet.id())
	.addPhysVolID(_X(barrel),0);
      motherVol.addPhysVol(lpv,lcdd.identity());
      sdet.add(layer);
    }
    sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,MultiLayerTracker);
