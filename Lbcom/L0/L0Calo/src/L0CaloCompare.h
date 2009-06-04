// $Id: L0CaloCompare.h,v 1.4 2009-06-04 15:14:17 robbep Exp $
#ifndef L0CALOCOMPARE_H 
#define L0CALOCOMPARE_H 1

// Include files
#include "CaloUtils/Calo2Dview.h"

class IHistogram1D ;

/** @class L0CaloCompare L0CaloCompare.h
 *
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
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  
protected:
  
private:
  bool m_fullMonitoring ;

  IHistogram1D*            m_histSpdMult_Comp ; 
  IHistogram1D*            m_histSumEt_Comp ;

  std::string              m_referenceDataSuffix ;
  std::string              m_checkDataSuffix ;

  std::vector< std::string >  m_mapCompareName ;
  std::vector< std::string >  m_mapCompareTitle ;
  std::vector< std::string >  m_mapAllName ;
  std::vector< std::string >  m_mapAllTitle ;
};
#endif // L0CALOCOMPARE_H
