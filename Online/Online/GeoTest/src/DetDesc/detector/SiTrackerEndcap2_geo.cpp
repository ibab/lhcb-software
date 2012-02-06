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
  
  template <> Ref_t DetElementFactory<SiTrackerEndcap2>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t   x_det     = e;
    Material    air       = lcdd.material(_X(Air));
    Material    vacuum    = lcdd.material(_X(Vacuum));
    int         sysID     = x_det.id();
    string      det_name  = x_det.nameStr();
    string      det_type  = x_det.typeStr();
    Tube        envelope   (lcdd,det_name+"_envelope");
    Volume      envelopeVol(lcdd,det_name+"_envelope_volume",envelope,air);
    DetElement  sdet       (lcdd,det_name,det_type,sysID);
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
      DetElement module (lcdd,m_nam,det_type+"/Module",sysID);

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
      Trapezoid m_envelope(lcdd, m_nam+"Trd", x1, x2, y1, y2, z);
      Volume    m_volume(lcdd, vol_nam, m_envelope, vacuum);

      module.setVolume(m_volume).setEnvelope(m_envelope);
      m_volume.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));

      for(ci.reset(), n_sensor=0, c_id=0, posY=-y1; ci; ++ci, ++c_id)  {
	xml_comp_t c       = ci;
	double     c_thick = c.thickness();
	Material   c_mat   = lcdd.material(c.materialStr());
	string     c_name  = m_nam + _toString(c_id,"_component%d");
	DetElement component(lcdd,c_name,det_type+"/Module/Component",sysID);
	Trapezoid trd(lcdd, c_name+"_trd", x1, x2, c_thick/2e0, c_thick/2e0, z);
	Volume    vol(lcdd, c_name, trd, c_mat);
	Position  pos(lcdd, c_name+"_position", 0e0, posY + c_thick/2e0, 0e0);
	Rotation  rot(lcdd, c_name+"_rotation");

	component.setVolume(vol).setEnvelope(trd);
	vol.setVisAttributes(lcdd.visAttributes(c.visStr()));

	PhysVol   phv(vol);
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
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,SiTrackerEndcap2);
