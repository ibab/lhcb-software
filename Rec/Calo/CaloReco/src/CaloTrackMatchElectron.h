// $Id: CaloTrackMatchElectron.h,v 1.3 2004-10-26 17:51:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTRACKTOOLS_CALOTRACKMATCHELECTRON_H
#define CALOTRACKTOOLS_CALOTRACKMATCHELECTRON_H 1
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

/** @class CaloTrackMatchElectron CaloTrackMatchElectron.h
 *
 *  Tool for matching of calorimeter object
 *  with tracker object using the whole available  information
 *  The tool is primary oriented to identification/selection of electrons
 *  (good match of calorimeter object with tracker object means that
 *  "calorimeter object is an electron").
 *
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */

class CaloTrackMatchElectron:
  virtual public     ICaloTrackMatch,
  public              CaloTrackMatchBase
{
  /** friend factory for tool creation
   * standard for GaudiTool
   */
  friend class ToolFactory<CaloTrackMatchElectron>;
  
public:
  
  /** the main matching method
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (state)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure
   */
  StatusCode match 
  ( const CaloPosition  *caloObj,
    const TrStoredTrack *trObj,
    double              &chi2 );
  
  /** the main matching method  
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure 
   */
  StatusCode match 
  ( const CaloPosition*   caloObj  , 
    const TrgTrack*       trObj    ,
    double&               chi2     ) ;

protected:
  /** standard Tool constructor
   *  @param type tool type   (useless)
   *  @param name tool name
   *  @param parent pointer to parent object
   *         (algorithm, service or another tool)
   */
  CaloTrackMatchElectron( const std::string &type,
			                    const std::string &name,
			                    const IInterface  *parent );

  /** destructor is protected and virtual
   * standard for GaudiTool
   */
  virtual ~CaloTrackMatchElectron();

private:
  /** default constructor in private!
   */
  CaloTrackMatchElectron();

  /** copy constructor is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchElectron( const CaloTrackMatchElectron &copy );

  /** assignement operator is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchElectron& operator=( const CaloTrackMatchElectron &copy );

  /** Makes struct with vector and covariance
   * with cluster data.
   * Vector looks like (e, x, y), the same as in CaloPosition object
   * @param Cluster data object
   * @return internal type struct with data
   */
  inline const MatchType1&
  prepareCluster( const CaloPosition *cluster )
  {
    m_matchCalo.params = cluster->parameters() ;
    m_matchCalo.cov    = cluster->covariance() ;
    
    m_matchCalo.error    =   0   ;
    m_matchCalo.inverted = false ;
    m_matchCalo.invert() ;
    
    return m_matchCalo ;
  };
  
  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different: (x, y, tx, ty, e),
   * so the function performs vector and matrix remake
   * @param Track data object
   * @return internal type struct with data
   */
  inline const MatchType1&
  prepareTrack( const TrStateP *trState )  
  { 
    const HepSymMatrix& stCov = trState -> stateCov();

    /// Make the vector of the format needed
    m_matchTrk1.params (1) = trState->x() ;
    m_matchTrk1.params (2) = trState->y() ;

    const double p = trState->p() ;
    
    m_matchTrk1.params (3) = p ;
    
    const double q = trState->qDivP() < 0 ? -1. : 1. ; // Q sign
    
    // to convert sigma(Q/P)->sigma(P)
    const double coeff = q * p * p ;
    
    // make the matrix. Recalculate elements concerned with momentum
    // since we need P and the original matrix contains Q/P
    m_matchTrk1.cov.fast(1, 1) = stCov.fast(1, 1);
    m_matchTrk1.cov.fast(2, 1) = stCov.fast(2, 1);
    m_matchTrk1.cov.fast(2, 2) = stCov.fast(2, 2);
    m_matchTrk1.cov.fast(3, 1) = stCov.fast(5, 1) * coeff;
    m_matchTrk1.cov.fast(3, 2) = stCov.fast(5, 2) * coeff;
    m_matchTrk1.cov.fast(3, 3) = stCov.fast(5, 5) * coeff * coeff;
    
    m_matchTrk1.error    =   0   ;
    m_matchTrk1.inverted = false ;
    m_matchTrk1.invert() ;
    
    return m_matchTrk1 ;
  };
  

  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different: (x, y, tx, ty, e),
   * so the function performs vector and matrix remake
   * @param Track data object
   * @return internal type struct with data
   */
  inline const MatchType2&
  prepareTrack( const TrgState *trgState , 
                const double    z        )  
  { 
    
    /// Make the vector of the format needed
    m_matchTrk2.params (1) =       trgState->x() ;
    m_matchTrk2.params (2) =       trgState->y() ;
    
    const double p = trgState->momentum() ;
    
    m_matchTrk2.params (3) = fabs( p ) ;
    
    const double sign = trgState->qOverP() < 0 ? -1 : 1 ; // Q sign
    
    // to convert sigma(Q/P)->sigma(P)
    const double coeff = sign * p * p ;
    
    // make the matrix. Recalculate elements concerned with momentum
    // since we need P and the original matrix contains Q/P
    m_matchTrk2.cov.fast(1, 1) = trgState->errX2() ;
    m_matchTrk2.cov.fast(2, 2) = trgState->errY2() ;    
    m_matchTrk2.cov.fast(3, 3) = trgState->errQOverP2() * coeff * coeff ;

    m_matchTrk2.error    =   0   ;
    m_matchTrk2.inverted = false ;
    m_matchTrk2.invert() ;
    
    return m_matchTrk2 ;
  };
  
private:
  
  MatchType1 m_matchCalo ;  
  MatchType1 m_matchTrk1 ;  
  MatchType2 m_matchTrk2 ;
  
};
// ============================================================================
// The End
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHELECTRON_H
// ============================================================================
