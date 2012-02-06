#ifndef DETDESC_FACTORIES_H
#define DETDESC_FACTORIES_H

#include "Reflex/PluginService.h"
#include "RVersion.h"
#include "DetDesc/lcdd/Detector.h"

namespace DetDesc {
  namespace Geometry {
    struct LCDD;
    struct SensitiveDetector;
    struct DetElement;
  }
  namespace XML {
    struct Handle_t;
  }
}

namespace DetDesc { namespace Geometry {
template <typename T> class DetElementFactory  {
public:
  static DetDesc::Geometry::RefHandle<TNamed> create(DetDesc::Geometry::LCDD& lcdd, const DetDesc::XML::Handle_t& e, DetDesc::Geometry::SensitiveDetector& sens);
};
  }  }

namespace {
  template < typename P > class Factory<P, TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*)> {
  public:
    static void Func(void *retaddr, void*, const std::vector<void*>& arg, void*) {
      DetDesc::Geometry::LCDD* lcdd = (DetDesc::Geometry::LCDD*)arg[0];
      DetDesc::XML::Handle_t*  elt  = (DetDesc::XML::Handle_t*)arg[1];
      DetDesc::Geometry::SensitiveDetector* sens = (DetDesc::Geometry::SensitiveDetector*)arg[2];
      DetDesc::Geometry::RefHandle<TNamed> handle = DetDesc::Geometry::DetElementFactory<P>::create(*lcdd,*elt,*sens);
      *(void**)retaddr = handle.ptr();
    }
  };
}

#define DECLARE_DETELEMENT_FACTORY(x)  PLUGINSVC_FACTORY(x,TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*))

#define DECLARE_NAMESPACE_DETELEMENT_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY(x,TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*))

#define DECLARE_NAMED_DETELEMENT_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY_WITH_ID(x,std::string(#x),TNamed*(DetDesc::Geometry::LCDD*,const DetDesc::XML::Handle_t*,DetDesc::Geometry::SensitiveDetector*))

#endif // DETDESC_FACTORIES_H
