// $Id: CaloSignalAlg.h,v 1.4 2005-01-12 09:14:33 ocallot Exp $
#ifndef   CALODIGIT_CALOSIGNALALG_H
#define   CALODIGIT_CALOSIGNALALG_H 1
// ============================================================================
// from Gaudi 
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiAlg/GaudiAlgorithm.h"

/// Det/CaloDet
#include "CaloDet/DeCalorimeter.h"

/** @class CaloSignalAlg CaloSignalAlg.h 
 *
 *  @brief Calorimeter Signal Processing Algorithm 
 *
 *  Algorithm responsible for Signal processing of MC-information.
 *  Converts the MCHit banks to MCCaloHits, processing the time information.
 *      
 *  @author: Olivier Callot  Olivier.Callot@cern.ch
 *  @date:   21 February 2001
 */

class CaloSignalAlg : public GaudiAlgorithm  {
  
public:
  /// Standard constructor
  CaloSignalAlg  ( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~CaloSignalAlg();  ///< Destructor

  virtual StatusCode initialize () ;  ///< Algorithm initialization  
  virtual StatusCode execute    () ;  ///< Algorithm execution
  
protected: 
  
private:
  std::string m_detectorName;      ///< Detector element location
  std::string m_inputData;         ///< Input container
  std::string m_outputData;        ///< Output container
  std::string m_previousData;      ///< address of 'previous' container
  std::string m_previousDigits;    ///< address of 'previous' Digits 
  double      m_minimalDeposit;    ///< Minimal energy to be kept
  double      m_backgroundScaling; ///< Downscale for the background hits
  bool        m_ignoreTimeInfo;    ///< flag to ignore SICBMC time info.
  
  //== Variables internally used

  bool         m_storePrevious;       ///< do we store the previous digits ?
  const DeCalorimeter* m_calo       ; ///< Detector Element pointer
  IRndmGenSvc*         m_rndmSvc    ; ///< random number service 
};

#endif // CALODIGIT_CALOSIGNALALG_H

