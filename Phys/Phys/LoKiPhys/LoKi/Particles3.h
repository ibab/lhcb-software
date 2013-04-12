// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES3_H 
#define LOKI_PARTICLES3_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/ImpactParamTool.h"
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
 *  @date 2006-02-19 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {   
    // ========================================================================
    /** @class ClosestApproach
     *
     *  evaluator of the closest approach 
     *  distance between 2 particles 
     *  
     *  The tool IDistanceCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IDistanceCalculator
     *  @see LoKi::Cuts::CLAPP
     *
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class GAUDI_API ClosestApproach 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public LoKi::Vertices::ImpactParamTool 
    {
    public:
      // ======================================================================
      /// constructor from the particle and the tool  
      ClosestApproach 
      ( const LHCb::Particle*                  particle , 
        const LoKi::Vertices::ImpactParamTool& tool     ) ;
      /// constructor from the particle and the tool  
      ClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool& tool     ,
        const LHCb::Particle*                  particle ) ;
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
      // ======================================================================
    public:
      // ======================================================================
      /// the actual evaluation
      result_type distance ( argument p ) const 
      { return distance_ ( p , particle() ) ; }
      /// the actual evaluation
      result_type distance_ 
      ( const LHCb::Particle* p1 ,
        const LHCb::Particle* p2 ) const ;
      // ======================================================================
      /// the actual evaluation of chi2 
      result_type chi2_ 
      ( const LHCb::Particle* p1 ,
        const LHCb::Particle* p2 ) const ;
      // ======================================================================
      /// the actual evaluation
      result_type chi2 ( argument p ) const 
      { return chi2_ ( p , particle() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// accessor to the particle 
      const LHCb::Particle* particle() const 
      { return m_particle ; }
      /// set new particle 
      void setParticle( const LHCb::Particle* value ) const 
      { m_particle = value ; }
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is private 
      ClosestApproach();
      // ======================================================================
    protected:
      // ======================================================================
      /// the particle 
      mutable const LHCb::Particle* m_particle ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ClosestApproachChi2
     *
     *  evaluator of the closest approach chi2 
     *  distance between 2 particles 
     *  
     *  The tool IDistanceCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IDistanceCalculator
     *  @see LoKi::Cuts::CLAPPCHI2 
     * 
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class GAUDI_API ClosestApproachChi2 : public ClosestApproach 
    {
    public:
      // ======================================================================
      /// constructor from the particle and the tool  
      ClosestApproachChi2 
      ( const LHCb::Particle*                  particle , 
        const LoKi::Vertices::ImpactParamTool& tool     ) ;
      /// constructor from the particle and the tool  
     ClosestApproachChi2 
     ( const LoKi::Vertices::ImpactParamTool& tool     ,
       const LHCb::Particle*                  particle ) ;
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
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is private 
      ClosestApproachChi2();
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinClosestApproach
     *
     *  evaluator of the minimal value of the 
     *  closest approach distance between the 
     *  particle and soem other particles 
     *  
     *  The tool IDistanceCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IDistanceCalculator
     *  @see LoKi::Particles::ClosestApproach
     * 
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class GAUDI_API MinClosestApproach 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::UniqueKeeper<LHCb::Particle>
    {
    public:
      // ======================================================================
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LHCb::Particle::ConstVector&        particles ,
        const LoKi::Vertices::ImpactParamTool&    tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::PhysTypes::Range&             particles ,
        const LoKi::Vertices::ImpactParamTool&    tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool&    tool      ,
        const LHCb::Particle::ConstVector&        particles ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool&    tool      , 
        const LoKi::PhysTypes::Range&             particles ) ;   
      // ======================================================================
      /** templated contructor 
       *  from the sequence of "particles"
       *  @param first 'begin'-iterator for the sequence 
       *  @param last  'end'-iterator for the sequence
       *  @param tool  helper tool needed for evaluation
       */
      template <class PARTICLE>
      MinClosestApproach 
      ( PARTICLE first , 
        PARTICLE last  ,
        const LoKi::Vertices::ImpactParamTool& tool ) 
      : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
      , LoKi::UniqueKeeper<LHCb::Particle>( first , last ) 
      , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
      {}      
      /** templated contructor 
       *  from the sequence of "particles"
       *  @param tool  helper tool needed for evaluation
       *  @param first 'begin'-iterator for the sequence 
       *  @param last  'end'-iterator for the sequence
       */
      template <class PARTICLE>
      MinClosestApproach 
      ( const LoKi::Vertices::ImpactParamTool& tool  ,
        PARTICLE                               first , 
        PARTICLE                               last  )
        : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
        , LoKi::UniqueKeeper<LHCb::Particle>( first , last ) 
        , m_fun       ( (const LHCb::Particle*) 0 , tool ) 
      {} 
      // ======================================================================
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
      // ======================================================================
    public:
      // ======================================================================
      /// the actual evaluation
      result_type distance ( argument p ) const ;
      /// the actual evaluation
      result_type chi2     ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      MinClosestApproach(); /// default constructor is private 
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Particles::ClosestApproach m_fun ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinClosestApproachChi2
     *
     *  evaluator of the minimal value of the chi2 of the  
     *  closest approach distance between the 
     *  particle and soem other particles 
     *  
     *  The tool IDistanceCalculator is used 
     *  for evaluation
     *
     *  @see LHCb::Particle
     *  @see IDistanceCalculator
     *  @see LoKi::Particles::ClosestApproachChi2
     * 
     *  @date 2003-03-17
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    class GAUDI_API MinClosestApproachChi2 : public MinClosestApproach 
    {
    public:
      // ======================================================================
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LHCb::Particle::ConstVector&     particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LoKi::PhysTypes::Range&          particles ,
        const LoKi::Vertices::ImpactParamTool& tool      ) ;
      /// constructor from the particle(s) and the tool  
      MinClosestApproachChi2 
      ( const LoKi::Vertices::ImpactParamTool& tool      ,
        const LHCb::Particle::ConstVector&     particles ) ;
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
        : LoKi::Particles::MinClosestApproach ( first , last , tool ) 
      {}
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
        : LoKi::Particles::MinClosestApproach ( first , last , tool ) 
      {}
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
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      MinClosestApproachChi2();               // default constructor is private 
      // ======================================================================
    } ;    
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES3_H
// ============================================================================
