// $Id: CaloAlignmentNtp.h,v 1.1 2010/05/20 09:55:38 odescham Exp $
#ifndef CALOALIGNEMENTNTP_H 
#define CALOALIGNEMENTNTP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/ProtoParticle.h"
#include "CaloUtils/ICaloElectron.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "CaloDet/DeCalorimeter.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "CaloInterfaces/ICaloHypoEstimator.h"

/** @class CaloAlignmentNtp CaloAlignmentNtp.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-12-11
 */
class CaloAlignmentNtp : public GaudiTupleAlg {
public: 
  /// Standard constructor
  CaloAlignmentNtp( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloAlignmentNtp( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


  ITrackExtrapolator* extrapolator(){return m_extrapolator;};

protected:

private:
  bool inRange( std::pair<double,double> range, double value){
    return ( value >= range.first) && (value <= range.second);
  }
  

  ITrackExtrapolator*  m_extrapolator;
  ICaloElectron* m_caloElectron;
  bool m_pairing;
  std::string m_extrapolatorType;
  std::pair<double,double> m_e;
  std::pair<double,double> m_et;
  std::pair<double,double> m_prs;
  std::pair<double,double> m_spd;
  std::pair<double,double> m_eop;
  std::string m_input;
  ICaloHypo2Calo* m_toSpd ;   
  ICaloHypo2Calo* m_toPrs ;
  DeCalorimeter* m_calo;
  IEventTimeDecoder* m_odin;
  bool m_histo;
  bool m_tuple;
  bool m_profil;
  ICaloHypoEstimator* m_estimator ;   
  ICaloHypoEstimator* estimator(){return m_estimator;}
  std::pair<double,double> m_dlle;
  std::pair<double,double> m_rdlle;
  std::pair<double,double> m_bMatch;
  std::pair<double,double> m_eMatch;
  std::pair<double,double> m_cMatch;
  std::pair<double,double> m_mas;
  std::pair<double,double> m_dist;
  double m_min;
  double m_max;
  int    m_thBin;
  int    m_bin;
};
#endif // CALOALIGNEMENTNTP_H
