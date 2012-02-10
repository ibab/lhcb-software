// $Id:$
//====================================================================
//  AIDA Detector description 
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DETDESC_DETECTOR_DETFACTORYHELPER_H
#define DETDESC_DETECTOR_DETFACTORYHELPER_H

#include "XML/XMLTags.h"
#include "XML/XMLElements.h"
#include "XML/lcdd/XMLDetector.h"
#include "DetDesc/lcdd/LCDD.h"
#include "DetDesc/lcdd/Factories.h"

// Helpers to access tags and attributes quickly without specifying explicitly namespaces
#define _X(a) DetDesc::XML::Tag_##a
#define _A(a) DetDesc::XML::Attr_##a

// Shortcuts to elements of the XML namespace
typedef DetDesc::XML::Collection_t          xml_coll_t;
typedef DetDesc::XML::Handle_t              xml_h;
typedef DetDesc::XML::DetElement            xml_det_t;
typedef DetDesc::XML::DetElement::Component xml_comp_t;
typedef DetDesc::XML::Dimension             xml_dim_t;
typedef DetDesc::Geometry::LCDD             lcdd_t;

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc { 

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {
    static inline std::string _toString(const XMLCh* value)  
      {
	return XML::_toString(value); 
      }

    template <typename T> inline std::string _toString(T value, const char* fmt)
      {
	return XML::_toString(value, fmt); 
      }
  }
}

#endif // DETDESC_DETECTOR_DETFACTORYHELPER_H
