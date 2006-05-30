// $Id: CaloTrackMatchBremm.h,v 1.9 2006-05-30 09:42:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALORECO_CALOTRACKMATCHBremm_H
#define CALORECO_CALOTRACKMATCHBremm_H 1
// ============================================================================
// Include files
// ============================================================================
// CaloInterfaces
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// CaloTools
// ============================================================================
#include "CaloTrackMatchBase.h"
// ============================================================================
template <class TOOL>
class  ToolFactory;  ///< from GaudiKernel
// ============================================================================

/** @class CaloTrackMatchBremm CaloTrackMatchBremm.h
 *
 *  Tool for matching of calorimeter object
 *  with direction of tracker object without
 *  usage of momentum/energy information.
 *  The tool is primary oriented to identification/selection of
 *  bremmstrahlung photons
 *  (good match of calorimeter object with direction of
 *  tracker object extrapolated from area before magned
 *  means that "calorimeter object is a bremmstrahlung photon").
 *
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */

class CaloTrackMatchBremm:
  virtual public     ICaloTrackMatch,
  public              CaloTrackMatchBase
{
  /// friend factory for tool creation
  friend class ToolFactory<CaloTrackMatchBremm>;

public:
  
  /** the main matching method
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (state)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure
   */
  StatusCode match 
  ( const LHCb::CaloPosition*  caloObj ,
    const LHCb::Track* trObj   ,
    double&              chi2    );
  
  
protected:

  /** standard constructor
   *  @param type tool type   (useless)
   *  @param name tool name
   *  @param parent pointer to parent object
   *         (algorithm, service or another tool)
   */
  CaloTrackMatchBremm 
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  
  /** destructor is protected and virtual
   */
  virtual ~CaloTrackMatchBremm();
  
private:
  
  /** default constructor in private!
   */
  CaloTrackMatchBremm();
  
  /** copy constructor is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchBremm( const CaloTrackMatchBremm &copy );

  /** assignement operator is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchBremm& operator=( const CaloTrackMatchBremm &copy );

  /** Makes struct with vector and covariance
   * with cluster data.
   * Vector looks like (x, y). Energy is not considered.
   * @param  cluster data object
   * @return internal type struct with data
   */
  inline const Match2D& 
  prepareCluster ( const LHCb::CaloPosition *cluster )
  { 
    const LHCb::CaloPosition::Parameters& center = cluster->parameters () ;
    const LHCb::CaloPosition::Covariance& spread  = cluster->covariance () ;
    m_matchCalo.params  (0)   = center(0)              ;
    m_matchCalo.params  (1)   = center(1)              ;
    m_matchCalo.cov(0,0) = spread(0,0)       ;
    m_matchCalo.cov(1,0) = spread(1,0)       ;
    m_matchCalo.cov(1,1) = spread(1,1)       ;

    m_matchCalo.error    =   0   ;
    m_matchCalo.inverted = false ;
    m_matchCalo.invert() ;

    return m_matchCalo ;
  };
  
  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different: (x, y, tx, ty, e),
   * so the function performs vector and matrix remake.
   * @param  trState Track data object
   * @return internal type struct with data
   */
  inline const Match2D& 
  prepareTrack ( LHCb::State *state )
  { 
    const Gaudi::TrackSymMatrix &stCov = state->covariance();
    m_matchTrack.params  (0)   = state->stateVector()[0];
    m_matchTrack.params  (1)   = state->stateVector()[1];
    m_matchTrack.cov(0,0) = stCov(0, 0);
    m_matchTrack.cov(1,0) = stCov(1, 0);
    m_matchTrack.cov(1,1) = stCov(1, 1); 

    m_matchTrack.error    =   0   ;
    m_matchTrack.inverted = false ;
    m_matchTrack.invert() ;
    
    return m_matchTrack   ;
  };

private:
  
  Match2D m_matchCalo ;  
  Match2D m_matchTrack ;  

private:
  
  // nominal position of bremm 
  double        m_bremZ    ;   
};
// ============================================================================
#endif // CALORECO_CALOTRACKMATCHBremm_H
