#include "XML/lcdd/XMLObjects.h"
#include "DetDesc/IDDescriptor.h"
#include <cmath>

using namespace std;
using namespace DetDesc::XML;
using DetDesc::IDDescriptor;

Header& Header::fromCompact(Document doc, Handle_t element, const XMLCh* fname)   {
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

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const Document& doc, const XMLCh* name, const XMLCh* val)
: RefElement(doc, Tag_constant, name)
{
  setAttr(Attr_value,val);
}

/// Constructor to be used when creating a new DOM tree
Constant::Constant(const Document& doc, const XMLCh* name)
: RefElement(doc, Tag_constant, name)
{
}

/// Constructor to be used when creating a new DOM tree
XYZ_element::XYZ_element(const Document& doc, const XMLCh* type, const XMLCh* name)   
: RefElement(doc, type, name)
{
  setAttr(Attr_x,Tag_NULL);
  setAttr(Attr_y,Tag_NULL);
  setAttr(Attr_z,Tag_NULL);
  setAttr(Attr_unit,"radian");
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
XYZ_element::XYZ_element(const Document& doc, const XMLCh* type, const XMLCh* name, double x, double y, double z)
: RefElement(doc, type, name)
{
  setAttr(Attr_x,x);
  setAttr(Attr_y,y);
  setAttr(Attr_z,z);
  setAttr(Attr_unit,"radian");
}

/// Constructor to be used when creating a new DOM tree. Automatically sets attributes
XYZ_element::XYZ_element(const Document& doc, const XMLCh* type, const XMLCh* name, const XMLCh* x, const XMLCh* y, const XMLCh* z)
: RefElement(doc, type, name)
{
  setAttr(Attr_x,x);
  setAttr(Attr_y,y);
  setAttr(Attr_z,z);
  setAttr(Attr_unit,"radian");
}

const XYZ_element& XYZ_element::fromXML(Handle_t h)  {
  if ( h )   {
    Attribute x = h.attr_nothrow(Attr_x);
    Attribute y = h.attr_nothrow(Attr_y);
    Attribute z = h.attr_nothrow(Attr_z);
    if ( x ) setAttr(Attr_x,x);
    if ( y ) setAttr(Attr_y,y);
    if ( z ) setAttr(Attr_z,z);
  }
  return *this;
}

const XYZ_element& XYZ_element::setX(double value)  const  {
  setAttr(Attr_x,value);
  return *this;
}

const XYZ_element& XYZ_element::setY(double value)  const  {
  setAttr(Attr_y,value);
  return *this;
}

const XYZ_element& XYZ_element::setZ(double value)  const  {
  setAttr(Attr_z,value);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Atom::Atom(const Document& doc, const XMLCh* name)  
: RefElement(doc, Tag_element, name)
{
}

/// Constructor to be used when creating a new DOM tree
Material::Material(const Document& doc, const XMLCh* name)  
: RefElement(doc, Tag_material, name)
{
}

/// Constructor to be used when creating a new DOM tree
VisAttr::VisAttr(const Document& doc, const XMLCh* name) 
: RefElement(doc,Tag_vis,name) 
{
  setAttr(Attr_line_style,"unbroken");
  setAttr(Attr_drawing_style,"wireframe");
  setAttr(Attr_show_daughters,Attr_true);
  Element color = setChild(Tag_color);
  color.setAttr(Attr_R,Attr_1);
  color.setAttr(Attr_B,Attr_1);
  color.setAttr(Attr_G,Attr_1);
  color.setAttr(Attr_alpha,Attr_1);
}

void VisAttr::setVisible(bool value)  {
  setAttr(Attr_visible,value);
}

/// Convert compact visualization attribute to LCDD visualization attribute
const VisAttr& VisAttr::fromCompact(Handle_t e)  const   {
  Attribute a_show_daughters = e.attr_nothrow(Attr_showDaughters);
  Attribute a_visible = e.attr_nothrow(Attr_visible);
  Attribute a_style = e.attr_nothrow(Attr_drawingStyle);
  Attribute a_alpha = e.attr_nothrow(Attr_alpha);
  Attribute a_R = e.attr_nothrow(Attr_r);
  Attribute a_B = e.attr_nothrow(Attr_b);
  Attribute a_G = e.attr_nothrow(Attr_g);

  setAttr(Attr_line_style,"unbroken");
  if ( a_style ) setAttr(Attr_drawing_style,a_style);
  if ( a_visible ) setAttr(Attr_visible,a_visible);
  if ( a_show_daughters ) setAttr(Attr_show_daughters,a_show_daughters);

  Element color = setChild(Tag_color);
  if ( a_alpha ) color.setAttr(Attr_alpha,a_alpha);
  if ( a_R ) color.setAttr(Attr_R,a_R);
  if ( a_B ) color.setAttr(Attr_B,a_B);
  if ( a_G ) color.setAttr(Attr_G,a_G);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Limit::Limit(const Document& doc, const XMLCh* name)
: RefElement(doc,Tag_limit,name)
{
  setAttr(Attr_value,Attr_1);
  setAttr(Attr_unit,Tag_mm);
  setAttr(Attr_particles,Tag_star);
}

void Limit::setParticles(const XMLCh* particleNames)   {
  setAttr(Attr_particles,particleNames);
}

void Limit::setValue(double value)   {
  setAttr(Attr_value,value);
}

void Limit::setUnit(const XMLCh* unit)   {
  setAttr(Attr_unit,Attr_unit);
}

/// Constructor to be used when creating a new DOM tree
LimitSet::LimitSet(const Document& doc, const XMLCh* name) 
: RefElement(doc,Tag_limitset,name)
{
}

void LimitSet::addLimit(const Limit& limit)   {
  m_element.append(limit);
}

/// Constructor to be used when creating a new DOM tree
Region::Region(const Document& doc, const XMLCh* name)  
: RefElement(doc,Tag_region,name)
{
  setAttr(Attr_store_secondaries,false);
  setAttr(Attr_threshold,10.0);
  setAttr(Attr_lunit,Tag_mm);
  setAttr(Attr_eunit,Tag_MeV);
  setAttr(Attr_cut,10.0);
}

Region& Region::setStoreSecondaries(bool value)  {
  setAttr(Attr_store_secondaries,value);
  return *this;
}

Region& Region::setThreshold(double value)  {
  setAttr(Attr_threshold,value);
  return *this;
}

Region& Region::setCut(double value)  {
  setAttr(Attr_cut,value);
  return *this;
}

Region& Region::setLengthUnit(const XMLCh* unit)  {
  setAttr(Attr_lunit,unit);
  return *this;
}

Region& Region::setEnergyUnit(const XMLCh* unit)  {
  setAttr(Attr_eunit,unit);
  return *this;
}

IDSpec::IDSpec(const Document& doc, const XMLCh* name, const IDDescriptor& dsc) 
: RefElement(doc,Tag_idspec,name)
{
  const IDDescriptor::FieldIDs& f = dsc.ids();
  const IDDescriptor::FieldMap& m = dsc.fields();
  setAttr(Attr_length, dsc.maxBit());
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

void IDSpec::addField(const XMLCh* name, const pair<int,int>& field)  {
  Element e(document(),Tag_idfield);
  e.setAttr(Attr_signed,field.second<0);
  e.setAttr(Attr_label,name);
  e.setAttr(Attr_start,field.first);
  e.setAttr(Attr_length,abs(field.second));
  m_element.append(e);
}
