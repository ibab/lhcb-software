// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Event/VeloEvent/VeloEvent/MCVeloHit.h,v 1.1.1.1 2000-11-23 13:39:24 ranjard Exp $
#ifndef LHCBEVENT_MCVELOHIT_H
#define LHCBEVENT_MCVELOHIT_H 1

// Include files
#include "LHCbEvent/MCTrackingHit.h"

// Forward declarations
class MCParticle;

// Externals 
extern const CLID& CLID_MCVeloHit;

//------------------------------------------------------------------------------
//
// ClassName:   MCVeloHit
//  
// Description: This class represents a simple data object
//              containing a Raw hit representation
//
// Author:      Bruce Hay
//
// Changes:     P.Binko 19/10/1999 : Changed the style conforming LHCbEvent
//                       Changed float to double
//                       Inline functions moved out from the class definition
//                       Formating of ASCII output
//
//------------------------------------------------------------------------------
class MCVeloHit : public MCTrackingHit {

public:
  /// Constructors
  MCVeloHit()   {
  }
  MCVeloHit( const MCVeloHit& copy ) 
  : MCTrackingHit(copy) {
  }

  /// Destructor
  virtual ~MCVeloHit()                           { }

  // Retrieve pointer to class definition structure for a persistent store
  virtual const CLID& clID() const                { 
    return MCVeloHit::classID(); 
  }
  static const CLID& classID()                    { 
    return CLID_MCVeloHit; 
  }
  /// Access wafer number 
  long waferNumber()    const    {
    return m_waferNumber;
  }
  /// Set wafer number 
  void setWaferNumber(long wafer)     {
    m_waferNumber = wafer;
  }
  /// Serialize the object for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:
  /// Wafer number
  long m_waferNumber;
};


//
// Inline code must be outside the class definition
//
// Serialize the object for writing
inline StreamBuffer& MCVeloHit::serialize( StreamBuffer& s ) const       {
  return MCTrackingHit::serialize(s) << m_waferNumber;
}

// Serialize the object for reading
inline StreamBuffer& MCVeloHit::serialize( StreamBuffer& s )             {
  return MCTrackingHit::serialize(s) >> m_waferNumber;
}

#endif // LHCBEVENT_MCVELOHIT_H





