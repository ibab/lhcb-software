// $Id: CaloAlignmentNtp.h,v 1.1 2010/05/20 09:55:38 odescham Exp $
#ifndef CALOALIGNEMENTNTP_H 
#define CALOALIGNEMENTNTP_H 1

// Include files
// from Gaudi
#include "Event/ProtoParticle.h"
#include "CaloUtils/ICaloElectron.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloInterfaces/ICaloHypoEstimator.h"
#include "CaloNtpBase.h"

/** @class CaloAlignmentNtp CaloAlignmentNtp.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-12-11
 */
class CaloAlignmentNtp : public CaloNtpBase {
public: 
  /// Standard constructor
  CaloAlignmentNtp( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloAlignmentNtp( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  bool m_pairing;
  ICaloHypoEstimator* m_estimator ;   
  ICaloHypoEstimator* estimator(){return m_estimator;}
  std::pair<double,double> m_eop;
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
  bool   m_brem;
  double m_r;
  int    m_b;
};
#endif // CALOALIGNEMENTNTP_H
