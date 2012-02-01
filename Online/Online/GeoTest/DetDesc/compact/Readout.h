// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DETDESC_GEOMETRY_READOUT_H
#define DETDESC_GEOMETRY_READOUT_H

// Framework include files
#include "DetDesc/Handle.h"
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

    /** @class Readout  Readout.h DetDesc/lcdd/Readout.h
      *
      * @author  M.Frank
      * @version 1.0
      */
    struct Readout : public NamedHandle {
      struct Object {
        Segmentation segmentation;
        NamedHandle  id;
      };
      /// Default constructor
      Readout() : NamedHandle() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Readout(const Handle<Q>& e) : NamedHandle(e) {}
      /// Initializing constructor
      Readout(const LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      /// Access IDDescription structure
      NamedHandle idSpec() const;
      /// Access segmentation structure
      Segmentation segmentation()  const;
      /// Assign IDDescription to readout structure
      void setIDDescriptor(const NamedHandle& spec)   const;
      /// Assign segmentation structure to readout
      void setSegmentation(const Segmentation& segment) const;
    };
  }       /* End namespace Geometry               */
}         /* End namespace DetDesc                */
#endif    /* DETDESC_GEOMETRY_READOUT_H           */
