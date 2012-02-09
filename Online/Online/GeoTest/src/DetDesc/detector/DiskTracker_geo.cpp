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
    DetElement sdet        (lcdd,det_name,det_type,x_det.id());
    Volume     motherVol = lcdd.pickMotherVolume(sdet);
    int n = 0;

    for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++n)  {
      xml_comp_t x_layer = i;
      string layer_name = det_name+_toString(n,"_layer%d");
      double  zmin = x_layer.inner_z();
      double  rmin = x_layer.inner_r();
      double  rmax = x_layer.outer_r();
      double  z    = zmin, layerWidth = 0.;
      Tube    layer_tub(lcdd,layer_name,rmin,rmax,2.*z,2.*M_PI);
      Volume  layer_vol(lcdd,layer_name+"_volume",layer_tub,air);
      DetElement layer(lcdd,layer_name,det_type+"/Layer",x_layer.id());
      int m = 0;

      layer.setVolume(layer_vol).setEnvelope(layer_tub);
      layer.setVisAttributes(lcdd,x_layer.visStr(),layer_vol);

      for(xml_coll_t j(x_layer,_X(slice)); j; ++j)  {
	double w = xml_comp_t(j).thickness();
	layerWidth += w;
      }
      for(xml_coll_t j(x_layer,_X(slice)); j; ++j, ++m)  {
	xml_comp_t x_slice = j;
	double     w = x_slice.thickness();
	Material mat = lcdd.material(x_slice.materialStr());
	string slice_name = layer_name+_toString(m,"_slice%d");
	Tube   slice_tub(lcdd,slice_name,rmin,rmax,2*M_PI);
	Volume slice_vol(lcdd,slice_name+"_volume", slice_tub, mat);
	// Slices have no extra id. Take the ID of the layer!
	DetElement slice(lcdd,slice_name,det_type+"/Layer/Slice",layer.id());

	slice.setVolume(slice_vol).setEnvelope(slice_tub);
	if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
	slice.setAttributes(lcdd,slice_vol,
			    x_slice.attr<string>(_A(region)),
			    x_slice.attr<string>(_A(limits)),
			    x_slice.visStr());
	PlacedVolume spv = layer_vol.placeVolume(slice_vol,Position(0,0,z-zmin-layerWidth/2+w/2));
	spv.addPhysVolID(_X(layer),n);
	layer.add(slice);
      }

      PlacedVolume lpv = motherVol.placeVolume(layer_vol,Position(0,0,zmin+layerWidth/2.));
      lpv.addPhysVolID(_X(system),sdet.id());
      lpv.addPhysVolID(_X(barrel),1);
      if ( reflect )  {
	PlacedVolume lpvR = motherVol.placeVolume(layer_vol,Position(0,0,-zmin-layerWidth/2),ReflectRot());
	lpvR.addPhysVolID(_X(system),sdet.id());
	lpvR.addPhysVolID(_X(barrel),2);
      }
      sdet.add(layer);
    }
    sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,DiskTracker);
