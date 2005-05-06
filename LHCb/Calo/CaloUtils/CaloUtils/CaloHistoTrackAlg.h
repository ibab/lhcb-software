// $Id: CaloHistoTrackAlg.h,v 1.1 2005-05-06 17:48:32 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOUTILS_CALOHistoTrackAlg_H 
#define CALOUTILS_CALOHistoTrackAlg_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloHistoAlg.h"
// ============================================================================
#include "CaloUtils/TrTrackUse.h"
// ============================================================================

// ============================================================================
/** @class CaloHistoTrackAlg CaloHistoTrackAlg.h
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
 *  @see CaloHistoAlg
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
class CaloHistoTrackAlg : public CaloHistoAlg 
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
  CaloHistoTrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  /// destructor (virtual and protected)
  virtual ~CaloHistoTrackAlg();
private:
  /// default  construstor  is  private 
  CaloHistoTrackAlg(); 
  /// copy     construstor  is  private 
  CaloHistoTrackAlg
  ( const CaloHistoTrackAlg& );
  /// assignement operator  is  private 
  CaloHistoTrackAlg& operator=
  ( const CaloHistoTrackAlg& );
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
#endif // CALOHistoTrackAlg_H
// ============================================================================
