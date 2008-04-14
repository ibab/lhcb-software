// $Id: L0CaloCompare.h,v 1.1 2008-04-14 07:46:45 robbep Exp $
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

  std::string              m_referenceDataSuffix ;
  std::string              m_checkDataSuffix ;

};
#endif // L0CALOCOMPARE_H
