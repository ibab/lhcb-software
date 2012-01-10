#include "XML/lcdd/XMLVolumes.h"
#include "XML/lcdd/XMLShapes.h"
#include "XML/lcdd/XMLObjects.h"

using namespace std;
using namespace DetDesc::XML;

Volume::Volume(const Document& document, const XMLCh* name)
: RefElement(document, Tag_volume, name)
{
}

Volume::Volume(const Document& document, const XMLCh* name, const Solid& s, const Material& m)   
: RefElement(document, Tag_volume, name)
{
  setSolid(s);
  setMaterial(m);
}

Volume::Volume(Handle_t e) : RefElement(e)  {
}

Attribute Volume::setMaterial(const Material& m)  const  {
  return setRef(Tag_materialref,m.refName()); 
}

Attribute Volume::setSolid(const Solid& s)  const  {
  return setRef(Tag_solidref,s.refName()); 
}

void Volume::addPhysVol(const PhysVol& volume)  const   {
  m_element.append(volume);
}

void Volume::setRegion(const RefElement& obj)  const   {
  addChild(Tag_regionref).setAttr(Attr_ref,obj.refName());
}

void Volume::setLimitSet(const RefElement& obj)  const   {
  addChild(Tag_limitsetref).setAttr(Attr_ref,obj.refName());
}

void Volume::setSensitiveDetector(const RefElement& obj) const  {
  addChild(Tag_sdref).setAttr(Attr_ref,obj.refName());
}

void Volume::setVisAttributes(const RefElement& obj) const   {
  if ( obj )  {
    addChild(Tag_visref).setAttr(Attr_ref,obj.refName());
  }
}

const XMLCh* Volume::solidRef() const  {
  return getRef(Tag_solidref);
}

/// Constructor to be used when creating a new DOM tree
PhysVol::PhysVol(const Document& document, Volume vol) : Element(document,Tag_physvol) {
  setChild(Tag_volumeref).setAttr(Attr_ref,vol.refName());
  setChild(Tag_positionref).setAttr(Attr_ref,Tag_identity_pos);
  setChild(Tag_rotationref).setAttr(Attr_ref,Tag_identity_rot);
}

/// Constructor to be used when creating a new DOM tree with additional parameters
PhysVol::PhysVol(const Document& document, Volume vol, const Position& pos, const Rotation& rot)
: Element(document,Tag_physvol)
{
  setChild(Tag_volumeref).setAttr(Attr_ref,vol.refName());
  setChild(Tag_positionref).setAttr(Attr_ref,pos.refName());
  setChild(Tag_rotationref).setAttr(Attr_ref,rot.refName());
}

PhysVol& PhysVol::addPhysVolID(const XMLCh* name, int value)  {
  Handle_t e = addChild(Tag_physvolid);
  e.setAttr(Attr_field_name, name);
  e.setAttr(Attr_value, value);
  return *this;
}

PhysVol& PhysVol::setZ(double value)   {
  Position pos = setChild(Tag_position);
  if ( m_element.hasChild(Tag_positionref) )
    m_element.removeChildren(Tag_positionref);
  setAttr(Attr_z,value);
  return *this;
}

PhysVol& PhysVol::setRotation(const RefElement& rot)   {
  child(Tag_rotationref).setAttr(Attr_ref,rot.refName());
  return *this;
}

PhysVol& PhysVol::setPosition(const RefElement& pos)   {
  child(Tag_positionref).setAttr(Attr_ref,pos.refName());
  return *this;
}
