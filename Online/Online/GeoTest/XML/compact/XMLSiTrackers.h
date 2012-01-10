#ifndef DETDESC_XMLSITRACKERS_H
#define DETDESC_XMLSITRACKERS_H
#include "XML/compact/XMLDetector.h"
#include <map>

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct SiTrackerBarrel : public Subdetector  {
      SiTrackerBarrel(Handle_t e) : Subdetector(e) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens);
      Volume buildModule(LCDD& lcdd, Component detector, Component node, const SensitiveDetector& sens);
    };
    struct SiTrackerEndcap : public Subdetector  {
      struct LayerDim {
        int    id;
        double inner_r, outer_r, inner_z, thickness, deltaphi;
      };
      std::map<const XMLCh*,Dimension> m_modParams;
      SiTrackerEndcap(Handle_t e) : Subdetector(e) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens);
      Volume makeWedge(LCDD& lcdd, const SensitiveDetector& sens, Component compact, Component layer, const LayerDim& dim);
      void makeBoxModule(LCDD& lcdd, const SensitiveDetector& sens, Component module, Volume moduleVolume);
      void makeTrdModule(LCDD& lcdd, const SensitiveDetector& sens, Component module, Volume moduleVolume);
      void makeTrapModule(LCDD& lcdd, const SensitiveDetector& sens, Component module, Volume moduleVolume);
    };
    struct SiTrackerEndcap2 : public Subdetector  {
      SiTrackerEndcap2(Handle_t e) : Subdetector(e) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens);
    };

  }
}         /* End namespace DetDesc       */
#endif    /* DETDESC_XMLSITRACKERS_H    */
