#include "XML/lcdd/XMLDetector.h"
#include "XML/lcdd/XMLObjects.h"
#include "XML/lcdd/XMLVolumes.h"
#include "XML/lcdd/XMLLCDD.h"
#include "xercesc/dom/DOM.hpp"

using namespace std;
using namespace DetDesc::XML;

double Dimension::x() const  {
  return m_element.attr<double>(Attr_x);
}

double Dimension::y() const  {
  return m_element.attr<double>(Attr_y);
}

double Dimension::z() const  {
  return m_element.attr<double>(Attr_z);
}

double Dimension::x(double default_val) const  {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_x);
  return val ? _toDouble(val) : default_val;
}

double Dimension::y(double default_val) const  {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_y);
  return val ? _toDouble(val) : default_val;
}

double Dimension::z(double default_val) const  {
  const XMLCh* val = m_element.attr_value_nothrow(Attr_z);
  return val ? _toDouble(val) : default_val;
}

double Dimension::x1() const  {
  return m_element.attr<double>(Attr_x1);
}

double Dimension::x2() const  {
  return m_element.attr<double>(Attr_x2);
}

double Dimension::y1() const  {
  return m_element.attr<double>(Attr_y1);
}

double Dimension::y2() const  {
  return m_element.attr<double>(Attr_y2);
}

double Dimension::rmin() const  {
  return m_element.attr<double>(Attr_rmin);
}

double Dimension::zmin() const  {
  return m_element.attr<double>(Attr_zmin);
}

double Dimension::rmax() const  {
  return m_element.attr<double>(Attr_rmax);
}

double Dimension::deltaphi() const  {
  return m_element.attr<double>(Attr_deltaphi);
}

double Dimension::outer_z() const  {
  return m_element.attr<double>(Attr_outer_z);
}

double Dimension::outer_r() const  {
  return m_element.attr<double>(Attr_outer_r);
}

double Dimension::inner_z() const  {
  return m_element.attr<double>(Attr_inner_z);
}

double Dimension::inner_r() const  {
  return m_element.attr<double>(Attr_inner_r);
}

bool   Dimension::reflect() const  {
  return m_element.attr<bool>(Attr_reflect);
}

double Dimension::gap() const  {
  return m_element.attr<double>(Attr_gap);
}

double Dimension::z_length() const  {
  return m_element.attr<double>(Attr_z_length);
}

double Dimension::zhalf() const  {
  return m_element.attr<double>(Attr_zhalf);
}

double Dimension::length() const  {
  return m_element.attr<double>(Attr_length);
}

double Dimension::width() const  {
  return m_element.attr<double>(Attr_width);
}

int Dimension::numsides() const  {
  return m_element.attr<int>(Attr_numsides);
}

double Dimension::r_size() const  {
  return m_element.attr<int>(Attr_r_size);
}

double Dimension::phi_size_max() const  {
  return m_element.attr<int>(Attr_phi_size_max);
}

double Dimension::outgoing_r() const  {
  return m_element.attr<double>(Attr_outgoing_r);
}

double Dimension::incoming_r() const  {
  return m_element.attr<double>(Attr_incoming_r);
}

double Dimension::crossing_angle() const  {
  return m_element.attr<double>(Attr_crossing_angle);
}

double Dimension::radius() const {
  return m_element.attr<double>(Attr_radius);
}

double Dimension::offset() const {
  return m_element.attr<double>(Attr_offset);
}

int Dimension::number() const {
  return m_element.attr<int>(Attr_number);
}

double Dimension::phi0() const {
  return m_element.attr<double>(Attr_phi0);
}

double Dimension::phi_tilt() const {
  return m_element.attr<double>(Attr_phi_tilt);
}

int Dimension::nphi() const {
  return m_element.attr<int>(Attr_nphi);
}

double Dimension::rc()  const {
  return m_element.attr<double>(Attr_rc);
}

double Dimension::dr()  const {
  return m_element.attr<double>(Attr_dr);
}

double Dimension::z0() const {
  return m_element.attr<double>(Attr_z0);
}

int    Dimension::nz() const {
  return m_element.attr<int>(Attr_nz);
}

int DetElement::Component::id()  const  {
  return m_element.attr<int>(Attr_id);
}

const XMLCh*  DetElement::Component::name()  const  {
  return m_element.attr<cpXMLCh>(Attr_name);
}

string  DetElement::Component::nameStr()  const  {
  return m_element.attr<string>(Attr_name);
}

string DetElement::Component::materialStr() const   {
  return m_element.attr<string>(Attr_material);
}

const XMLCh* DetElement::Component::module()  const  {
  return m_element.attr<cpXMLCh>(Attr_module);
}

string  DetElement::Component::moduleStr()  const  {
  return m_element.hasAttr(Attr_module) ? m_element.attr<string>(Attr_module) : string();
}

int DetElement::Component::repeat()  const  {
  return m_element.attr<int>(Attr_repeat);
}

double DetElement::Component::thickness() const  {
  return m_element.attr<double>(Attr_thickness);
}

bool DetElement::Component::isSensitive() const  {
  return m_element.hasAttr(Attr_sensitive) && m_element.attr<bool>(Attr_sensitive);
}

const XMLCh* DetElement::Component::material() const   {
  return m_element.attr<cpXMLCh>(Attr_material);
}

const  XMLCh* DetElement::Component::vis() const   {
  return m_element.hasAttr(Attr_vis) ? m_element.attr<cpXMLCh>(Attr_vis) : 0;
}

