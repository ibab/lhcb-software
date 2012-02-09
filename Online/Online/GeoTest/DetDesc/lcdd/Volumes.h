// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DETDESC_GEOMETRY_VOLUMES_H
#define DETDESC_GEOMETRY_VOLUMES_H

// Framework include files
#include "DetDesc/Handle.h"
#include "DetDesc/lcdd/Shapes.h"
#include "DetDesc/lcdd/Objects.h"

// C/C++ include files
#include <map>

#include "TGeoShape.h"
#include "TGeoNode.h"

// Forward declarations
class TGeoVolume;

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  class IDDescriptor;

  /*
  *   XML namespace declaration
  */
  namespace Geometry  {

    // Forward declarations
    struct LCDD;
    struct Region;
    struct LimitSet;
    struct Material;
    struct Volume;
    struct PlacedVolume;
    struct Position;
    struct Rotation;
    struct VisAttr;
    struct SensitiveDetector;

    /** @class PlacedVolume Volume.h  DetDesc/lcdd/Volume.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct PlacedVolume : Handle<TGeoNode> {
      /// Constructor to be used when reading the already parsed DOM tree
      PlacedVolume(const TGeoNode* e) : Handle<TGeoNode>(e) {}
      /// Default constructor
      PlacedVolume() : Handle<TGeoNode>() {}
      /// Copy assignment
      PlacedVolume(const PlacedVolume& e) :  Handle<TGeoNode>(e) {}
      /// Add identifier
      PlacedVolume& addPhysVolID(const std::string& name, int value);
    };
  
    /** @class Volume Volume.h  DetDesc/lcdd/Volume.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Volume : public Handle<TGeoVolume>  {
      typedef Handle<TGeoVolume> Base;
      struct Object  {
        Region            Attr_region;
        LimitSet          Attr_limits;
        VisAttr           Attr_vis;
        Ref_t       Attr_sens_det;
        Object() : Attr_region(), Attr_limits(), Attr_vis(), Attr_sens_det() {}
      };
      /// Default constructor
      Volume() : Base(0) {}

      /// Copy from handle
      Volume(const Volume& v) : Base(v) {}

      /// Copy from arbitrary Element
      template <typename T> Volume(const Handle<T>& v) : Base(v) {}

      /// Constructor to be used when creating a new geometry tree.
      Volume(LCDD& lcdd, const std::string& name);

      /// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
      Volume(LCDD& lcddument, const std::string& name, const Solid& s, const Material& m);

      /// Set the volume's solid shape
      void setSolid(const Solid& s)  const;
      /// Set the volume's material
      void setMaterial(const Material& m)  const;

      /// Place daughter volume. The position and rotation are the identity
      PlacedVolume placeVolume(const Volume& vol)  const  
      { return placeVolume(vol,IdentityPos());                        }
      /// Place un-rotated daughter volume at the given position.
      PlacedVolume placeVolume(const Volume& vol, const Position& pos)  const;
      /// Place rotated daughter volume. The position is automatically the identity position
      PlacedVolume placeVolume(const Volume& vol, const Rotation& rot)  const;
      /// Place translated and rotated daughter volume
      PlacedVolume placeVolume(const Volume& vol, const Position& pos, const Rotation& rot)  const;

      /// Place daughter volume. The position and rotation are the identity
      PlacedVolume placeVolume(const Volume& vol, const IdentityPos& pos)  const;
      /// Place daughter volume. The position and rotation are the identity
      PlacedVolume placeVolume(const Volume& vol, const IdentityRot& pos)  const;

      void setRegion(const Region& obj)  const;
      void setLimitSet(const LimitSet& obj)  const;
      void setSensitiveDetector(const SensitiveDetector& obj) const;
      void setVisAttributes(const VisAttr& obj) const;
      Solid solid() const;
      Material material() const;
      VisAttr  visAttributes() const;
      Ref_t sensitiveDetector() const;
      Region region() const;
      /// Auto conversion to underlying ROOT object
      operator TGeoVolume*() const     { return m_element; }
    };

  }       /* End namespace Geometry           */
}         /* End namespace DetDesc            */
#endif    /* DETDESC_GEOMETRY_VOLUMES_H       */
