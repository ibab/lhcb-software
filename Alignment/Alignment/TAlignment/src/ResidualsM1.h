// $Id: ResidualsM1.h,v 1.1 2010-02-06 11:14:28 svecchi Exp $
#ifndef RESIDUALSM1_H 
#define RESIDUALSM1_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

class DeMuonDetector;
class IMuonHitDecode;
class IMuonPadRec;
class IMuonFastPosTool;
class ITrackEtrapolator;

/** @class ResidualsM1 ResidualsM1.h
 *  
 *
 *  @author Stefania Vecchi
 *  @date   2010-01-22
 */
class ResidualsM1 : public GaudiTupleAlg {
public: 
  /// Standard constructor
  ResidualsM1( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ResidualsM1( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static const int nREGIONS = 4;
  AIDA::IHistogram1D  *m_S_resx_a[nREGIONS], *m_S_resy_a[nREGIONS], *m_S_resx_c[nREGIONS], *m_S_resy_c[nREGIONS],
                      *m_B_resx_a[nREGIONS], *m_B_resy_a[nREGIONS], *m_B_resx_c[nREGIONS], *m_B_resy_c[nREGIONS];
  
protected:

private:
  std::string m_trackToolName, m_muonHitDecodeName;
  
  int m_myStation;
  
  DeMuonDetector*       m_muonDet; 
  IMuonHitDecode*       m_decTool;
  IMuonPadRec*          m_padTool;
  IMuonFastPosTool*     m_posTool;

  ITrackExtrapolator*   m_extrapolator;
  std::string           m_tTracksLocation;
  std::string           m_nExtrapolator;
  bool                  m_do_ntuple, m_do_histos;
  

  double                m_zM1;
  int                   m_iMS;
  double                m_nFOI;
  
  
  std::vector<float>    m_x_fit, m_y_fit, m_z_fit, m_dx_fit, m_dy_fit, m_tx_fit, m_ty_fit, m_dtx_fit, m_dty_fit,
                        m_chi2, m_hit, m_noise, 
                        m_momentum;
  std::vector<int>      m_region;
  
  std::vector< std::vector<float> >
                       mm_x_hit,   mm_y_hit,   mm_z_hit, mm_Dx_hit,   mm_Dy_hit,   
                       mm_x_noise, mm_y_noise, mm_z_noise, mm_Dx_noise, mm_Dy_noise;
  static const int m_track_max = 30;
  static const int  m_hit_max = 30;
  void clearArrays();


};
#endif // RESIDUALSM1_H
