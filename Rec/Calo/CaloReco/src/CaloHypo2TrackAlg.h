// $Id: CaloHypo2TrackAlg.h,v 1.4 2004-03-08 13:45:25 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOREC_CALOHypo2TrackAlg_H 
#define CALOREC_CALOHypo2TrackAlg_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"
// forward declarations 
class ICaloTrackMatch;   // CaloInterfaces

/** @class CaloHypo2TrackAlg CaloHypo2TrackAlg.h
 *  
 *  Algorithm which performs the matching of 
 *  CaloHypo objects with reconstructed tracks.
 *  Algorithm produces the relation table between 
 *  Hypos and tracks with weight = chi2 of matching.
 *  
 *  The actual matching is performed by matching tool
 *
 *  Algorithm could be used as a member of reconstruction chain 
 *  or in as "relation builder algorithm" for 'Associator' tool 
 *  
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @see ICaloTrackMatch 
 *  @see Associator
 *  @see AssociatorWeighed
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloHypo2TrackAlg : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypo2TrackAlg>;
  
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
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloHypo2TrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloHypo2TrackAlg();
  
private:

  /// default  construstor  is  private 
  CaloHypo2TrackAlg(); 
  /// copy     construstor  is  private 
  CaloHypo2TrackAlg
  ( const CaloHypo2TrackAlg& );
  /// assignement operator  is  private 
  CaloHypo2TrackAlg& operator=
  ( const CaloHypo2TrackAlg& );

private:
  
  /// address of Tracks 
  std::string        m_tracks       ;
  
  /// cut value 
  double             m_cut          ;
  // track flags to be used
  
  // use *ONLY* 'unique' tracks 
  bool               m_unique       ;
  
  // use 'error'   tracks  
  bool               m_error        ;
  // use 'forward' tracks  
  bool               m_forward      ;
  // use  'match'  tracks
  bool               m_matched      ;
  // use  'seed'   tracks
  bool               m_seed         ;
  // use  'velo'   tracks
  bool               m_velo         ;
  // use  'veloTT' tracks
  bool               m_veloTT       ;
  // use  'veloBack' tracks
  bool               m_veloBack     ;
  // use  'downstream' tracks (new naming convention!)
  bool               m_downstream   ;
  
  // matching tool 
  std::string        m_matchType    ;
  std::string        m_matchName    ;
  ICaloTrackMatch*   m_match        ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOHypo2TrackAlg_H
// ============================================================================
