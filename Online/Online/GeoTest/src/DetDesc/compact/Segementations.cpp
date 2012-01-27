#include "DetDesc/compact/Segmentations.h"
#include "../Internals.h"
#include <iostream>

using namespace std;
using namespace DetDesc::Geometry;

/// Constructor to be used when reading the already parsed object
Segmentation::Segmentation(const Element& e) : RefElement(e)  
{
}

Segmentation::Segmentation(LCDD& /* lcdd */, const string& type)
{
  assign(new Value<TNamed,Segmentation::Object>(),"segmentation",type);
}

bool Segmentation::useForHitPosition() const   {
  return getAttr(Attr_useForHitPosition) != 0;
}

const string Segmentation::type() const   {
  return first_value<TNamed>(*this)->GetTitle();
}

ProjectiveCylinder::ProjectiveCylinder(LCDD& lcdd) 
: Segmentation(lcdd,"projective_cylinder")   {}

/// Accessors: get number of bins in theta
int ProjectiveCylinder::thetaBins() const  {
  return getAttr(data.cylindrical_binning.Attr_ntheta);
}

/// Accessors: get number of bins in phi
int ProjectiveCylinder::phiBins() const  {
  return getAttr(data.cylindrical_binning.Attr_nphi);
}

/// Accessors: set number of bins in theta
void ProjectiveCylinder::setThetaBins(int value)  {
  setAttr(data.cylindrical_binning.Attr_ntheta,value);
}

/// Accessors: set grid size in Y
void ProjectiveCylinder::setPhiBins(int value)  {
  setAttr(data.cylindrical_binning.Attr_nphi,value);
}

NonProjectiveCylinder::NonProjectiveCylinder(LCDD& lcdd)
: Segmentation(lcdd, "nonprojective_cylinder")
{
}

double NonProjectiveCylinder::gridSizeZ() const  {
  return getAttr(data.cylindrical_grid.Attr_grid_size_z);
}

double NonProjectiveCylinder::gridSizePhi() const  {
  return getAttr(data.cylindrical_grid.Attr_grid_size_phi);
}

/// Accessors: set number of bins in theta
void NonProjectiveCylinder::setThetaBinSize(double value)   {
  setAttr(data.cylindrical_grid.Attr_grid_size_phi,value);
}

/// Accessors: set grid size in Y
void NonProjectiveCylinder::setPhiBinSize(double value)   {
  setAttr(data.cylindrical_grid.Attr_grid_size_z,value);
}

/// Constructor to be used when reading the already parsed DOM tree
ProjectiveZPlane::ProjectiveZPlane(const Element& e) : Segmentation(e)  
{
}

/// Constructor to be used when creating a new DOM tree.
ProjectiveZPlane::ProjectiveZPlane(LCDD& lcdd) 
: Segmentation(lcdd,"projective_zplane")
{
}

/// Accessors: get number of bins in phi
int ProjectiveZPlane::phiBins() const  {
  return getAttr(data.cylindrical_binning.Attr_nphi);
}

/// Accessors: get number of bins in theta
int ProjectiveZPlane::thetaBins() const  {
  return getAttr(data.cylindrical_binning.Attr_ntheta);
}

/// Accessors: set number of bins in theta
void ProjectiveZPlane::setThetaBins(int value)  {
  setAttr(data.cylindrical_binning.Attr_ntheta,value);
}

/// Accessors: set grid size in Y
void ProjectiveZPlane::setPhiBins(int value)  {
  setAttr(data.cylindrical_binning.Attr_nphi,value);
}

/// Constructor to be used when creating a new object. Data are taken from the input handle
GridXY::GridXY(LCDD& lcdd, const std::string& tag) 
: Segmentation(lcdd,tag) 
{
}

/// Constructor to be used when creating a new object.
GridXY::GridXY(LCDD& lcdd, const std::string& tag, double size_x, double size_y)
: Segmentation(lcdd,tag)
{
  setAttr(data.cartesian_grid.Attr_grid_size_x,size_x);
  setAttr(data.cartesian_grid.Attr_grid_size_y,size_y);
}

/// Accessors: set grid size in X
void GridXY::setGridSizeX(double value)  {
  setAttr(data.cartesian_grid.Attr_grid_size_x,value);
}

/// Accessors: set grid size in Y
void GridXY::setGridSizeY(double value)  {
  setAttr(data.cartesian_grid.Attr_grid_size_y,value);
}

/// Constructor to be used when creating a new DOM tree.
GridXYZ::GridXYZ(LCDD& lcdd)   
: GridXY(lcdd, "grid_xyz")
{
}

/// Constructor to be used when creating a new object.
GridXYZ::GridXYZ(LCDD& lcdd, double size_x, double size_y, double size_z)
: GridXY(lcdd, "grid_xyz", size_x, size_y)
{
  setAttr(data.cartesian_grid.Attr_grid_size_z,size_z);
}

void GridXYZ::setGridSizeZ(double value)  {
  setAttr(data.cartesian_grid.Attr_grid_size_z,value);
}
