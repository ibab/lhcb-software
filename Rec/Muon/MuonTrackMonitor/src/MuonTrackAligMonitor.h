#ifndef MUONTRACKALIHMONITOR_H 
#define MUONTRACKALIGMONITOR_H 1

#include <vector>
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class MuonTrackAligMonitor MuonTrackAligMonitor.h
 *  
 *
 *  @author 
 *  @date   2009-02-25
 */
class ITrackExtrapolator;
class ITrackChi2Calculator;
class DeMuonDetector;

class MuonTrackAligMonitor : public GaudiHistoAlg {
  
public: 
  MuonTrackAligMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~MuonTrackAligMonitor( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
    
private:
  AIDA::IHistogram1D  *m_h_chi2,  *m_h_p;
  std::vector<AIDA::IHistogram1D*> m_h_resxL_a,m_h_resyL_a,m_h_resxL_c,m_h_resyL_c;
  std::vector<AIDA::IHistogram1D*> m_h_resxM_a,m_h_resyM_a,m_h_resxM_c,m_h_resyM_c;
  AIDA::IHistogram2D  *m_h_xy, *m_h_txty;
 
  AIDA::IProfile1D *m_p_resxx, *m_p_resxy, *m_p_resxtx,*m_p_resxty, *m_p_restxx, *m_p_restxy, *m_p_restxtx,
    *m_p_restxty, *m_p_resyx, *m_p_resyy, *m_p_resytx, *m_p_resyty, *m_p_restyx, *m_p_restyy, *m_p_restytx, *m_p_restyty,
    *m_resxhsL, *m_resyhsL,*m_resxhsM, *m_resyhsM;
  
  ITrackExtrapolator   *m_extrapolator; ///< extrapolator
  ITrackChi2Calculator *m_chi2Calculator;
  
  DeMuonDetector*       m_muonDet; 
  bool  m_LongToMuonMatch, m_IsCosmics;
  double m_pCut, m_chi2nCut, m_chi2matchCut;
  double m_zM1;
  std::string m_histoLevel;

  std::string m_extrapolatorName, m_Chi2CalculatorName;
  bool m_notOnline;
  bool m_expertMode;

};
#endif // MUONTRACKALIGMONITOR_H
