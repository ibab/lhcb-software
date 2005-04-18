// $Id: OnlineAddress.h,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
//====================================================================
//	OnlineAddress.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/OnlineAddress.h,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
#ifndef GAUDIONLINE_ADDRESS_H
#define GAUDIONLINE_ADDRESS_H 1

// Framework include files
#include "GaudiKernel/GenericAddress.h"

/*
 *  GaudiOnline namespace declaration
 */
namespace GaudiOnline  {

  // Forward declarations
  class StreamDescriptor;

  /** @class GaudiOnline::Address OnlineAddress.h GaudiOnline/OnlineAddress.h
    *
    * Description:
    *
    * Definition of a transient link which is capable of locating
    * an object in the persistent storage.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class Address : public GenericAddress {
  protected:
    /// Pointer to SFC communication structure
    const StreamDescriptor* m_com;

  public:
    /// Full constructor: all arguments MUST be valid, no checks!
    Address(long type, const CLID& clid, const StreamDescriptor* comm_data);
    /// Standard Destructor
    virtual ~Address();
    /// Access to input stream descriptor
    const StreamDescriptor* descriptor() const  {      return m_com;      }
  };
}

#endif // GAUDIONLINE_ADDRESS_H
