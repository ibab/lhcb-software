// $Id: SerializeAddress.h,v 1.1 2008-06-17 13:23:14 frankb Exp $
//====================================================================
//	SerializeAddress.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiSerialize/GaudiSerialize/SerializeAddress.h,v 1.1 2008-06-17 13:23:14 frankb Exp $
#ifndef SERIALIZE_SERIALIZEADDRESS_H
#define SERIALIZE_SERIALIZEADDRESS_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "TBuffer.h"

/*
 *  Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class SerializeAddress SerializeAddress.h GaudiSerialize/SerializeAddress.h
   *
   * Description:
   *
   * Definition of a transient link which is capable of locating
   * an object in the persistent storage.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class SerializeAddress : virtual public GenericAddress {
  protected:
    /// TBuffer object
    TBuffer* m_buffer;
  public:
    /// Full constructor
    SerializeAddress( long svc,
		      const CLID& clid,
		      const std::string& p1="", 
		      const std::string& p2="",
		      unsigned long ip1=0,
		      unsigned long ip2=0)
      : GenericAddress(svc,clid,p1,p2,ip1,ip2), m_buffer(0) 
      {
      }

      /// Standard Destructor
      virtual ~SerializeAddress() {
	if ( m_buffer ) delete m_buffer;
      }
      /// Access to TBuffer object containing object data
      TBuffer* buffer() const                  { return m_buffer;     }
      /// Set buffer containing object data
      void setBuffer(TBuffer* b)               {  m_buffer = b;       }
  };
}      // End namespace Gaudi
#endif // SERIALIZE_SerializeAddress_H
