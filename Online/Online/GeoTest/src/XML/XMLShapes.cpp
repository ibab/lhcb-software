#include "XML/lcdd/XMLShapes.h"
#include "XML/lcdd/XMLObjects.h"
#include <stdexcept>

using namespace std;
using namespace DetDesc::XML;

/// Constructor to be used when creating a new DOM tree
ZPlane::ZPlane(const Document& doc, const XMLCh* rmin, const XMLCh* rmax, const XMLCh* z)  
: Element(doc,Tag_zplane)
{
  setAttr(Attr_rmin,rmin);
  setAttr(Attr_rmax,rmax);
  setAttr(Attr_z,z);
}

ZPlane::ZPlane(const Document& doc, double rmin, double rmax, double z)
: Element(doc,Tag_zplane)
{
  setAttr(Attr_rmin,rmin);
  setAttr(Attr_rmax,rmax);
  setAttr(Attr_z,z);
}

ZPlane& ZPlane::setRmin(double value)  {
  setAttr(Attr_rmin,value);
  return *this;
}

ZPlane& ZPlane::setRmax(double value)  {
  setAttr(Attr_rmax,value);
  return *this;
}

ZPlane& ZPlane::setZ(double value)  {
  setAttr(Attr_z,value);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Box::Box(const Document& document, const XMLCh* name)
: Solid(document,Tag_box, name)
{
}

/// Constructor to be used when creating a new DOM tree
Box::Box(const Document& document, const XMLCh* name, double x, double y, double z)
: Solid(document,Tag_box, name)
{
  setDimensions(x,y,z);
}

/// Set the box dimensions
Box& Box::setDimensions(double x, double y, double z)   {
  setAttr(Attr_x,x);
  setAttr(Attr_y,y);
  setAttr(Attr_z,z);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Polycone::Polycone(const Document& document, const XMLCh* name)
: Solid(document, Tag_polycone, name)
{
  setAttr(Attr_startphi,Tag_NULL);
  setAttr(Attr_deltaphi,Tag_TWOPI);
}

/// Constructor to be used when creating a new DOM tree
Polycone::Polycone(const Document& document, const XMLCh* name, double start, double delta)  
: Solid(document, Tag_polycone, name)
{
  setAttr(Attr_startphi,start);
  setAttr(Attr_deltaphi,delta);
}

/// Add all Z-planes to the Polycone
void Polycone::addZPlanes(Handle_t e)  {
  int num = 0;
  Document doc(document());
  for(Collection_t c(e,Tag_zplane); c; ++c)  {
    m_element.append(ZPlane(doc,c.attr<cpXMLCh>(Attr_rmin),c.attr<cpXMLCh>(Attr_rmax),c.attr<cpXMLCh>(Attr_z)));
    ++num;
  }
  if ( num < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
}

/// Constructor to be used when creating a new DOM tree
Tube::Tube(const Document& doc, const XMLCh* name) : Solid(doc,Tag_tube,name) {
  setAttr(Attr_deltaphi,Tag_TWOPI);
  setAttr(Attr_rmin,Tag_NULL);
  setAttr(Attr_rmax,Tag_NULL);
  setAttr(Attr_zhalf,Tag_NULL);
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
Tube::Tube(const Document& doc, const XMLCh* name, double rmi, double rma, double z, double deltaPhi)
: Solid(doc,Tag_tube,name)
{
  setAttr(Attr_deltaphi,deltaPhi);
  setAttr(Attr_rmin,rmi);
  setAttr(Attr_rmax,rma);
  setAttr(Attr_zhalf,z);
}

/// Set the box dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double deltaPhi)
{
  setAttr(Attr_deltaphi,deltaPhi);
  setAttr(Attr_rmin,rmin);
  setAttr(Attr_rmax,rmax);
  setAttr(Attr_zhalf,z);
  return *this;
}

void Tube::fromCompact(Handle_t source)  {
  setAttr(Attr_deltaphi,Tag_TWOPI);
  setAttr(Attr_rmin,source.attr<cpXMLCh>(Attr_rmin));
  setAttr(Attr_rmax,source.attr<cpXMLCh>(Attr_rmax));
  setAttr(Attr_z,source.attr<double>(Attr_zhalf)*2.);
}

/// Constructor to be used when creating a new DOM tree
Cone::Cone(const Document& doc, const XMLCh* name)
: Solid(doc, Tag_cone, name)
{
  setAttr(Attr_startphi,Tag_NULL);
  setAttr(Attr_deltaphi,Tag_TWOPI);
  setAttr(Attr_rmin1,Tag_NULL);
  setAttr(Attr_rmax1,Tag_NULL);
  setAttr(Attr_rmin2,Tag_NULL);
  setAttr(Attr_rmax2,Tag_NULL);
  setAttr(Attr_z,Tag_NULL);
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
Cone::Cone(const Document& doc, const XMLCh* name,
            double rmin1,
            double rmin2,
            double rmax1,
            double rmax2,
            double z,
            double startphi,
            double deltaphi)
: Solid(doc, Tag_cone, name)
{
  setAttr(Attr_startphi,startphi);
  setAttr(Attr_deltaphi,deltaphi);
  setAttr(Attr_rmin1,rmin1);
  setAttr(Attr_rmax1,rmax1);
  setAttr(Attr_rmin2,rmin2);
  setAttr(Attr_rmax2,rmax2);
  setAttr(Attr_z,z);
}

Cone& Cone::setZ(double value)  {
  setAttr(Attr_z,value);
  return *this;
}

Cone& Cone::setRmin1(double value)  {
  setAttr(Attr_rmin1,value);
  return *this;
}

Cone& Cone::setRmax1(double value)  {
  setAttr(Attr_rmax1,value);
  return *this;
}

Cone& Cone::setRmin2(double value)  {
  setAttr(Attr_rmin2,value);
  return *this;
}

Cone& Cone::setRmax2(double value)  {
  setAttr(Attr_rmax2,value);
  return *this;
}

Cone& Cone::setStartPhi(double value)  {
  setAttr(Attr_startphi,value);
  return *this;
}

Cone& Cone::setDeltaPhi(double value)  {
  setAttr(Attr_deltaphi,value);
  return *this;
}

void Cone::fromCompact(Handle_t source)   {
  setAttr(Attr_startphi,Tag_NULL);
  setAttr(Attr_deltaphi,Tag_TWOPI);
  setAttr(Attr_rmin1,source.attr<cpXMLCh>(Attr_rmin1));
  setAttr(Attr_rmax1,source.attr<cpXMLCh>(Attr_rmin2));
  setAttr(Attr_rmin2,source.attr<cpXMLCh>(Attr_rmax1));
  setAttr(Attr_rmax2,source.attr<cpXMLCh>(Attr_rmax2));
  setAttr(Attr_z,source.attr<cpXMLCh>(Attr_z));
}

/// Constructor to be used when creating a new DOM tree
Trapezoid::Trapezoid(const Document& doc, const XMLCh* name) : Solid(doc,Tag_trd,name) {
  setAttr(Attr_X1,Tag_NULL);
  setAttr(Attr_X2,Tag_NULL);
  setAttr(Attr_Y1,Tag_NULL);
  setAttr(Attr_Y2,Tag_NULL);
  setAttr(Attr_Z,Tag_NULL);
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
Trapezoid::Trapezoid(const Document& doc, const XMLCh* name, double x1, double x2, double y1, double y2, double z)
: Solid(doc,Tag_trd,name) 
{
  setAttr(Attr_X1,x1);
  setAttr(Attr_X2,x2);
  setAttr(Attr_Y1,y1);
  setAttr(Attr_Y2,y2);
  setAttr(Attr_Z,z);
}

Trapezoid& Trapezoid::setX1(double value)  {
  setAttr(Attr_X1,value*2e0);
  return *this;
}

Trapezoid& Trapezoid::setX2(double value)  {
  setAttr(Attr_X2,value*2e0);
  return *this;
}

Trapezoid& Trapezoid::setY1(double value)  {
  setAttr(Attr_Y1,value*2e0);
  return *this;
}

Trapezoid& Trapezoid::setY2(double value)  {
  setAttr(Attr_Y2,value*2e0);
  return *this;
}

Trapezoid& Trapezoid::setZ(double value)  {
  setAttr(Attr_Z,value*2e0);
  return *this;
}

double Trapezoid::x1() const  {
  return attr<double>(Attr_X2);
}

double Trapezoid::x2() const  {
  return attr<double>(Attr_X2);
}

double Trapezoid::y1() const  {
  return attr<double>(Attr_Y1);
}

double Trapezoid::y2() const  {
  return attr<double>(Attr_Y2);
}

double Trapezoid::z() const  {
  return attr<double>(Attr_z);
}

Trap::Trap( const Document& doc, const XMLCh* name,
            double z,
            double theta,
            double phi,
            double y1,
            double x1,
            double x2,
            double alpha1,
            double y2,
            double x3,
            double x4,
            double alpha2)
: Solid(doc,Tag_trap, name)
{
  setAttr(Attr_z,z*2);
  setAttr(Attr_theta,theta);
  setAttr(Attr_phi,phi);
  setAttr(Attr_y1,y1*2);
  setAttr(Attr_x1,x1*2);
  setAttr(Attr_x2,x2*2);
  setAttr(Attr_alpha1,alpha1);
  setAttr(Attr_y2,y2*2);
  setAttr(Attr_x3,x3*2);
  setAttr(Attr_x4,x4*2);
  setAttr(Attr_alpha2,alpha2);

  setAttr(Attr_aunit,Tag_radian);
  setAttr(Attr_lunit,Tag_mm);
}

double Trap::zHalfLength()  const   {
  return attr<double>(Attr_z);
}

double Trap::theta()  const   {
  return attr<double>(Attr_theta);
}

double Trap::phi()  const   {
  return attr<double>(Attr_phi);
}

double Trap::yHalfLength1()  const   {
  return attr<double>(Attr_y1);
}

double Trap::xHalfLength1()  const   {
  return attr<double>(Attr_x1);
}

double Trap::xHalfLength2()  const   {
  return attr<double>(Attr_x2);
}

double Trap::alpha1()  const   {
  return attr<double>(Attr_alpha1);
}

double Trap::yHalfLength2()  const   {
  return attr<double>(Attr_y2);
}

double Trap::xHalfLength3()  const   {
  return attr<double>(Attr_x3);
}

double Trap::xHalfLength4()  const   {
  return attr<double>(Attr_x4);
}

double Trap::alpha2()  const   {
  return attr<double>(Attr_alpha2);
}

/// Constructor to be used when creating a new DOM tree
PolyhedraRegular::PolyhedraRegular(const Document& doc, const XMLCh* name, int nsides, double rmin, double rmax, double zlen)  
: Solid(doc, Tag_polyhedra, name)
{
  setAttr(Attr_startphi,Tag_NULL);
  setAttr(Attr_deltaphi,Tag_TWOPI);
  setAttr(Attr_numsides,nsides);
  if ( rmin<0e0 || rmin>rmax )
    throw runtime_error("PolyhedraRegular: Illegal argument rmin:<"+_toString(rmin)+"> is invalid!");
  else if ( rmax<0e0 )
    throw runtime_error("PolyhedraRegular: Illegal argument rmax:<"+_toString(rmax)+"> is invalid!");
  append(ZPlane(doc,rmin,rmax, zlen/2e0));
  append(ZPlane(doc,rmin,rmax,-zlen/2e0));
}

/// Constructor to be used when creating a new DOM tree
BooleanSolid::BooleanSolid(const Document& doc, const XMLCh* type, const XMLCh* name)
: Solid(doc, type, name)
{
  addChild(Tag_first);
  addChild(Tag_second);
  addChild(Tag_positionref);
  addChild(Tag_rotationref);
}

BooleanSolid& BooleanSolid::setFirstSolid(const Solid& s)   {
  child(Tag_first).setAttr(Attr_ref,s.refName());
  return *this;
}

BooleanSolid& BooleanSolid::setSecondSolid(const Solid& s)  {
  child(Tag_second).setAttr(Attr_ref,s.refName());
  return *this;
}

BooleanSolid& BooleanSolid::setPosition(const Position& pos)  {
  child(Tag_positionref).setAttr(Attr_ref,pos.refName());
  return *this;
}

BooleanSolid& BooleanSolid::setRotation(const Rotation& rot)  {
  child(Tag_rotationref).setAttr(Attr_ref,rot.refName());
  return *this;
}

/// Constructor to be used when creating a new DOM tree
SubtractionSolid::SubtractionSolid(const Document& doc, const XMLCh* name)
: BooleanSolid(doc, Tag_subtraction, name)
{
}
