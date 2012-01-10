#ifndef DETDESC_XMLCALORIMETER_H
#define DETDESC_XMLCALORIMETER_H
#include "XML/compact/XMLDetector.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct Volume;
    struct SensitiveDetector;

    struct CylindricalBarrelCalorimeter : public Subdetector  {
      CylindricalBarrelCalorimeter(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens=Handle_t(0));
    };
    struct CylindricalEndcapCalorimeter : public Subdetector  {
      CylindricalEndcapCalorimeter(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens=Handle_t(0));
    };
    struct PolyhedraEndcapCalorimeter2 : public Subdetector  {
      PolyhedraEndcapCalorimeter2(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens=Handle_t(0));
    };
    struct PolyhedraBarrelCalorimeter2 : public Subdetector  {
      PolyhedraBarrelCalorimeter2(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens=Handle_t(0));
      void placeStaves( LCDD&         lcdd, 
                        const Tag_t&  detName, 
                        double        rmin, 
                        int           numsides, 
                        double        total_thickness, 
                        Volume        envelopeVolume, 
                        double        innerAngle, 
                        Volume        sectVolume);
    };
    struct EcalBarrel : public Subdetector  {
      EcalBarrel(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens=Handle_t(0));
    };
  }
}         /* End namespace DetDesc       */
#endif    /* DETDESC_XMLCALORIMETER_H    */
