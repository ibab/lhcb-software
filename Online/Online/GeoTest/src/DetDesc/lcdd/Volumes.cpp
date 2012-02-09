// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/lcdd/LCDD.h"

// ROOT include files
#include "TColor.h"
#include "TGeoShape.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"

// C/C++ include files
#include <climits>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DetDesc::Geometry;

namespace DetDesc  { namespace Geometry  {
  template <> struct Value<TGeoVolume,Volume::Object> 
    : public TGeoVolume, public Volume::Object  
  {
    Value(const char* name, TGeoShape* s=0, TGeoMedium* m=0) 
      : TGeoVolume(name,s,m) {}
    virtual ~Value() {}
  };
}}

Volume::Volume(LCDD& lcdd, const string& name)    {
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str());
  lcdd.addVolume(*this);
}

Volume::Volume(LCDD& lcdd, const string& name, const Solid& s, const Material& m) {
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str(),s);
  setMaterial(m);
  lcdd.addVolume(*this);
}

void Volume::setMaterial(const Material& m)  const  {
  if ( m.isValid() )   {
    TGeoMedium* medium = m._ptr<TGeoMedium>();
    if ( medium )  {
      m_element->SetMedium(medium);
      return;
    }
    throw runtime_error("Volume: Medium "+string(m.name())+" is not registered with geometry manager.");
  }
  throw runtime_error("Volume: Attempt to assign invalid material.");
}

void Volume::setSolid(const Solid& solid)  const  {
  m_element->SetShape(solid);
}

static PlacedVolume _addNode(TGeoVolume* parent, TGeoVolume* daughter, TGeoMatrix* transform) {
  TObjArray* a = parent->GetNodes();
  Int_t id = a ? a->GetEntries() : 0;
  parent->AddNode(daughter,id,transform);
  TGeoNode* n = parent->GetNode(id);
  return PlacedVolume(n);
}

PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoRotation rotation("",rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
    TGeoCombiTrans* transform = new TGeoCombiTrans(nam.c_str(),pos.x,pos.y,pos.z,0);
    transform->SetRotation(rotation);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const Position& pos)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoTranslation* transform = new TGeoTranslation(nam.c_str(),pos.x,pos.y,pos.z);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    TGeoRotation* transform = new TGeoRotation(nam.c_str(),rot.phi*RAD_2_DEGREE,rot.theta*RAD_2_DEGREE,rot.psi*RAD_2_DEGREE);
    return _addNode(m_element,volume,transform);
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityPos& /* pos */)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    return _addNode(m_element,volume,new TGeoIdentity(nam.c_str()));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

PlacedVolume Volume::placeVolume(const Volume& volume, const IdentityRot& /* rot */)  const  {
  if ( volume.isValid() )   {
    string nam = string(volume.name())+"_placement";
    return _addNode(m_element,volume,new TGeoIdentity(nam.c_str()));
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::setRegion(const Region& obj)  const   {
  data<Object>()->Attr_region = obj;
}

void Volume::setLimitSet(const LimitSet& obj)  const   {
  data<Object>()->Attr_limits = obj;
}

void Volume::setSensitiveDetector(const SensitiveDetector& obj) const  {
  data<Object>()->Attr_sens_det = obj;
}

void Volume::setVisAttributes(const VisAttr& attr) const   {
  if ( attr.isValid() )  {
    VisAttr::Object* vis = attr.data<VisAttr::Object>();
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    m_element->SetFillColor(bright);
    m_element->SetLineColor(dark);
    m_element->SetLineStyle(vis->lineStyle);
    m_element->SetLineWidth(10);
    m_element->SetVisibility(vis->visible ? kTRUE : kFALSE);
    m_element->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
  data<Object>()->Attr_vis = attr;
}

Solid Volume::solid() const   {
  return Solid((*this)->GetShape());
}

Material Volume::material() const   {
  return Handle<TGeoMaterial>(m_element->GetMaterial());
}

VisAttr Volume::visAttributes() const   {
  return data<Object>()->Attr_vis;
}

Ref_t Volume::sensitiveDetector() const    {
  return data<Object>()->Attr_sens_det;
}

Region Volume::region() const   {
  return data<Object>()->Attr_region;
}

PlacedVolume& PlacedVolume::addPhysVolID(const std::string& /* name */, int /* value */) {
  return *this;
}
