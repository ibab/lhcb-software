// $Id: CaloTrackMatchPhotonAlg.h,v 1.1.1.1 2002-11-13 20:46:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/05/21 14:18:21  ibelyaev
// New package
//
// ============================================================================
#ifndef CALOREC_CALOTRACKMATCHPHOTONALG_H
#define CALOREC_CALOTRACKMATCHPHOTONALG_H 1
// Include files
// from STL
#include <string>
// from Gaudi
#include "CaloKernel/CaloAlgorithm.h"
// forward declaration
class ICaloTrackMatch ;  // from CaloInterfaces

/** @class CaloTrackMatchPhotonAlg CaloTrackMatchPhotonAlg.h
 *
 *  Algorithm which builds the relation table,
 *  needed for photon identification:
 *  it is 2D-matching of reconstructed tracks with
 *  cluster posiitonb and cluster 2nd order moments
 *  The actual matching is performed by tool.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */
class CaloTrackMatchPhotonAlg
  : public CaloAlgorithm
{
  /// friend factory
  friend class AlgFactory<CaloTrackMatchPhotonAlg>;

public:

  /**  standard Algorithm initialization
   *   @see CaloAlgorithm
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code
   */
  virtual StatusCode initialize ();

  /**  standard Algorithm execution
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code
   */
  virtual StatusCode execute    ();

  /**  standard Algorithm finalization
   *   @see CaloAlgorithm
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code
   */
  virtual StatusCode finalize   ();

protected:

  /** Standard constructor
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @param name algorithm name
   *  @param pSvc service locator
   */
  CaloTrackMatchPhotonAlg
  ( const std::string&  name ,
    ISvcLocator*        pSvc );

  /// destructor (protected and virtual)
  virtual ~CaloTrackMatchPhotonAlg() ;

private:

  ///  default  constructor  is private
  CaloTrackMatchPhotonAlg();
  ///  copy     constructor  is private
  CaloTrackMatchPhotonAlg
  ( const CaloTrackMatchPhotonAlg& );
  ///  assignement operator  is private
  CaloTrackMatchPhotonAlg& operator=
  ( const CaloTrackMatchPhotonAlg& );

private:
  
  std::string       m_matchTypeName ;
  ICaloTrackMatch*  m_match         ;
  
  std::string       m_trackData     ;
  
  double            m_chi2Cut       ;
  
};

// ============================================================================
// The END
// ============================================================================
#endif // CALOREC_CALOTRACKMATCHPHOTONALG_H
// ============================================================================
