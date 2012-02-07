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
#include "TGeoTrd2.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

namespace DetDesc { namespace Geometry {
  
  template <> Ref_t DetElementFactory<EcalBarrel>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    static double tolerance = 0e0;
    Layering    layering (e);
    xml_det_t   x_det     = e;
    Material    air       = lcdd.material("Air");
    Material    vacuum    = lcdd.material("Vacuum");
    int         det_id    = x_det.id();
    string      det_name  = x_det.nameStr();
    string      det_type  = x_det.typeStr();
    xml_comp_t  x_staves  = x_det.child(_X(staves));
    xml_comp_t  x_dim     = x_det.child(_X(dimensions));
    int         nsides    = x_dim.numsides();
    double      inner_r   = x_dim.rmin();
    double      mod_y1    = x_dim.z();
    double      mod_y2    = mod_y1;
    double      dphi      = M_PI*2e0 / double(nsides);
    double      hphi      = dphi/2e0;
    double      mod_z     = layering.totalThickness();
    double      mod_y_off = inner_r + mod_z/2e0;
    double      outer_r   = inner_r + mod_z;
    
    double      totThick  = mod_z;
    DetElement  sdet      (lcdd,det_name,det_type,det_id);
    Volume      motherVol = lcdd.pickMotherVolume(sdet);
    PolyhedraRegular hedra(lcdd,det_name+"_polyhedra",nsides,inner_r,inner_r+totThick+tolerance*2e0,mod_y1);
    Volume      envelope  (lcdd,det_name+"_envelope",hedra,air);
    Rotation    rot       (lcdd,det_name+"_rotation",0,0,M_PI/nsides);
    PhysVol envPhv(envelope);

    envPhv.addPhysVolID("system",det_id);
    envPhv.addPhysVolID("barrel",0);
    motherVol.addPhysVol(envPhv,lcdd.position("identity_pos"),rot);

    // Compute trapezoid measurements.
    double bo = std::tan(hphi) * outer_r;
    double bi = std::tan(hphi) * inner_r;
    
    // Compute the dx per layer, using side 
    // triangle calculations (from Norman Graf).
    double gamma = (M_PI * 2e0) / nsides;
    double dx = mod_z / std::sin(gamma);
    
    // The offset of a stave, derived from the dx term.
    double mod_x_off = dx / 2.0;
    
    // Compute the top and bottom face measurements.
    double mod_x2 = 2 * bo - dx;
    double mod_x1 = 2 * bi + dx;
		
    // Create the trapezoid for the stave.
    Trapezoid trd(lcdd,det_name + "_module_trd", 
		  mod_x1/2.-tolerance, // Outer side, i.e. the "short" X side.
		  mod_x2/2.-tolerance, // Inner side, i.e. the "long" X side.
		  mod_y1/2.-tolerance, // Corresponds to subdetector (or module) Z.
		  mod_y2/2.-tolerance, // "
		  mod_z/2.-tolerance); // Thickness, in Y for top stave, when rotated.

