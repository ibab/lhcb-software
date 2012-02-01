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

static UInt_t unique_physvol_id = INT_MAX-1;

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

void Volume::addPhysVol(const PhysVol& volume, const Position& pos, const Rotation& rot)  const  {
  if ( volume.isValid() )   {
    TGeoCombiTrans*  c = new TGeoCombiTrans(pos,rot);
    m_element->AddNode(volume,--unique_physvol_id,c);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::addPhysVol(const PhysVol& volume, const Transform& matrix)  const  {
  if ( volume.isValid() )   {
    m_element->AddNode(volume.ptr(),--unique_physvol_id,matrix.ptr());
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::addPhysVol(const PhysVol& volume, const Position& matrix)  const  {
  if ( volume.isValid() )   {
    m_element->AddNode(volume.ptr(),--unique_physvol_id,matrix.ptr());
    return;
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
  //#if 0
  // debug only, but if removed, does not plot nicely anymore....
  m_element->SetVisibility(kTRUE);
  m_element->SetVisDaughters(kTRUE);
  m_element->SetVisLeaves(kTRUE);
  m_element->SetVisContainers(kTRUE);
  m_element->SetTransparency(30);
  //#endif
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

RefHandle<TNamed> Volume::sensitiveDetector() const    {
  return data<Object>()->Attr_sens_det;
}

Region Volume::region() const   {
  return data<Object>()->Attr_region;
}

PhysVol& PhysVol::addPhysVolID(const std::string& /* name */, int /* value */) {
  return *this;
}
