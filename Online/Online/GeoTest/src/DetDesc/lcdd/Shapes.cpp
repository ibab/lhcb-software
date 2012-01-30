#define _USE_MATH_DEFINES
#include "DetDesc/lcdd/LCDD.h"
#include "../Internals.h"

// C/C++ include files
#include <stdexcept>

// ROOT includes
#include "TGeoShape.h"
#include "TGeoCone.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoTrd2.h"
#include "TGeoArb8.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace DetDesc::Geometry;

ZPlane::ZPlane(double rmin, double rmax, double z)
: m_rmin(rmin), m_rmax(rmax), m_z(z)
{
}

ZPlane& ZPlane::setRmin(double value)  {
  m_rmin = value;
  return *this;
}

ZPlane& ZPlane::setRmax(double value)  {
  m_rmax = value;
  return *this;
}

ZPlane& ZPlane::setZ(double value)  {
  m_z = value;
  return *this;
}

template<typename T> 
void Solid_type<T>::_setDimensions(double* param) {
  this->ptr()->SetDimensions(param);
  this->ptr()->ComputeBBox();
}

/// Assign pointrs and register solid to geometry
template<typename T> 
void Solid_type<T>::_assign(LCDD& lcdd, T* n, const std::string& nam, const std::string& tit, bool cbbox) {
  assign(n,nam,tit);
  if ( cbbox ) n->ComputeBBox();
  lcdd.addSolid(Solid(this->ptr()));
}

void Box::make(LCDD& lcdd, const std::string& name, double x, double y, double z)  {
  _assign(lcdd,new TGeoBBox(x,y,z),name,"box");
}

