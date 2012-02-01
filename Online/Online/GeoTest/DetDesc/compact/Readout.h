#ifndef DETDESC_GEOMETRY_READOUT_H
#define DETDESC_GEOMETRY_READOUT_H

// Framework include files
#include "DetDesc/Elements.h"
#include "DetDesc/IDDescriptor.h"
#include "DetDesc/compact/Segmentations.h"

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
    struct Readout : public RefElement_type<TNamed> {
      struct Object {
        Segmentation             segmentation;
        RefElement_type<TNamed>  id;
      };
      /// Default constructor
      Readout() : RefElement_type<TNamed>() {}
      /// Constructor to be used when reading the already parsed object
      template <class Q> Readout(const Element_type<Q>& e) : RefElement_type<TNamed>(e) {}
      /// Initializing constructor
      Readout(const LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      /// Access IDDescription structure
      RefElement_type<TNamed> idSpec() const;
      /// Access segmentation structure
      Segmentation segmentation()  const;
      /// Assign IDDescription to readout structure
      void setIDDescriptor(const RefElement_type<TNamed>& spec)   const;
      /// Assign segmentation structure to readout
      void setSegmentation(const Segmentation& segment) const;
    };
  }       /* End namespace Geometry               */
}         /* End namespace DetDesc                */
#endif    /* DETDESC_GEOMETRY_READOUT_H           */
