// $Id: BeamGasTrigVertexCut.h,v 1.1.1.1 2008-07-15 13:53:25 phopchev Exp $
#ifndef INCLUDE_BEAMGASTRIGVERTEXCUT_H
#define INCLUDE_BEAMGASTRIGVERTEXCUT_H 1

#include "HltBase/HltAlgorithm.h"

class BeamGasTrigVertexCut : public HltAlgorithm {

public: 
  /// Standard constructor
  BeamGasTrigVertexCut( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BeamGasTrigVertexCut(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 private:
  std::string        m_RZTracksLocation; // Name of container with input RZ Tracks 
  float		     m_zMin;	 // Lower limit of the z_r0 histo
  float		     m_zMax;	 // Upper limit of the z_r0 histo
  int		     m_nBins;    // N of bins of the z_r0 histo  
  int                m_binWidth; // bin-width of the z_r0 histo
  int		     m_maxCut;   // value of the max-bin that triggers the event
  
  float		     m_zMinPosCut;  // We don't plot z_r0 of tracks ...
  float		     m_zMaxPosCut;  // ... extrapolating to this (lumi) region
  unsigned           m_trigEventsZnegative;
  unsigned           m_trigEventsZpositive;
  
};
#endif // INCLUDE_BEAMGASTRIGVERTEXCUT_H 
