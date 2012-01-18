#ifndef DETDESC_COMPACT_CONVERSION_H
#define DETDESC_COMPACT_CONVERSION_H

// C/C++ include files
#include <map>
#include <iostream>

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  // Forward declarations
  namespace XML  {
    struct Handle_t;
  };

  /*
   *   XML namespace declaration
   */
  namespace Geometry  {

    // Forward declarations
    struct LCDD;
    struct Handle_t;
    struct Element;
    struct RefElement;
    struct SensitiveDetector;

    // Function prototypes used for object conversions
    template <typename T, typename Q> Element toObject(LCDD& lcdd, const Q& xml);
    template <typename T, typename Q> RefElement toRefObject(LCDD& lcdd, const Q& xml);
    template <typename T, typename Q> RefElement toRefObject(LCDD& lcdd, const Q& xml, SensitiveDetector& sens);


    /** @class LCDDActor Conversions.h  DetDesc/compact/Conversions.h
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    struct LCDDActor  {
      LCDD&  lcdd;
      LCDDActor(LCDD& l) : lcdd(l) {}
    };

    /** @class Converter Conversions.h  DetDesc/compact/Conversions.h
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct Converter : public LCDDActor {
      typedef T to_type;
      Converter(LCDD& lcdd) : LCDDActor(lcdd) {}
      void operator()(const XML::Handle_t& xml) const;
    };

    /** @class Printer Conversions.h  DetDesc/compact/Conversions.h
      *
      *  @author   M.Frank
      *  @version  1.0
      */
    template <typename T> struct PrintMap  {
      typedef typename std::map<std::string,Handle_t> container_type;
      typedef T item_type;
      const LCDD&   lcdd;
      std::ostream& os;
      std::string   text;
      std::map<std::string,Handle_t> cont;
      PrintMap(const LCDD& l, std::ostream& stream, const std::map<std::string,Handle_t>& c, const std::string& t="") : lcdd(l), os(stream), text(t), cont(c)  {}
      void operator()() const;
    };
    template <typename T> struct Printer  {
      const LCDD&   lcdd;
      std::ostream& os;
      std::string   prefix;
      Printer(const LCDD& l, std::ostream& stream, const std::string& p="") : lcdd(l), os(stream), prefix(p) {}
      void operator()(const T& value) const;
    };
    inline const char* yes_no(bool value) { return value ? "YES" : "NO "; }
  }
}         /* End namespace DetDesc   */
#endif    /* DETDESC_COMPACT_CONVERSION_H    */
