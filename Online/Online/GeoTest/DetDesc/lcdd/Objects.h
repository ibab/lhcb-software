// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DETDESC_GEOMETRY_OBJECTS_H
#define DETDESC_GEOMETRY_OBJECTS_H

// Framework include files
#include "DetDesc/Handle.h"
class TMap;
class TGeoElement;
class TGeoMaterial;
class TGeoMatrix;
class TGeoRotation;
class TGeoTranslation;

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  // Forward declarations
  class IDDescriptor;

  /*
   *   XML namespace declaration
   */
  namespace Geometry  {

    // Forward declarations
    struct LCDD;

    /** @class Author Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Author : public Ref_t  {
      /// Default constructor
      Author() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Author(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Author(LCDD& doc);
      //void setAuthorName(const char* nam)    {  setAttr("name",nam); }
      //void setAuthorEmail(const char* addr)  {  setAttr("email",addr); }
    };

    /** @class Header Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Header : public Ref_t  {
      /// Default constructor
      Header() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Header(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Header(LCDD& doc);
    };

    /** @class Constant Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Constant : public Ref_t  {
      /// Default constructor
      Constant() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Constant(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Constant(LCDD& doc, const std::string& name);
      /// Constructor to be used when creating a new DOM tree
      Constant(LCDD& doc, const std::string& name, const std::string& val);
    };

    /** @class Transformation Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Transform : public Handle<TGeoMatrix> {
      /// Default constructor
      Transform() : Handle<TGeoMatrix>() {}
      /// Default constructor
      Transform(TGeoMatrix* m) : Handle<TGeoMatrix>(m) {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Transform(const Handle<Q>& e) : Handle<TGeoMatrix>(e) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Transform(LCDD& doc, const std::string& name);
    };

    /** @class Position Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Position : public Handle<TGeoTranslation>  {
      /// Default constructor
      Position() : Handle<TGeoTranslation>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Position(const Handle<Q>& e) : Handle<TGeoTranslation>(e) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Position(LCDD& doc, const std::string& name, double x, double y, double z);
    };

    /** @class Rotation Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Rotation : public Handle<TGeoRotation>  {
      /// Default constructor
      Rotation() : Handle<TGeoRotation>() {}
      /// Constructor to be used RefElement reading the already parsed DOM tree
      template <typename Q> 
      Rotation(const Handle<Q>& e) : Handle<TGeoRotation>(e) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Rotation(LCDD& doc, const std::string& name);
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Rotation(LCDD& doc, const std::string& name, double x, double y, double z);
    };

    /** @class Atom Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Atom : public Handle<TGeoElement>  {
      /// Default constructor
      Atom() : Handle<TGeoElement>() {}
      /// Constructor to be used when creating a new DOM tree
      template <typename Q> 
      Atom(const Handle<Q>& e) : Handle<TGeoElement>(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Atom(LCDD& doc, const std::string& name, const std::string& formula, int Z, int N, double density);
    };

    /** @class Material Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Material : public Handle<TGeoMaterial>  {
      /// Default constructor
      Material() : Handle<TGeoMaterial>() {}
      /// Constructor to be used when creating a new DOM tree
      template <typename Q> 
      Material(const Handle<Q>& e) : Handle<TGeoMaterial>(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Material(LCDD& doc, const std::string& name);
    };

    /** @class VisAttr Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct VisAttr : public Ref_t  {
      struct Object  {
        int           color;
        unsigned char drawingStyle, lineStyle, showDaughters, visible;
        Object() : color(0), drawingStyle(true), showDaughters(true), visible(true)  {}
      };
      enum DrawingStyle { 
        WIREFRAME=0x1,
        LAST_DRAWING_STYLE
      };
      enum LineStyle  {
        SOLID=0x1,
        DASHED=0x2,
        LAST_LINE_STYLE
      };
      /// Default constructor
      VisAttr() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      VisAttr(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      VisAttr(LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      /// Set Flag to show/hide daughter elements
      void setShowDaughters(bool value);
      /// Set line style
      void setLineStyle(LineStyle style);
      /// Set drawing style
      void setDrawingStyle(DrawingStyle style);
      /// Set visibility flag
      void setVisible(bool value);
      /// Set alpha value
      void setAlpha(float value);
      /// Set object color
      void setColor(float red, float green, float blue);
    };

    /** @class Limit Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Limit : public Ref_t  {
      typedef std::pair<std::string,double> Object;

      /// Constructor to be used when creating a new DOM tree
      Limit(LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      void setParticles(const std::string& particleNames);
      void setValue(double value);
      void setUnit(const std::string& unit);
    };

    /** @class LimitSet Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct LimitSet : public Ref_t  {
      typedef TMap Object;
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      LimitSet(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      LimitSet(LCDD& doc, const std::string& name);
      void addLimit(const Ref_t& limit);
    };

    /** @class Region Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Region : public Ref_t  {
      struct Object  {
        double      Attr_threshold;
        double      Attr_cut;
        bool        Attr_store_secondaries;
        std::string Attr_lunit, Attr_eunit;
      };
      /// Default constructor
      Region() : Ref_t() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Region(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      Region(LCDD& doc, const std::string& name);

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      Region& setStoreSecondaries(bool value);
      Region& setThreshold(double value);
      Region& setCut(double value);
      Region& setLengthUnit(const std::string& unit);
      Region& setEnergyUnit(const std::string& unit);
    };

    /** @class IDSpec Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct IDSpec : public Ref_t   {
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      IDSpec(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor to be used when creating a new DOM tree
      IDSpec(LCDD& doc, const std::string& name, const IDDescriptor& dsc);
      void addField(const std::string& name, const std::pair<int,int>& field);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DetDesc            */
#endif    /* DETDESC_GEOMETRY_OBJECTS_H       */
