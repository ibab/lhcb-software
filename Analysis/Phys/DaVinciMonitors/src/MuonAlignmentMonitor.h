// $Id: MuonAlignmentMonitor.h,v 1.1 2010-02-02 11:42:36 ggiacomo Exp $
#ifndef MUONTRACKMONITOR_H 
#define MUONTRACKMONITOR_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

/** @class MuonAlignmentMonitor MuonAlignmentMonitor.h
 *  
 *
 *  @author A.Petrella and S.Vecchi
 *  @date   2009-02-25
 */
class ITrackExtrapolator;
class ITrackChi2Calculator;
class DeMuonDetector;

class MuonAlignmentMonitor : public GaudiHistoAlg {

 public: 
  MuonAlignmentMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonAlignmentMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static const int nSTATION = 5;

  AIDA::IHistogram1D  *m_h_chi2, *m_h_resx_a[nSTATION], *m_h_resy_a[nSTATION], *m_h_resx_c[nSTATION], *m_h_resy_c[nSTATION], *m_h_p;
  AIDA::IHistogram2D  *m_h_xy, *m_h_txty;
 
  AIDA::IProfile1D *m_p_resxx, *m_p_resxy, *m_p_resxtx,*m_p_resxty, *m_p_restxx, *m_p_restxy, *m_p_restxtx,
    *m_p_restxty, *m_p_resyx, *m_p_resyy, *m_p_resytx, *m_p_resyty, *m_p_restyx, *m_p_restyy, *m_p_restytx, *m_p_restyty;
  
  ITrackExtrapolator   *m_extrapolator; ///< extrapolator
  ITrackChi2Calculator *m_chi2Calculator;
  
  DeMuonDetector*       m_muonDet; 
  bool m_IsLongTrackState, m_LongToMuonMatch, m_IsCosmics;
  double m_pCut;

  std::string m_extrapolatorName, m_Chi2CalculatorName;


};
#endif // MUONTRACKMONITOR_H
