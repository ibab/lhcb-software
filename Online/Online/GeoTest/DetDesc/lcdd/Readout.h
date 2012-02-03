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
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/Segmentations.h"

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

    /** @class Alignment  Readoutn.h DetDesc/lcdd/Readout.h
      *
      * @author  M.Frank
      * @version 1.0
      */
    struct Alignment : public NamedHandle {
      struct Object {
        Volume volume;
      };
      /// Default constructor
      Alignment() : NamedHandle() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Alignment(const Handle<Q>& e) : NamedHandle(e) {}
      /// Initializing constructor
      Alignment(const LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
    };

    /** @class Conditions  Readout.h DetDesc/lcdd/Readout.h
      *
      * @author  M.Frank
      * @version 1.0
      */
    struct Conditions : public NamedHandle {
      struct Object {
      };
      /// Default constructor
      Conditions() : NamedHandle() {}
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Conditions(const Handle<Q>& e) : NamedHandle(e) {}
      /// Initializing constructor
      Conditions(const LCDD& doc, const std::string& name);
      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
    };

  }       /* End namespace Geometry               */
}         /* End namespace DetDesc                */
#endif    /* DETDESC_GEOMETRY_READOUT_H           */
