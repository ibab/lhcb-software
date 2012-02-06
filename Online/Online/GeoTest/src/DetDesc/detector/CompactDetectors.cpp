// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/detector/DetFactoryHelper.h"
#include "DetDesc/detector/CompactDetectors.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;
#if 0
namespace DetDesc { namespace Geometry {
  
  template <> Ref_t DetElementFactory<SiVertexBarrel>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  
  {  return Ref_t(0);  }
  //template <> Ref_t DetElementFactory<SiVertexEndcap2>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  
  //{  return Ref_t(0);  }
  template <> Ref_t DetElementFactory<SiTrackerBarrel>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  
  {  return Ref_t(0);  }
}}

//DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,SiVertexBarrel);
//DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,SiVertexEndcap2);
//DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,SiTrackerBarrel);
#endif

