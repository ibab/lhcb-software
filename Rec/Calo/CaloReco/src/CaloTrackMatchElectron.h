// $Id: CaloTrackMatchElectron.h,v 1.2 2003-06-23 13:11:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// Revision 1.9  2002/07/17 15:55:15  ibelyaev
//  improved printout
//
// Revision 1.8  2002/07/08 17:02:01  ibelyaev
//  play a little bit with the output level
//
// Revision 1.7  2002/07/08 15:40:08  ibelyaev
//  update dealing with track flags
//
// ============================================================================
#ifndef CALOTRACKTOOLS_CALOTRACKMATCHELECTRON_H
#define CALOTRACKTOOLS_CALOTRACKMATCHELECTRON_H 1

// Include files
// CaloInterfaces
#include "CaloInterfaces/ICaloTrackMatch.h"

// CaloTrackTools, local
#include "CaloTrackMatchBase.h"

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
  StatusCode match ( const CaloPosition  *caloObj,
                     const TrStoredTrack *trObj,
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
   * @param Cluster data object
   * @return internal type struct with data
   */
  MatchType prepareCluster( const CaloPosition *cluster )
  {
    return MatchType( cluster->parameters() , cluster->covariance() );  
  };
  
  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different: (x, y, tx, ty, e),
   * so the function performs vector and matrix remake
   * @param Track data object
   * @return internal type struct with data
   */
  MatchType prepareTrack( const TrStateP *trState )  
  { 
    const HepSymMatrix& stCov = trState -> stateCov();
    /// Make the vector of the format needed
    HepVector v(3);
    v[0] = trState->x() ;
    v[1] = trState->y() ;
    v[2] = trState->p() ;
    
    HepSymMatrix m(3);
    const int sign = trState->qDivP() < 0 ? -1 : 1 ; // Q sign
    
    /// to convert sigma(Q/P)->sigma(P)
    const double coeff = -1 * sign * v[2] * v[2];
    
    /// make the matrix. Recalculate elements concerned with momentum
    /// since we need P and the original matrix contains Q/P
    m.fast(1, 1) = stCov.fast(1, 1);
    m.fast(2, 1) = stCov.fast(2, 1);
    m.fast(2, 2) = stCov.fast(2, 2);
    m.fast(3, 1) = stCov.fast(5, 1) * coeff;
    m.fast(3, 2) = stCov.fast(5, 2) * coeff;
    m.fast(3, 3) = stCov.fast(5, 5) * coeff * coeff;
    
    return MatchType( v , m ) ;
  };


};
// ============================================================================
// The End
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHELECTRON_H
// ============================================================================
