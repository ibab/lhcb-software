// $Id: CaloTrackMatchPhoton.h,v 1.2 2003-06-23 13:11:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// Revision 1.6  2002/07/05 09:06:31  ibelyaev
//  optimization and tuning for different track types
//
// Revision 1.5  2002/07/04 10:24:51  kbeloous
// TrStateP used only for Electron matching, 
// for all the rest matchings any state is sufficient
// ============================================================================
#ifndef CALOTRACKTOOLS_CALOTRACKMATCHPHOTON_H
#define CALOTRACKTOOLS_CALOTRACKMATCHPHOTON_H 1
// Include files
// CaloInterfaces
#include "CaloInterfaces/ICaloTrackMatch.h"
// CaloTrackTools, local
#include "CaloTrackMatchBase.h"

template <class TOOL>
class  ToolFactory;  ///< from GaudiKernel

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
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (state)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure
   */
  StatusCode match ( const CaloPosition  *caloObj,
                     const TrStoredTrack *trObj,
                     double              &chi2 );

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
   * with cluster data.
   * Vector looks like (x, y). Energy is not considered.
   * @param Cluster data object
   * @return internal type struct with data
   */
  MatchType prepareCluster( const CaloPosition *cluster )
  { return MatchType( cluster->center(), cluster->spread() ); }
  
  /** Makes struct with vector and covariance
   * with Track data.
   * Returned format is the same as for Cluster.
   * Input format of Track is quite different:
   *   TrStateP: (x, y, tx, ty, q/p);
   *   TrStateL: (x, y, tx, ty),
   * so the function performs vector and matrix remake.
   * @param Track data object
   * @return internal type struct with data
   */
  MatchType prepareTrack( TrState *trState )
  { HepVector    v(2);
    HepSymMatrix m(2);
    const HepSymMatrix &stCov = trState->stateCov();
    v[0]        = trState->stateVector()[0];
    v[1]        = trState->stateVector()[1];
    m.fast(1,1) = stCov.fast(1, 1);
    m.fast(2,1) = stCov.fast(2, 1);
    m.fast(2,2) = stCov.fast(2, 2); 
    return MatchType(v, m);
  };
  
};
// ============================================================================
// The End 
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHPHOTON_H
// ============================================================================
