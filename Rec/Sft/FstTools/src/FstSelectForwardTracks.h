#ifndef FSTSELECTFORWARDTRACKS_H 
#define FSTSELECTFORWARDTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FstSelectForwardTracks FstSelectForwardTracks.h
 *  Select the Forward track for the First Software Trigger
 *
 *  @author Olivier Callot
 *  @date   2012-10-09
 */
class FstSelectForwardTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FstSelectForwardTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FstSelectForwardTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputTracksName;
  std::string m_pvName;
  std::string m_outputTracksName;
  float  m_minIP;
  float  m_maxIP;
  float  m_minIP2;
  float  m_maxIP2;
  float  m_minIPChi2;
  float  m_maxIPChi2;
  
  float  m_minPt;
  float  m_maxChi2Ndf;
  
};
#endif // FSTSELECTFORWARDTRACKS_H
