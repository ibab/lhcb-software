// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/lcdd/LCDD.h"
#include "DetDesc/IDDescriptor.h"

#include "TMap.h"
#include "TColor.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TGeoElement.h"
#include "TGeoMaterial.h"

#include <cmath>

using namespace std;
using namespace DetDesc::Geometry;
using DetDesc::IDDescriptor;

#if 0
Header& Header::fromCompact(Document doc, Handle_t element, const string& fname)   {
  Element    info = element;
  Author     aut(doc);
  RefElement det(doc,Tag_detector,info.attr<cpXMLCh>(Attr_name));
  RefElement gen(doc,Tag_generator, Strng_t("GeomCnv++"));

  gen.setAttr(Attr_file,fname);  
  gen.setAttr(Attr_version,"1.0");
  gen.setAttr(Attr_checksum,"None");

  aut.setAttr(Attr_name,info.attr<cpXMLCh>(Tag_author));

  append(gen);
  append(aut);
  append(doc.clone(info.child(Tag_comment),true));
  return *this;
}
#endif

/// Constructor to be used when creating a new DOM tree
Author::Author(LCDD& /* lcdd */)  {
  m_element = new TNamed("","author");
}

/// Constructor to be used when creating a new DOM tree
Header::Header(LCDD& /* lcdd */)   {
  m_element = new TNamed("","header");
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(LCDD& lcdd, const string& nam, const string& val)
{
  m_element = new TNamed(nam.c_str(),val.c_str());
  lcdd.add(*this);
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(LCDD& lcdd, const string& name)   {
  m_element = new TNamed(name.c_str(),"");
  lcdd.add(*this);
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
Transform::Transform(LCDD& lcdd, const std::string& name)   
{
  assign(new TGeoCombiTrans(name.c_str(),0,0,0,0),"","transformation");
  lcdd.addTransform(*this);
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
Position::Position(LCDD& lcdd, const std::string& name, double x, double y, double z)
{
  assign(new TGeoTranslation(name.c_str(),x,y,z),"","position");
  lcdd.addTransform(*this);
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
Rotation::Rotation(LCDD& lcdd, const std::string& name, double theta, double phi, double psi)
{
  // Note: compact supplies: theta, phi, psi
  // but root wants:         phi, theta, psi!
  assign(new TGeoRotation(name.c_str(),phi*RAD_2_DEGREE,theta*RAD_2_DEGREE,psi*RAD_2_DEGREE),"","rotation");
  lcdd.addTransform(*this);
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
Rotation::Rotation(LCDD& lcdd, const std::string& name)
{
  assign(new TGeoRotation(name.c_str(),0,0,0),"","rotation");
  lcdd.addTransform(*this);
}

/// Constructor to be used when creating a new DOM tree
Atom::Atom(LCDD& /* lcdd */, const string& name, const string& formula, int Z, int N, double density) {
  TGeoElementTable* t = TGeoElement::GetElementTable();
  TGeoElement* e = t->FindElement(name.c_str());
  if ( !e ) {
    t->AddElement(name.c_str(), formula.c_str(), Z, N, density);
    e = t->FindElement(name.c_str());
  }
  m_element = e;
}

/// Constructor to be used when creating a new DOM tree
Material::Material(LCDD& /* lcdd */, const string& name)   {
  TGeoMaterial* mat = gGeoManager->GetMaterial(name.c_str());
  m_element = mat;
}

/// Constructor to be used when creating a new DOM tree
VisAttr::VisAttr(LCDD& /* lcdd */, const string& name)    {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  assign(obj, name, "vis");
  obj->color  = 2;
  setLineStyle(SOLID);
  setDrawingStyle(WIREFRAME);
  setShowDaughters(true);
  setAlpha(0.1f);
}

/// Set Flag to show/hide daughter elements
void VisAttr::setShowDaughters(bool value)   {
  _data().showDaughters = value;
}

/// Set visibility flag
void VisAttr::setVisible(bool value)   {
  _data().visible = value;
}

/// Set line style
void VisAttr::setLineStyle(LineStyle value)  {
  _data().lineStyle = value;
}

/// Set drawing style
void VisAttr::setDrawingStyle(DrawingStyle value)   {
  _data().drawingStyle = value;
}

/// Set alpha value
void VisAttr::setAlpha(float /* value */)   {
  //TNamed* obj = first_value<TNamed>(*this);
  //obj->SetAlpha(value);
}

/// Set object color
void VisAttr::setColor(float red, float green, float blue)   {
  _data().color = TColor::GetColor(red,green,blue);
}

/// Constructor to be used when creating a new DOM tree
Limit::Limit(LCDD& /* lcdd */, const string& name)   {
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  assign(obj,name,"*");
  obj->first  = "mm";
  obj->second = 1.0;
}

void Limit::setParticles(const string& particleNames)   {
  m_element->SetTitle(particleNames.c_str());
}

void Limit::setValue(double value)   {
  _data().second = value;
}

void Limit::setUnit(const string& value)   {
  _data().first = value;
}

/// Constructor to be used when creating a new DOM tree
LimitSet::LimitSet(LCDD& /* lcdd */, const string& name)   {
  assign(new Value<TNamed,TMap>(),name,"limitset");
}

void LimitSet::addLimit(const Ref_t& limit)   {
  data<TMap>()->Add(limit.ptr(),limit.ptr());
}

/// Constructor to be used when creating a new DOM tree
Region::Region(LCDD& /* lcdd */, const string& name)   {
  Value<TNamed,Object>* p = new Value<TNamed,Object>();
  assign(p, name, "region");
  p->Attr_store_secondaries = false;
  p->Attr_threshold = 10.0;
  p->Attr_lunit = "mm";
  p->Attr_eunit = "MeV";
  p->Attr_cut = 10.0;
}

Region& Region::setStoreSecondaries(bool value)  {
  _data().Attr_store_secondaries = value;
  return *this;
}

Region& Region::setThreshold(double value)  {
  _data().Attr_threshold = value;
  return *this;
}

Region& Region::setCut(double value)  {
  _data().Attr_cut = value;
  return *this;
}

Region& Region::setLengthUnit(const string& unit)  {
  _data().Attr_lunit = unit;
  return *this;
}

Region& Region::setEnergyUnit(const string& unit)  {
  _data().Attr_eunit = unit;
  return *this;
}
#undef setAttr

#if 0
IDSpec::IDSpec(LCDD& lcdd, const string& name, const IDDescriptor& dsc) 
: RefElement(doc,Tag_idspec,name)
{
  const IDDescriptor::FieldIDs& f = dsc.ids();
  const IDDescriptor::FieldMap& m = dsc.fields();
  _data().Attr_length = dsc.maxBit();
  for(IDDescriptor::FieldIDs::const_iterator i=f.begin(); i!=f.end();++i)  {
    int ident = (*i).first;
    const string& nam = (*i).second;
    const pair<int,int>& fld = m.find(nam)->second;
    addField(nam,fld);
  }
}

void IDSpec::addField(const string& name, const pair<int,int>& field)  {
  addField(Strng_t(name),field);
}

void IDSpec::addField(const string& name, const pair<int,int>& field)  {
  Element e(document(),Tag_idfield);
  e._data().Attr_signed = field.second<0;
  e._data().Attr_label = name;
  e._data().Attr_start = field.first;
  e._data().Attr_length = abs(field.second);
  m_element.append(e);
}
#endif
