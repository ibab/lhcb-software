#include "../Internals.h"
#include "DetDesc/lcdd/LCDD.h"
#include "DetDesc/lcdd/Volumes.h"

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
  lcdd.addVolume(RefElement(m_element));
}

Volume::Volume(LCDD& lcdd, const string& name, const Solid& s, const Material& m)   
{
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str(),s._ptr<TGeoShape>());
  setMaterial(m);
  lcdd.addVolume(RefElement(m_element));
}

void Volume::setMaterial(const Material& m)  const  {
  if ( m.isValid() )   {
    TGeoMedium* medium = m._ptr<TGeoMedium>();
    if ( medium )  {
      (*this)->SetMedium(medium);
      return;
    }
    throw runtime_error("Volume: Medium "+string(m.name())+" is not registered with geometry manager.");
  }
  throw runtime_error("Volume: Attempt to assign invalid material.");
}

void Volume::setSolid(const Solid& solid)  const  {
  _ptr<TGeoVolume>()->SetShape(solid._ptr<TGeoShape>());
}

void Volume::addPhysVol(const PhysVol& volume, const Position& pos, const Rotation& rot)  const  {
  Volume vol(volume);
  TGeoVolume* phys_vol = vol;
  TGeoVolume* log_vol  = _ptr<TGeoVolume>();
  if ( phys_vol )   {
    TGeoTranslation* t = pos._ptr<TGeoTranslation>();
    TGeoRotation*    r = rot._ptr<TGeoRotation>();
    TGeoCombiTrans*  c = new TGeoCombiTrans(*t,*r);
    log_vol->AddNode(phys_vol, --unique_physvol_id, c);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::addPhysVol(const PhysVol& volume, const Transformation& matrix)  const  {
  Volume vol(volume);
  TGeoVolume* phys_vol = vol;
  TGeoVolume* log_vol  = _ptr<TGeoVolume>();
  if ( phys_vol )   {
    TGeoMatrix* m = value<TGeoMatrix>(matrix);
    log_vol->AddNode(phys_vol, --unique_physvol_id, m);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::setRegion(const Region& obj)  const   {
  data<TGeoVolume,Object>()->Attr_region = obj;
}

void Volume::setLimitSet(const LimitSet& obj)  const   {
  data<TGeoVolume,Object>()->Attr_limits = obj;
}

void Volume::setSensitiveDetector(const SensitiveDetector& obj) const  {
  data<TGeoVolume,Object>()->Attr_sens_det = obj;
}

void Volume::setVisAttributes(const VisAttr& attr) const   {
  TGeoVolume* vol = *this;
  Object*     val = data<TGeoVolume,Object>();
  if ( attr.isValid() )  {
    VisAttr::Object* vis = second_value<TNamed>(attr);
    Color_t bright = TColor::GetColorBright(vis->color);
    Color_t dark   = TColor::GetColorDark(vis->color);
    vol->SetFillColor(bright);
    vol->SetLineColor(dark);
    vol->SetLineStyle(vis->lineStyle);
    vol->SetLineWidth(10);
    vol->SetVisibility(vis->visible ? kTRUE : kFALSE);
    vol->SetVisDaughters(vis->showDaughters ? kTRUE : kFALSE);
  }
  //#if 0
  // debug only, but if removed, does not plot nicely anymore....
  vol->SetVisibility(kTRUE);
  vol->SetVisDaughters(kTRUE);
  vol->SetVisLeaves(kTRUE);
  vol->SetVisContainers(kTRUE);
  vol->SetTransparency(30);
  //#endif
  val->Attr_vis = attr;
}

Solid Volume::solid() const   {
  return Solid((*this)->GetShape());
}

Material Volume::material() const   {
  return Material((*this)->GetMaterial());
}

VisAttr Volume::visAttributes() const   {
  Object* val = second_value<TGeoVolume>(*this);
  return val->Attr_vis;
}

RefElement Volume::sensitiveDetector() const    {
  Object* val = second_value<TGeoVolume>(*this);
  return val->Attr_sens_det;
}

Region Volume::region() const   {
  Object* val = second_value<TGeoVolume>(*this);
  return val->Attr_region;
}

PhysVol& PhysVol::addPhysVolID(const std::string& /* name */, int /* value */) {
  return *this;
}
