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
  m_element = new Value<TGeoVolume,Volume::Object>(name.c_str());
  setSolid(s);
  setMaterial(m);

  // hack !!!
  TGeoVolume* vol = first_value<TGeoVolume>(*this);
  // debug only
  vol->SetVisibility(kTRUE);
  vol->SetVisDaughters(kTRUE);
  vol->SetVisLeaves(kTRUE);
  vol->SetVisContainers(kTRUE);
  vol->SetTransparency(70);
  lcdd.addVolume(RefElement(m_element));
}

void Volume::setMaterial(const Material& m)  const  {
  if ( m.isValid() )   {
    TGeoMedium* medium = value<TGeoMedium>(m);
    if ( medium )  {
      TGeoVolume* vol = first_value<TGeoVolume>(*this);
      vol->SetMedium(medium);
      return;
    }
    throw runtime_error("Volume: Medium "+string(m.name())+" is not registered with geometry manager.");
  }
  throw runtime_error("Volume: Attempt to assign invalid material.");
}

void Volume::setSolid(const Solid& solid)  const  {
  TGeoVolume* val = first_value<TGeoVolume>(*this);
  val->SetShape(&solid.shape());
}

void Volume::addPhysVol(const PhysVol& volume, const Position& pos, const Rotation& rot)  const  {
  Volume vol(volume);
  TGeoVolume* phys_vol = first_value<TGeoVolume>(vol);
  TGeoVolume* log_vol  = first_value<TGeoVolume>(*this);
  if ( phys_vol )   {
    TGeoTranslation* t = value<TGeoTranslation>(pos);
    TGeoRotation*    r = value<TGeoRotation>(rot);
    TGeoCombiTrans*  c = new TGeoCombiTrans(*t,*r);
    log_vol->AddNode(phys_vol, --unique_physvol_id, c);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::addPhysVol(const PhysVol& volume, const Transformation& matrix)  const  {
  Volume vol(volume);
  TGeoVolume* phys_vol = first_value<TGeoVolume>(vol);
  TGeoVolume* log_vol  = first_value<TGeoVolume>(*this);
  if ( phys_vol )   {
    TGeoMatrix* m = value<TGeoMatrix>(matrix);
    log_vol->AddNode(phys_vol, --unique_physvol_id, m);
    return;
  }
  throw runtime_error("Volume: Attempt to assign an invalid physical volume.");
}

void Volume::setRegion(const Region& obj)  const   {
  Object* val = second_value<TGeoVolume>(*this);
  val->Attr_region = obj;
}

void Volume::setLimitSet(const LimitSet& obj)  const   {
  Object* val = second_value<TGeoVolume>(*this);
  val->Attr_limits = obj;
}

void Volume::setSensitiveDetector(const SensitiveDetector& obj) const  {
  Object* val = second_value<TGeoVolume>(*this);
  val->Attr_sens_det = obj;
}

void Volume::setVisAttributes(const VisAttr& attr) const   {
  TGeoVolume* vol = first_value<TGeoVolume>(*this);
  Object*     val = second_value<TGeoVolume>(*this);
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
  val->Attr_vis = attr;
}

Solid Volume::solid() const   {
  return Solid(first_value<TGeoVolume>(*this)->GetShape());
}

Material Volume::material() const   {
  return Material(first_value<TGeoVolume>(*this)->GetMaterial());
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

const TGeoVolume* Volume::volume() const  {
  return first_value<TGeoVolume>(*this);
}

PhysVol& PhysVol::addPhysVolID(const std::string& /* name */, int /* value */) {
  return *this;
}
