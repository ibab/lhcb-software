#ifndef DETDESC_GEOMETRY_VOLUMES_H
#define DETDESC_GEOMETRY_VOLUMES_H

// Framework include files
#include "DetDesc/Elements.h"
#include "DetDesc/lcdd/Shapes.h"
#include "DetDesc/lcdd/Objects.h"

// C/C++ include files
#include <map>

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
    struct Solid;
    struct Region;
    struct LimitSet;
    struct Material;
    struct Volume;
    struct PhysVol;
    struct Position;
    struct Rotation;
    struct VisAttr;
    struct SensitiveDetector;
  
    struct Volume : public RefElement  {
      struct Object  {
        Region            Attr_region;
        LimitSet          Attr_limits;
        VisAttr           Attr_vis;
        RefElement        Attr_sens_det;
        Object() : Attr_region(0), Attr_limits(0), Attr_vis(0), Attr_sens_det(0) {}
      };
      /// Copy from RefElement
      Volume(const Element& v) : RefElement(v) {}
      /// Copy from handle
      Volume(const Volume& v) : RefElement(v) {}
      /// Constructor to be used when creating a new geometry tree.
      Volume(LCDD& lcdd, const std::string& name);
      /// Constructor to be used when creating a new geometry tree. Also sets materuial and solid attributes
      Volume(LCDD& lcddument, const std::string& name, const Solid& s, const Material& m);
      void setSolid(const Solid& s)  const;
      void setMaterial(const Material& m)  const;
      //No. not for WIN32!! void addPhysVol(const PhysVol& vol)  const;
      void addPhysVol(const PhysVol& vol, const Transformation& tr)  const;
      void addPhysVol(const PhysVol& vol, const Position& pos, const Rotation& rot)  const;
      void setRegion(const Region& obj)  const;
      void setLimitSet(const LimitSet& obj)  const;
      void setSensitiveDetector(const SensitiveDetector& obj) const;
      void setVisAttributes(const VisAttr& obj) const;
      Solid solid() const;
      Material material() const;
      VisAttr  visAttributes() const;
      RefElement sensitiveDetector() const;
      Region region() const;
      /// Auto conversion to underlying ROOT object
      operator TGeoVolume*() const     { return _ptr<TGeoVolume>(); }
      /// Overloaded operator -> to access underlying object
      TGeoVolume* operator->() const   { return _ptr<TGeoVolume>(); }
    };

    struct PhysVol : RefElement {
      /// Constructor to be used when reading the already parsed DOM tree
      PhysVol(const RefElement& e) : RefElement(e) {}
      PhysVol& addPhysVolID(const std::string& name, int value);
    };
  }       /* End namespace Geometry           */
}         /* End namespace DetDesc            */
#endif    /* DETDESC_GEOMETRY_VOLUMES_H       */
