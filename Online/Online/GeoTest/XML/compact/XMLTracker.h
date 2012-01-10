#ifndef DETDESC_XMLTRACKER_H
#define DETDESC_XMLTRACKER_H
#include "XML/compact/XMLDetector.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct MultiLayerTracker : public Subdetector  {
      /// Constructor to be used when reading the already parsed DOM tree
      MultiLayerTracker(Handle_t e) : Subdetector(e) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens);
    };

    struct DiskTracker : public Subdetector  {
      /// Constructor to be used when reading the already parsed DOM tree
      DiskTracker(Handle_t e) : Subdetector(e) {}
      /// Constructor to be used when creating a new DOM tree
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens);
    };

  }
}         /* End namespace DetDesc       */
#endif    /* DETDESC_XMLTRACKER_H        */
