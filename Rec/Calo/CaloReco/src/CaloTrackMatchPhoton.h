// $Id: CaloTrackMatchPhoton.h,v 1.5 2005-03-07 15:37:15 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTRACKTOOLS_CALOTRACKMATCHPHOTON_H
#define CALOTRACKTOOLS_CALOTRACKMATCHPHOTON_H 1
// ============================================================================
// Include files
// ============================================================================
// CaloInterfaces
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// CaloTrackTools, local
// ============================================================================
#include "CaloTrackMatchBase.h"
// ============================================================================
template <class TOOL>
class  ToolFactory;  ///< from GaudiKernel
// ============================================================================

/** @class CaloTrackMatchPhoton CaloTrackMatchPhoton.h
 *
 *  Tool for matching of calorimeter object
 *  with tracker object withour using of momentum/energy information.
 *  The tool is primary oriented to identification/selection of photons
 *  (good match of calorimeter object with tracker object means that
 *  "calorimeter object is not a photon").
 *
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */

class CaloTrackMatchPhoton:
  virtual public     ICaloTrackMatch,
  public              CaloTrackMatchBase
{
  /// friend factory for tool creation
  friend class ToolFactory<CaloTrackMatchPhoton>;

public:

  /** the main matching method
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (state)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure
   */
  StatusCode match 
  ( const CaloPosition  *caloObj ,
    const TrStoredTrack *trObj   ,
    double&              chi2    ) ;
  
  /** the main matching method  
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure 
   */
  StatusCode match 
  ( const CaloPosition* caloObj , 
    const TrgTrack*     trObj   ,
    double&             chi2    ) ;
  
protected:

  /** standard constructor
   *  @param type tool type   (useless)
   *  @param name tool name
   *  @param parent pointer to parent object
   *         (algorithm, service or another tool)
   */
  CaloTrackMatchPhoton( const std::string &type,
                        const std::string &name,
                        const IInterface  *parent );

  /** destructor is protected and virtual
   */
  virtual ~CaloTrackMatchPhoton();

private:
  /** default constructor in private!
   */
  CaloTrackMatchPhoton();

  /** copy constructor is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchPhoton( const CaloTrackMatchPhoton &copy );

  /** assignement operator is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchPhoton& operator=( const CaloTrackMatchPhoton &copy );

private:
  
  /** Makes struct with vector and covariance
   *  with cluster data.
   *  Vector looks like (x, y). Energy is not considered.
   *  @param  cluster data object
   *  @return internal type struct with data
   */
  inline const MatchType1& 
  prepareCluster ( const CaloPosition *cluster )
  { 
    const HepVector&    center = cluster->center() ;
    const HepSymMatrix& spread = cluster->spread() ;
    
    m_matchCalo.params  (1)   = center     (1)    ;
    m_matchCalo.params  (2)   = center     (2)    ;
    m_matchCalo.cov.fast(1,1) = spread.fast(1,1)  ;
    m_matchCalo.cov.fast(2,1) = spread.fast(2,1)  ;
    m_matchCalo.cov.fast(2,2) = spread.fast(2,2)  ;
    
    m_matchCalo.error    =   0   ;
    m_matchCalo.inverted = false ;
    m_matchCalo.invert()         ;

    return m_matchCalo ;
  };
  
  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different:
   *   TrStateP: (x, y, tx, ty, q/p);
   *   TrStateL: (x, y, tx, ty),
   * so the function performs vector and matrix remake.
   * @param  trState Track data object
   * @return internal type struct with data
   */
  inline const MatchType1& 
  prepareTrack ( TrState *trState )
  { 
    const HepVector&    params = trState->stateVector  () ;
    const HepSymMatrix& cov    = trState->stateCov     () ;
    
    m_matchTrk1.params  (1)    = params(1);
    m_matchTrk1.params  (2)    = params(2);
    m_matchTrk1.cov.fast(1,1) = cov.fast(1, 1);
    m_matchTrk1.cov.fast(2,1) = cov.fast(2, 1);
    m_matchTrk1.cov.fast(2,2) = cov.fast(2, 2); 

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
  prepareTrack ( const TrgState& trgState ,
                 const double    z        )
  { 
    m_matchTrk2.params (1)    = trgState . x     (z) ;
    m_matchTrk2.params (2)    = trgState . y     (z) ;
    
    
    const double dz = z - trgState.z() ;
    
    const HepSymMatrix& cov = trgState.covariance() ;
    m_matchTrk2.cov.fast(1,1) = 
      cov.fast(1,1) + 2.0 * dz * cov.fast(3,1) + dz * dz * cov.fast(3,3) ;
    m_matchTrk2.cov.fast(2,2) = 
      cov.fast(2,2) + 2.0 * dz * cov.fast(4,2) + dz * dz * cov.fast(4,4) ;
    
    m_matchTrk2.error    =   0   ;
    m_matchTrk2.inverted = false ;
    m_matchTrk2.invert() ;

    return m_matchTrk2   ;
  };
  
private:
  
  MatchType1 m_matchCalo ;  
  MatchType1 m_matchTrk1 ;  
  MatchType2 m_matchTrk2 ;

};
// ============================================================================
// The End 
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHPHOTON_H
// ============================================================================
