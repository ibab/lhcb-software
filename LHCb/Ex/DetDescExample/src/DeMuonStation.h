// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/DeMuonStation.h,v 1.1.1.1 2001-03-13 15:11:14 cattaneb Exp $
#ifndef DDEXAMPLE_DEMUONSTATION_H
#define DDEXAMPLE_DEMUONSTATION_H 1


// Include files
#include "DetDesc/DetectorElement.h"


// External declarations
extern const CLID& CLID_DEMuonStation;


//------------------------------------------------------------------------------
//
// ClassName:   DeMuonStation 
//  
// Description: Example of an especific detertor element class.
//
//------------------------------------------------------------------------------


class DeMuonStation: public DetectorElement
{
public:
  // Constructors
  DeMuonStation();
  DeMuonStation( double t, double x, double y );

  // Destructor
  ~DeMuonStation();

  const CLID& clID() const {
    return classID();
  }

  // Retrieve reference to class identifier
  static const CLID& classID();

  // Get/Set thickness
  double thickness();
  void   setThickness( double t );

  // Get/Set pad dimensions
  double padx();
  double pady();
  void   setPadx( double x );
  void   setPady( double y );

private:

  double    m_thickness; // Aluminium plate thickness
  double    m_padx;      // Pad X dimension
  double    m_pady;      // Pad Y dimension
};

#endif    // DDEXAMPLE_DEMUONSTATION_H
