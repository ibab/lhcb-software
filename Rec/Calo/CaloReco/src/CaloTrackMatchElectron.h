// $Id: CaloTrackMatchElectron.h,v 1.6 2005-11-22 16:15:57 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.4  2005/03/07 15:37:15  cattanem
// fix doxygen warnings
//
// Revision 1.3  2004/10/26 17:51:42  ibelyaev
//  add 'photon' matching for Trg Tracks
//
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
    const Track *trObj,
    double              &chi2 );
  

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
   * @param  cluster data object
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
   * @param  trState Track data object
   * @return internal type struct with data
   */
  inline const MatchType1&
  prepareTrack( const State *state )  
  { 
    const HepSymMatrix& stCov = state -> covariance();

    /// Make the vector of the format needed
    m_matchTrk1.params (1) = state->x() ;
    m_matchTrk1.params (2) = state->y() ;

    const double p = state->p() ;
    
    m_matchTrk1.params (3) = p ;
    
    const double q = state->qOverP() < 0 ? -1. : 1. ; // Q sign
    
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
   * @param  state    Track data object
   * @param  z        Not used!!
   * @return internal type struct with data
   */
  inline const MatchType2&
  prepareTrack( const State *state , 
                const double    z        )  
  { 
    
    /// Make the vector of the format needed
    m_matchTrk2.params (1) =       state->x() ;
    m_matchTrk2.params (2) =       state->y() ;
    
    const double p = state->p() ;
    
    m_matchTrk2.params (3) = fabs( p ) ;
    
    const double sign = state->qOverP() < 0 ? -1 : 1 ; // Q sign
    
    // to convert sigma(Q/P)->sigma(P)
    const double coeff = sign * p * p ;
    
    // make the matrix. Recalculate elements concerned with momentum
    // since we need P and the original matrix contains Q/P
    m_matchTrk2.cov.fast(1, 1) = state->errX2() ;
    m_matchTrk2.cov.fast(2, 2) = state->errY2() ;    
    m_matchTrk2.cov.fast(3, 3) = state->errQOverP2() * coeff * coeff ;

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
