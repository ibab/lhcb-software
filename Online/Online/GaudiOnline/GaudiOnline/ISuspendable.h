// $Id: ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $
//====================================================================
//  ISuspendable.h
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//  Description: Interface describing suspendable and resumable objects.
//  Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $
#ifndef GAUDIONLINE_ISUSPENDABLE_H
#define GAUDIONLINE_ISUSPENDABLE_H

// Framework include files
#include "GaudiKernel/IInterface.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  // Declaration of the interface ID ( interface id, major version, minor version) 
  static const InterfaceID IID_ISuspendable("ISuspendable", 1 , 0); 

  /** @class ISuspendable ISuspendable.h GaudiOnline/ISuspendable.h
    *
    * @author  M.Frank
    */
  class ISuspendable : virtual public IInterface {
  public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_ISuspendable; }

    /// Suspend object operation
    virtual StatusCode suspend() const = 0;

    /// Resume object operation
    virtual StatusCode resume() const = 0;
  };
}
#endif // GAUDIONLINE_ISUSPENDABLE_H