/// Set the box dimensions
Box& Box::setDimensions(double x, double y, double z)   {
  double params[] = {x,y,z};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Polycone::Polycone(LCDD& lcdd, const string& name)   {
  _assign(lcdd,new TGeoPcon(0,RAD_2_DEGREE * (2.*M_PI),0),name,"polycone",false);
}

/// Constructor to be used when creating a new DOM tree
Polycone::Polycone(LCDD& lcdd, const string& name, double start, double delta)   {
  _assign(lcdd,new TGeoPcon(RAD_2_DEGREE*start,RAD_2_DEGREE*delta,0),name,"polycone",false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(LCDD& lcdd, const std::string& name, double start, double delta, const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)
{
  vector<double> params;
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(RAD_2_DEGREE * start);
  params.push_back(RAD_2_DEGREE * delta);
  params.push_back(rmin.size());
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]);
    params.push_back(rmin[i]);
    params.push_back(rmax[i]);
  }
  _assign(lcdd, new TGeoPcon(&params[0]),name,"polycone");
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z)  {
  TGeoPcon* s = *this;
  vector<double> params;
  size_t num = s->GetNz();
  if ( rmin.size() < 2 )  {
    throw runtime_error("PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.push_back(s->GetPhi1());
  params.push_back(s->GetDphi());
  params.push_back(num+rmin.size());
  for( size_t i=0; i<num; ++i )  {
    params.push_back(s->GetZ(i));
    params.push_back(s->GetRmin(i));
    params.push_back(s->GetRmax(i));
  }
  for( size_t i=0; i<rmin.size(); ++i )  {
    params.push_back(z[i]);
    params.push_back(rmin[i]);
    params.push_back(rmax[i]);
  }
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
void Tube::make(LCDD& lcdd, const string& name, double rmin, double rmax, double z, double deltaPhi)
{
  _assign(lcdd,new TGeoTubeSeg(rmin,rmax,z,0.,RAD_2_DEGREE*deltaPhi),name,"tube");
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double deltaPhi)
{
  double params[] = {rmin,rmax,z,0.,RAD_2_DEGREE * deltaPhi};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
void Cone::make(LCDD&  lcdd, const string& name,
		double z,
		double rmin1,
		double rmax1,
		double rmin2,
		double rmax2)
{
  _assign(lcdd,new TGeoCone(z,rmin1,rmax1,rmin2,rmax2),name,"cone");
}

Cone& Cone::setDimensions(double z,double rmin1,double rmax1,double rmin2,double rmax2)  {
  double params[] = {z,rmin1,rmax1,rmin2,rmax2};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
Trapezoid::Trapezoid(LCDD& lcdd, const string& name)  {
  _assign(lcdd,new TGeoTrd2(0,0,0,0,0),name,"trd2");
}

/// Constructor to be used when creating a new DOM tree with attribute initialization
Trapezoid::Trapezoid(LCDD& lcdd, const string& name, double x1, double x2, double y1, double y2, double z)  {
  _assign(lcdd,new TGeoTrd2(x1,x2,y1,y2,z),name,"trd2");
}

/// Set the Trapezoid dimensions
Trapezoid& Trapezoid::setDimensions(double x1, double x2, double y1, double y2, double z)
{
  double params[] = {x1,x2,y1,y2,z};
  _setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setX1(double value)  {
  TGeoTrd2* s = *this;
  double params[] = {value,s->GetDx2(),s->GetDy1(),s->GetDy2(),s->GetDz()};
  _setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setX2(double value)  {
  TGeoTrd2* s = *this;
  double params[] = {s->GetDx2(),value,s->GetDy1(),s->GetDy2(),s->GetDz()};
  _setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setY1(double value)  {
  TGeoTrd2* s = *this;
  double params[] = {s->GetDx1(),s->GetDx2(),value,s->GetDy2(),s->GetDz()};
  _setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setY2(double value)  {
  TGeoTrd2* s = *this;
  double params[] = {s->GetDx1(),s->GetDx2(),s->GetDy1(),value,s->GetDz()};
  _setDimensions(params);
  return *this;
}

Trapezoid& Trapezoid::setZ(double value)  {
  TGeoTrd2* s = *this;
  double params[] = {s->GetDx1(),s->GetDx2(),s->GetDy1(),s->GetDy2(),value};
  _setDimensions(params);
  return *this;
}

Trap::Trap( LCDD&  lcdd, const string& name,
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
{
  _assign(lcdd,new TGeoTrap(z,RAD_2_DEGREE*theta,RAD_2_DEGREE*phi,y1,x1,x2,alpha1,y2,x3,x4,alpha2),name,"trap");
}

/// Set the trap dimensions
Trap& Trap::setDimensions(double z,double theta,double phi,
                         double y1,double x1,double x2,double alpha1,
                         double y2,double x3,double x4,double alpha2)  {
  double params[] = {z,RAD_2_DEGREE * theta,RAD_2_DEGREE * phi,y1,x1,x2,alpha1,y2,x3,x4,alpha2};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new DOM tree
PolyhedraRegular::PolyhedraRegular(LCDD& lcdd, const string& name, int nsides, double rmin, double rmax, double zlen)  
{
  if ( rmin<0e0 || rmin>rmax )
    throw runtime_error("PolyhedraRegular: Illegal argument rmin:<"+_toString(rmin)+"> is invalid!");
  else if ( rmax<0e0 )
    throw runtime_error("PolyhedraRegular: Illegal argument rmax:<"+_toString(rmax)+"> is invalid!");
  _assign(lcdd,new TGeoPgon(),name,"polyhedra",false);
  double params[] = {
    RAD_2_DEGREE * 2e0 * M_PI,
    RAD_2_DEGREE * 2e0 * M_PI,
    double(nsides),
    2e0,
    zlen/2e0,rmin,rmax,
    -zlen/2e0,rmin,rmax
  };
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new DOM tree
BooleanSolid::BooleanSolid(LCDD& lcdd, const string& name, const string& type, const string& expr)
{
  _assign(lcdd,new TGeoCompositeShape(expr.c_str()),name,type);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(LCDD& lcdd, const std::string& name, const std::string& expr)
  : BooleanSolid(lcdd, name, "subtraction", expr)
{
}


/// Constructor to be used when creating a new DOM tree
SubtractionSolid::SubtractionSolid(LCDD& lcdd, const string& name, const Solid& shape1, const Solid& shape2)
  : BooleanSolid(lcdd, name, "subtraction", shape1.name()+string("-")+shape2.name())
{
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(LCDD& lcdd, const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot)
{
  string combi1 = name+"_combitrans_first";
  string combi2 = name+"_combitrans_first";
  TGeoTranslation* t = pos._ptr<TGeoTranslation>();
  TGeoRotation*    r = rot._ptr<TGeoRotation>();
  //const double*  v = t->GetTranslation();

  TGeoTranslation* firstPos = new TGeoTranslation(0,0,0);
  TGeoRotation*    firstRot = new TGeoRotation();
  firstRot->RotateZ(0);
  firstRot->RotateY(0);
  firstRot->RotateX(0);

  TGeoMatrix* firstMatrix  = new TGeoCombiTrans(*firstPos,firstRot->Inverse());
  TGeoMatrix* secondMatrix = new TGeoCombiTrans(*t,r->Inverse());
  TGeoCompositeShape* composite = 
    new TGeoCompositeShape(name.c_str(),
                           new TGeoSubtraction(shape1._ptr<TGeoShape>(),
					       shape2._ptr<TGeoShape>(),
                                               firstMatrix,secondMatrix));
  composite->ComputeBBox();
  _assign(lcdd, composite, "", "subtraction");
}

//  template class DetDesc::Geometry::Element_type<X>;
//  template class DetDesc::Geometry::RefElement_type<X>;

#define INSTANTIATE(X)  \
  template class DetDesc::Geometry::Solid_type<X>

INSTANTIATE(TGeoBBox);
INSTANTIATE(TGeoPcon);
INSTANTIATE(TGeoPgon);
INSTANTIATE(TGeoTube);
INSTANTIATE(TGeoTubeSeg);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCone);
INSTANTIATE(TGeoCompositeShape);

