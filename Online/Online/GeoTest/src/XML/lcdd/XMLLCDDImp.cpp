#include "XMLLCDDImp.h"
#include "XML/compact/XMLReadouts.h"

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

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

using namespace xercesc;
using namespace std;
using namespace DetDesc;
using namespace DetDesc::XML;

LCDDImp::LCDDImp() : DocumentHandler()  {
  m_root    = 0;
  m_idDict  = 0;
  m_limits  = 0;
  m_regions = 0;
  m_display = 0;
  m_gdml    = 0;
  m_fields  = 0;
  m_structure = 0;
  m_materials = 0;
  m_solids = 0;
}

LCDD& LCDDImp::addReadout(const RefElement& readout) {
  m_readouts[_toString(readout.name())] = readout; 
  return *this;
}

RefElement LCDDImp::getRefChild(Handle_t e, const XMLCh* name, bool do_throw)  const  {
  string nam = _toString(name);
  for(Collection_t c(e->getChildNodes()); c; ++c)   {
    string tag = c.attr<string>(Attr_name);
    if ( tag == nam )  {
     return RefElement(c.current());
    }
  }
  if ( do_throw )  {
    throw runtime_error("Cannot find a child with the reference name:"+nam);
  }
  return RefElement(0);
}

RefElement LCDDImp::getRefChild(Handle_t e, const XMLCh* tag, const XMLCh* name, bool do_throw)  const {
  string nam = _toString(name);
  for(Collection_t c(e,tag); c; ++c)   {
    string tag = c.attr<string>(Attr_name);
    if ( tag == nam )  {
     return RefElement(c.current());
    }
  }
  if ( do_throw )  {
    throw runtime_error("Cannot find a child with the reference name:"+nam);
  }
  return RefElement(0);
}


RefElement LCDDImp::pickMotherVolume(const Subdetector& sd) const  {     // throw if not existing
  return RefElement(m_worldVol);
}

void LCDDImp::endDocument()  {
  Document doc  = document(); 
  Volume world  = volume(Tag_world_volume);
  Volume trkVol = volume(Tag_tracking_volume);
  Material  air = material(Tag_Air);

  world.setMaterial(air);
  trkVol.setMaterial(air);

  // Move to end
  structure().remove(trkVol);
  structure().append(trkVol);
  Region trackingRegion(doc,Strng_t("TrackingRegion"));
  trackingRegion.setThreshold(1);
  trackingRegion.setStoreSecondaries(true);
  add(trackingRegion);
  trkVol.setRegion(trackingRegion);
    
  // Move to end
  structure().remove(world);
  structure().append(world);

  // Set the world volume to invisible.
  VisAttr worldVis(document(),Strng_t("WorldVis"));
  worldVis.setVisible(false);
  world.setVisAttributes(worldVis);
  add(worldVis);
  
  // Set the tracking volume to invisible.
  VisAttr trackingVis(document(),Strng_t("TrackingVis"));
  trackingVis.setVisible(false);               
  trkVol.setVisAttributes(trackingVis);
  add(trackingVis); 
}

RefElement LCDDImp::readout(const XMLCh* name)  const  {
  ReadoutMap::const_iterator i=m_readouts.find(_toString(name));
  if ( i != m_readouts.end() )  {
    return Readout((*i).second);
  }
  return Readout(0);
}

void LCDDImp::convertMaterials(const string& fname)  {
  convertMaterials(load(fname).root());
}

void LCDDImp::fromCompact(const string& fname)  {
  fromCompact(load(fname).root());
}

void LCDDImp::convertMaterials(Handle_t materials)  {
  XMLConverter<Materials>(*this)(materials);
}

void LCDDImp::addStdMaterials()   {
  convertMaterials("file:../cmt/elements.xml");
  convertMaterials("file:../cmt/materials.xml");
}

Document LCDDImp::create()  {
  #define ns_location "http://www.lcsim.org.schemas/lcdd/1.0"

  const XMLByte empty_lcdd[] =
    "<!--                                                               \n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "      ++++   Linear collider detector description LCDD in C++  ++++\n"
    "      ++++                                                     ++++\n"
    "      ++++                              M.Frank CERN/LHCb      ++++\n"
    "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    "-->\n"
    "<lcdd \n"
    "xmlns:lcdd=\"" ns_location "\"\n"
    "xmlns:xs=\"http://www/w3.org/2001/XMLSchema-instance\"\n"
    ">\n"
    "</lcdd>";

  Document doc = parse(empty_lcdd,sizeof(empty_lcdd));
  doc->setXmlStandalone(true);
  doc->setStrictErrorChecking(true);
  m_doc  = doc;
  m_root = doc.root();
  m_root->setAttributeNS(Strng_t("http://www/w3.org/2001/XMLSchema-instance"),
                         Strng_t("xs:noNamespaceSchemaLocation"),
                         Strng_t(ns_location "/lcdd.xsd"));

  Box worldSolid(doc,Tag_world_box);
  m_root.append(m_header    = Header(doc));
  m_root.append(m_idDict    = Element(doc,Tag_iddict));
  m_root.append(m_detectors = Element(doc,Tag_sensitive_detectors));
  m_root.append(m_limits    = Element(doc,Tag_limits));
  m_root.append(m_regions   = Element(doc,Tag_regions));
  m_root.append(m_display   = Element(doc,Tag_display));
  m_root.append(m_gdml      = Element(doc,Tag_gdml));
  m_root.append(m_fields    = Element(doc,Tag_fields));

  m_gdml.append(m_define    = Element(doc,Tag_define));
  m_gdml.append(m_materials = Element(doc,Tag_materials));
  m_gdml.append(m_solids    = Element(doc,Tag_solids));
  m_gdml.append(m_structure = Element(doc,Tag_structure));
  m_gdml.append(m_setup     = Element(doc,Tag_setup));

  worldSolid.setAttr(Attr_x,"world_x");
  worldSolid.setAttr(Attr_y,"world_y");
  worldSolid.setAttr(Attr_z,"world_z");
  add(worldSolid);

  add(Rotation(doc,Tag_identity_rot));
  add(Rotation(doc,Tag_reflect_rot,Tag_PI,Tag_NULL,Tag_NULL));
  add(Position(doc,Tag_identity_pos));

  Volume world(doc,Tag_world_volume);
  add(world);
  world.append(Element(doc,Tag_materialref));
  //node.setAttr("ref",m_???
  world.append(Element(doc,Tag_solidref));

  Tube trackingSolid(doc,Tag_tracking_cylinder);
  trackingSolid.setAttr(Attr_rmax,"tracking_region_radius");
  trackingSolid.setAttr(Attr_z,"2*tracking_region_zmax");
  trackingSolid.setAttr(Attr_deltaphi,Tag_TWOPI);
  add(trackingSolid);

  Volume tracking(doc,Tag_tracking_volume);
  add(tracking);
  tracking.setSolid(trackingSolid);
  world.addPhysVol(PhysVol(doc,tracking));

  Element ref_world(doc,Tag_world);
  ref_world.setAttr(Attr_ref,world.refName());
  m_setup.append(ref_world);
  m_worldVol = world;
  m_trackingVol = tracking;
  return doc;
}

void LCDDImp::fromCompact(Handle_t compact)   {
  try {
    XMLConverter<Compact>(*this)(compact);
  }
  catch(const exception& e)  {
    cout << "Exception:" << e.what() << endl;
  }
  catch(xercesc::DOMException& e)  {
    cout << "XML-DOM Exception:" << _toString(e.msg) << endl;
  } 
  catch(...)  {
    cout << "UNKNOWN Exception" << endl;
  }
}
