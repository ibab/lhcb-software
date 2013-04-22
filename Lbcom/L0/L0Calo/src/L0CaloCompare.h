// $Id: L0CaloCompare.h,v 1.7 2010-05-20 16:45:14 robbep Exp $
#ifndef L0CALOCOMPARE_H 
#define L0CALOCOMPARE_H 1

// Include files
#include "CaloUtils/Calo2Dview.h"

// AIDA
#include "AIDA/IHistogram1D.h"

/** @class L0CaloCompare L0CaloCompare.h
 *
 *  Monitoring algorithm for the L0 Calorimeter trigger
 *  Scans the L0CaloCandidate and fills histograms.
 *
 */
class L0CaloCompare: public Calo2Dview
{

public:

  /// Standard constructor
  L0CaloCompare(const std::string& name, ISvcLocator* pSvcLocator );
  
  /// Standard destructor
  virtual ~L0CaloCompare( ); 
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  
private:

  /// Prints name of candidate
  std::string candidateTypeName( int type ) const ;

private:

  bool m_fullMonitoring ;

  AIDA::IHistogram1D*            m_histSpdMult_Comp ; 
  AIDA::IHistogram1D*            m_histSumEt_Comp ;
  AIDA::IHistogram1D*            m_histEleErrorC  ;
  AIDA::IHistogram1D*            m_histPhoErrorC  ;
  AIDA::IHistogram1D*            m_histHadErrorC  ;
  AIDA::IHistogram1D*            m_histPilErrorC  ;
  AIDA::IHistogram1D*            m_histPigErrorC  ;

  AIDA::IHistogram1D*            m_histTotalCount ;
  AIDA::IHistogram1D*            m_histErrorCount ;

  std::string              m_referenceDataSuffix ;
  std::string              m_checkDataSuffix ;

  std::vector< std::string >  m_mapCompareName ;
  std::vector< std::string >  m_mapCompareTitle ;
  std::vector< std::string >  m_mapAllName ;
  std::vector< std::string >  m_mapAllTitle ;
  std::vector< std::string >  m_errorCounterName ;
  std::vector< std::string >  m_errorCounterTitle ;

  std::vector<AIDA::IHistogram1D*>  m_errorCounterHisto ;

  std::set< unsigned int > m_idles ; ///< List of idle BCIds
  /// Vector of Idles BCIds (to set via job options)
  std::vector< unsigned int > m_idleBCIdVector ;
  
};
#endif // L0CALOCOMPARE_H
