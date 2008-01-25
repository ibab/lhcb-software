// $Id: ParticleContextCuts.h,v 1.1 2008-01-25 14:42:22 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLECONTEXTCUTS_H 
#define LOKI_PARTICLECONTEXTCUTS_H 1
// ============================================================================
// Incldue files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles20.h"
// ============================================================================
/** @file
 *  The file with the collection of "context-dependent" functors 
 *
 *  Essentially all these functord depends on "event-data" and 
 *  get the nesessary "context-dependent" data from Algorithm Context Service
 *
 *  The basic ingredients here:
 *   - LoKi Service 
 *   - Algorithm Context Service
 *   - PhysDesktop
 *   - LoKi::getPhysDesktop
 *   - Gaudi::Utils::getDValgorithm
 *
 *  @attention There are no direct needs to use all these "Context" 
 *             functors inside the native LoKi-based C++ code, 
 *             for all of them, there are more efficient, transparent, 
 *             clear and safe analogues...
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-01-16
 */
// ============================================================================
namespace LoKi
{
  namespace Cuts
  {
    // ========================================================================
    /** @var BPVDIRA
     *  The special "context-dependent" version of DIRA functor.
     *  It takes the best primary vertex from IPhysDesktop tool
     *
     *  @code 
     *
     *  // B-candidate:
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor
     *  const double dira = BPVDIRA ( B ) ;
     *
     *  @endcode
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *
     *  @see LoKi::Cuts::DIRA
     *  @see LoKi::Particles::CosineDirectionAngle
     *  @see LoKi::Particles::CosineDirectionAngleWithTheBestPrimaryVertex
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    const LoKi::Particles::CosineDirectionAngleWithTheBestPV          BPVDIRA ;
    // ========================================================================
    /** @typedef BPVIP 
     *  The special "context-dependent" version of LoKi::Particles::ImpPar functor
     *  which gets the related primary vertex from IPhysDesktop tool
     *
     *  @code
     *
     *   // constuct the functor 
     *   const BPIP bpip  = BPIP ("")  ;
     *
     *   // B-candidate
     *   const LHCb::Particle* B = ... ;
     *
     *   // use the functor
     *   const double ip = bpip ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Particles::ImpParWithTheBestPV
     *  @see LoKi::Particles::ImpPar
     *  @see IGeomDispCalculator
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24
     */
    typedef LoKi::Particles::ImpParWithTheBestPV                        BPVIP ;
    // ========================================================================
    /** @typedef BPVIPCHI2
     *  The special "context-dependent" version of LoKi::Particles::ImpParChi2 
     *  functor which gets the related primary vertex from IPhysDesktop tool
     *
     *  @code
     *
     *   // constuct the functor 
     *   const BPIPCHI2 bpipchi2  = BPIPCHI2 ("")  ;
     *
     *   // B-candidate
     *   const LHCb::Particle* B = ... ;
     *
     *   // use the functor
     *   const double chi2 = bpipchi2 ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Particles::ImpParChi2WithTheBestPV
     *  @see LoKi::Particles::ImpParChi2
     *  @see IGeomDispCalculator
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24
     */
    typedef LoKi::Particles::ImpParChi2WithTheBestPV                BPVIPCHI2 ;
    // ========================================================================
    /** @typedef MIPSOURCE 
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices form "source"
     *
     *  @code 
     *
     *  // get the helper functor-"source"
     *  const MIPSOURCE::Source& source = ... ;
     *
     *  // construct the main functor:
     *  const MIPSOURCE mip  = MIPSOURCE ( source , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = mip ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MinImpParWithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *  @see LoKi::Particles::SourceDesktop
     *  @see LoKi::Particles::SourceTES
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParWithSource                    MIPSOURCE ;
    // ========================================================================
    /** @typedef MIPDV
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices from desktop
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MIPDV mip  = MIPDV ( "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = mip ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MinImpParDV
     *  @see LoKi::Particles::MinImpParWithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParDV                                MIPDV ;
    // ========================================================================
    /** @typedef MIPTES
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices from transient store
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MIPTES mip  = MIPTES ( LHCb::PrimVertexLocation::Default , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = mip ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MinImpParTES
     *  @see LoKi::Particles::MinImpParWithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParTES                              MIPTES ;
    // ========================================================================
    /** @typedef MIPCHI2SOURCE 
     *  The special version of LoKi::Particles::MinImpParChi2 functor
     *  which gets all the primary vertices form "source"
     *
     *  @code 
     *
     *  // get the helper functor-"source"
     *  const MIPCHI2SOURCE::Source& source = ... ;
     *
     *  // construct the main functor:
     *  const MIPCHI2SOURCE mip  = MIPCHI2SOURCE ( source , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = mip ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MinImpParChi2WithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *  @see LoKi::Vertices::SourceDesktop
     *  @see LoKi::Vertices::SourceTES
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParChi2WithSource            MIPCIH2SOURCE ;
    // ========================================================================
    /** @typedef MIPCHI2DV
     *  The special version of LoKi::Particles::MinImpParChi2 functor
     *  which gets all the primary vertices from desktop
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MIPCHI2DV mip  = MIPCHI2DV ( "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = mip ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MinImpParChi2DV
     *  @see LoKi::Particles::MinImpParChi2WithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParChi2DV                        MIPCHI2DV ;
    // ========================================================================
    /** @typedef MIPCHI2TES
     *  The special version of LoKi::Particles::MinImpParChi2 functor
     *  which gets all the primary vertices from transient store
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MIPCHI2TES mip  = MIPCHI2TES ( LHCb::PrimVertexLocation::Default , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = mip ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MinImpParChi2TES
     *  @see LoKi::Particles::MinImpParChi2WithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParChi2TES                      MIPCHI2TES ;
    // ========================================================================
    /** @var BPVVD 
     *  The special "context-dependent" version of LoKi::Particles::VertexDistance 
     *  functor which gets "the best primary vertex" from IPhysDesktop
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the functor 
     *  const double vd = BPVVD ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VertexDistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexDistance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24        
     */
    const LoKi::Particles::VertexDistanceDV                             BPVVD ;
    // ========================================================================
    /** @var BPVVDSIGN 
     *  The special "context-dependent" version 
     *  of LoKi::Particles::VertexSignedDistance 
     *  functor which gets "the best primary vertex" from IPhysDesktop
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the functor 
     *  const double vd = BPVVDSIGN ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VertexSignedDistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexSignedDistance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24        
     */
    const LoKi::Particles::VertexSignedDistanceDV                   BPVVDSIGN ;
    // ========================================================================
    /** @var BPVVDDOT
     *  The special "context-dependent" version 
     *  of LoKi::Particles::VertexDotDistance 
     *  functor which gets "the best primary vertex" from IPhysDesktop
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the functor 
     *  const double vd = BPVVDDOT ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VertexDotDistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexDotDistance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24        
     */
    const LoKi::Particles::VertexDotDistanceDV                       BPVVDDOT ;
    // ========================================================================
    /** @var BPVVDCHI2
     *  The special "context-dependent" version 
     *  of LoKi::Particles::VertexChi2Distance 
     *  functor which gets "the best primary vertex" from IPhysDesktop
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the functor 
     *  const double chi2 = BPVVDCHI2 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VertexChi2DistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexChi2Distance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24        
     */
    const LoKi::Particles::VertexChi2DistanceDV                     BPVVDCHI2 ;
    // ========================================================================
    



  } // end of namespace LoKi::Cuts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLECONTEXTCUTS_H
// ============================================================================
