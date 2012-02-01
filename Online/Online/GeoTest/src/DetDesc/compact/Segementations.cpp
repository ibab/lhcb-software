// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/compact/Segmentations.h"
#include <iostream>

using namespace std;
using namespace DetDesc::Geometry;

Segmentation::Segmentation(LCDD& /* lcdd */, const string& type)
{
  assign(new Value<TNamed,Segmentation::Object>(),"segmentation",type);
}

bool Segmentation::useForHitPosition() const   {
  return data<Object>()->Attr_useForHitPosition != 0;
}

const string Segmentation::type() const   {
  return m_element->GetTitle();
}

ProjectiveCylinder::ProjectiveCylinder(LCDD& lcdd) 
: Segmentation(lcdd,"projective_cylinder")   {}

/// Accessors: get number of bins in theta
int ProjectiveCylinder::thetaBins() const  {
  return data<Object>()->data.cylindrical_binning.Attr_ntheta;
}

/// Accessors: get number of bins in phi
int ProjectiveCylinder::phiBins() const  {
  return data<Object>()->data.cylindrical_binning.Attr_nphi;
}

/// Accessors: set number of bins in theta
void ProjectiveCylinder::setThetaBins(int value)  {
  data<Object>()->data.cylindrical_binning.Attr_ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveCylinder::setPhiBins(int value)  {
  data<Object>()->data.cylindrical_binning.Attr_nphi = value;
}

NonProjectiveCylinder::NonProjectiveCylinder(LCDD& lcdd)
: Segmentation(lcdd, "nonprojective_cylinder")
{
}

double NonProjectiveCylinder::gridSizeZ() const  {
  return data<Object>()->data.cylindrical_grid.Attr_grid_size_z;
}

double NonProjectiveCylinder::gridSizePhi() const  {
  return data<Object>()->data.cylindrical_grid.Attr_grid_size_phi;
}

/// Accessors: set number of bins in theta
void NonProjectiveCylinder::setThetaBinSize(double value)   {
  data<Object>()->data.cylindrical_grid.Attr_grid_size_phi = value;
}

/// Accessors: set grid size in Y
void NonProjectiveCylinder::setPhiBinSize(double value)   {
  data<Object>()->data.cylindrical_grid.Attr_grid_size_z = value;
}

/// Constructor to be used when creating a new DOM tree.
ProjectiveZPlane::ProjectiveZPlane(LCDD& lcdd) 
: Segmentation(lcdd,"projective_zplane")
{
}

/// Accessors: get number of bins in phi
int ProjectiveZPlane::phiBins() const  {
  return data<Object>()->data.cylindrical_binning.Attr_nphi;
}

/// Accessors: get number of bins in theta
int ProjectiveZPlane::thetaBins() const  {
  return data<Object>()->data.cylindrical_binning.Attr_ntheta;
}

/// Accessors: set number of bins in theta
void ProjectiveZPlane::setThetaBins(int value)  {
  data<Object>()->data.cylindrical_binning.Attr_ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveZPlane::setPhiBins(int value)  {
  data<Object>()->data.cylindrical_binning.Attr_nphi = value;
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
  data<Object>()->data.cartesian_grid.Attr_grid_size_x = size_x;
  data<Object>()->data.cartesian_grid.Attr_grid_size_y = size_y;
}

/// Accessors: set grid size in X
void GridXY::setGridSizeX(double value)  {
  data<Object>()->data.cartesian_grid.Attr_grid_size_x = value;
}

/// Accessors: set grid size in Y
void GridXY::setGridSizeY(double value)  {
  data<Object>()->data.cartesian_grid.Attr_grid_size_y = value;
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
  data<Object>()->data.cartesian_grid.Attr_grid_size_z = size_z;
}

void GridXYZ::setGridSizeZ(double value)  {
  data<Object>()->data.cartesian_grid.Attr_grid_size_z = value;
}
