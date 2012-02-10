// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DETDESC_XMLDETECTOR_H
#define DETDESC_XMLDETECTOR_H

// Framework include files
#include "XML/XMLTags.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct SensitiveDetector;
    struct Detector;
    struct Volume;
    struct LCDD;

    struct Dimension : public Element  {
      Dimension() : Element(Handle_t(0)) {}
      Dimension(Handle_t e) : Element(e) {}
      Dimension(const Element& e) : Element(e) {}
      // Box:
      double x() const;
      double y() const;
      double z() const;
      // Functions return defaults rather than throwing exceptions
      double x(double default_val) const;
      double y(double default_val) const;
      double z(double default_val) const;

      // Trapezoid:
      double x1() const;
      double x2() const;
      double y1() const;
      double y2() const;
      // Tube
      double rmin() const;
      double rmax() const;
      double zhalf() const;
      double deltaphi() const;

      double outer_z() const;
      double outer_r() const;
      double inner_z() const;
      double inner_r() const;
      double z_length() const;
      double length() const;
      double width() const;
      double gap() const;
      double r_size() const;
      double phi_size_max() const;
      bool   reflect() const;
      bool   reflect(bool default_value) const;
      double outgoing_r() const;
      double incoming_r() const;
      double crossing_angle() const;

      double zmin() const;

      double radius() const;
      double offset() const;
      double offset(double default_value) const;
      int    number() const;

      int numsides() const;

      double phi0() const;
      double phi0(double default_value) const;
      double phi_tilt() const;
      int    nphi() const;
      double rc()  const;
      double dr()  const;
      double dz()  const;
      double dz(double default_value)  const;
      double r()  const;
      double r(double default_value)  const;
      
      double z0() const;
      double zstart() const;
      int    nz() const;
      
    };

    struct DetElement : public Dimension  {
      struct Component : public Dimension {
        Component(Handle_t e) : Dimension(e) {}
        Component(const Element& e) : Dimension(e) {}
        int    id() const;
        int    id(int default_value) const;
        int    repeat()  const;
        double thickness() const;
        bool   isSensitive() const;
        const  XMLCh* name() const;
        std::string   nameStr() const;
	const XMLCh*  type() const;
	std::string   typeStr() const;
        const  XMLCh* module() const;
        std::string   moduleStr() const;
        const  XMLCh* material() const;
        std::string   materialStr() const;
        const  XMLCh* vis() const;
        std::string   visStr() const;
        std::string   regionStr() const;
        std::string   limitsStr() const;
	Dimension     dimensions()  const;
      };
      DetElement(Handle_t e) : Dimension(e)  {}
      Handle_t handle()  const { return m_element; }
      void setAttributes(const LCDD& lcdd, Handle_t node, const Volume& volume);
      void setVisAttributes(const LCDD& lcdd, Handle_t node, const Volume& volume);
      void setRegion(const LCDD& lcdd, Handle_t node, const Volume& volume);
      void setLimitSet(const LCDD& lcdd, Handle_t node, const Volume& volume);
      void setCombineHits(Handle_t node, const SensitiveDetector& sens);
      int  id() const;
      const XMLCh* name() const;
      std::string nameStr() const;
      const XMLCh* type() const;
      std::string typeStr() const;
      const XMLCh* material() const;
      std::string materialStr() const;
      const  XMLCh* vis() const;
      std::string visStr() const;
      std::string   regionStr() const;
      std::string   limitsStr() const;
      Dimension dimensions()  const;
      bool isTracker() const;
      bool isCalorimeter() const;
      bool isInsideTrackingVolume() const;
      void check(bool condition, const std::string& msg) const;
      template <typename T> T attr(const XMLCh* tag) const { return m_element.attr<T>(tag); }
    };

    struct SensitiveDetector : public RefElement  {
      SensitiveDetector() : RefElement(Handle_t(0)) {}
      SensitiveDetector(Handle_t e) : RefElement(e) {}
      SensitiveDetector(const Document& doc, const XMLCh* type, const XMLCh* name);
      void setHitsCollection(const RefElement& spec);
      void setIDSpec(const RefElement& spec);
      void setSegmentation(Element seg);
    };

    struct CompactDetector : public RefElement  {
      CompactDetector(Element e) : RefElement(e) {}
      const  XMLCh* name() const;
      const  XMLCh* type() const;
      const  XMLCh* readout()  const;
    };

  }
}         /* End namespace DetDesc       */
#endif    /* DETDESC_XMLDETECTOR_H    */
