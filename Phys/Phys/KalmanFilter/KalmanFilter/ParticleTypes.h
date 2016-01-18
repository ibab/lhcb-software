// ============================================================================
#ifndef KALMANFILTER_PARTICLETYPES_H 
#define KALMANFILTER_PARTICLETYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
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
 *  @author Vanya BELYAEV ibelyaev@itep.ru
 *  @date   2006-04-12
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace KalmanFilter
  {
    // ========================================================================
    /** @enum ParticleType 
     *  the particle types for Kalman Filter-based vertex fitter
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-01-04
     */
    enum ParticleType
      {
        // ====================================================================
        /// Unknown/unspecified particle type 
        UnspecifiedParticle = 0 ,          // Unknown/unspecified particle type 
        /// Short-lived (verticing) particles
        ShortLivedParticle      ,               //         Short-lived particle
        /// Long-lived/stable particle
        LongLivedParticle       ,               //   Long-lived/stable particle
        /// Photon-like 
        GammaLikeParticle       ,               //         Photon-like particle 
        /// Di-Photon-like 
        DiGammaLikeParticle     ,               //      Di-Photon-like particle 
        /// Merged-pi0-like 
        MergedPi0LikeParticle   ,               //     Merged-pi0-like particle 
        /// Jet-like 
        JetLikeParticle         ,               //            Jet-like particle 
        /// Jet-like 
        RhoPlusLikeParticle                     //          rho+-like particles 
        // ====================================================================
      } ;
    // ========================================================================
  } //                                    end of namespace LoKi::KalmanFilter 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KALMANFILTER_PARTICLETYPES_H
// ============================================================================
