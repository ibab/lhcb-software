// $Id: CaloTrackAlg.h,v 1.1 2004-10-26 20:35:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOREC_CALOTrackAlg_H 
#define CALOREC_CALOTrackAlg_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// from CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================

/** @class CaloTrackAlg CaloTrackAlg.h
 *  
 *  Helper base algorithm form implementation 'track'-related algorithms
 *
 *  It helps to select the appropriate tracks for further processing
 *  Track is 'selected' if it fullfills 
 *
 *   - general features
 *   - category 
 *   - "algorithm" 
 *
 *  For "default" configuration *ALL* tracks are selected 
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @see ICaloTrackMatch 
 *  @see Associator
 *  @see AssociatorWeighed
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2004-10-26
 */

class CaloTrackAlg : public CaloAlgorithm 
{

public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm finalization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize  ();
 
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloTrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloTrackAlg();
  
private:
  
  /// default  construstor  is  private 
  CaloTrackAlg(); 
  /// copy     construstor  is  private 
  CaloTrackAlg
  ( const CaloTrackAlg& );
  /// assignement operator  is  private 
  CaloTrackAlg& operator=
  ( const CaloTrackAlg& );

protected:
  
  /** check if the track to be used 
   *  @param track track object 
   *  @return decision
   */
  bool        use  ( const TrStoredTrack* track ) const ;
  
  /** make 'bit' representation of track categories/types/algorithms
   *  @param trObj the track object
   *  @return 'bit'-representation of the track (useful for debugging)
   */
  std::string bits ( const TrStoredTrack* trObj ) const ;

protected :
  
  // track flags to be used
  
  // general (the top precedence) 

  // use *ONLY* 'unique' tracks 
  bool               m_uniqueOnly ;  
  // use 'error' tracks  
  bool               m_error      ;

  // by category 
  
  // use "isLong"       tracks 
  bool               m_isLong       ;
  // use "isUpstream"   tracks 
  bool               m_isUpstream   ;
  // use "isDownstream" tracks 
  bool               m_isDownstream ;
  // use "isVelotrack"  tracks 
  bool               m_isVelotrack  ;
  // use "isBackward"   tracks 
  bool               m_isBackward   ;
  // use "isTtrack"     tracks 
  bool               m_isTtrack     ;
  
  // by algorithm 
  
  // use "velo"        tracks 
  bool               m_velo      ;
  // use "seed"        tracks 
  bool               m_seed      ;
  // use "match"       tracks 
  bool               m_match     ;
  // use "forward"     tracks 
  bool               m_forward   ;
  // use "follow"      tracks 
  bool               m_follow    ;
  // use "veloTT"      tracks 
  bool               m_veloTT    ;
  // use "veloBack"    tracks 
  bool               m_veloBack  ;
  // use "ksTrack"     tracks 
  bool               m_ksTrack   ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTrackAlg_H
// ============================================================================
