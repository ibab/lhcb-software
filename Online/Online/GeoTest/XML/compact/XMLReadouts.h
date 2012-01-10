#ifndef DETDESC_XMLREADOUT_H
#define DETDESC_XMLREADOUT_H
#include "XML/XMLTags.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    struct Segmentation;

    struct Readout : public RefElement {
      /// Constructor to be used when reading the already parsed DOM tree
      Readout(Handle_t e);
      /// Initializing constructor
      Readout(const Document& doc, Handle_t h);
      const XMLCh* name() const;
      Handle_t idSpec() const;
      Handle_t segmentation()  const;
      void setupIDDescriptor(Handle_t h)  const;
      void setSegmentation(Handle_t source) const;
    };

  }
}         /* End namespace DetDesc   */
#endif    /* DETDESC_XMLREADOUT_H     */
