// $Id: Particles3.h,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PARTICLES3_H 
#define LOKI_PARTICLES3_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/ImpactParamTool.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-19 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    /** @class ClosestApproach
     *
     *  evaluator of the closest approach 
     *  distance between 2 particles 
     *  
     *  The tool IGeomDispCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IGeomDispCalculator
     * 
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class ClosestApproach : 
      public LoKi::Function<const LHCb::Particle*> , 
      public LoKi::Vertices::ImpactParamTool 
    {
    public:
      /// constructor from the particle and the tool  
      ClosestApproach 
      ( const LHCb::Particle*                  particle , 
        const LoKi::Vertices::ImpactParamTool& tool     ) ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ClosestApproach* clone() const 
      { return new ClosestApproach(*this) ; }
      /// MANDATORY: destructor 
      virtual ~ClosestApproach(){};
      /// MANDATORY: the only one essential method 
      virtual result_type operator () ( argument p ) const
      { return distance ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluation
      result_type distance( argument p ) const ;
    public:
      /// accessor to the particle 
      const LHCb::Particle* particle() const 
      { return m_particle ; }
      /// set new particle 
      void setParticle( const LHCb::Particle* value ) const 
      { m_particle = value ; }
    private:
      // default constructor is private 
      ClosestApproach();
    private:
      mutable const LHCb::Particle* m_particle ;
    } ;


    /** @class ClosestApproachChi2
     *
     *  evaluator of the closest approach chi2 
     *  distance between 2 particles 
     *  
     *  The tool IGeomDispCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IGeomDispCalculator
     * 
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class ClosestApproachChi2 : 
      public LoKi::Function<const LHCb::Particle*> , 
      public LoKi::Vertices::ImpactParamTool 
    {
    public:
      /// constructor from the particle and the tool  
      ClosestApproachChi2 
      ( const LHCb::Particle*  particle , 
        const LoKi::Vertices::ImpactParamTool& tool     ) ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ClosestApproachChi2* clone() const 
      { return new ClosestApproachChi2(*this) ; }
      /// MANDATORY: destructor 
      virtual ~ClosestApproachChi2(){};
      /// MANDATORY: the only one essential method 
      virtual result_type operator () ( argument p ) const 
      { return chi2( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluation
      result_type chi2( argument p ) const ;
    public:
      /// accessor to the particle 
      const LHCb::Particle* particle() const { return m_particle ; }
      /// set new particle 
      void setParticle ( const LHCb::Particle* value ) const 
      { m_particle = value ; }
    private:
      // default constructor is private 
      ClosestApproachChi2();
    private:
      mutable const LHCb::Particle* m_particle ;
    } ;

    /** @class MinClosestApproach
     *
     *  evaluator of the minimal value of the 
     *  closest approach distance between the 
     *  particle and soem other particles 
     *  
     *  The tool IGeomDispCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IGeomDispCalculator
     *  @see LoKi::Particles::ClosestApproach
     * 
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class MinClosestApproach : 
      public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LHCb::Particle::Vector&          particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LHCb::Particle::ConstVector&     particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const SmartRefVector<LHCb::Particle>&  particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::PhysTypes::Range&          particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool& tool      ,
        const LHCb::Particle::Vector&          particles ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool& tool      ,
        const LHCb::Particle::ConstVector&     particles ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool& tool      , 
        const SmartRefVector<LHCb::Particle>&  particles ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool& tool      , 
        const LoKi::PhysTypes::Range&          particles ) ;
      /** templated contructor 
       *  from the sequence of "particles"
       *  @param first 'begin'-iterator for the sequence 
       *  @param last  'end'-iterator for the sequence
       *  @param helper tool needed for evaluation
       */
      template <class PARTICLE>
      MinClosestApproach 
      ( PARTICLE first , 
        PARTICLE last  ,
        const LoKi::Vertices::ImpactParamTool& tool ) 
      : LoKi::Function<const LHCb::Particle*> ()
      , m_particles ( first , last ) 
      , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
      { removeParticle() ; } ;
      /** templated contructor 
       *  from the sequence of "particles"
       *  @param helper tool needed for evaluation
       *  @param first 'begin'-iterator for the sequence 
       *  @param last  'end'-iterator for the sequence
       */
      template <class PARTICLE>
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool& tool  ,
        PARTICLE                               first , 
        PARTICLE                               last  )
        : LoKi::Function<const LHCb::Particle*> ()
        , m_particles ( first , last ) 
        , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
      { removeParticle() ; } ;
      /// copy constructor 
      MinClosestApproach 
      ( const MinClosestApproach& right    ) ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinClosestApproach* clone() const 
      { return new MinClosestApproach(*this) ; }
      /// MANDATORY: destructor 
      virtual ~MinClosestApproach(){};
      /// MANDATORY: the only one essential method 
      virtual result_type operator () ( argument p ) const 
      { return distance ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluation
      result_type distance( argument p ) const ;
    public:
      /** add the particle to the list
       *  @param p particle to be added 
       *  @return the actual number of particles in the list
       */
      size_t addParticle ( const LHCb::Particle* p ) 
      { 
        m_particles.push_back( p ) ;
        removeParticle() ;
        return m_particles.size() ;
      } ;
      /** add the particles to the list
       *  @param first 'begin'-itrator for the sequence of  particles
       *  @param last  'end'-itrator for the sequence of  particles
       *  @return the actual number of particles in the list
       */
      template <class PARTICLE>
      size_t addParticles ( PARTICLE first , PARTICLE last ) 
      {
        m_particles.insert( m_particles.end() , first , last ) ;
        removeParticle() ;
        return m_particles.size() ;        
      } ;
      /** remove the particle form the list
       *  @param p particle to be removed 
       *  @return the actual number of particles in the list
       */
      size_t removeParticle ( const LHCb::Particle* p = 0 ) ;      
    private:
      // default constructor is private 
      MinClosestApproach();
    private:
      typedef std::vector<const LHCb::Particle*> PARTICLES ;
      PARTICLES                        m_particles ;
      LoKi::Particles::ClosestApproach m_fun ;
    } ;

    /** @class MinClosestApproachChi2
     *
     *  evaluator of the minimal value of the chi2 of the  
     *  closest approach distance between the 
     *  particle and soem other particles 
     *  
     *  The tool IGeomDispCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IGeomDispCalculator
     *  @see LoKi::Particles::ClosestApproachChi2
     * 
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class MinClosestApproachChi2 : 
      public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LHCb::Particle::Vector&          particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LHCb::Particle::ConstVector&     particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const SmartRefVector<LHCb::Particle>&  particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LoKi::PhysTypes::Range&          particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool      ,
        const LHCb::Particle::Vector&          particles ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool      ,
        const LHCb::Particle::ConstVector&     particles ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool      , 
        const SmartRefVector<LHCb::Particle>&  particles ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool      , 
        const LoKi::PhysTypes::Range&          particles ) ;
      /** templated contructor 
       *  from the sequence of "particles"
       *  @param first 'begin'-iterator for the sequence 
       *  @param last  'end'-iterator for the sequence
       *  @param tool helepr tool needed for evaluations 
       */
      template <class PARTICLE>
      MinClosestApproachChi2 
      ( PARTICLE first , 
        PARTICLE last  ,
        const LoKi::Vertices::ImpactParamTool& tool ) 
        : LoKi::Function<const LHCb::Particle*> ()
          , m_particles ( first , last ) 
          , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
      { removeParticle() ; };
      /** templated contructor 
       *  from the sequence of "particles"
       *  @param tool helepr tool needed for evaluations 
       *  @param first 'begin'-iterator for the sequence 
       *  @param last  'end'-iterator for the sequence
       */
      template <class PARTICLE>
      MinClosestApproachChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool  , 
        PARTICLE                               first , 
        PARTICLE                               last  )
        : LoKi::Function<const LHCb::Particle*> ()
        , m_particles ( first , last ) 
          , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
      { removeParticle() ; };
      /// copy constructor 
      MinClosestApproachChi2 
      ( const MinClosestApproachChi2& right    ) ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinClosestApproachChi2* clone() const 
      { return new MinClosestApproachChi2(*this) ;}
      /// MANDATORY: destructor 
      virtual ~MinClosestApproachChi2(){} ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator () ( argument p ) const 
      { return chi2( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /// the actual evaluation
      result_type chi2 ( argument p ) const ;
    public:
      /** add the particle to the list
       *  @param p particle to be added 
       *  @return the actual number of particles in the list
       */
      size_t addParticle ( const LHCb::Particle* p ) 
      { 
        m_particles.push_back( p ) ;
        removeParticle() ;
        return m_particles.size() ;
      } ;
      /** add the particles to the list
       *  @param first 'begin'-itrator for the sequence of  particles
       *  @param last  'end'-itrator for the sequence of  particles
       *  @return the actual number of particles in the list
       */
      template <class PARTICLE>
      size_t addParticles ( PARTICLE first , PARTICLE last ) 
      {
        m_particles.insert( m_particles.end() , first , last ) ;
        removeParticle() ;
        return m_particles.size() ;        
      } ;
      /** remove the particle form the list
       *  @param p particle to be removed 
       *  @return the actual number of particles in the list
       */
      size_t removeParticle ( const LHCb::Particle* p = 0 ) ;
    private:
      // default constructor is private 
      MinClosestApproachChi2();
    private:
      typedef std::vector<const LHCb::Particle*> PARTICLES ;
      PARTICLES                            m_particles ;
      LoKi::Particles::ClosestApproachChi2 m_fun       ;
    } ;    

  } ; // end of namespace LoKi::Particles
} ; // end of namespace LoKi


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES3_H
// ============================================================================