    Volume mod_vol(lcdd,det_name+"_module",trd,air);
#if 0
    //buildBarrelStave(lcdd, sens, module_volume);
    {
      Rotation irot = lcdd.rotation("identity_rot");
      // Parameters for computing the layer X dimension:
      double z = trd->GetDy1();
      double trd_z = trd->GetDz();
      // Starting X dimension for the layer.
      double l_dim_x = trd->GetDx1();		
      // Adjacent angle of triangle.
      double adj = (trd->GetDx1() - trd->GetDx2()) / 2.;		
      // Hypotenuse of triangle.
      double hyp = std::sqrt(trd_z * trd_z + adj * adj);
      // Lower-right angle of triangle.
      double beta = std::acos(adj / hyp);
      // Primary coefficient for figuring X.
      double tan_beta = std::tan(beta); 
      double subdetector_thickness = layering.totalThickness();
      double l_pos_z = -(subdetector_thickness / 2);
		
      // Loop over the sets of layer elements in the detector.
      int l_num = 0;
      for(xml_coll_t li(x_det,_X(layer)); li; ++li)  {
	xml_comp_t x_layer = li;
	int repeat = x_layer.repeat();
	// Loop over number of repeats for this layer.
	for (int j=0; j<repeat; j++)    {
	  string l_name = det_name + _toString(l_num,"_layer%d");
	  // Compute this layer's thickness.
	  double l_thickness = layering.layer(l_num)->thickness();
	  // Increment the Z position to place this layer.
	  l_pos_z += l_thickness / 2;
	  // Position of the layer.
	  Position l_pos(lcdd,l_name+"_position",0,0,l_pos_z);
	  // Compute the X dimension for this layer.
	  double xcut = (l_thickness / tan_beta);
	  l_dim_x -= xcut;
	  Box    l_box(lcdd,l_name+"_box",l_dim_x*2 - tolerance,z*2-tolerance,l_thickness-tolerance);
	  Volume l_vol(lcdd,l_name,l_box,air);
	  DetElement layer(lcdd,l_name,det_type+"/Layer",det_id);

	  layer.setVolume(l_vol).setEnvelope(l_box);
	  sdet.add(layer);

	  // Loop over the sublayers or slices for this layer.
	  int s_num = 0;
	  double s_pos_z = -(l_thickness / 2);
	  for(xml_coll_t si(x_layer,_X(slice)); si; ++si)  {
	    xml_comp_t x_slice = si;
	    string     s_name = l_name + _toString(s_num,"_slice%d");
	    double     s_thickness = x_slice.thickness();
	    // Increment Z position of slice.
	    s_pos_z += s_thickness / 2;
	    Position   s_pos(lcdd,s_name+"_position",0,0,s_pos_z);
	    Box        s_box(lcdd,s_name+"_box",l_dim_x*2 - tolerance,z*2 - tolerance, s_thickness - tolerance);
	    Volume     s_vol(lcdd,s_name,s_box,lcdd.material(x_slice.materialStr()));
	    DetElement slice(lcdd,s_name,det_type+"/Layer/Slice",det_id);

	    slice.setVolume(s_vol).setEnvelope(s_box);
	    layer.add(slice);
	    if ( x_slice.isSensitive() ) s_vol.setSensitiveDetector(sens);
	    
	    slice.setAttributes(lcdd,s_vol,
				x_slice.attr<string>(_A(region)),
				x_slice.attr<string>(_A(limits)),
				x_slice.visStr());

	    // Slice placement.
	    PhysVol slice_physvol(s_vol);
	    slice_physvol.addPhysVolID("layer", l_num);
	    slice_physvol.addPhysVolID("slice", s_num);
	    l_vol.addPhysVol(slice_physvol,s_pos,irot);					
					
	    // Increment Z position of slice.
	    s_pos_z += s_thickness / 2;
					
	    // Increment slice number.
	    ++s_num;
	  }
	  
	  // Set region, limitset, and vis of layer.
	  layer.setAttributes(lcdd,l_vol,
			      x_layer.attr<string>(_A(region)),
			      x_layer.attr<string>(_A(limits)),
			      x_layer.visStr());

	  // Place the layer.
	  PhysVol layer_physvol(l_vol);
	  layer_physvol.addPhysVolID("layer", l_num);
	  mod_vol.addPhysVol(layer_physvol,l_pos,irot);
	  // Increment to next layer Z position.
	  l_pos_z += l_thickness / 2;	  
	  // Increment layer number.
	  ++l_num;
	}
      }
      l_num++;
    }

    // Set stave visualization.
    if (x_staves)   {
      mod_vol.setVisAttributes(lcdd.visAttributes(x_staves.visStr()));
    }
    // Phi start for a stave.
    double phi = M_PI / nsides;

    // Create nsides staves.
    for (int i = 0; i < nsides; i++)      {			
      // Module number.
      int m_num = i;
      // Compute the stave position; derived from calculations in Mokka 
      // Geometry/Tesla/Ecal02.cc
      double m_pos_x = mod_x_off * std::cos(phi) - mod_y_off * std::sin(phi);
      double m_pos_y = mod_x_off * std::sin(phi) + mod_y_off * std::cos(phi);
      double m_pos_z = 0;
      Position m_pos(lcdd,det_name+_toString(m_num,"_module%d_position"),m_pos_x, m_pos_y,m_pos_z);
      Rotation m_rot(lcdd,det_name+_toString(m_num,"_module%d_rotation"),M_PI * 0.5, phi, 0);

      PhysVol pv(mod_vol);
      pv.addPhysVolID("module",m_num);
      pv.addPhysVolID("system",det_id);
      pv.addPhysVolID("barrel", 0);
      envelope.addPhysVol(pv,lcdd.position("identity_pos"),m_rot);
      // increment phi
      phi -= dphi;
    }
#endif
    // Set envelope volume attributes.
    sdet.setAttributes(lcdd, envelope,
		       x_det.attr<string>(_A(region)),
		       x_det.attr<string>(_A(limits)),
		       x_det.visStr());
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,EcalBarrel);
