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
  
  template <> Ref_t DetElementFactory<PolyhedraEndcapCalorimeter2>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t   x_det     = e;
    xml_dim_t   dim       = x_det.dimensions();

    int         det_id    = x_det.id();
    string      det_name  = x_det.nameStr();
    string      det_type  = x_det.typeStr();
    Material    air       = lcdd.material(_X(Air));
    Material    gapMat    = air;
    int         numsides  = dim.numsides();
    double      rmin      = dim.rmin();
    double      rmax      = dim.rmax()*std::cos(M_PI/numsides);
    double      zmin      = dim.zmin();
    Layering    layering(x_det);
    double      totalThickness = layering.totalThickness();
    DetElement  sdet(lcdd,det_name,det_type,x_det.id());
    Volume      motherVol = lcdd.pickMotherVolume(sdet);

    PolyhedraRegular polyhedra(lcdd,det_name+"_polyhedra",numsides,rmin,rmax,totalThickness);
    Volume           envelopeVol(lcdd,det_name+"_envelope",polyhedra,air);

    int l_num = 0;
    int layerType   = 0;
    double layerZ   = -totalThickness/2;

    for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
      xml_comp_t       x_layer  = c;
      double           l_thick  = layering.layer(l_num)->thickness();
      string           l_name   = det_name + _toString(layerType,"_layer%d");
      int              l_repeat = x_layer.repeat();
      PolyhedraRegular l_solid(lcdd,l_name+"_solid",numsides,rmin,rmax,l_thick);
      Volume           l_vol  (lcdd,l_name+"_volume", l_solid, air);
      DetElement       l_elt  (lcdd,l_name,det_type+"/Layer",det_id);
      
      int s_num = 0;
      double sliceZ = -l_thick/2;
      sdet.add(l_elt.setVolume(l_vol).setEnvelope(l_solid));
      for(xml_coll_t s(x_layer,_X(slice)); s; ++s)  {
	xml_comp_t x_slice = s;
	string     s_name  = l_name + _toString(s_num,"_slice%d");
	double     s_thick = x_slice.thickness();
	Material   s_mat   = lcdd.material(x_slice.materialStr());
	PolyhedraRegular s_solid(lcdd,s_name+"_solid",numsides,rmin,rmax,s_thick);
	Volume           s_vol  (lcdd,s_name+"_volume",s_solid,s_mat);
	
	if ( x_slice.isSensitive() ) s_vol.setSensitiveDetector(sens);
	s_vol.setVisAttributes(lcdd.visAttributes(x_slice.visStr()));
	sliceZ += s_thick/2;
	PlacedVolume s_phv = l_vol.placeVolume(s_vol,Position(0,0,sliceZ));
	s_phv.addPhysVolID("slice",s_num);
	s_num++;
      }
      l_vol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
      if ( l_repeat <= 0 ) throw std::runtime_error(det_name+"> Invalid repeat value");
      for(int j=0; j<l_repeat; ++j) {
	string phys_lay = det_name + _toString(l_num,"_layer%d");
	layerZ += l_thick/2;
	PlacedVolume  phys_vol = envelopeVol.placeVolume(l_vol,Position(0,0,layerZ));
	phys_vol.addPhysVolID("layer", l_num);
	layerZ += l_thick/2;
	++l_num;
      }
      ++layerType;
    }

    sdet.setAttributes(lcdd,envelopeVol,
		       x_det.attr<string>(_A(region)),
		       x_det.attr<string>(_A(limits)),
		       x_det.visStr());

    PlacedVolume  physvol = motherVol.placeVolume(envelopeVol,
						  Position(0,0,zmin+totalThickness/2),
						  Rotation(0,0,M_PI/numsides));
    physvol.addPhysVolID("system",det_id);
    physvol.addPhysVolID("barrel",1);        
        
    bool reflect = x_det.hasAttr(_A(reflect)) ? x_det.attr<bool>(_A(reflect)) : true;
    if ( reflect ) {
      physvol = motherVol.placeVolume(envelopeVol,
				      Position(0,0,-(zmin+totalThickness/2)),
				      Rotation(M_PI,0,M_PI/numsides));
      physvol.addPhysVolID("system",det_id);
      physvol.addPhysVolID("barrel",2);
    }
    return sdet;
  }
}}
DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,PolyhedraEndcapCalorimeter2);
