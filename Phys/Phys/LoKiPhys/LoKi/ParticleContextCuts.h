// $Id: ParticleContextCuts.h,v 1.3 2008-03-30 13:43:36 ibelyaev Exp $
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
     *  const double costheta = BPVDIRA ( B ) ;
     *
     *  @endcode
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *
     *  @see LoKi::Cuts::BESTPVCOSPF
     *  @see LoKi::Cuts::DIRA
     *  @see LoKi::Particles::CosineDirectionAngle
     *  @see LoKi::Particles::CosineDirectionAngleWithTheBestPrimaryVertex
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    const LoKi::Particles::CosineDirectionAngleWithTheBestPV          BPVDIRA ;
    // ========================================================================
    /** @var BESTPVCOSPF
     *  The special "context-dependent" version of DIRA functor.
     *  It takes the best primary vertex from IPhysDesktop tool
     *
     *  @code 
     *
     *  // B-candidate:
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor
     *  const double costheta = BESTPVDIRA ( B ) ;
     *
     *  @endcode
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @see LoKi::Cuts::BPVDIRA
     *  @see LoKi::Cuts::DIRA
     *  @see LoKi::Particles::CosineDirectionAngle
     *  @see LoKi::Particles::CosineDirectionAngleWithTheBestPrimaryVertex
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */    
    const LoKi::Particles::CosineDirectionAngleWithTheBestPV      BESTPVCOSPF ;
    // ========================================================================
    /** @typedef BPVIP 
     *  The special "context-dependent" version of LoKi::Particles::ImpPar functor
     *  which gets the related primary vertex from IPhysDesktop tool
     *
     *  @code
     *
     *   // construct the functor 
     *   const BPVIP fun  = BPVIP ("")  ;
     *
     *   // B-candidate
     *   const LHCb::Particle* B = ... ;
     *
     *   // use the functor
     *   const double ip = fun ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Cuts::BESTPVIP
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
    /** @typedef BESTPVIP 
     *  The special "context-dependent" version of LoKi::Particles::ImpPar functor
     *  which gets the related primary vertex from IPhysDesktop tool
     *
     *  @code
     *
     *   // construct the functor 
     *   const BESPPVIP fun  = BESTPVIP ("")  ;
     *
     *   // B-candidate
     *   const LHCb::Particle* B = ... ;
     *
     *   // use the functor
     *   const double ip = fun ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Cuts::BPVIP
     *  @see LoKi::Particles::ImpParWithTheBestPV
     *  @see LoKi::Particles::ImpPar
     *  @see IGeomDispCalculator
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24
     */
    typedef LoKi::Particles::ImpParWithTheBestPV                     BESTPVIP ;
    // ========================================================================
    /** @typedef BPVIPCHI2
     *  The special "context-dependent" version of LoKi::Particles::ImpParChi2 
     *  functor which gets the related primary vertex from IPhysDesktop tool
     *
     *  @code
     *
     *   // construct the functor 
     *   const BPIPCHI2 fun  = BPIPCHI2 ("")  ;
     *
     *   // B-candidate
     *   const LHCb::Particle* B = ... ;
     *
     *   // use the functor
     *   const double chi2 = fun ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Cuts::BESTPVIPCHI2 
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
    /** @typedef BESTPVIPCHI2
     *  The special "context-dependent" version of LoKi::Particles::ImpParChi2 
     *  functor which gets the related primary vertex from IPhysDesktop tool
     *
     *  @code
     *
     *   // constuct the functor 
     *   const BESTPIPCHI2 fun  = BESTPIPCHI2 ("")  ;
     *
     *   // B-candidate
     *   const LHCb::Particle* B = ... ;
     *
     *   // use the functor
     *   const double chi2 = fun ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Cuts::BPVIPCHI2 
     *  @see LoKi::Particles::ImpParChi2WithTheBestPV
     *  @see LoKi::Particles::ImpParChi2
     *  @see IGeomDispCalculator
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-24
     */
    typedef LoKi::Particles::ImpParChi2WithTheBestPV             BESTPVIPCHI2 ;
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
     *  const MIPSOURCE fun  = MIPSOURCE ( source , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MINIPWITHSOURCE
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
    /** @typedef MINIPWITHSOURCE 
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices form "source"
     *
     *  @code 
     *
     *  // get the helper functor-"source"
     *  const MINIPWITHSOURCE::Source& source = ... ;
     *
     *  // construct the main functor:
     *  const MINIPWITHSOURCE fun  = MINIPWITHSOURCE ( source , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MIPSOURCE
     *  @see LoKi::Particles::MinImpParWithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *  @see LoKi::Particles::SourceDesktop
     *  @see LoKi::Particles::SourceTES
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
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
     *  @see LoKi::Cuts::MINIPWITHDV 
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
    /** @typedef MINIPWITHDV
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices from desktop
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MINIPWITHDV fun  = MINIPWITHDV ( "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MINIPWITHDV 
     *  @see LoKi::Particles::MinImpParDV
     *  @see LoKi::Particles::MinImpParWithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParDV                          MINIPWITHDV ;
    // ========================================================================
    /** @typedef MIPTES
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices from transient store
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MIPTES fun  = MIPTES ( LHCb::PrimVertexLocation::Default , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MINIPWITHTES
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
    /** @typedef MINIPWITHTES
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices from transient store
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MINIPWITHTES fun  = MINIPWITHPTES ( LHCb::PrimVertexLocation::Default , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MIPTES
     *  @see LoKi::Particles::MinImpParTES
     *  @see LoKi::Particles::MinImpParWithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParTES                         MINIPWITHTES ;
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
     *  const MIPCHI2SOURCE fun  = MIPCHI2SOURCE ( source , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MINIPCHI2WITHSOURCE
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
    typedef LoKi::Particles::MinImpParChi2WithSource            MIPCHI2SOURCE ;
    // ========================================================================
    /** @typedef MINIPCHI2WITHSOURCE 
     *  The special version of LoKi::Particles::MinImpParChi2 functor
     *  which gets all the primary vertices form "source"
     *
     *  @code 
     *
     *  // get the helper functor-"source"
     *  const MINIPCHI2WITHSOURCE::Source& source = ... ;
     *
     *  // construct the main functor:
     *  const MINIPCHI2WITHSOURCE fun  = MINIPCHI2WITHSOURCE ( source , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MIPCHI2SOURCE
     *  @see LoKi::Particles::MinImpParChi2WithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *  @see LoKi::Vertices::SourceDesktop
     *  @see LoKi::Vertices::SourceTES
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParChi2WithSource      MINIPCHI2WITHSOURCE ;
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
     *  @see LoKi::Particles::MINIPCHI2WITHDV
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
    /** @typedef MINIPCHI2WITHDV
     *  The special version of LoKi::Particles::MinImpParChi2 functor
     *  which gets all the primary vertices from desktop
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MINIPCHI2WITHDV fun  = MINIPCHI2WITHDV ( "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MICHI2DV
     *  @see LoKi::Particles::MinImpParChi2DV
     *  @see LoKi::Particles::MinImpParChi2WithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParChi2DV                  MINIPCHI2WITHDV ;
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
     *  @see LoKi::Particles::MINIPCHI2WITHTES
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
    /** @typedef MINIPCHI2WITHTES
     *  The special version of LoKi::Particles::MinImpParChi2 functor
     *  which gets all the primary vertices from transient store
     *
     *  @code 
     *
     *  // construct the functor:
     *  const MINIPCHI2WITHTES fun  = MINIPCHI2WITHTES ( LHCb::PrimVertexLocation::Default , "" ) ;
     *
     *  // get B-candidate 
     *  const LHCb::Particle* B = ... ;
     *  
     *  // use the functor:
     *  const doule value = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::MIPCHI2TES
     *  @see LoKi::Particles::MinImpParChi2TES
     *  @see LoKi::Particles::MinImpParChi2WithSource 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    typedef LoKi::Particles::MinImpParChi2TES                MINIPCHI2WITHTES ;
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
     *  @see LoKi::Cuts::BESTPVVERTSEP
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
    /** @var BESTPVVERTSEP
     *  The special "context-dependent" version of LoKi::Particles::VertexDistance 
     *  functor which gets "the best primary vertex" from IPhysDesktop
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the functor 
     *  const double vd = BESTPVVERTSEP( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::BPVVD
     *  @see LoKi::Cuts::VertexDistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexDistance
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
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
    const LoKi::Particles::VertexDistanceDV                     BESTPVVERTSEP ;
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
     *  @see LoKi::Cuts::BESTPVVERTSEPSIGN
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
    /** @var BESTPVVERTSEPSIGN 
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
     *  const double vd = BESTPVVERTSEPSIGN ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::BPVVDSIGN
     *  @see LoKi::Cuts::VertexSignedDistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexSignedDistance
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
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
    const LoKi::Particles::VertexSignedDistanceDV          BESTPVVERTSEPSIGN ;
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
     *  @see LoKi::Cuts::BESTPVVERTSEPALONGMOM
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
    /** @var BESTPVVERTSEPALONGMOM 
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
     *  const double vd = BESTPVVERTSEPALONGMOM ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::BPVVDDOT 
     *  @see LoKi::Cuts::VertexDotDistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexDotDistance
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
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
    const LoKi::Particles::VertexDotDistanceDV          BESTPVVERTSEPALONGMOM ;
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
     *  @see LoKi::Cuts::BESTPVVERTSEPCHI2
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
    /** @var BESTPVVERTSEPCHI2 
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
     *  @see LoKi::Cuts::BPVVDCHI2
     *  @see LoKi::Cuts::VertexChi2DistanceDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexChi2Distance
     *
     *  The name comes from Vava GLIGOROV v.gligorov@physics.gla.ac.uk
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
    const LoKi::Particles::VertexChi2DistanceDV              BESTPVVERTSEPCHI2 ;
    // ========================================================================
    /** @typedef BPVLTIME
     *  The special "context-dependent" version of 
     *  LoKi::Particles::LifeTime functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from GaudiAlgorithm 
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // prepare the functor:
     *  Fun fun = BPVLTIME ( "PropertimeFitter/ProperTime:PUBLIC" ) ;
     *
     *  // use the functor 
     *  const double ltime = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::LifeTimeDV
     *  @see LoKi::Particles::LifeTime
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTime
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-02-10
     */
    typedef LoKi::Particles::LifeTimeDV                              BPVLTIME ;
    // ========================================================================    
    /** @typedef BPVLTCHI2
     *  The special version of LoKi::Particles::LifeTimeChi2 functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from GaudiAlgorithm 
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // prepare the functor:
     *  Fun fun = BPVLTCHI2 ( "PropertimeFitter/ProperTime:PUBLIC" ) ;
     *
     *  // use the functor 
     *  const double chi2 = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Particles::LifeTimeChi2DV
     *  @see LoKi::Particles::LifeTimeChi2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTimeChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    typedef LoKi::Particles::LifeTimeChi2DV                         BPVLTCHI2 ;
    // ========================================================================    
    /** @typedef BPVLTSIGNCHI2 
     *  The special version of LoKi::Particles::LifeTimeChi2 functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from GaudiAlgorithm 
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // prepare the functor:
     *  Fun fun = BPVLTSIGNCHI2 ( "PropertimeFitter/ProperTime:PUBLIC" ) ;
     *
     *  // use the functor 
     *  const double chi2 = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::BPVLTSIGNCHI2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTimeSignedChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    typedef LoKi::Particles::LifeTimeSignedChi2DV               BPVLTSIGNCHI2 ;
    // ========================================================================    
    /** @class BPVLTFITCHI2
     *  The special version of LoKi::Particles::LifeTimeFitChi2 functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from DVAlgorithm 
     *
     *  @code
     *
     *  // get B-candidate:
     *  const LHCb::Particle* B = ... ;
     *
     *  // prepare the functor:
     *  Fun fun = BPVLTFITCHI2 ( "PropertimeFitter/ProperTime:PUBLIC" ) ;
     *
     *  // use the functor 
     *  const double chi2 = fun ( B ) ;
     *
     *  @endcode 
     *
     *  @attention This functor is *NOT* (time/sigma)**2, this functor evaluates 
     *             the quality/consistency of the properlifetime fit
     *
     *  @see LoKi::Particles::LifeTimeFitChi2DV
     *  @see LoKi::Particles::LifeTimeFitChi2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTimeSignedChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    typedef LoKi::Particles::LifeTimeFitChi2DV                   BPVLTFITCHI2 ;
    // ========================================================================
    /** @var BPVVDZ 
     *
     *  The functor which evaluates Delta(Z) for the end-vertex of the 
     *  particle and "the best primary vertex" from the Desktop
     *
     *  @code 
     * 
     *  const LHCb::Particle* B = ... ;
     * 
     *  const double deltaZ = BPVVDZ ( B ) ;
     *
     *  @endcode
     *
     *  The concept and the name come 
     *     from Sean BRISBANE s.brisbane1@physics.ox.ac.uk
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */
    const LoKi::Particles::VertexZDistanceWithTheBestPV                BPVVDZ ;
    // ========================================================================
    /** @var BPVVDRHO
     *
     *  The functor which evaluates Delta(Rho) (cylindrical) 
     *  for the end-vertex of the 
     *  particle and "the best primary vertex" from the Desktop
     *
     *  @code 
     * 
     *  const LHCb::Particle* B = ... ;
     * 
     *  const double deltaRho = BPVVDRHO ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Cuts::BPVVDR
     * 
     *  The concept and the name come 
     *     from Sean BRISBANE s.brisbane1@physics.ox.ac.uk
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */
    const LoKi::Particles::VertexRhoDistanceWithTheBestPV           BPVVDRHO  ;
    // ========================================================================
    /** @var BPVVDR
     *
     *  The functor which evaluates Delta(Rho) (cylindrical) 
     *  for the end-vertex of the 
     *  particle and "the best primary vertex" from the Desktop
     *
     *  @code 
     * 
     *  const LHCb::Particle* B = ... ;
     * 
     *  const double deltaRho = BPVVDR ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Cuts::BPVVDRHO 
     *
     *  The concept and the name come 
     *     from Sean BRISBANE s.brisbane1@physics.ox.ac.uk
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */
    const LoKi::Particles::VertexRhoDistanceWithTheBestPV             BPVVDR ;
    // ========================================================================
    /** @typedef MINVDSOURCE 
     *  The simlpe functor which  evaluates the minimal distance 
     *  between the end=verted of the particle and any vertex from 
     *  list (presumably the primary vertex). The vertices are extracted 
     *  from the "source":
     *
     *  @code 
     *  
     *   const MINVDSOURCE minVD ( VSOURCEDV ( PRIMARY ) ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     *
     *   const double minDistance = minVD ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VMINVDSOURCE 
     *  @see LoKi::Cuts::MINVDDV 
     *  @see LoKi::Cuts::MINVDTES
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */ 
    typedef LoKi::Particles::MinVertexDistanceWithSource         MINVDSOURCE ;
    // ========================================================================
    /** @typedef MINVDDV 
     *  The simlpe functor which  evaluates the minimal distance 
     *  between the end=verted of the particle and any vertex from 
     *  list (presumably the primary vertex). The vertices are extracted 
     *  from the desktop.
     *
     *  @code 
     *  
     *   const MINVDDV minVD ( PRIMARY ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     *
     *   const double minDistance = minVD ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VMINVDDV
     *  @see LoKi::Cuts::MINVDSOURCE 
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */ 
    typedef LoKi::Particles::MinVertexDistanceDV                     MINVDDV ;
    // ========================================================================
    /** @typedef MINVDTES
     *  The simlpe functor which  evaluates the minimal distance 
     *  between the end=verted of the particle and any vertex from 
     *  list (presumably the primary vertex). The vertices are extracted 
     *  from the TES
     *
     *  @code 
     *  
     *   const MINVDTES minVD ("SomeLocationInTES", PRIMARY ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     *
     *   const double minDistance = minVD ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VMINVDTES
     *  @see LoKi::Cuts::MINVDSOURCE 
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */
    typedef LoKi::Particles::MinVertexDistanceTES                   MINVDTES ;
    // ========================================================================
    /** @typedef MINVDCHI2SOURCE 
     *  The simple functor which  evaluates the minimal chi2-distance 
     *  between the end=verted of the particle and any vertex from 
     *  list (presumably the primary vertex). The vertices are extracted 
     *  from the "source":
     *
     *  @code 
     *  
     *   const MINVDCHI2SOURCE minCHI2 ( VSOURCEDV ( PRIMARY ) ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     *
     *   const double minChi2 = minCHI2 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VMINVDCHI2SOURCE 
     *  @see LoKi::Cuts::MINVDCHI2DV 
     *  @see LoKi::Cuts::MINVDCHI2TES
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */ 
    typedef LoKi::Particles::MinVertexChi2DistanceWithSource MINVDCHI2SOURCE ;
    // ========================================================================
    /** @typedef MINVDCHI2DV
     *  The simlpe functor which  evaluates the minimal chi2-distance 
     *  between the end-vertex of the particle and any vertex from 
     *  list (presumably the primary vertex). The vertices are extracted 
     *  from the desktop.
     *
     *  @code 
     *  
     *   const MINVDCHI2DV minCHI2 ( PRIMARY ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     *
     *   const double minChi2 = minCHI2 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VMINVDCHI2DV
     *  @see LoKi::Cuts::MINVDCHI2SOURCE 
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */ 
    typedef LoKi::Particles::MinVertexChi2DistanceDV             MINVDCHI2DV ;
    // ========================================================================
    /** @typedef MINVDCHI2TES
     *  The simlpe functor which  evaluates the minimal chi2-distance 
     *  between the end-vertex of the particle and any vertex from 
     *  list (presumably the primary vertex). The vertices are extracted 
     *  from the TES
     *
     *  @code 
     *  
     *   const MINVDCHI2TES minCHI2 ("SomeLocationInTES", PRIMARY ) ;
     *  
     *   const LHCb::Particle* B = ... ;
     *
     *   const double minChi2 = minCHI2 ( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::VMINVDCHI2TES
     *  @see LoKi::Cuts::MINVDCHI2SOURCE 
     * 
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */
    typedef LoKi::Particles::MinVertexChi2DistanceTES           MINVDCHI2TES ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLECONTEXTCUTS_H
// ============================================================================
