// $Id: L0CaloCompare.h,v 1.7 2010-05-20 16:45:14 robbep Exp $
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
  /// Prints name of candidate
  std::string candidateTypeName( int type ) const ;

  bool m_fullMonitoring ;

  IHistogram1D*            m_histSpdMult_Comp ; 
  IHistogram1D*            m_histSumEt_Comp ;
  IHistogram1D*            m_histEleErrorC  ;
  IHistogram1D*            m_histPhoErrorC  ;
  IHistogram1D*            m_histHadErrorC  ;
  IHistogram1D*            m_histPilErrorC  ;
  IHistogram1D*            m_histPigErrorC  ;

  IHistogram1D*            m_histTotalCount ;
  IHistogram1D*            m_histErrorCount ;

  std::string              m_referenceDataSuffix ;
  std::string              m_checkDataSuffix ;

  std::vector< std::string >  m_mapCompareName ;
  std::vector< std::string >  m_mapCompareTitle ;
  std::vector< std::string >  m_mapAllName ;
  std::vector< std::string >  m_mapAllTitle ;
  std::vector< std::string >  m_errorCounterName ;
  std::vector< std::string >  m_errorCounterTitle ;

  std::vector<IHistogram1D*>  m_errorCounterHisto ;

  std::set< unsigned int > m_idles ; ///< List of idle BCIds
  /// Vector of Idles BCIds (to set via job options)
  std::vector< unsigned int > m_idleBCIdVector ;
  
};
#endif // L0CALOCOMPARE_H
