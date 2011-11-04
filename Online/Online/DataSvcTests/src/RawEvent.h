//====================================================================
//	RawEvent.h
//--------------------------------------------------------------------
//
//	Package    : PersistencySvc (The LHCb PersistencySvc service)
//
//  Description: Definition of example RawEvent data object
//
//	Author     : M.Frank
//  Created    : 13/1/99
//
//====================================================================
#ifndef PERSISTENCYSVC_RAWEVENT_H
#define PERSISTENCYSVC_RAWEVENT_H
// C++ includes
#include <iostream>
// Framework includes
#include "GaudiKernel/DataObject.h"

namespace Tests {

  // External declarations
  extern const CLID& CLID_RawEvent;

  /** @name The Raw Event class.
   * 
   *  Example data object
   *
   *  @author Markus Frank
   */
  class RawEvent : public DataObject      {
  protected:
  public:
    /// Standard Constructor
    RawEvent()
      : DataObject()   
      {
      }
      /// Standard Destructor
      virtual ~RawEvent()   {
	std::cout << " ~RawEvent:" << this << " .. ";
      }
      /// Retrieve Pointer to class defininition structure
      virtual const CLID& clID() const    {
	return CLID_RawEvent;
      }
  };
}
#endif // PERSISTENCYSVC_RAWEVENT_H
