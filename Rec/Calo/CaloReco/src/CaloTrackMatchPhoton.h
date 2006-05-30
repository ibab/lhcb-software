// $Id: CaloTrackMatchPhoton.h,v 1.8 2006-05-30 09:42:06 odescham Exp $
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
  ( const LHCb::CaloPosition  *caloObj ,
    const LHCb::Track *trObj   ,
    double&              chi2    ) ;
  
  
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
  inline const Match2D& 
  prepareCluster ( const LHCb::CaloPosition *cluster )
  { 
    const LHCb::CaloPosition::Center& center = cluster->center() ;
    const LHCb::CaloPosition::Spread& spread = cluster->spread() ;
    
    m_matchCalo.params  (0)   = center     (0)    ;
    m_matchCalo.params  (1)   = center     (1)    ;
    m_matchCalo.cov(0,0) = spread(0,0)  ;
    m_matchCalo.cov(1,0) = spread(1,0)  ;
    m_matchCalo.cov(1,1) = spread(1,1)  ;
    
    m_matchCalo.error    =   0   ;
    m_matchCalo.inverted = false ;
    m_matchCalo.invert()         ;

    return m_matchCalo ;
  };
  
  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different:
   *   TrState 6D (x, y, z,px,py,pz)
   * so the function performs vector and matrix remake.
   * @param  State Track data object
   * @return internal type struct with data
   */
  inline const Match2D& 
  prepareTrack ( LHCb::State *state )
  { 
    const Gaudi::TrackVector&    params = state->stateVector() ;
    const Gaudi::TrackSymMatrix& cov    = state->covariance() ;
    
    m_matchTrack.params  (0)    = params(0);
    m_matchTrack.params  (1)    = params(1);
    m_matchTrack.cov(0,0) = cov(0, 0);
    m_matchTrack.cov(1,0) = cov(1, 0);
    m_matchTrack.cov(1,1) = cov(1, 1); 

    m_matchTrack.error    =   0   ;
    m_matchTrack.inverted = false ;
    m_matchTrack.invert() ;
    return m_matchTrack   ;
  };
  
  
private:
  
  Match2D m_matchCalo ;  
  Match2D m_matchTrack ;  

};
// ============================================================================
// The End 
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHPHOTON_H
// ============================================================================
