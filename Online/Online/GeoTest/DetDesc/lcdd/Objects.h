#ifndef DETDESC_GEOMETRY_OBJECTS_H
#define DETDESC_GEOMETRY_OBJECTS_H

// Framework include files
#include "DetDesc/Elements.h"
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
    struct Author : public RefElement_type<TNamed>  {
      /// Default constructor
      Author() : RefElement_type<TNamed>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Author(const Element_type<Q>& e) : RefElement_type<TNamed>(e)  {}
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
    struct Header : public RefElement_type<TNamed>  {
      /// Default constructor
      Header() : RefElement_type<TNamed>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Header(const Element_type<Q>& e) : RefElement_type<TNamed>(e)  {}
      /// Constructor to be used when creating a new DOM tree
      Header(LCDD& doc);
    };

    /** @class Constant Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Constant : public RefElement_type<TNamed>  {
      /// Default constructor
      Constant() : RefElement_type<TNamed>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Constant(const Element_type<Q>& e) : RefElement_type<TNamed>(e)  {}
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
    struct Transformation : public RefElement_type<TGeoMatrix> {
      /// Default constructor
      Transformation() : RefElement_type<TGeoMatrix>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Transformation(const Element_type<Q>& e) : RefElement_type<TGeoMatrix>(e) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Transformation(LCDD& doc, const std::string& name);
    };

    /** @class Position Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Position : public RefElement_type<TGeoTranslation>  {
      /// Default constructor
      Position() : RefElement_type<TGeoTranslation>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Position(const Element_type<Q>& e) : RefElement_type<TGeoTranslation>(e) {}
      /// Constructor to be used when creating a new DOM tree. Automatically sets attributes
      Position(LCDD& doc, const std::string& name, double x, double y, double z);
    };

    /** @class Rotation Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Rotation : public RefElement_type<TGeoRotation>  {
      /// Default constructor
      Rotation() : RefElement_type<TGeoRotation>() {}
      /// Constructor to be used RefElement reading the already parsed DOM tree
      template <typename Q> 
      Rotation(const Element_type<Q>& e) : RefElement_type<TGeoRotation>(e) {}
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
    struct Atom : public RefElement_type<TGeoElement>  {
      /// Default constructor
      Atom() : RefElement_type<TGeoElement>() {}
      /// Constructor to be used when creating a new DOM tree
      template <typename Q> 
      Atom(const Element_type<Q>& e) : RefElement_type<TGeoElement>(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Atom(LCDD& doc, const std::string& name, const std::string& formula, int Z, int N, double density);
    };

    /** @class Material Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Material : public RefElement_type<TGeoMaterial>  {
      /// Default constructor
      Material() : RefElement_type<TGeoMaterial>() {}
      /// Constructor to be used when creating a new DOM tree
      template <typename Q> 
      Material(const Element_type<Q>& e) : RefElement_type<TGeoMaterial>(e) {}
      /// Constructor to be used when reading the already parsed DOM tree
      Material(LCDD& doc, const std::string& name);
    };

    /** @class VisAttr Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct VisAttr : public RefElement_type<TNamed>  {
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
      VisAttr() : RefElement_type<TNamed>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      VisAttr(const Element_type<Q>& e) : RefElement_type<TNamed>(e) {}
      /// Constructor to be used when creating a new DOM tree
      VisAttr(LCDD& doc, const std::string& name);
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
    struct Limit : public RefElement_type<TNamed>  {
      typedef std::pair<std::string,double> Object;

      /// Constructor to be used when creating a new DOM tree
      Limit(LCDD& doc, const std::string& name);
      void setParticles(const std::string& particleNames);
      void setValue(double value);
      void setUnit(const std::string& unit);
    };

    /** @class LimitSet Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct LimitSet : public RefElement_type<TNamed>  {
      typedef TMap Object;
      /// Constructor to be used when reading the already parsed DOM tree
      LimitSet() : RefElement_type<TNamed>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      LimitSet(const Element_type<Q>& e) : RefElement_type<TNamed>(e) {}
      /// Constructor to be used when creating a new DOM tree
      LimitSet(LCDD& doc, const std::string& name);
      void addLimit(const RefElement& limit);
    };

    /** @class Region Objects.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct Region : public RefElement_type<TNamed>  {
      struct Object  {
        double      Attr_threshold;
        double      Attr_cut;
        bool        Attr_store_secondaries;
        std::string Attr_lunit, Attr_eunit;
      };
      /// Default constructor
      Region() : RefElement_type<TNamed>() {}
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      Region(const Element_type<Q>& e) : RefElement_type<TNamed>(e) {}
      /// Constructor to be used when creating a new DOM tree
      Region(LCDD& doc, const std::string& name);

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
    struct IDSpec : public RefElement_type<TNamed>   {
      /// Constructor to be used when reading the already parsed DOM tree
      template <typename Q> 
      IDSpec(const Element_type<Q>& e) : RefElement_type<TNamed>(e) {}
      /// Constructor to be used when creating a new DOM tree
      IDSpec(LCDD& doc, const std::string& name, const IDDescriptor& dsc);
      void addField(const std::string& name, const std::pair<int,int>& field);
    };

  }       /* End namespace Geometry           */
}         /* End namespace DetDesc            */
#endif    /* DETDESC_GEOMETRY_OBJECTS_H       */
