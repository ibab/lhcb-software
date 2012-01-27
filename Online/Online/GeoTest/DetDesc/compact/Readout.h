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
      Readout(const Element& e) : RefElement(e) {}
      /// Initializing constructor
      Readout(const LCDD& doc, const std::string& name);
      /// Access IDDescription structure
      RefElement idSpec() const;
      /// Access segmentation structure
      Element segmentation()  const;
      /// Assign IDDescription to readout structure
      void setIDDescriptor(RefElement spec)   const;
      /// Assign segmentation structure to readout
      void setSegmentation(Element segment) const;
    };
  }       /* End namespace Geometry               */
}         /* End namespace DetDesc                */
#endif    /* DETDESC_GEOMETRY_READOUT_H           */
