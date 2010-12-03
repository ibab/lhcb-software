// $Id: $
// ============================================================================
#ifndef KALMANFILTER_ERRORCODES_H 
#define KALMANFILTER_ERRORCODES_H 1
// ============================================================================
// Include files
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-04-12
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
    /// error codes from various routines 
    enum ErrorCodes {
      // ======================================================================
      /// Error in Matrix Inversion (I)
      ErrorInMatrixInversion1      = 403 ,  // Error in Matrix Inversion
      /// Error in Matrix Inversion (II)
      ErrorInMatrixInversion2      = 404 ,  // Error in Matrix Inversion
      /// Error in Matrix Inversion (III)
      ErrorInMatrixInversion3      = 405 ,  // Error in Matrix Inversion
      /// Error in Matrix Inversion (IV)
      ErrorInMatrixInversion4      = 406 ,  // Error in Matrix Inversion
      /// Error in InputData 
      ErrorInInputData             = 407 ,  // Error in Input  Data
      /// Error from CaloMomentum 
      ErrorFromCaloMomentum        = 408 ,  // Error from CaloMomentum
      /// Error from Particle Transporter 
      ErrorFromParticleTransporter = 410 ,  // Error from particle transporter
      /// Error in "Gamma-Like" particle  
      ErrorGammaLikeParticle       = 411 ,  // Error in Gamma-Like particle
      /// Error in "DiGamma-Like" particle  
      ErrorDiGammaLikeParticle     = 412 ,  // Error in DiGamma-Like particle
      /// Error in Gamma-'tranport' 
      ErrorGammaTransport          = 413    // Error in Gamma-transport 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                      end of namespace LoKi::KalmanFilter 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // KALMANFILTER_ERRORCODES_H
// ============================================================================
