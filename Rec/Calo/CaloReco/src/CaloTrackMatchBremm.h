// $Id: CaloTrackMatchBremm.h,v 1.7 2005-11-07 12:12:43 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTRACKTOOLS_CALOTRACKMATCHBremm_H
#define CALOTRACKTOOLS_CALOTRACKMATCHBremm_H 1
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
  ( const CaloPosition*  caloObj ,
    const Track* trObj   ,
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
  inline const MatchType1& 
  prepareCluster ( const CaloPosition *cluster )
  { 
    const HepVector&    center = cluster->parameters () ;
    const HepSymMatrix& spread = cluster->covariance () ;
    m_matchCalo.params  (1)   = center(1)              ;
    m_matchCalo.params  (2)   = center(2)              ;
    m_matchCalo.cov.fast(1,1) = spread.fast(1,1)       ;
    m_matchCalo.cov.fast(2,1) = spread.fast(2,1)       ;
    m_matchCalo.cov.fast(2,2) = spread.fast(2,2)       ;

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
  inline const MatchType1& 
  prepareTrack ( State *state )
  { 
    const HepSymMatrix &stCov = state->covariance();
    m_matchTrk1.params  (1)   = state->stateVector()[0];
    m_matchTrk1.params  (2)   = state->stateVector()[1];
    m_matchTrk1.cov.fast(1,1) = stCov.fast(1, 1);
    m_matchTrk1.cov.fast(2,1) = stCov.fast(2, 1);
    m_matchTrk1.cov.fast(2,2) = stCov.fast(2, 2); 

    m_matchTrk1.error    =   0   ;
    m_matchTrk1.inverted = false ;
    m_matchTrk1.invert() ;
    
    return m_matchTrk1   ;
  };

  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different:
   *   TrStateP: (x, y, tx, ty, q/p);
   *   TrStateL: (x, y, tx, ty),
   * so the function performs vector and matrix remake.
   * @param  trgState Track data object
   * @return internal type struct with data
   */
  inline const MatchType2& 
  prepareTrack ( const State* state ,
                 const double    z        )
  { 
    m_matchTrk2.params  (1)   = state -> x     () ;//OD was x(z)
    m_matchTrk2.params  (2)   = state -> y     () ;//OD was x(z)
    m_matchTrk2.cov.fast(1,1) = state -> errX2 ( ) ;
    m_matchTrk2.cov.fast(2,2) = state -> errY2 ( ) ; 

    m_matchTrk2.error    =   0   ;
    m_matchTrk2.inverted = false ;
    m_matchTrk2.invert() ;

    return m_matchTrk2   ;
  };
  
private:
  
  MatchType1 m_matchCalo ;  
  MatchType1 m_matchTrk1 ;  
  MatchType2 m_matchTrk2 ;

private:
  
  // nominal position of bremm 
  double        m_bremZ    ;   
};
// ============================================================================
// The End
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHBremm_H
// ============================================================================
