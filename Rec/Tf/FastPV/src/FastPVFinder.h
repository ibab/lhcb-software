// $Id: $
#ifndef FASTPVFINDER_H 
#define FASTPVFINDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FastPVFinder FastPVFinder.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2011-11-15
 */
class FastPVFinder : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FastPVFinder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FastPVFinder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  StatusCode updateBeamSpot();
  
private:
  std::string m_inputLocation;
  std::string m_outputLocation;
  std::string m_beamSpotCondition;
  
  double m_maxIPToBeam;
  unsigned int m_minTracksInPV;
  double m_maxDeltaZ;
  double m_maxChi2ToAdd;
  double m_maxChi2Fit;
  
  double m_xBeam;
  double m_yBeam;

  bool m_debug;

};
#endif // FASTPVFINDER_H
