// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DETDESC_LCDD_DETECTOR_H
#define DETDESC_LCDD_DETECTOR_H

// Framework include files
#include "DetDesc/Handle.h"
#include "DetDesc/lcdd/Objects.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/Readout.h"
#include "DetDesc/lcdd/Segmentations.h"

// C/C++ include files
#include <map>


/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry  {

    struct SensitiveDetector;
    struct Detector;
    struct LCDD;

    /** @class SensitiveDetector Detector.h DetDesc/lcdd/Detector.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct SensitiveDetector : public Ref_t  {
      struct Object  {
        int          Attr_verbose;
        int          Attr_combine_hits;
        double       Attr_ecut;
        std::string  Attr_eunit;
        std::string  Attr_hits_collection;
        Segmentation Attr_segmentation;
        Ref_t  Attr_id;
        Object() : Attr_verbose(0), Attr_segmentation() {}
      };

      /// Default constructor
      SensitiveDetector() : Ref_t() {}
      /// Templated constructor for handle conversions
      template <typename Q>
      SensitiveDetector(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor for a new sensitive detector element
      SensitiveDetector(const LCDD& lcdd, const std::string& type, const std::string& name);

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      /// Access the type of the sensitive detector
      std::string type() const;
      /// Set flag to handle hits collection
      SensitiveDetector& setCombineHits(bool value);
      /// Assign the name of the hits collection
      SensitiveDetector& setHitsCollection(const std::string& spec);
      /// Assign the IDDescriptor reference
      SensitiveDetector& setIDSpec(const Ref_t& spec);
      /// Assign the readout segmentation reference
      SensitiveDetector& setSegmentation(const Segmentation& seg);
      
    };

    /** @class SubDetector Detector.h DetDesc/lcdd/Detector.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct DetElement : public Ref_t   {
      typedef std::map<std::string,DetElement> Children;
      struct Object  {
        int               Attr_id;
        int               Attr_combine_hits;
        Solid             Attr_envelope;
        Volume            Attr_volume;
        Material          Attr_material;
        VisAttr           Attr_visualization;
        Readout           Attr_readout;
	Alignment         Attr_alignment;
	Conditions        Attr_conditions;
        Children          Attr_children;
        Object();
      };

      void check(bool condition, const std::string& msg) const;

      /// Default constructor
      DetElement() : Ref_t()  {}
      /// Templated constructor for handle conversions
      template<typename Q> DetElement(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor for a new subdetector element
      DetElement(const LCDD& lcdd, const std::string& name, const std::string& type, int id);

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      DetElement& setVisAttributes(const LCDD& lcdd, const std::string& solid, const Volume& volume);
      DetElement& setRegion(const LCDD& lcdd, const std::string& name, const Volume& volume);
      DetElement& setLimitSet(const LCDD& lcdd, const std::string& name, const Volume& volume);
      DetElement& setAttributes(const LCDD& lcdd, const Volume& volume,
				const std::string& region, 
				const std::string& limits, 
				const std::string& vis);

      DetElement&     setCombineHits(bool value, SensitiveDetector& sens);
      DetElement&     add(const DetElement& sub_element);
      int             id() const;
      std::string     type() const;
      bool            isTracker() const;
      bool            isCalorimeter() const;
      bool            isInsideTrackingVolume() const;
      bool            combineHits() const;
      Material        material() const;
      VisAttr         visAttr() const;
      Readout         readout() const;
      DetElement&     setReadout(const Readout& readout);
      /// Access the logical volume of the detector element
      Volume          volume() const;
      /// Set the logical volume of the detector element
      DetElement&     setVolume(const Volume& volume);
      /// Access the shape of the envelope
      Solid           envelope() const;
      /// Set envelope shape to the detector element
      DetElement&     setEnvelope(const Solid& solid);
      /// Access to the list of children
      const Children& children() const;
      /// Access to individual children by name
      DetElement      child(const std::string& name) const;
    };

  }       /* End namespace Geometry      */
}         /* End namespace DetDesc       */
#endif    /* DETDESC_LCDD_DETECTOR_H     */
