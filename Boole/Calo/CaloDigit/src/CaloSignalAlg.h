// $Id: CaloSignalAlg.h,v 1.2 2003-06-23 11:43:04 ocallot Exp $
#ifndef   CALODIGIT_CALOSIGNALALG_H
#define   CALODIGIT_CALOSIGNALALG_H 1
// ============================================================================
// from STL 
#include <cmath>
#include <string>
#include <vector>
// from Gaudi 
#include "GaudiKernel/IRndmGenSvc.h"
// from CaloKernel
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloSignalAlg CaloSignalAlg.h 
 *
 *  @brief Calorimeter Signal Processing Algorithm 
 *
 *  Algorithm responsible for Signal processing of MC-information.
 *  Converts the RAWH banks to MCCaloHits, processing the time information.
 *  Note that the time is ignored so far, as the rest of the FORTRAN Calo 
 *  software doesn't accept clusters without Truth information.
 *      
 *  @author: Olivier Callot  Olivier.Callot@cern.ch
 *  @date:   21 February 2001
 */

class CaloSignalAlg : 
  public CaloAlgorithm 
{
  /// frined factory for instantiatio
  friend class AlgFactory<CaloSignalAlg>;
  
public:
  
  /** standard initialization 
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** standard execution 
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute    () ;
  
  /** standard finalization
   *  @see CaloAlgorithm 
   *  @see     Algorithm 
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected: 
  
  /** Standard Constructor
   *  @param name name of algorithm
   *  @param pSvcLocator pointer to service locator 
   */
  CaloSignalAlg
  ( const std::string& name        , 
    ISvcLocator*       pSvcLocator );
  
  /// Destructor (virtual and protected)
  virtual ~CaloSignalAlg();
  
private:
  
  /// default  constructor  is  private 
  CaloSignalAlg();
  /// copy     constructor  is  private 
  CaloSignalAlg
  ( const CaloSignalAlg& );
  /// assignement operator is  private 
  CaloSignalAlg& operator=
  ( const CaloSignalAlg& );
  
private:   
  //== JobOptions variables

  std::string  m_previousData       ; ///< address of 'previous' container
  std::string  m_previousDigits     ; ///< address of 'previous' Digits 
  double       m_minimalDeposit     ; ///< Minimal energy to be kept
  double       m_backgroundScaling  ; ///< Downscale for the background hits
  bool         m_ignoreTimeInfo     ; ///< flag to ignore SICBMC time info.
  
  //== Variables internally used

  bool         m_storePrevious;       ///< do we store the previous digits ?
  const DeCalorimeter* m_calo       ; ///< Detector Element pointer
  IRndmGenSvc*         m_rndmSvc    ; ///< random number service 
};

#endif // CALODIGIT_CALOSIGNALALG_H
// ============================================================================
