// $Id: CaloHypo2TrgTrackAlg.h,v 1.1 2004-12-10 17:12:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOREC_CALOHypo2TrgTrackAlg_H 
#define CALOREC_CALOHypo2TrgTrackAlg_H 1
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
// forward declarations 
// ============================================================================
class ICaloTrackMatch;   // CaloInterfaces
// ============================================================================

/** @class CaloHypo2TrgTrackAlg CaloHypo2TrgTrackAlg.h
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

class CaloHypo2TrgTrackAlg : public CaloAlgorithm
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypo2TrgTrackAlg>;
  
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
  CaloHypo2TrgTrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloHypo2TrgTrackAlg();
  
private:

  /// default  construstor  is  private 
  CaloHypo2TrgTrackAlg(); 
  /// copy     construstor  is  private 
  CaloHypo2TrgTrackAlg
  ( const CaloHypo2TrgTrackAlg& );
  /// assignement operator  is  private 
  CaloHypo2TrgTrackAlg& operator=
  ( const CaloHypo2TrgTrackAlg& );
  
private:
  
  /// address of Tracks 
  std::string        m_tracks       ;
  
  /// cut value 
  double             m_cut          ;
  // track flags to be used
  
  // matching tool 
  std::string        m_matchType    ;
  std::string        m_matchName    ;
  ICaloTrackMatch*   m_match        ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOHypo2TrgTrackAlg_H
// ============================================================================
