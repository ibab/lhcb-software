// $Id: Particles35.h,v 1.1 2010-05-28 10:02:25 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES35_H 
#define LOKI_PARTICLES35_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class ProtoPFun
     *  simple adaptor for proto-particle functor
     *  @see LoKi::Cuts::PPFUN 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    class ProtoPFun : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from protoparticle-function
      ProtoPFun 
      ( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function& fun ,
        const double                                                bad ) ;
      /// constructor from protoparticle-function
      ProtoPFun 
      ( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function& fun ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ProtoPFun() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoPFun* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ProtoPFun () ;                    // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the functor itself 
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Function>::Type m_fun ;
      /// bad value 
      double               m_bad ;                        // bad value 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class ProtoPCut
     *  simple adaptor for proto-particle functor
     *  @see LoKi::Cuts::PPCUT 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    class ProtoPCut : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from protoparticle-function
      ProtoPCut
      ( const LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Predicate& cut ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ProtoPCut() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoPCut* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ProtoPCut () ;                    // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the functor itself 
      LoKi::Assignable<LoKi::BasicFunctors<const LHCb::ProtoParticle*>::Predicate>::Type m_cut ;
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class TrackFun
     *  simple adaptor for track functor
     *  @see LoKi::Cuts:TRFUN 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    class TrackFun : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from protoparticle-function
      TrackFun 
      ( const LoKi::BasicFunctors<LHCb::Track>::Function& fun , 
        const double                                     bad ) ;
      /// constructor from protoparticle-function
      TrackFun 
      ( const LoKi::BasicFunctors<LHCb::Track>::Function& fun ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrackFun() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrackFun* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrackFun () ;                    // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the functor itself 
      LoKi::Assignable<LoKi::BasicFunctors<LHCb::Track>::Function>::Type m_fun ;
      /// bad value 
      double               m_bad ;                        // bad value 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TrackCut
     *  simple adaptor for track functor
     *  @see LoKi::Cuts:TRCUT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    class TrackCut : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from protoparticle-function
      TrackCut 
      ( const LoKi::BasicFunctors<LHCb::Track>::Predicate& cut ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrackCut() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrackCut* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrackCut () ;                    // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the functor itself 
      LoKi::Assignable<LoKi::BasicFunctors<LHCb::Track>::Predicate>::Type m_cut ;
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef PPFUN
     *  Simple adaptor for ptotoparticle functor 
     *  @code 
     *
     *    // get protoparticle functor 
     *    const LoKi::Types::PPFunc& func = ...; 
     *
     *    // construct adaptor
     *    const PPFUN fun = PPFUN ( func ) ;
     *  
     *    const LHCb::Particle* pion = ... ;
     *  
     *    // use adapter!
     *    const double result = fun ( pion ) ;
     *
     *  @endcode
     *  @see LoKi::Types::PPFunc
     *  @see LoKi::Particles::ProtoPFun
     *  @see LHCb::ProtoParticle
     *  @see LoKi::PPTypes 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    typedef LoKi::Particles::ProtoPFun                                  PPFUN ;
    // ========================================================================
    /** @typedef PPCUT
     *  Simple adaptor for ptotoparticle predicate
     *  @code 
     *
     *    // get protoparticle functor 
     *    const LoKi::Types::PPCuts& cuts = ...; 
     *
     *    // construct adaptor
     *    const PPCUT fun = PPCUT ( cuts ) ;
     *  
     *    const LHCb::Particle* pion = ... ;
     *  
     *    // use adapter!
     *    const bool ok = fun ( pion ) ;
     *
     *  @endcode
     *  @see LoKi::Types::PPCuts
     *  @see LoKi::Particles::ProtoPCut
     *  @see LHCb::ProtoParticle
     *  @see LoKi::PPTypes 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    typedef LoKi::Particles::ProtoPCut                                  PPCUT ;
    // ========================================================================
    /** @typedef TRFUN
     *  Simple adaptor for track functor 
     *  @code 
     *
     *    // get protoparticle functor 
     *    const LoKi::Types::TrFunc& func = ...; 
     *
     *    // construct adaptor
     *    const TRFUN fun = TRFUN ( func ) ;
     *  
     *    const LHCb::Particle* pion = ... ;
     *  
     *    // use adapter!
     *    const double result = fun ( pion ) ;
     *
     *  @endcode
     *  @see LoKi::Types::TrFunc
     *  @see LoKi::Particles::TrackFun
     *  @see LHCb::Track
     *  @see LoKi::TrackTypes 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    typedef LoKi::Particles::TrackFun                                  TRFUN ;
    // ========================================================================
    /** @typedef TRCUT
     *  Simple adaptor for track predicate
     *  @code 
     *
     *    // get track functor 
     *    const LoKi::Types::TrCuts& cuts = ...; 
     *
     *    // construct adaptor
     *    const TRCUT fun = TRCUT ( cuts ) ;
     *  
     *    const LHCb::Particle* pion = ... ;
     *  
     *    // use adapter!
     *    const bool ok = fun ( pion ) ;
     *
     *  @endcode
     *  @see LoKi::Types::TrCuts
     *  @see LoKi::Particles::TrackCut
     *  @see LHCb::Track
     *  @see LoKi::TrackTypes 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-26
     */
    typedef LoKi::Particles::TrackCut                                  TRCUT ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================  
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES35_H
