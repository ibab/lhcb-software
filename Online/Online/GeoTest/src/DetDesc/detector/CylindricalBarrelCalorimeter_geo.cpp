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
  
  template <> Ref_t DetElementFactory<CylindricalBarrelCalorimeter>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)   {
    xml_det_t  x_det     = e;
    xml_dim_t  dim       = x_det.dimensions();
    Material   air       = lcdd.material(_X(Air));
    string     det_name  = x_det.nameStr();
    string     det_type  = x_det.typeStr();
    Tube       envelope   (lcdd,det_name+"_envelope");
    Volume     envelopeVol(lcdd,det_name+"_envelope_volume",envelope,air);
    DetElement sdet       (lcdd,det_name,det_type,x_det.id());
    double     z    = dim.outer_z();
    double     rmin = dim.inner_r();
    double     r    = rmin;
    int        n    = 0;

    envelope.setDimensions(rmin,r,2.*z);
    sdet.setVolume(envelopeVol).setEnvelope(envelope);
    for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
      xml_comp_t x_layer = c;
      for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
	string layer_name = det_name + _toString(n,"_layer%d");
	Tube   layer_tub(lcdd,layer_name);
	Volume layer_vol(lcdd,layer_name+"_volume",layer_tub,air);
	DetElement layer(lcdd,layer_name,"CylindricalBarrelCalorimeter/Layer",sdet.id());
	double rlayer = r;

	layer.setVolume(layer_vol).setEnvelope(layer_tub);
	for(xml_coll_t l(x_layer,_X(slice)); l; ++l, ++m)  {
	  xml_comp_t x_slice = l;
	  Material   slice_mat  = lcdd.material(x_slice.materialStr());
	  string     slice_name = layer_name + _toString(m,"slice%d");
	  Tube       slice_tube(lcdd,slice_name);
	  Volume     slice_vol (lcdd,slice_name+"_volume",slice_tube,slice_mat);
	  DetElement slice(lcdd,slice_name,"CylindricalBarrelCalorimeter/Layer/Slice",sdet.id());
	  double     router = r + x_slice.thickness();

	  slice.setVolume(slice_vol).setEnvelope(slice_tube);
	  if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
	  slice_tube.setDimensions(r,router,z * 2);
	  r = router;
	  // Set region of slice
	  slice.setRegion(lcdd,x_slice.attr<string>(_A(region)),slice_vol);
	  // set limits of slice
	  slice.setLimitSet(lcdd,x_slice.attr<string>(_A(limits)),slice_vol);
	  // Set vis attributes of slice
	  slice.setVisAttributes(lcdd,x_slice.visStr(),slice_vol);
	  // Instantiate physical volume
	  layer_vol.placeVolume(slice_vol,IdentityPos());
	  layer.add(slice);
	}
	layer.setVisAttributes(lcdd,x_layer.visStr(),layer_vol);
	layer_tub.setDimensions(rlayer,r,z * 2);

	PlacedVolume layer_physvol = envelopeVol.placeVolume(layer_vol,IdentityPos());
	layer_physvol.addPhysVolID(_A(layer),n);
	sdet.add(layer);
	++n;
      }
    }
    // Set region of slice
    sdet.setRegion(lcdd,x_det.attr<string>(_A(region)),envelopeVol);
    // set limits of slice
    sdet.setLimitSet(lcdd,x_det.attr<string>(_A(limits)),envelopeVol);
    // Set vis attributes of slice
    sdet.setVisAttributes(lcdd,x_det.visStr(),envelopeVol);

    PlacedVolume physvol = lcdd.pickMotherVolume(sdet).placeVolume(envelopeVol,IdentityPos());
    physvol.addPhysVolID(_A(system),sdet.id()).addPhysVolID(_A(barrel),0);
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,CylindricalBarrelCalorimeter);
