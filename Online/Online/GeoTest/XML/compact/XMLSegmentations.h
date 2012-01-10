#ifndef DETDESC_XMLSEGMENTATIONS_H
#define DETDESC_XMLSEGMENTATIONS_H
#include "XML/XMLTags.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct Segmentation : public Element   {
      /// Constructor to be used when reading the already parsed DOM tree
      Segmentation(Handle_t e);
      Segmentation(const Document& doc, const XMLCh* type);
      bool useForHitPosition() const;
      const XMLCh* type() const;
      /// Create LCDD segementation from compact description
      Segmentation fromCompact(const Document& doc)  const;
    };

    struct ProjectiveCylinder : public Segmentation  {
      /// Constructor to be used when reading the already parsed DOM tree
      ProjectiveCylinder(Handle_t e) : Segmentation(e) {}
      ProjectiveCylinder(const Document& doc);
      /// Create LCDD segmentation object from compact description
      Segmentation fromCompact(Handle_t seg)   const;
      int thetaBins() const;
      int phiBins() const;
    };

    struct NonProjectiveCylinder : public Segmentation  {
      /// Constructor to be used when reading the already parsed DOM tree
      NonProjectiveCylinder(Handle_t e) : Segmentation(e) {}
      NonProjectiveCylinder(const Document& doc);
      double gridSizeZ() const;
      double gridSizePhi() const;
      /// Create LCDD segmentation object from compact description
      Segmentation fromCompact(Handle_t seg)   const;
    };

    struct ProjectiveZPlane : public Segmentation  {
      /// Constructor to be used when reading the already parsed DOM tree
      ProjectiveZPlane(Handle_t e);
      ProjectiveZPlane(const Document& doc);
      int thetaBins() const;
      int phiBins() const;
      /// Create LCDD segmentation object from compact description
      Segmentation fromCompact(Handle_t seg)   const;
    };

    struct GridXYZ : public Segmentation  {
      /// Constructor to be used when reading the already parsed DOM tree
      GridXYZ(Handle_t h) : Segmentation(h) {}
      /// Constructor to be used when creating a new DOM tree.
      GridXYZ(const Document& doc);
      /// Accessors
      void setGridSizeX(double value);
      void setGridSizeY(double value);
      void setGridSizeZ(double value);
      /// Create LCDD segmentation object from compact description
      Segmentation fromCompact(Handle_t seg)   const;
    };

    struct GridXY : public Segmentation   {
      /// Constructor to be used when reading the already parsed DOM tree
      GridXY(Handle_t h) : Segmentation(h) {}
      /// Constructor to be used when creating a new DOM tree. Data are taken from the input handle
      GridXY(const Document& doc, const XMLCh* tag) : Segmentation(doc,tag) {}
      /// Create LCDD segmentation object from compact description
      Segmentation fromCompact(Handle_t seg)   const;
    };

    struct CartesianGridXY : public GridXY   {
      /// Constructor to be used when reading the already parsed DOM tree
      CartesianGridXY(Handle_t element) : GridXY(element) {}
      /// Constructor to be used when creating a new DOM tree. Data are taken from the input handle
      CartesianGridXY(const Document& doc) : GridXY(doc,Tag_cartesian_grid_xy) {}
    };

    struct GlobalGridXY : public GridXY   {
      /// Constructor to be used when reading the already parsed DOM tree
      GlobalGridXY(Handle_t element) : GridXY(element) {}
      /// Constructor to be used when creating a new DOM tree. Data are taken from the input handle
      GlobalGridXY(const Document& doc) : GridXY(doc,Tag_global_grid_xy) {}
    };

  }
}         /* End namespace DetDesc   */
#endif    /* DETDESC_XMLSEGMENTATIONS_H     */
