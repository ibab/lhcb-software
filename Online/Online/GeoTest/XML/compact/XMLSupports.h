#ifndef DETDESC_XMLSUPPORTS_H
#define DETDESC_XMLSUPPORTS_H
#include "XML/compact/XMLDetector.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct PolyconeSupport : public Subdetector  {
      PolyconeSupport(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const Handle_t& sens=Handle_t(0));
    };

    struct TubeSegment : public Subdetector  {
      TubeSegment(Handle_t element) : Subdetector(element) {}
      Subdetector& fromCompact(LCDD& lcdd, const Handle_t& sens=Handle_t(0));
    };

  }
}         /* End namespace DetDesc       */
#endif    /* DETDESC_XMLSUPPORTS_H       */
