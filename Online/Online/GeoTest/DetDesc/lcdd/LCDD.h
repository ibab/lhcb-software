// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DETDESC_LCDD_LCDD_H
#define DETDESC_LCDD_LCDD_H

// Framework includes
#include "DetDesc/Handle.h"
#include "DetDesc/lcdd/Objects.h"
#include "DetDesc/lcdd/Shapes.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/Readout.h"
#include "DetDesc/lcdd/Detector.h"
#include "DetDesc/lcdd/Segmentations.h"

// C/C++ include files
#include <map>

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {
  
  /*
   *   XML namespace declaration
   */
  namespace Geometry  {

    /** @class LCDD LCDD.h
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    struct LCDD {
      typedef std::map<std::string,Handle<> > HandleMap;

      virtual ~LCDD() {}

      //virtual Document document() const = 0;
      virtual void create() = 0;
      virtual void init() = 0;
      virtual void endDocument() = 0;
      virtual void addStdMaterials() = 0;

      virtual Volume   worldVolume() const = 0;
      virtual Volume   trackingVolume() const = 0;

      virtual const HandleMap& header()  const = 0;
      virtual const HandleMap& constants()  const = 0;
      virtual const HandleMap& regions() const = 0;
      virtual const HandleMap& structure()  const = 0;
      virtual const HandleMap& solids()  const = 0;
      virtual const HandleMap& materials()  const = 0;
      virtual const HandleMap& detectors()  const = 0;
      virtual const HandleMap& readouts() const = 0;
      virtual const HandleMap& visAttributes() const = 0;
      virtual const HandleMap& limitsets()  const = 0;

      virtual Region      region(const std::string& name)  const = 0;
      virtual VisAttr     visAttributes(const std::string& name) const = 0;
      virtual LimitSet    limitSet(const std::string& name)  const = 0;
      virtual Material    material(const std::string& name)  const = 0;
      virtual Readout     readout(const std::string& name)  const = 0;
      virtual Ref_t       idSpec(const std::string& name)  const = 0;
      virtual Volume      pickMotherVolume(const DetElement& sd) const = 0;
      virtual Constant    constant(const std::string& name) const = 0;
      virtual Solid       solid(const std::string& name) const = 0;
      virtual Volume      volume(const std::string& name) const = 0;
      virtual DetElement  detector(const std::string& name) const = 0;

      virtual LCDD& add(const Constant& constant) = 0;
      //virtual LCDD& add(const Solid& solid) = 0;
      virtual LCDD& add(const Region& region) = 0;
      //virtual LCDD& add(const Volume& vol) = 0;
      virtual LCDD& add(const Material& mat) = 0;
      virtual LCDD& add(const VisAttr& attr) = 0;
      virtual LCDD& add(const LimitSet& limset) = 0;
      virtual LCDD& add(const DetElement& detector) = 0;

      virtual LCDD& addIDSpec(const Ref_t& element) = 0;
      virtual LCDD& addConstant(const Ref_t& element) = 0;
      virtual LCDD& addMaterial(const Ref_t& element) = 0;
      virtual LCDD& addVisAttribute(const Ref_t& element) = 0;
      virtual LCDD& addSensitiveDetector(const Ref_t& e) = 0;
      virtual LCDD& addLimitSet(const Ref_t& limset) = 0;
      virtual LCDD& addRegion(const Ref_t& region) = 0;
      virtual LCDD& addReadout(const Ref_t& readout) = 0;
      virtual LCDD& addDetector(const Ref_t& detector) = 0;
      virtual LCDD& addSolid(const Ref_t& detector) = 0;
      virtual LCDD& addVolume(const Ref_t& detector) = 0;

      //---Factory method-------
      static LCDD& getInstance(void);
      virtual void dump() const = 0;
      
    };
  }       /* End namespace Geometry  */
}         /* End namespace DetDesc   */
#endif    /* DETDESC_LCDD_LCDD_H     */
