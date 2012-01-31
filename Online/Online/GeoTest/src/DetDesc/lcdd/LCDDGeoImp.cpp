#define _USE_MATH_DEFINES
#include "LCDDGeoImp.h"
#include "../compact/Conversions.h"
#include "XML/DocumentHandler.h"

#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/framework/StdOutFormatTarget.hpp"

#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/sax/EntityResolver.hpp"
#include "xercesc/sax/InputSource.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/util/XMLUni.hpp"
#include "xercesc/util/XMLURL.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/ErrorHandler.hpp"

// C+_+ include files
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

#include "XML/Evaluator.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;
namespace DetDesc  { XmlTools::Evaluator& evaluator();  }
namespace DetDesc  { namespace XML {    void tags_init(); }}

LCDD& LCDD::getInstance() {
  static LCDD* s_lcdd = new LCDDImp();
  return *s_lcdd; 
}

LCDDImp::LCDDImp() : m_worldVol(), m_trackingVol(), m_reflect(), m_identity()  {
  evaluator();
  XML::tags_init();
}

Volume LCDDImp::pickMotherVolume(const Subdetector&) const  {     // throw if not existing
  return m_worldVol;
}

Element LCDDImp::getRefChild(const HandleMap& e, const std::string& name, bool do_throw)  const  {
  HandleMap::const_iterator i = e.find(name);
  if ( i != e.end() )  {
    return (*i).second;
  }
  if ( do_throw )  {
    throw runtime_error("Cannot find a child with the reference name:"+name);
  }
  return 0;
}

template<class T> LCDD& 
LCDDImp::__add(const RefElement& x, ObjectHandleMap& m,Int_t (TGeoManager::*func)(T*))  {
  T* obj = dynamic_cast<T*>(x.ptr());
  if ( obj )  {
    m.append(x);
    if ( func ) (gGeoManager->*func)(obj);
    return *this;
  }
  throw InvalidObjectError("Attempt to add an object, which is of the wrong type.");
}

LCDD& LCDDImp::addVolume(const RefElement& x)    {
  //return __add(x,m_structure,&TGeoManager::AddVolume);
  m_structure.append(x);
  return *this;
}
#include "TGeoTube.h"
#include "TGeoPcon.h"
#include "TGeoCompositeShape.h"
LCDD& LCDDImp::addSolid(const RefElement& x)     {
  TGeoShape *o, *obj = dynamic_cast<TGeoShape*>(x.ptr());
  if ( (o=dynamic_cast<TGeoPcon*>(obj)) )  {
    m_structure.append<TGeoShape>(x);
  }
  else if ( (o=dynamic_cast<TGeoTube*>(obj)) )  {
    m_structure.append<TGeoShape>(x);
  }
  else if ( (o=dynamic_cast<TGeoCompositeShape*>(obj)) )  {
    m_structure.append<TGeoShape>(x);
  }
  else  {
    m_structure.append<TGeoShape>(x);
    //__add(x,m_structure,&TGeoManager::AddShape);
  }
#if 0
  cout << obj->GetName() << " " << (void*)obj
    << " Index:" << gGeoManager->GetListOfShapes()->IndexOf(obj) << endl;
#endif
  return *this;
}

LCDD& LCDDImp::addRotation(const RefElement& x)  {
  TGeoMatrix* obj = dynamic_cast<TGeoMatrix*>(x.ptr());
  if ( 0 == obj ) {
    cout << "+++ Attempt to store invalid matrix object:" << typeid(*x.ptr()).name() << endl;
  }
  if ( obj->IsRegistered() ) {
    cout << "+++ Attempt to register already registered matrix:" << obj->GetName() << endl;
  }
  // cout << "+++ Register Rotation[" << m_doc->GetListOfMatrices()->GetEntries() << "]:" << obj->GetName() << endl;

  return __add(x,m_rotations,&TGeoManager::AddTransformation);
  //m_rotations.append<TGeoMatrix>(x);
  //return *this;
}

LCDD& LCDDImp::addPosition(const RefElement& x)  {
  TGeoMatrix* obj = dynamic_cast<TGeoMatrix*>(x.ptr());
  if ( 0 == obj ) {
    cout << "+++ Attempt to store invalid matrix object:" << typeid(*x.ptr()).name() << endl;
  }
  if ( obj->IsRegistered() ) {
    cout << "+++ Attempt to register already registered matrix:" << obj->GetName() << endl;
  }
  /// cout << "+++ Register Position [" << m_doc->GetListOfMatrices()->GetEntries() << "]:" << obj->GetName() << endl;
  return __add(x,m_positions,&TGeoManager::AddTransformation);
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

void LCDDImp::create()  {
  gGeoManager = new TGeoManager();
}

void LCDDImp::init()  {
  LCDD& lcdd = *this;

  m_identity = Transformation(lcdd,"identity");
  Box worldSolid(lcdd,"world_box","world_x","world_y","world_z");
  Rotation identity_rot(lcdd,"identity_rot",0,0,0);
  Position identity_pos(lcdd,"identity_pos",0,0,0);
  m_reflect = Rotation(lcdd,"reflect_rot",M_PI,0.,0.);

  Material air = material("Air");
  Volume world(lcdd,"world_volume",worldSolid,air);

  Tube trackingSolid(lcdd,"tracking_cylinder",
		     0.,
		     _toDouble("tracking_region_radius"),
		     _toDouble("2*tracking_region_zmax"),M_PI);
  Volume tracking(lcdd,"tracking_volume",trackingSolid,air);
//  world.addPhysVol(PhysVol(lcdd,tracking,"tracking_volume"));

  //RefElement ref_world(lcdd,"world",world.refName());
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
  Printer<const LCDD*>(*this,cout)(this);
}

