#ifndef TRACKMUONMATCHMONITOR_H 
#define TRACKMUONMATCHMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

class DeMuonDetector;
class ITrackEtrapolator;

/** @class TrackMuonMatchMonitor TrackMuonMatchMonitor.h
 *  
 *
 *  @author Stefania Vecchi
 *  @date   2010-01-22
 */
class TrackMuonMatchMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  TrackMuonMatchMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackMuonMatchMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  
  DeMuonDetector*       m_muonDet;
  ITrackExtrapolator*   m_extrapolator;
  std::string           m_tTracksLocation;
  std::string           m_nExtrapolator;
  

  double m_zM1, m_MAXsizeX, m_MAXsizeY;
  int    m_iMS;
  double m_maxErrX ;
  double m_maxErrY ;

  static const int nREGIONS = 4;
  AIDA::IHistogram1D  *m_resx_a[nREGIONS], *m_resy_a[nREGIONS], *m_resx_c[nREGIONS], *m_resy_c[nREGIONS];
  double m_hisxmax[nREGIONS] ;

};
#endif // TRACKMUONMATCHMONITOR_H 
