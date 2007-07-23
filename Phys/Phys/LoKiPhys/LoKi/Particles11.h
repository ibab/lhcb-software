// $Id: Particles11.h,v 1.6 2007-07-23 17:35:43 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES11_H 
#define LOKI_PARTICLES11_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-22 
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  { 
    // ========================================================================
    /** @class IsAParticle
     *  The trivial predicate which 
     *  evalautes to "true" if particle 
     *  is in the list of particles 
     *  
     *  @see LHCb::Particle
     *
     *  @see LoKi::Cuts::IS 
     *  @see LoKi::Cuts::IN
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22 
     */
    class IsAParticle 
      : public LoKi::Predicate<const LHCb::Particle*>
      , public LoKi::UniqueKeeper<LHCb::Particle>
    {
    public:
      /// constructor from one particle 
      IsAParticle 
      ( const LHCb::Particle*                     p ) ;
      /// constructor from container of particles
      IsAParticle 
      ( const LHCb::Particles*                    p ) ;
      /// constructor from container of particles
      IsAParticle 
      ( const SmartRefVector<LHCb::Particle>&     p ) ;
      /// constructor from vector of particles 
      IsAParticle 
      ( const LHCb::Particle::Vector&             p ) ;
      /// constructor from vector of particles 
      IsAParticle 
      ( const LHCb::Particle::ConstVector&        p ) ;
      /// constructor from container of particle 
      IsAParticle 
      ( const LoKi::PhysTypes::Range&             p ) ;
      /// constructor from container of particle 
      IsAParticle 
      ( const LoKi::Keeper<LHCb::Particle>&       p ) ;
      /// constructor from container of particle 
      IsAParticle 
      ( const LoKi::UniqueKeeper<LHCb::Particle>& p ) ;      
      /** templated constructor from sequence of particles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class PARTICLE>
      IsAParticle
      ( PARTICLE first , 
        PARTICLE last  ) 
        : LoKi::Predicate<const LHCb::Particle*>()
        , LoKi::UniqueKeeper<LHCb::Particle>( first , last ) 
      {};
      /// copy constructor 
      IsAParticle( const IsAParticle& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~IsAParticle() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsAParticle* clone() const 
      { return new IsAParticle(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // The default costructor is disabled 
      IsAParticle();
    } ;
    // ========================================================================    
    /** @class IsAParticleInTree
     *  The trivial predicate which 
     *  evalautes to "true" if particle or any of dauhter particles 
     *  is in the list of particles 
     *  
     *  @see LHCb::Particle
     *
     *  @see LoKi::Cuts::ISINTREE
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22 
     */
    class IsAParticleInTree : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// constructor from one particle 
      IsAParticleInTree 
      ( const LHCb::Particle*                 p ) ;
      /// constructor from container of particles
      IsAParticleInTree
      ( const LHCb::Particles*                p ) ;
      /// constructor from container of particles
      IsAParticleInTree
      ( const SmartRefVector<LHCb::Particle>& p ) ;
      /// constructor from vector of particles 
      IsAParticleInTree
      ( const LHCb::Particle::Vector&         p ) ;
      /// constructor from vector of particles 
      IsAParticleInTree
      ( const LHCb::Particle::ConstVector&    p ) ;
      /// constructor from container of particle 
      IsAParticleInTree
      ( const LoKi::PhysTypes::Range&         p ) ;
      /// constructor from container of particle 
      IsAParticleInTree
      ( const LoKi::Keeper<LHCb::Particle>&   p ) ;
      /// constructor from container of particle 
      IsAParticleInTree
      ( const LoKi::UniqueKeeper<LHCb::Particle>& p ) ;      
      /** templated constructor from sequence of particles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class PARTICLE>
      IsAParticleInTree
      ( PARTICLE first , 
        PARTICLE last  ) 
        : LoKi::Predicate<const LHCb::Particle*>()
        , m_cut ( first , last ) 
      {};
      /// copy constructor 
      IsAParticleInTree( const IsAParticleInTree& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~IsAParticleInTree() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsAParticleInTree* clone() const 
      { return new IsAParticleInTree(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    public:
      bool   empty () const { return m_cut.empty () ; }
      size_t size  () const { return m_cut.size  () ; }
    private:
      // The defualt costructor is disabled 
      IsAParticleInTree();
    private:
      // the evaluator 
      LoKi::Particles::IsAParticle m_cut ;
    } ;
    // ========================================================================    
    /** @class IsAParticleFromTree
     *  The trivial predicate which 
     *  evalautes to "true" if particle comes from the 
     *  decay trees of other particles from the list 
     *  
     *  @see LHCb::Particle
     *
     *  @see LoKi::Cuts::FROMTREE
     *  @see LoKi::Cuts::FROM
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-22 
     */
    class IsAParticleFromTree
      : public LoKi::Predicate<const LHCb::Particle*>
      , public LoKi::UniqueKeeper<LHCb::Particle>
    {
    public:
      /// constructor from one particle 
      IsAParticleFromTree 
      ( const LHCb::Particle*                 p ) ;
      /// constructor from container of particles
      IsAParticleFromTree
      ( const LHCb::Particles*                p ) ;
      /// constructor from container of particles
      IsAParticleFromTree
      ( const SmartRefVector<LHCb::Particle>& p ) ;
      /// constructor from vector of particles 
      IsAParticleFromTree
      ( const LHCb::Particle::Vector&         p ) ;
      /// constructor from vector of particles 
      IsAParticleFromTree
      ( const LHCb::Particle::ConstVector&    p ) ;
      /// constructor from container of particle 
      IsAParticleFromTree
      ( const LoKi::PhysTypes::Range&         p ) ;
      /// constructor from container of particle 
      IsAParticleFromTree
      ( const LoKi::Keeper<LHCb::Particle>&   p ) ;
      /// constructor from container of particle 
      IsAParticleFromTree
      ( const LoKi::UniqueKeeper<LHCb::Particle>& p ) ;      
      /** templated constructor from sequence of particles 
       *  @param first 'begin'-iterator of the sequence 
       *  @param end   'end'-iterator of the sequence 
       */
      template <class PARTICLE>
      IsAParticleFromTree
      ( PARTICLE first , 
        PARTICLE last  ) 
        : LoKi::Predicate<const LHCb::Particle*>()
        , LoKi::UniqueKeeper<LHCb::Particle> ( first , last ) 
      {};
      /// copy constructor 
      IsAParticleFromTree( const IsAParticleFromTree& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~IsAParticleFromTree() {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsAParticleFromTree* clone() const 
      { return new IsAParticleFromTree(*this); }
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // The defualt costructor is disabled 
      IsAParticleFromTree();
    } ;
    // ========================================================================    
  } // end of namespace Particles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES11_H
// ============================================================================
