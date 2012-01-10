#ifndef DETDESC_XML_COMPACT_FORWARDDETECTOR_H
#define DETDESC_XML_COMPACT_FORWARDDETECTOR_H
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

    struct ForwardDetector : public Subdetector  {
      ForwardDetector(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const SensitiveDetector& sens=Handle_t(0));
    };
  }
}         /* End namespace DetDesc                    */
#endif    /* DETDESC_XML_COMPACT_FORWARDDETECTOR_H    */
