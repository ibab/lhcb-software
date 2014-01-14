#ifndef FSTSELECTVELOTRACKS_H 
#define FSTSELECTVELOTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/ITrackSelector.h"

/** @class FstSelectVeloTracks FstSelectVeloTracks.h
 *  Select the Velo track for the First Software Trigger
 *
 *  @author Olivier Callot
 *  @date   2012-10-08
 */
class FstSelectVeloTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FstSelectVeloTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FstSelectVeloTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputTracksName;
  std::string m_pvName;
  std::string m_outputTracksName;
  unsigned int m_minVeloClusters;
  float  m_minIP;
  float  m_maxIP;
  float  m_minIP2;
  float  m_maxIP2;
  bool   m_validateWithTT;
  ITrackSelector* m_validationTool;

  int    m_nEvents;
  int    m_nTotTracks;
  int    m_goodIPTracks;
  int    m_nSelTracks;
  int m_goodNHitsTracks;

  // Flag that turns off the algorithm
  bool m_doNothing;
};
#endif // FSTSELECTVELOTRACKS_H
