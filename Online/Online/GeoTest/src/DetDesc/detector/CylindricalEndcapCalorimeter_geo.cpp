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
#include "DetDesc/Layering.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

namespace DetDesc { namespace Geometry {
  
  template <> Ref_t DetElementFactory<CylindricalEndcapCalorimeter>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t  x_det     = e;
    xml_dim_t  dim       = x_det.dimensions();
    Material   air       = lcdd.material(_X(Air));
    string     det_name  = x_det.nameStr();
    string     det_type  = x_det.typeStr();
    Tube       envelope   (lcdd,det_name+"_envelope");
    Volume     envelopeVol(lcdd,det_name+"_envelope_volume",envelope,air);
    DetElement sdet       (lcdd,det_name,det_type,x_det.id());
    Volume     motherVol = lcdd.pickMotherVolume(sdet);
    bool       reflect   = dim.reflect();
    double     zmin      = dim.inner_z();
    double     rmin      = dim.inner_r();
    double     rmax      = dim.outer_r();
    double     totWidth  = Layering(x_det).totalThickness();
    double     z    = zmin;
    int        n    = 0;

    for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
      xml_comp_t x_layer = c;
      double layerWidth = 0;
      for(xml_coll_t l(x_layer,_X(slice)); l; ++l)
	layerWidth += xml_comp_t(l).thickness();
      for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
	double     zlayer = z;
	string     layer_name = det_name + _toString(n,"_layer%d");
	Tube       layer_tub(lcdd,layer_name);
	Volume     layer_vol(lcdd,layer_name+"_volume",layer_tub,air);
	DetElement layer(lcdd,layer_name,"CylindricalEndcapCalorimeter/Layer",sdet.id());

	layer.setVolume(layer_vol).setEnvelope(layer_tub);
	for(xml_coll_t l(x_layer,_X(slice)); l; ++l, ++m)  {
	  xml_comp_t x_slice = l;
	  double     w = x_slice.thickness();
	  string     slice_name = layer_name + _toString(m,"slice%d");
	  Material   slice_mat  = lcdd.material(x_slice.materialStr());
	  Tube       slice_tube(lcdd,slice_name, rmin,rmax,w);
	  Volume     slice_vol (lcdd,slice_name+"_volume", slice_tube, slice_mat);
	  DetElement slice(lcdd,slice_name,"CylindricalEndcapCalorimeter/Layer/Slice",sdet.id());

	  slice.setVolume(slice_vol).setEnvelope(slice_tube);
	  if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);

	  // Set attributes of slice
	  slice.setAttributes(lcdd, slice_vol, 
			      x_slice.attr<string>(_A(region)), 
			      x_slice.attr<string>(_A(limits)),
			      x_slice.visStr());

	  layer_vol.placeVolume(slice_vol,Position(0,0,z-zlayer-layerWidth/2+w/2));
	  z += w;
	  layer.add(slice);
	}
	layer.setVisAttributes(lcdd,x_layer.visStr(),layer_vol);
	layer_tub.setDimensions(rmin,rmax,layerWidth);

	Position layer_pos(0,0,zlayer-zmin-totWidth/2+layerWidth/2);
	PlacedVolume layer_phys = envelopeVol.placeVolume(layer_vol,layer_pos);
	layer_phys.addPhysVolID("layer",n);
	sdet.add(layer);
	++n;
      }
    }
    envelope.setDimensions(rmin,rmax,totWidth,2.*M_PI);
    // Set attributes of slice
    sdet.setAttributes(lcdd, envelopeVol,
		       x_det.attr<string>(_A(region)),
		       x_det.attr<string>(_A(limits)),
		       x_det.visStr());

    PlacedVolume phv=motherVol.placeVolume(envelopeVol,Position(0,0,zmin+totWidth/2));
    phv.addPhysVolID(_A(system),sdet.id())
      .addPhysVolID(_A(barrel),1);

    if ( reflect )   {
      phv=motherVol.placeVolume(envelopeVol,Position(0,0,-zmin-totWidth/2),ReflectRot());
      phv.addPhysVolID(_A(system),sdet.id())
	.addPhysVolID(_A(barrel),2);
    }
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,CylindricalEndcapCalorimeter);
