// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "LCDDGeoImp.h"
#include "../compact/Conversions.h"
#include "XML/DocumentHandler.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

#include "TGeoManager.h"
#include "TGeoMatrix.h"

namespace DetDesc  { namespace Geometry { struct Compact; }}
using namespace DetDesc::Geometry;
using namespace DetDesc;
using namespace std;

LCDD& LCDD::getInstance() {
  static LCDD* s_lcdd = new LCDDImp();
  return *s_lcdd; 
}

LCDDImp::LCDDImp() : m_worldVol(), m_trackingVol(), m_reflect(), m_identity()  {
}

Volume LCDDImp::pickMotherVolume(const Subdetector&) const  {     // throw if not existing
  return m_worldVol;
}

LCDD& LCDDImp::addVolume(const Ref_t& x)    {
  m_structure.append(x);
  return *this;
}

LCDD& LCDDImp::addSolid(const Ref_t& x)    {
  m_structure.append<TGeoShape>(x);
  return *this;
}

LCDD& LCDDImp::addTransform(const Ref_t& x)  {
  //int n = gGeoManager->GetListOfMatrices()->GetEntries();
  //cout << "+++ Register Rotation[" << n << "]:" << x->GetName() << endl;
  m_transforms.append<TGeoMatrix>(x);
  return *this;
}

void LCDDImp::convertMaterials(const string& fname)  {
  convertMaterials(XML::DocumentHandler().load(fname).root());
}

void LCDDImp::convertMaterials(XML::Handle_t materials)  {
  Converter<Materials>(*this)(materials);
}

void LCDDImp::addStdMaterials()   {
  convertMaterials("file:../cmt/elements.xml");
  convertMaterials("file:../cmt/materials.xml");
}

void LCDDImp::fromCompact(const string& fname)  {
  fromCompact(XML::DocumentHandler().load(fname).root());
}

Rotation LCDDImp::rotation(const std::string& name) const {
  Rotation rot = getRefChild(m_transforms,name,false);
  if ( !rot.isValid() ) {
    TObjArray*  arr = gGeoManager->GetListOfMatrices();
    TGeoMatrix* mat = (TGeoMatrix*)arr->FindObject(name.c_str());
    if ( mat ) return Rotation(Transform(mat));
  }
  return rot;
}

Position LCDDImp::position(const std::string& name) const {
  Position pos = getRefChild(m_transforms,name,false);
  if ( !pos.isValid() ) {
    TObjArray*  arr = gGeoManager->GetListOfMatrices();
    TGeoMatrix* mat = (TGeoMatrix*)arr->FindObject(name.c_str());
    if ( mat ) return Position(Transform(mat));
  }
  return pos;
}

Transform LCDDImp::transform(const std::string& name) const {
  Transform tr = getRefChild(m_transforms,name,false);
  if ( !tr.isValid() ) {
    TObjArray*  arr = gGeoManager->GetListOfMatrices();
    TGeoMatrix* mat = (TGeoMatrix*)arr->FindObject(name.c_str());
    if ( mat ) return Transform(mat);
  }
  return tr;
}

Handle<TObject> LCDDImp::getRefChild(const HandleMap& e, const std::string& name, bool do_throw)  const  {
  HandleMap::const_iterator i = e.find(name);
  if ( i != e.end() )  {
    return (*i).second;
  }
  if ( do_throw )  {
    throw runtime_error("Cannot find a child with the reference name:"+name);
  }
  return 0;
}

void LCDDImp::endDocument()  {
  LCDD& lcdd = *this;
  Volume world  = volume("world_volume");
  Volume trkVol = volume("tracking_volume");
  Material  air = material("Air");

  world.setMaterial(air);
  trkVol.setMaterial(air);

  Region trackingRegion(lcdd,"TrackingRegion");
  trackingRegion.setThreshold(1);
  trackingRegion.setStoreSecondaries(true);
  add(trackingRegion);
  trkVol.setRegion(trackingRegion);
    
  // Set the world volume to invisible.
  VisAttr worldVis(lcdd,"WorldVis");
  worldVis.setVisible(false);
  world.setVisAttributes(worldVis);
  add(worldVis);
  
  // Set the tracking volume to invisible.
  VisAttr trackingVis(lcdd,"TrackingVis");
  trackingVis.setVisible(false);               
  trkVol.setVisAttributes(trackingVis);
  add(trackingVis); 
}

void LCDDImp::create()  {
  gGeoManager = new TGeoManager();
}

void LCDDImp::init()  {
  LCDD& lcdd = *this;

  m_identity = Transform(lcdd,"identity");
  Box worldSolid(lcdd,"world_box","world_x","world_y","world_z");
  Rotation identity_rot(lcdd,"identity_rot",0,0,0);
  Rotation identity_rot_rev(lcdd,"inverse_identity_rot",0,0,0);
  Position identity_pos(lcdd,"identity_pos",0,0,0);
  m_reflect = Rotation(lcdd,"reflect_rot",M_PI,0.,0.);

  *(identity_rot_rev.ptr()) = identity_rot_rev->Inverse();

  Material air = material("Air");
  Volume world(lcdd,"world_volume",worldSolid,air);

  Tube trackingSolid(lcdd,"tracking_cylinder",
		     0.,
		     _toDouble("tracking_region_radius"),
		     _toDouble("2*tracking_region_zmax"),M_PI);
  Volume tracking(lcdd,"tracking_volume",trackingSolid,air);
//  world.addPhysVol(PhysVol(lcdd,tracking,"tracking_volume"));

  //Ref_t ref_world(lcdd,"world",world.refName());
  //m_setup.append(ref_world);
  m_worldVol    = world;
  m_trackingVol = tracking;
  gGeoManager->SetTopVolume(m_worldVol);
}

void LCDDImp::fromCompact(XML::Handle_t compact)   {
  try {
    Converter<Compact>(*this)(compact);
  }
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
  }
  catch(xercesc::DOMException& e)  {
    cout << "XML-DOM Exception:" << XML::_toString(e.msg) << endl;
  } 
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
  }
}

void LCDDImp::dump() const  {
  TGeoManager* mgr = gGeoManager;
  mgr->CloseGeometry();
  mgr->SetVisLevel(4);
  mgr->SetVisOption(1);
  m_worldVol->Draw("ogl");
  cout << "Total number of object verifications:" << num_object_validations() << endl;
  Printer<const LCDD*>(*this,cout)(this);
}

