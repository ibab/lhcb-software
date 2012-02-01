// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/IDDescriptor.h"
#include "DetDesc/lcdd/LCDD.h"

using namespace std;
using namespace DetDesc::Geometry;

Subdetector::Object::Object()  
: Attr_id(0), Attr_combine_hits(0), Attr_envelope(),
  Attr_volume(), Attr_material(), Attr_visualization(), Attr_readout()
{
}

/// Constructor for a new subdetector element
Subdetector::Subdetector(const LCDD& /* lcdd */, const string& name, const string& type, int id)
{
  assign(new Value<TNamed,Object>(),name,type);
  _data().Attr_id = id;
}

string Subdetector::type() const   {
  return m_element ? m_element->GetTitle() : "";
}

int Subdetector::id() const   {
  return _data().Attr_id;
}

Material Subdetector::material() const  {
  return _data().Attr_material;
}

bool Subdetector::combineHits() const   {
  return _data().Attr_combine_hits != 0;
}

VisAttr Subdetector::visAttr() const  {
  return _data().Attr_visualization;
}

Readout Subdetector::readout() const   {
  return _data().Attr_readout;
}

Subdetector& Subdetector::setReadout(const Readout& readout)   {
  _data().Attr_readout = readout;
  return *this;
}

const Subdetector::Children& Subdetector::children() const   {
  return _data().Attr_children;
}

void Subdetector::check(bool condition, const string& msg) const  {
  if ( condition )  {
    throw runtime_error(msg);
  }
}

Subdetector& Subdetector::add(const Subdetector& sdet)  {
  if ( isValid() )  {
    pair<Children::iterator,bool> r = _data().Attr_children.insert(make_pair(sdet.name(),sdet));
    if ( r.second ) return *this;
    throw runtime_error("Subdetector::add: Element "+string(sdet.name())+" is already present [Double-Insert]");
  }
  throw runtime_error("Subdetector::add: Self is not defined [Invalid Handle]");
}

Volume  Subdetector::volume() const    {
  return _data().Attr_volume;
}

Subdetector& Subdetector::setVolume(const Volume& volume)  {
  _data().Attr_volume = volume;
  _data().Attr_material = volume.material();
  return *this;
}

Solid  Subdetector::envelope() const    {
  return _data().Attr_envelope;
}

Subdetector& Subdetector::setEnvelope(const Solid& solid)   {
  _data().Attr_envelope = solid;
  return *this;
}

#include "TGeoVolume.h"
Subdetector& Subdetector::setVisAttributes(const LCDD& lcdd, const string& name, const Volume& volume)  {
  if ( !name.empty() )   {
    VisAttr attr = lcdd.visAttributes(name);
    _data().Attr_visualization = attr;
    volume.setVisAttributes(attr);
  }
  else  {
    /*
    string nam = this->name();
    TGeoVolume* vol = volume;
    vol->SetVisibility(kTRUE);
    vol->SetVisDaughters(kTRUE);
    vol->SetVisLeaves(kTRUE);
    vol->SetVisContainers(kTRUE);
    vol->SetTransparency(50);

    string tag = node.tag();
    if ( tag == Tag_slice.str() )  // Slices turned off by default
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleNoDaughters));
    else if ( tag == Tag_layer.str() )  // Layers turned off, but daughters possibly visible
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleWithDaughters));
    else if ( tag == Tag_module.str() )  // Tracker modules similar to layers
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleWithDaughters));
    else if ( tag == Tag_module_component.str() )  // Tracker modules components turned off by default
      volume.setVisAttributes(lcdd.visAttributes(Attr_InvisibleNoDaughters));
  */
  }
  return *this;
}

Subdetector& Subdetector::setRegion(const LCDD& lcdd, const string& name, const Volume& volume)  {
  if ( isValid() )  {
    if ( !name.empty() )  {
      volume.setRegion(lcdd.region(name));
    }
    return *this;
  }
  throw runtime_error("Subdetector::setRegion: Self is not defined [Invalid Handle]");
}

Subdetector& Subdetector::setLimitSet(const LCDD& lcdd, const string& name, const Volume& volume)  {
  if ( isValid() )  {
    if ( !name.empty() )  {
      volume.setLimitSet(lcdd.limitSet(name));
    }
    return *this;
  }
  throw runtime_error("Subdetector::setLimitSet: Self is not defined [Invalid Handle]");
}

Subdetector& Subdetector::setAttributes(const LCDD& lcdd, const Volume& volume,
                                        const std::string& region, 
                                        const std::string& limits, 
                                        const std::string& vis)
{
  setRegion(lcdd,region,volume);
  setLimitSet(lcdd,limits,volume);
  return setVisAttributes(lcdd,vis,volume);
}

Subdetector& Subdetector::setCombineHits(bool value, SensitiveDetector& sens)   {
  if ( isTracker() )  {
    _data().Attr_combine_hits = value;
    sens.setCombineHits(value);
  }
  return *this;
}

bool Subdetector::isTracker() const   {
  if ( isValid() )  {
    string typ = type();
    if ( typ.find("Tracker") != string::npos && _data().Attr_readout.isValid() )   {
      return true;
    }
  }
  return false;
}

bool Subdetector::isCalorimeter() const   {
  if ( isValid() )  {
    string typ = type();
    if ( typ.find("Calorimeter") != string::npos && _data().Attr_readout.isValid() ) {
      return true;
    }
  }
  return false;
}

//#if 0
bool Subdetector::isInsideTrackingVolume() const  {
  //if ( isValid() && hasAttr(Attr_insideTrackingVolume) )
  //  return attr<bool>(Attr_insideTrackingVolume);
  //else if ( isTracker() )
  //  return true;
  return false;
}
//#endif

SensitiveDetector::SensitiveDetector(const LCDD& /* lcdd */, const std::string& type, const std::string& name) 
{
  /*
    <calorimeter ecut="0" eunit="MeV" hits_collection="EcalEndcapHits" name="EcalEndcap" verbose="0">
      <global_grid_xy grid_size_x="3.5" grid_size_y="3.5"/>
      <idspecref ref="EcalEndcapHits"/>
    </calorimeter>
  */
  assign(new Value<TNamed,Object>(),name,type);
  _data().Attr_ecut = 0e0;
  _data().Attr_eunit = "MeV";
  _data().Attr_verbose = 0;
}

/// Access the type of the sensitive detector
string SensitiveDetector::type() const  {
  return m_element ? m_element->GetTitle() : "";
}

/// Assign the IDDescriptor reference
SensitiveDetector& SensitiveDetector::setIDSpec(const NamedHandle& spec)  {
  _data().Attr_id = spec;
  return *this;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setHitsCollection(const string& collection)  {
  _data().Attr_hits_collection = collection;
  return *this;
}

/// Assign the name of the hits collection
SensitiveDetector& SensitiveDetector::setCombineHits(bool value)  {
  int v = value ? 1 : 0;
  _data().Attr_combine_hits = v;
  return *this;
}

/// Assign the readout segmentation reference
SensitiveDetector& SensitiveDetector::setSegmentation(const Segmentation& segmentation)   {
  if ( segmentation.isValid() )  {
    _data().Attr_segmentation = segmentation;
    return *this;
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}
