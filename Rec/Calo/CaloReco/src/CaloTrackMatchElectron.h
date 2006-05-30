// $Id: CaloTrackMatchElectron.h,v 1.7 2006-05-30 09:42:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2005/11/22 16:15:57  cattanem
// fix doxygen warnings
//
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
#ifndef CALORECO_CALOTRACKMATCHELECTRON_H
#define CALORECO_CALOTRACKMATCHELECTRON_H 1
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
  ( const LHCb::CaloPosition  *caloObj,
    const LHCb::Track *trObj,
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
  inline const Match3D&
  prepareCluster( const LHCb::CaloPosition *cluster )
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
  inline const Match3D&
  prepareTrack( const LHCb::State *state )  
  { 
    const Gaudi::TrackSymMatrix stCov = state -> covariance();

    /// Make the vector of the format needed
    m_matchTrack.params (0) = state->x() ;
    m_matchTrack.params (1) = state->y() ;
    m_matchTrack.params (2) = state->p() ;

    const double p = state->p() ;
    const double q = state->qOverP() < 0 ? -1. : 1. ; // Q sign
    // to convert sigma(Q/P)->sigma(P)
    const double coeff = q * p * p ;
    
    // make the matrix. Recalculate elements concerned with momentum
    // since we need P and the original matrix contains Q/P
    m_matchTrack.cov(0, 0) = stCov(0, 0);
    m_matchTrack.cov(1, 0) = stCov(1, 0);
    m_matchTrack.cov(1, 1) = stCov(1, 1);
    m_matchTrack.cov(2, 0) = stCov(4, 0) * coeff;
    m_matchTrack.cov(2, 1) = stCov(4, 2) * coeff;
    m_matchTrack.cov(2, 2) = stCov(4, 4) * coeff * coeff;
    
    m_matchTrack.error    =   0   ;
    m_matchTrack.inverted = false ;
    m_matchTrack.invert() ;
    
    return m_matchTrack ;
  };
  

private:
  
  Match3D m_matchCalo ;  
  Match3D m_matchTrack ;  
  
};
// ============================================================================
#endif // CALORECO_CALOTRACKMATCHELECTRON
