#ifndef DETDESC_GEOMETRY_SOLIDS_H
#define DETDESC_GEOMETRY_SOLIDS_H

// Framework include files
#include "DetDesc/Elements.h"

// C/C++ include files
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
#include <vector>

// Forward declarations
class TGeoShape;
class TGeoBBox;
class TGeoPcon;
class TGeoPgon;
class TGeoCone;
class TGeoTrd2;
class TGeoTrap;
class TGeoTubeSeg;
class TGeoCompositeShape;
/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   Geometry namespace declaration
  */
  namespace Geometry  {

    // Forward declarations
    struct Position;
    struct Rotation;

    /**@class ZPlane
    *  Not identifyable object - no RefElement
    *
    *   @author  M.Frank
    *   @version 1.0
    */
    struct ZPlane  {
      double m_rmin, m_rmax, m_z;
      /// Constructor to be used when creating a new object
      ZPlane(double rmin, double rmax, double z);
      /// Set values
      ZPlane& setRmin(double value);
      ZPlane& setRmax(double value);
      ZPlane& setZ(double value);
      double rmin() const { return m_rmin; }
      double rmax() const { return m_rmax; }
      double z()    const { return m_z;    }
    };

    template <typename T> struct Solid_type : public RefElement_type<T>  {
      protected:
      typedef T Implementation;
      void _setDimensions(double* param);
      /// Assign pointrs and register solid to geometry
      void _assign(LCDD& lcdd, Implementation* n, const std::string& nam, const std::string& tit, bool cbbox=true);
      

      public:

      /// Default constructor for uninitialized object
      Solid_type() : RefElement_type<Implementation>() {}

      /// Direct assignment using the implementation pointer 
      Solid_type(Implementation* p) : RefElement_type<Implementation>(p) {}

      /// Constructor to be used when reading the already parsed object
      Solid_type(const Element_type<Implementation>& e) : RefElement_type<Implementation>(e) {}

      /// Constructor to be used when reading the already parsed object: need to check pointers
      template <typename Q> 
      Solid_type(const Element_type<Q>& e) : RefElement_type<T>(e) {}

      /// Auto conversion to underlying ROOT object
      operator Implementation*() const     { return this->m_element; }

      /// Overloaded operator -> to access underlying object
      Implementation* operator->() const   { return this->m_element; }
    };
    typedef Solid_type<TGeoShape> Solid;


    struct Box : public Solid_type<TGeoBBox>  {
      protected:
      void make(LCDD& lcdd, const std::string& name, double x, double y, double z);

      public:
      /// Constructor to be used when reading the already parsed box object
      template <typename Q> 
      Box(const Element_type<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new box object
      Box(LCDD& lcdd, const std::string& name) 
      {  make(lcdd, name, 0, 0, 0); }

      /// Constructor to be used when creating a new box object
      Box(LCDD& lcdd, const std::string& name, double x, double y, double z)
      {  make(lcdd, name, x, y, z); }

      /// Constructor to be used when creating a new box object
      template<class X, class Y, class Z>
      Box(LCDD& lcdd, const std::string& name, const X& x, const Y& y, const Z& z)
      {  make(lcdd, name, _toDouble(x),_toDouble(y),_toDouble(z)); }

      /// Set the box dimensions
      Box& setDimensions(double x, double y, double z);
    };

    struct Polycone : public Solid_type<TGeoPcon>  {
      /// Constructor to be used when reading the already parsed polycone object
      template <typename Q> Polycone(const Element_type<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new polycone object
      Polycone(LCDD& lcdd, const std::string& name);

      /// Constructor to be used when creating a new polycone object
      Polycone(LCDD& lcdd, const std::string& name, double start, double delta);

      /// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
      Polycone(LCDD& lcdd, const std::string& name, double start, double delta, const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);

      /// Add Z-planes to the Polycone
      void addZPlanes(const std::vector<double>& rmin, const std::vector<double>& rmax, const std::vector<double>& z);
    };

    struct Tube : public Solid_type<TGeoTubeSeg>  {
      protected:
      void make(LCDD& lcdd, const std::string& name,double rmin,double rmax,double z,double deltaPhi);

      public:
      /// Constructor to assign an object
      template <typename Q> Tube(const Element_type<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new tube object
      Tube(LCDD& lcdd, const std::string& name)
      { make(lcdd,name,0, 0, 0, 0); }

      /// Constructor to be used when creating a new tube object with attribute initialization
      Tube(LCDD& lcdd, const std::string& name, double rmin, double rmax, double z, double deltaPhi=2*M_PI)
      { make(lcdd,name,rmin, rmax, z, deltaPhi); }

      /// Constructor to be used when creating a new tube object with attribute initialization
      template<class RMIN, class RMAX, class Z, class DELTAPHI>
      Tube(LCDD& lcdd, const std::string& name, const RMIN& rmin, const RMAX& rmax, const Z& z, const DELTAPHI& deltaPhi)  
      {
        make(lcdd,name,_toDouble(rmin),_toDouble(rmax),_toDouble(z),_toDouble(deltaPhi));
      }

      /// Set the box dimensions
      Tube& setDimensions(double rmin, double rmax, double z, double deltaPhi=2*M_PI);
    };

    struct Cone : public Solid_type<TGeoCone>  {
      protected:
      void make(LCDD& lcdd, const std::string& name,double z,double rmin1,double rmax1,double rmin2,double rmax2);

      public:

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Cone(const Element_type<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new object
      Cone(LCDD& lcdd, const std::string& name)
      { make(lcdd,name,0, 0, 0, 0, 0); }

      /// Constructor to be used when creating a new object with attribute initialization
      Cone(LCDD& lcdd, const std::string& name,
           double z,
           double rmin1,
           double rmax1,
           double rmin2,
           double rmax2)
      { make(lcdd,name,z,rmin1,rmax1,rmin2,rmax2); }

      template<typename Z, typename RMIN1, typename RMAX1, typename RMIN2, typename RMAX2>
      Cone(LCDD& lcdd, const std::string& name, const Z& z, const RMIN1& rmin1, const RMAX1& rmax1, const RMIN2& rmin2, const RMAX2& rmax2)
      { make(lcdd,name,_toDouble(z),_toDouble(rmin1),_toDouble(rmax1),_toDouble(rmin2),_toDouble(rmax2)); }

      /// Set the box dimensions
      Cone& setDimensions(double z,double rmin1,double rmax1,double rmin2,double rmax2);
    };

    struct Trap : public Solid_type<TGeoTrap>  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Trap( const Element_type<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new object with attribute initialization
      Trap( LCDD& lcdd, const std::string& name,
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
            double alpha2);

      /// Set the trap dimensions
      Trap& setDimensions(double z,double theta,double phi,
                          double y1,double x1,double x2,double alpha1,
                          double y2,double x3,double x4,double alpha2);
    };

    struct Trapezoid : public Solid_type<TGeoTrd2> {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Trapezoid(const Element_type<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new object
      Trapezoid(LCDD& lcdd, const std::string& name);
      /// Constructor to be used when creating a new object with attribute initialization
      Trapezoid(LCDD& lcdd, const std::string& name, double x1, double x2, double y1, double y2, double z);
      /// Set the Trapezoid dimensions
      Trapezoid& setDimensions(double x1, double x2, double y1, double y2, double z);
      Trapezoid& setX1(double value);
      Trapezoid& setX2(double value);
      Trapezoid& setY1(double value);
      Trapezoid& setY2(double value);
      Trapezoid& setZ(double value);
      double x1() const;
      double x2() const;
      double y1() const;
      double y2() const;
      double z() const;
    };

    struct PolyhedraRegular : public Solid_type<TGeoPgon>  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> 
      PolyhedraRegular(const RefElement_type<Q>& e) : Solid_type<Implementation>(e) {}
      /// Constructor to be used when creating a new object
      PolyhedraRegular(LCDD& lcdd, const std::string& name, int nsides, double rmin, double rmax, double zlen);
    };

    struct BooleanSolid : public Solid_type<TGeoCompositeShape>  {
      protected:
      /// Constructor to be used when reading the already parsed object
      BooleanSolid() : Solid_type<Implementation>() {}

      public:
      /// Constructor to be used when reading the already parsed object
      template <class Q> 
      BooleanSolid(const Element_type<Q>& e) : Solid_type<Implementation>(e) {}

      /// Constructor to be used when creating a new object
      BooleanSolid(LCDD& lcdd, const std::string& type, const std::string& name, const std::string& expr);
    };

    struct SubtractionSolid : public BooleanSolid  {
      /// Constructor to be used when reading the already parsed object
      template<typename Q> SubtractionSolid(const RefElement_type<Q>& e) : BooleanSolid(e) {}
      /// Constructor to be used when creating a new object
      SubtractionSolid(LCDD& lcdd, const std::string& name, const std::string& expr);
      /// Constructor to be used when creating a new object
      SubtractionSolid(LCDD& lcdd, const std::string& name, const Solid& shape1, const Solid& shape2);
      /// Constructor to be used when creating a new object
      SubtractionSolid(LCDD& lcdd, const std::string& name, const Solid& shape1, const Solid& shape2, const Position& pos, const Rotation& rot);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DetDesc            */
#endif    /* DETDESC_GEOMETRY_SOLIDS_H        */
