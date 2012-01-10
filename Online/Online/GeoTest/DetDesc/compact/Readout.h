#ifndef DETDESC_GEOMETRY_READOUT_H
#define DETDESC_GEOMETRY_READOUT_H

// Framework include files
#include "DetDesc/Elements.h"
#include "DetDesc/IDDescriptor.h"

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {
  /*
  *   Geometry namespace declaration
  */
  namespace Geometry  {

    /**@class Readout  Readout.h DetDesc/lcdd/Readout.h
      *
      * @author  M.Frank
      * @version 1.0
      */
    struct Readout : public RefElement {
      struct Object {
        Element     segmentation;
        RefElement  id;
        Object() : segmentation(0) {}
      };
      /// Constructor to be used when reading the already parsed object
      Readout(Handle_t e);
      /// Initializing constructor
      Readout(const Document& doc, const std::string& name);
      /// Access IDDescription structure
      Handle_t idSpec() const;
      /// Access segmentation structure
      Handle_t segmentation()  const;
      /// Assign IDDescription to readout structure
      void setIDDescriptor(Handle_t spec)   const;
      /// Assign segmentation structure to readout
      void setSegmentation(Handle_t source) const;
    };
  }       /* End namespace Geometry               */
}         /* End namespace DetDesc                */
#endif    /* DETDESC_GEOMETRY_READOUT_H           */
