// $Id: RecVeloEvent.h,v 1.3 2002-01-14 09:55:17 cattanem Exp $
#ifndef LHCBEVENT_RECVELOEVENT_H
#define LHCBEVENT_RECVELOEVENT_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StreamBuffer.h"
#include "LHCbEvent/Definitions.h"


// Externals 
static const CLID& CLID_RecVeloEvent = 610;


//-----------------------------------------------------------------------------
//
// ClassName:   RecVeloEvent
//  
// Description: Essential information of the reconstructed velo event
//
// Author:      Bruce Hay
//
//-----------------------------------------------------------------------------


class RecVeloEvent : public DataObject  {

public:
	/// Constructors
  // "name" argument no longer necessary for Gaudi v9
  // kept for backward compatibility, but generate warning of unused variable 
	RecVeloEvent( const char* name = "RecVeloEvent" ) { }

	/// Destructor
        virtual ~RecVeloEvent()                           { }

	/// Retrieve reference to class definition structure
	virtual const CLID& clID() const  { return RecVeloEvent::classID(); }
	static const CLID& classID()      { return CLID_RecVeloEvent; }

  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const                 {
    DataObject::serialize(s);
    return s << m_dummy;
  }
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s )                       {
    DataObject::serialize(s);
    return s >> m_dummy;
  }

  /// Output operator (ASCII)
  friend std::ostream& operator<< (std::ostream& s, const RecVeloEvent& obj) {
	return obj.fillStream(s);
  }
  /// Fill the output stream (ASCII)
	virtual std::ostream& fillStream( std::ostream& s ) const {
    return s
      << "class RecVeloEvent :"
      << "\n    Dummy     = "
      << LHCbEventField( LHCbEvent::field12 )
      << m_dummy;
	}

private:
  /// Dummy data member
	double                        m_dummy;

};


#endif    // LHCBEVENT_RECVELOEVENT_H


