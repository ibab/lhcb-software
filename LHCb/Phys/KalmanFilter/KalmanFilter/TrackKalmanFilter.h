// $Id: $
// ============================================================================
#ifndef TRACKKALMANFILTER_H 
#define TRACKKALMANFILTER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
// ============================================================================
// forward decalrations 
// ============================================================================
namespace LHCb { class State ; }  // Event/TrackEvent
// ============================================================================
/** @file 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-11-02
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace KalmanFilter 
  {
    // ========================================================================
    /** @class TrackEntry4
     *  Helper class useful for kinematical fits.
     *  It stores in the useful format the most important 
     *  values needed for the vertex Kalman fit procedure 
     *  @author Vanya BELYAEV Ivan.Belyaev
     *  @date   2010-11-02
     */
    class GAUDI_API TrackEntry4
    {
    public:
      // ======================================================================
      /// the state 
      const LHCb::State* m_state ;                                 // the state 
      // ======================================================================
    public:
      // ======================================================================
      //
      // "Measurements": The parameter vector alpha=(tx,ty,x,y,"z")
      //
      /// \f$\vec{x}=(x,y,z)\f$ 
      Gaudi::Vector3        m_parx ; // x=(x,y,"z")  
      /// \f$\vec{q}=(t_x,t_y)\f$ 
      Gaudi::Vector2        m_parq ; // q=(tx,ty)
      // inverse of the (modified?) covariance submatrix for (x,y,"z")
      //
      /// \f$C_{\vec{x}}^{-1}\f$
      Gaudi::SymMatrix3x3   m_vxi  ;  // C(x,x)^{-1}
      /// covarinace matrix (q vs x)
      Gaudi::Matrix2x3      m_qvsx ;  // to be loaded 
      //
      //      Projectors A & B are trivial and omitted 
      //
      //      Kalman filter values:
      /// \f$\chi^2\f$
      double                            m_chi2 ; // chi2 of kalman step/fit 
      /// \f$\vec{x}=(x,y,z)\f$ 
      Gaudi::Vector3                    m_x    ; // vertex position  (x,y,z)
      /// \f$C_{\vec{x}}=cov(\vec{x})\f$
      Gaudi::SymMatrix3x3               m_c    ; // position errors  
      /// \f$C_{\vec{x}}^{-1}\f$
      Gaudi::SymMatrix3x3               m_ci   ; // position errors(inverse), gain matrix 
      /// \f$\vec{q}=(t_x,t_y)\f$ 
      Gaudi::Vector2                    m_q    ; // parameter vector (tx,ty)
      /// \f$C_q=cov(\vec{q})\f$
      Gaudi::SymMatrix2x2               m_d    ; // covariance matrix for q-parameters
      /// \f$E=cov(\vec{x},vec{q})\f$
      Gaudi::Matrix2x3                  m_e    ; // covariance matrix for (x,q)
      /// \f$F=WB^{T}GA\f$
      Gaudi::Matrix2x3                  m_f    ; // auxillary matrix F 
      // ======================================================================
    } ; //                         end of class LoKi::KalmanFilter::TrackEntry4
    // ========================================================================
    /// the actual type of the container of entries 
    typedef std::vector<TrackEntry4>                            TrackEntries4 ;
    // ========================================================================
    /** make one step of Kalman filter 
     *  @param entry (update)       measurement to be updated 
     *  @param x     (input)        the initial position of the vertex 
     *  @param ci    (input)        its gain matrix 
     *  @param chi2  (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-02
     */
    GAUDI_API
    StatusCode step 
    ( LoKi::KalmanFilter::TrackEntry4&  entry , 
      const Gaudi::XYZPoint&            x     , 
      const Gaudi::SymMatrix3x3&        ci    , 
      const double                      chi2  ) ;
    // ========================================================================    
    /** make one step of Kalman filter 
     *  @param entry (update)       measurement to be updated 
     *  @param x     (input)        the initial position of the vertex 
     *  @param ci    (input)        its gain matrix 
     *  @param chi2  (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-02
     */
    GAUDI_API
    StatusCode step 
    ( LoKi::KalmanFilter::TrackEntry4&  entry , 
      const Gaudi::Vector3&             x     , 
      const Gaudi::SymMatrix3x3&        ci    , 
      const double                      chi2  ) ;
    // ========================================================================
    /** make one step of Kalman filter (similar to seeding)
     *  @param entry1 (update)       measurements to be updated 
     *  @param entry2 (update)       measurements to be updated 
     *  @param chi2  (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-11-02
     */
    GAUDI_API
    StatusCode step 
    ( LoKi::KalmanFilter::TrackEntry4&  entry1   ,
      LoKi::KalmanFilter::TrackEntry4&  entry2   , 
      const double                      chi2     ) ;
    // ========================================================================
    /** make one step of Kalman filter (similar to seeding)
     *  @param entry1 (update)       measurements to be updated 
     *  @param entry2 (update)       measurements to be updated 
     *  @param entry3 (update)       measurements to be updated 
     *  @param chi2  (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-11-02
     */
    GAUDI_API
    StatusCode step 
    ( LoKi::KalmanFilter::TrackEntry4&  entry1   ,
      LoKi::KalmanFilter::TrackEntry4&  entry2   , 
      LoKi::KalmanFilter::TrackEntry4&  entry3   , 
      const double                      chi2     ) ;
    // ========================================================================
    /** make one step of Kalman filter (similar to seeding)
     *  @param entry1 (update)       measurements to be updated 
     *  @param entry2 (update)       measurements to be updated 
     *  @param entry3 (update)       measurements to be updated 
     *  @param entry4 (update)       measurements to be updated 
     *  @param chi2  (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-11-02
     */
    GAUDI_API
    StatusCode step 
    ( LoKi::KalmanFilter::TrackEntry4&  entry1   ,
      LoKi::KalmanFilter::TrackEntry4&  entry2   , 
      LoKi::KalmanFilter::TrackEntry4&  entry3   , 
      LoKi::KalmanFilter::TrackEntry4&  entry4   , 
      const double                      chi2     ) ;
    // ========================================================================
    /// kalman smoothing  
    GAUDI_API
    StatusCode smooth  ( LoKi::KalmanFilter::TrackEntries4& entries ) ;
    // ========================================================================
    /// load the data from the state 
    GAUDI_API
    StatusCode load    ( const LHCb::State* state , TrackEntry4& entry ) ;
    // ========================================================================
  } //                                      end of namespace LoKi::KalmanFilter   
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // TRACKKALMANFILTER_H
// ============================================================================

