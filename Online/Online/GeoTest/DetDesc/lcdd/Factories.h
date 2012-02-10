// $Id:$
//====================================================================
//  AIDA Detector description 
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DETDESC_FACTORIES_H
#define DETDESC_FACTORIES_H

#include "Reflex/PluginService.h"
#include "RVersion.h"
#include "DetDesc/lcdd/Detector.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {
  /*
   *   XML sub-namespace declaration
   */
  namespace XML {
    struct Handle_t;
  }

  /*
   *   Geometry sub-namespace declaration
   */
  namespace Geometry {

    // Forward declarations
    struct LCDD;
    struct SensitiveDetector;
    struct DetElement;


    template <typename T> class DetElementFactory  {
    public:
      static Ref_t create(LCDD& lcdd, const XML::Handle_t& e, SensitiveDetector& sens);
    };
  }  
}

namespace {
  template < typename P > class Factory<P, TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*)> {
  public:
    typedef DetDesc::Geometry::LCDD  LCDD;
    typedef DetDesc::XML::Handle_t   xml_h;
    typedef DetDesc::Geometry::Ref_t Ref_t;
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      LCDD*  lcdd = (LCDD* )arg[0];
      xml_h* elt  = (xml_h*)arg[1];
      DetDesc::Geometry::SensitiveDetector* sens = (DetDesc::Geometry::SensitiveDetector*)arg[2];
      Ref_t handle = DetDesc::Geometry::DetElementFactory<P>::create(*lcdd,*elt,*sens);
      *(void**)retaddr = handle.ptr();
    }
  };
}

#define DECLARE_DETELEMENT_FACTORY(x)  PLUGINSVC_FACTORY(x,TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*))

#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY(x,TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*))

#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY_WITH_ID(x,std::string(#x),TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*))

#endif // DETDESC_FACTORIES_H
