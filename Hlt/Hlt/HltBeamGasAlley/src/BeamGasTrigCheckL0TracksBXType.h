
#ifndef INCLUDE_BEAMGASTRIGCHECKL0TRACKSBXTYPE_H
#define INCLUDE_BEAMGASTRIGCHECKL0TRACKSBXTYPE_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

//-----------------------------------------------------------------------------
// Simple GaudiAlgorithm which just confirms
// the L0 decision and existance of RZ Tracks
//-----------------------------------------------------------------------------

class BeamGasTrigCheckL0TracksBXType : public GaudiAlgorithm {

public:

  /// Standard Constructor
  BeamGasTrigCheckL0TracksBXType(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~BeamGasTrigCheckL0TracksBXType(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode    execute(); ///< Algorithm event execution
  virtual StatusCode   finalize(); ///< Algorithm finalize

private:
  std::string	m_l0Location;		///< Location of the L0 Decision
  std::string	m_PVRTTracksLocation;	///< Location of the PVRT output tracks
  std::string   m_odinLocation;         ///< Location of ODIN
  unsigned int     m_bxType1;              ///< Type of bunch-crossing we will accept 
  unsigned int     m_bxType2;              ///< Type of bunch-crossing we will accept 
  bool          m_checkL0;              ///< Are we going to accept only L0-passed events
  bool          m_checkTracks;          ///< Are we going to accept only events with already reconstructed RZ tracks
  bool          m_checkBXType;          ///< Are we going to accept only events with the specified BX type

};
#endif // INCLUDE_BEAMGASTRIGCHECKL0TRACKSBXTYPE_H