string  DetElement::Component::visStr()  const  {
  return m_element.hasAttr(Attr_vis) ? m_element.attr<string>(Attr_vis) : string();
}

int DetElement::id() const   {
  Attribute a = m_element.attr_nothrow(Attr_id);
  return a ? m_element.attr<int>(Attr_id) : -1;
}

const XMLCh* DetElement::name() const   {
  return m_element.attr<cpXMLCh>(Attr_name);
}

string DetElement::nameStr() const   {
  return m_element.attr<string>(Attr_name);
}

const XMLCh*  DetElement::type()  const  {
  return m_element.attr<cpXMLCh>(Attr_type);
}

string  DetElement::typeStr()  const  {
  return m_element.attr<string>(Attr_type);
}

const XMLCh*  DetElement::vis()  const  {
  return m_element.attr<cpXMLCh>(Attr_vis);
}

string  DetElement::visStr()  const  {
  return m_element.attr<string>(Attr_vis);
}

Dimension DetElement::dimensions()  const  {
  return Dimension(m_element.child(Tag_dimensions));
}

const XMLCh* DetElement::material() const  {
  Handle_t   h = m_element.child(Tag_material);
  if ( h && h.hasAttr(Attr_name) )  {
    return h.attr<cpXMLCh>(Attr_name);
  }
  return 0;
}

string DetElement::materialStr() const  {
  Handle_t   h = m_element.child(Tag_material);
  if ( h && h.hasAttr(Attr_name) )  {
    return h.attr<string>(Attr_name);
  }
  return "";
}

void DetElement::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

void DetElement::setAttributes(const LCDD& lcdd, Handle_t node, const Volume& volume)  {
  setRegion(lcdd, node, volume);
  setLimitSet(lcdd, node, volume);
  setVisAttributes(lcdd, node, volume);
}

void DetElement::setVisAttributes(const LCDD& lcdd, Handle_t node, const Volume& volume)  {
  Attribute attr = node.attr_nothrow(Attr_vis);
  if ( attr )   {
    volume.setVisAttributes(lcdd.visAttributes(attr->getValue()));
  }
  else  {
    string tag = node.tag();
    if ( tag == Tag_slice.str() )  // Slices turned off by default
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleNoDaughters));
    else if ( tag == Tag_layer.str() )  // Layers turned off, but daaughters possibly visible
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleWithDaughters));
    else if ( tag == Tag_module.str() )  // Traker modules similar to layers
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleWithDaughters));
    else if ( tag == Tag_module_component.str() )  // Traker modules components turned off by default
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleNoDaughters));
  }
}

void DetElement::setRegion(const LCDD& lcdd, Handle_t node, const Volume& volume)  {
  Attribute attr = node.attr_nothrow(Attr_region);
  if ( attr )  {
    volume.setRegion(lcdd.region(attr->getValue()));
  }
}

void DetElement::setLimitSet(const LCDD& lcdd, Handle_t node, const Volume& volume)  {
  Attribute attr = node.attr_nothrow(Attr_limits);
  if ( attr )  {
    volume.setLimitSet(lcdd.limitSet(attr->getValue()));
  }
}

void DetElement::setCombineHits(Handle_t node, const SensitiveDetector& sens)   {
  if ( node.hasAttr(Attr_combineHits) )  {
    if ( isTracker() )  {
      sens.setAttr(Attr_combine_hits,node.attr<Attribute>(Attr_combineHits));
    }
  }
}

bool DetElement::isTracker() const   {
  if ( m_element )  {
    string type = attr<string>(Attr_type);
    if ( type.find("Tracker") != string::npos && hasAttr(Attr_readout) ) {
      return true;
    }
  }
  return false;
}

bool DetElement::isCalorimeter() const   {
  if ( m_element )  {
    string type = attr<string>(Attr_type);
    if ( type.find("Calorimeter") != string::npos && hasAttr(Attr_readout) ) {
        return true;
    }
  }
  return false;
}

bool DetElement::isInsideTrackingVolume() const  {
  if ( m_element && hasAttr(Attr_insideTrackingVolume) )
    return attr<bool>(Attr_insideTrackingVolume);
  else if ( isTracker() )
    return true;
  return false;
}

SensitiveDetector::SensitiveDetector(const Document& doc, const XMLCh* type, const XMLCh* name) 
: RefElement(doc, type, name)
{
  setAttr(Attr_ecut,Tag_NULL);
  setAttr(Attr_eunit,"MeV");
  setAttr(Attr_verbose,Tag_NULL);
}

void SensitiveDetector::setIDSpec(const RefElement& spec)  {
  Handle_t e = document().createElt(Tag_idspecref);
  e.setAttr(Attr_ref,spec.refName());
  append(e);
}

void SensitiveDetector::setHitsCollection(const RefElement& spec)  {
  setAttr(Attr_hits_collection,spec.refName());
}

void SensitiveDetector::setSegmentation(Element seg)   {
  if ( seg )  {
    Handle_t h = m_element.child(seg.tagName(),false);
    if ( h ) m_element->replaceChild(h,seg);
    else m_element.append(seg);
    return;
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}

const XMLCh* CompactDetector::readout()  const  {
  return hasAttr(Attr_readout) ? attr<cpXMLCh>(Attr_readout) : 0;
}

const XMLCh* CompactDetector::name() const  {
  return attr<cpXMLCh>(Attr_name);
}

const XMLCh* CompactDetector::type() const  {
  return attr<cpXMLCh>(Attr_type);
}
