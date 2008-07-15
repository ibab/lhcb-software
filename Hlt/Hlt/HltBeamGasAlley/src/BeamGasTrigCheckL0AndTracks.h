// $Id: BeamGasTrigCheckL0AndTracks.h,v 1.1.1.1 2008-07-15 13:53:25 phopchev Exp $
#ifndef INCLUDE_BEAMGASTRIGCHECKL0ANDTRACKS_H
#define INCLUDE_BEAMGASTRIGCHECKL0ANDTRACKS_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/L0DUReport.h"

// Simple GaudiAlgorithm which just checks
// the L0 decision and existance of RZ Tracks

class BeamGasTrigCheckL0AndTracks : public GaudiAlgorithm {

public:

  /// Standard Constructor
  BeamGasTrigCheckL0AndTracks(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~BeamGasTrigCheckL0AndTracks(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode    execute(); ///< Algorithm event execution
  virtual StatusCode   finalize(); ///< Algorithm finalize

private:
  std::string	m_l0Location;		///< Location of the L0 Decision
  std::string	m_PVRTTracksLocation;	///< Location of the PVRT output tracks
  
};
#endif // INCLUDE_BEAMGASTRIGCHECKL0ANDTRACKS_H

