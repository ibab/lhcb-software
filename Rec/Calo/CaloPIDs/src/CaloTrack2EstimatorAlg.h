// $Id: CaloTrack2EstimatorAlg.h,v 1.1.1.1 2003-03-13 18:52:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/11/17 17:09:26  ibelyaev
//  new set of options and tools
// 
// ============================================================================
#ifndef CALOTRACK2ESTIMATORALG_H 
#define CALOTRACK2ESTIMATORALG_H 1
// Include files
// from STL
#include <string>
// from Calo
#include "CaloKernel/CaloAlgorithm.h"
// forward declarations 
class ICaloTrackIdEval;

/** @class CaloTrack2EstimatorAlg CaloTrack2EstimatorAlg.h
 *  
 *  The generic algorithm to associate Tracks to some Calo estimators
 * 
 *  The obvious concrete implementation:
 *   - Prs  energy (used for e+/e- ID)
 *   - Hcal energy (used for e+/e- and mu+/mu- ID)
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-14
 */
class CaloTrack2EstimatorAlg : public CaloAlgorithm 
{
  // friend factory for instantiation
  friend class AlgFactory<CaloTrack2EstimatorAlg>;
public:

  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:
  
  /** Standard constructor
   *  @param name algorithm name 
   *  @param pSvc service locator 
   */
  CaloTrack2EstimatorAlg( const std::string& name , 
                          ISvcLocator*       pSvc );
  
  // destructor 
  virtual ~CaloTrack2EstimatorAlg() ; 
  
private:
  
  // actual type of evaluator to be used 
  std::string         m_evalType ;
  // actual name of evaluator to be used
  std::string         m_evalName ;
  // evaluator itself 
  ICaloTrackIdEval*   m_eval     ;
  // low  limit 
  double              m_low      ;
  // high limit 
  double              m_high     ;
  // number of skips 
  unsigned long       m_skip     ;
  
  // use *ONLY* 'unique' tracks 
  bool               m_unique    ;
  
  // use 'error'   tracks  
  bool               m_error     ;
  // use 'forward' tracks  
  bool               m_forward   ;
  // use  'match'  tracks
  bool               m_matched   ;
  // use  'seed'   tracks
  bool               m_seed      ;
  // use  'velo'   tracks
  bool               m_velo      ;
  // use  'veloTT' tracks
  bool               m_veloTT    ;
  // use  'veloBack' tracks
  bool               m_veloBack  ;
  // use  'upstream' tracks
  bool               m_upstream  ;

};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTRACK2ESTIMATORALG_H
// ============================================================================

