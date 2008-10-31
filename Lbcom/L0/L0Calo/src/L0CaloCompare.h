// $Id: L0CaloCompare.h,v 1.3 2008-10-31 13:20:12 robbep Exp $
#ifndef L0CALOCOMPARE_H 
#define L0CALOCOMPARE_H 1
// Include files

// from Gaudi 
//#include "GaudiAlg/GaudiHistoAlg.h"
//#include "GaudiAlg/GaudiTupleAlg.h"
#include "CaloUtils/Calo2Dview.h"
#include "CaloDet/DeCalorimeter.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

/** @class L0CaloCompare L0CaloCompare.h
 *  Monitoring algorithm for the L0 Calorimeter trigger
 *  Scans the L0CaloCandidate and fills histograms.
 *
 */
class L0CaloCompare: public Calo2Dview {
public:
  /// Standard constructor
  L0CaloCompare(const std::string& name, ISvcLocator* pSvcLocator );

  /// Standard destructor
  virtual ~L0CaloCompare( ); 

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  DeCalorimeter* m_ecal            ; ///< Pointer to Ecal detector element
  DeCalorimeter* m_hcal            ; ///< Pointer to Hcal detector element

  int m_nEvents ; 
  int m_nUsefulEvents ; 
  bool m_fullMonitoring ; 

  IHistogram1D*            m_histSpdMult_Comp ; 
  IHistogram1D*            m_histSumEt_Comp ; 
  
  IHistogram1D * m_bcIdErrorsEle , * m_bcIdErrorsPho , 
    * m_bcIdErrorsPil , * m_bcIdErrorsPig , 
    * m_bcIdErrorsHad , * m_bcIdErrorsSpd ;
  std::string              m_referenceDataSuffix ;
  std::string              m_checkDataSuffix ;

};
#endif // L0CALOCOMPARE_H
