// $Id: CaloTrackAlg.h,v 1.1 2005-05-06 17:48:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOUTILS_CALOTrackAlg_H 
#define CALOUTILS_CALOTrackAlg_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================
#include "CaloUtils/TrTrackUse.h"
// ============================================================================

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
// ============================================================================
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
  bool        use  ( const TrStoredTrack* track ) const 
  { return m_use.use ( track ) ; }
  /** make 'bit' representation of track categories/types/algorithms
   *  @param trObj the track object
   *  @return 'bit'-representation of the track (useful for debugging)
   */
  std::string bits ( const TrStoredTrack* trObj ) const 
  { return m_use.bits ( trObj ) ; }
protected :
  // the actual object 
  TrTrackUse m_use ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTrackAlg_H
// ============================================================================
