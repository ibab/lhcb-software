#ifndef FSTSELECTFORWARDTRACKSPARTONE_H 
#define FSTSELECTFORWARDTRACKSPARTONE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class Fstselectforwardtrackspartone Fstselectforwardtrackspartone.h
 *  Select the Forward track for the First Software Trigger
 *
 *  @author Olivier Callot
 *  @date   2012-10-09
 */
class FstSelectForwardTracksPartOne : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FstSelectForwardTracksPartOne( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FstSelectForwardTracksPartOne( ); ///< Destructor

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
  float  m_minP;
  float  m_maxChi2Ndf;

  //== Counters
  int m_nTracks;
  int m_nPtOK;
  int m_nChi2OK;
  int m_nIPOK;
  int m_nIPSOK;
  int m_nInput;
};
#endif // FSTSELECTFORWARDTRACKS_H
