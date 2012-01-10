#include "XML/compact/XMLSegmentations.h"
#include <iostream>

using namespace std;
using namespace DetDesc::XML;

/// Constructor to be used when reading the already parsed DOM tree
Segmentation::Segmentation(Handle_t e) : Element(e)  {}

Segmentation::Segmentation(const Document& doc, const XMLCh* type) : Element(doc,type)  {}

bool Segmentation::useForHitPosition() const   {
  return attr<bool>(Attr_useForHitPosition);
}

const XMLCh* Segmentation::type() const   {
  return m_element.attr<cpXMLCh>(Attr_type);
}

Segmentation Segmentation::fromCompact(const Document& doc)   const  {
  string seg_typ = attr<string>(Attr_type);
  if ( seg_typ == "GridXYZ" )
    return GridXYZ(doc).fromCompact(*this);
  else if ( seg_typ == "RegularNgonCartesianGridXY" )
    return GridXYZ(doc).fromCompact(*this);
  else if ( seg_typ == "EcalBarrelCartesianGridXY" )
    return GridXYZ(doc).fromCompact(*this);
  else if ( seg_typ == "GlobalGridXY" )
    return GlobalGridXY(doc).fromCompact(*this);
  else if ( seg_typ == "CartesianGridXY" )
    return CartesianGridXY(doc).fromCompact(*this);
  else if ( seg_typ == "ProjectiveCylinder" )
    return ProjectiveCylinder(doc).fromCompact(*this);
  else if ( seg_typ == "NonProjectiveCylinder" )
    return NonProjectiveCylinder(doc).fromCompact(*this);
  else if ( seg_typ == "ProjectiveZPlane" )
    return ProjectiveZPlane(doc).fromCompact(*this);
  else 
    cout << "Request to create UNKNOWN segmentation of type:" << seg_typ << endl;
  return Segmentation(0);
}

ProjectiveCylinder::ProjectiveCylinder(const Document& doc) 
: Segmentation(doc,Tag_projective_cylinder)   {}

int ProjectiveCylinder::thetaBins() const  {
  return attr<int>(Attr_ntheta);
}
int ProjectiveCylinder::phiBins() const  {
  return attr<int>(Attr_nphi);
}

/// Create LCDD segmentation object from compact description
Segmentation ProjectiveCylinder::fromCompact(Handle_t seg)   const  {
  if ( seg.hasAttr(Attr_thetaBins) ) setAttr(Attr_ntheta,seg.attr<cpXMLCh>(Attr_thetaBins));
  if ( seg.hasAttr(Attr_phiBins) ) setAttr(Attr_nphi,seg.attr<cpXMLCh>(Attr_phiBins));
  return *this;
}

NonProjectiveCylinder::NonProjectiveCylinder(const Document& doc)
: Segmentation(doc, Tag_nonprojective_cylinder)
{
  setAttr(Attr_grid_size_phi,Tag_NULL);
  setAttr(Attr_grid_size_z,Tag_NULL);
}

double NonProjectiveCylinder::gridSizeZ() const  {
  return m_element.attr<double>(Attr_grid_size_z);
}

double NonProjectiveCylinder::gridSizePhi() const  {
  return m_element.attr<double>(Attr_grid_size_phi);
}

/// Create LCDD segmentation object from compact description
Segmentation NonProjectiveCylinder::fromCompact(Handle_t seg)    const   {
  if ( seg.hasAttr(Attr_gridSizePhi) ) setAttr(Attr_grid_size_phi,seg.attr<cpXMLCh>(Attr_gridSizePhi));
  if ( seg.hasAttr(Attr_gridSizeZ)   ) setAttr(Attr_grid_size_z,seg.attr<cpXMLCh>(Attr_gridSizeZ));
  return *this;
}

/// Constructor to be used when reading the already parsed DOM tree
ProjectiveZPlane::ProjectiveZPlane(Handle_t e) : Segmentation(e)  {
}

/// Constructor to be used when creating a new DOM tree.
ProjectiveZPlane::ProjectiveZPlane(const Document& doc) 
: Segmentation(doc,Tag_projective_zplane)   {}

int ProjectiveZPlane::thetaBins() const  {
  return attr<int>(Attr_ntheta);
}
int ProjectiveZPlane::phiBins() const  {
  return attr<int>(Attr_nphi);
}
/// Create LCDD segmentation object from compact description
Segmentation ProjectiveZPlane::fromCompact(Handle_t seg)   const  {
  if ( seg.hasAttr(Attr_thetaBins) ) setAttr(Attr_ntheta,seg.attr<cpXMLCh>(Attr_thetaBins));
  if ( seg.hasAttr(Attr_phiBins) ) setAttr(Attr_nphi,seg.attr<cpXMLCh>(Attr_phiBins));
  return *this;
}

/// Constructor to be used when creating a new DOM tree.
GridXYZ::GridXYZ(const Document& doc)   
: Segmentation(doc, Tag_grid_xyz)
{
  setAttr(Attr_grid_size_x,Tag_NULL);
  setAttr(Attr_grid_size_y,Tag_NULL);
  setAttr(Attr_grid_size_z,Tag_NULL);
}

void GridXYZ::setGridSizeX(double value)  {
  setAttr(Attr_grid_size_x,value);
}

void GridXYZ::setGridSizeY(double value)  {
  setAttr(Attr_grid_size_y,value);
}

void GridXYZ::setGridSizeZ(double value)  {
  setAttr(Attr_grid_size_z,value);
}

/// Create LCDD segmentation object from compact description
Segmentation GridXYZ::fromCompact(Handle_t seg)   const  {
  if ( seg.hasAttr(Attr_gridSizeX) ) setAttr(Attr_grid_size_x,seg.attr<cpXMLCh>(Attr_gridSizeX));
  if ( seg.hasAttr(Attr_gridSizeY) ) setAttr(Attr_grid_size_y,seg.attr<cpXMLCh>(Attr_gridSizeY));
  if ( seg.hasAttr(Attr_gridSizeZ) ) setAttr(Attr_grid_size_z,seg.attr<cpXMLCh>(Attr_gridSizeZ));
  return *this;
}

/// Create LCDD segmentation object from compact description
Segmentation GridXY::fromCompact(Handle_t seg)   const  {
  if ( seg.hasAttr(Attr_gridSizeX) ) setAttr(Attr_grid_size_x,seg.attr<cpXMLCh>(Attr_gridSizeX));
  if ( seg.hasAttr(Attr_gridSizeY) ) setAttr(Attr_grid_size_y,seg.attr<cpXMLCh>(Attr_gridSizeY));
  return *this;
}
