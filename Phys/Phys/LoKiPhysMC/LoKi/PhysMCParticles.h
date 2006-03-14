// $Id: PhysMCParticles.h,v 1.2 2006-03-14 19:37:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PHYSMCPARTICLES_H 
#define LOKI_PHYSMCPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Keeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
// LoKiMC  
// ============================================================================
#include "LoKi/MCTypes.h"
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/PhysMCTypes.h"
#include "LoKi/MCMatch.h"
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
 *  @date 2006-02-20 
 */
// ============================================================================

namespace LoKi 
{
  /// forward declarations 
  class MCMatch ;
  
  namespace Particles 
  {
    /** @class MCTruth MCTruth.h LoKi/MCTruth.h
     *  
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2003-01-28
     */
    class MCTruth 
      : public LoKi::Predicate<const LHCb::Particle*>
      , public LoKi::Keeper<LHCb::MCParticle>
    {
    public:      
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param particle  pointer to MC particle object 
       */
      MCTruth 
      ( const LoKi::MCMatch&     match    , 
        const LHCb::MCParticle*  particle ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCMatch&            match   , 
        const LoKi::MCTypes::MCRange&  range   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCMatch&              match   , 
        const LoKi::MCTypes::MCContainer& range   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCMatch&                       match   , 
        const std::vector<const LHCb::MCParticle*> range   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCMatch&                       match   , 
        const LoKi::Keeper<LHCb::MCParticle>&      range   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCMatch&                        match   , 
        const LoKi::UniqueKeeper<LHCb::MCParticle>& range   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param begin begin iterator of any sequence of MC particles  
       *  @param end   end   iterator of any sequence of MC particles  
       */
      template <class MCPARTICLE>
      MCTruth 
      ( const LoKi::MCMatch&    match    , 
        MCPARTICLE              begin    , 
        MCPARTICLE              end      ) 
        : LoKi::Predicate<const LHCb::Particle*> () 
        , LoKi::Keeper<LHCb::MCParticle> ( first , last ) 
        , m_match     ( match       )
      {};
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param particle  pointer to MC particle object 
       */
      MCTruth 
      ( const LHCb::MCParticle*  particle ,
        const LoKi::MCMatch&     match    ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCTypes::MCRange&  range    ,
        const LoKi::MCMatch&            match   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCTypes::MCContainer& range   ,
        const LoKi::MCMatch&              match   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const std::vector<const LHCb::MCParticle*> range   ,
        const LoKi::MCMatch&                       match   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::Keeper<LHCb::MCParticle>&      range   ,
        const LoKi::MCMatch&                       match   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::UniqueKeeper<LHCb::MCParticle>& range   ,
        const LoKi::MCMatch&                        match   ) ;
      /** constructor 
       *  @param begin begin iterator of any sequence of MC particles  
       *  @param end   end   iterator of any sequence of MC particles  
       *  @param match MCMatch object (working horse)
       */
      template <class MCPARTICLE>
      MCTruth 
      ( MCPARTICLE              begin    , 
        MCPARTICLE              end      ,
        const LoKi::MCMatch&    match    )
        : LoKi::Predicate<const LHCb::Particle*> () 
        , LoKi::Keeper<LHCb::MCParticle> ( first , last ) 
        , m_match ( match )
      {};
      /// copy constructor 
      MCTruth( const MCTruth& right ) ;
      /// MANDATORY: virual destructor 
      virtual ~MCTruth() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual MCTruth* clone () const ; 
      /// MANDATORY: the only one essential method ("function")
      virtual result_type operator () ( argument ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    public:
      /** add additional particle to the 
       *  list of MCParticles to be matched 
       *  @param mcp MC particle
       */
      MCTruth& add ( const LHCb::MCParticle* mcp  ) 
      { addObject ( mcp ) ; return *this ; }
      /** add additional particles to the 
       *  list of MCParticles to be matched 
       *  @param range new range of MC particles 
       */
      MCTruth& add ( const LoKi::MCTypes::MCRange& range ) 
      { return add ( range.begin() , range.end() ) ; }
      /** add few additional particles to the list of 
       *  MCParticles  to be matched 
       *  @param first "begin" iterator of sequence 
       *  @param last   
       */
      template <class MCPARTICLE>
      MCTruth& add 
      ( MCPARTICLE first  , 
        MCPARTICLE last   ) 
      { addObjects ( first, last ) ; return *this ; } ;      
    public:
      /// implicit convertion to match object
      operator const MCMatch& () const { return m_match ; }
    private:
      // default constructor is disabled 
      MCTruth();
      // assignement is disabled 
      MCTruth& operator=( const MCTruth& ) ;
    private:
      // match-object 
      LoKi::MCMatch m_match     ; ///< match-object 
    } ;

  } ; // end of namespace Particles
  
  namespace MCParticles
  {    
    /** @class RCTruth PhsyMCParticles.h LoKi/PhysMCParticles.h
     *  
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2003-01-28
     */
    class RCTruth 
      : public LoKi::Predicate<const LHCb::MCParticle*> 
      , public LoKi::Keeper<LHCb::Particle>
    {
    public:      
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param particle  pointer to particle object 
       */
      RCTruth 
      ( const LoKi::MCMatch&  match    , 
        const LHCb::Particle* particle ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range "container" of particles 
       */
      RCTruth 
      ( const LoKi::MCMatch&      match , 
        const LoKi::Types::Range& range ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&          match , 
        const LHCb::Particle::Vector& range ) ;      
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&               match , 
        const LHCb::Particle::ConstVector& range ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&                match , 
        const LoKi::Keeper<LHCb::Particle>& range ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&                      match , 
        const LoKi::UniqueKeeper<LHCb::Particle>& range ) ;      
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param begin begin iterator of any sequence of particles  
       *  @param end   end   iterator of any sequence of particles  
       */
      template <class PARTICLE>
      RCTruth 
      ( const LoKi::MCMatch&  match    , 
        PARTICLE              begin    , 
        PARTICLE              end      ) 
        : LoKi::Predicate<const LHCb::MCParticle*>( ) 
        , LoKi::Keeper<LHCb::Particle> ( first , last )  
        , m_match ( match )
      {} ;
      /** constructor 
       *  @param particle  pointer to particle object 
       *  @param match MCMatch object (working horse)
       */
      RCTruth 
      ( const LHCb::Particle* particle ,
        const LoKi::MCMatch&  match    ) ;
      /** constructor 
       *  @param range "container" of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth 
      ( const LoKi::Types::Range& range ,
        const LoKi::MCMatch&      match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LHCb::Particle::Vector& range ,
        const LoKi::MCMatch&          match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LHCb::Particle::ConstVector& range ,
        const LoKi::MCMatch&               match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LoKi::Keeper<LHCb::Particle>& range ,
        const LoKi::MCMatch&                match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LoKi::UniqueKeeper<LHCb::Particle>& range ,
        const LoKi::MCMatch&                      match ) ;
      /** constructor 
       *  @param begin begin iterator of any sequence of particles  
       *  @param end   end   iterator of any sequence of particles  
       *  @param match MCMatch object (working horse)
       */
      template <class PARTICLE>
      RCTruth 
      ( PARTICLE              begin    , 
        PARTICLE              end      ,
        const LoKi::MCMatch&  match    ) 
        : LoKi::Predicate<const LHCb::MCParticle*>( ) 
        , LoKi::Keeper<LHCb::Particle> ( first , last )  
        , m_match ( match )
      {} ;
      /// copy constructor
      RCTruth( const RCTruth& right ) ;
      /// MANDATORY : virtual destructor destructor 
      virtual ~RCTruth();
      /// MANDATORY: clone method ("virtual constructor")
      virtual RCTruth* clone    () const ;
      /// MANDATORY: the only one essential method ("function")
      virtual result_type operator () ( argument ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    public:
      /** add additional particle to the 
       *  list of Particles to be matched 
       *  @param particle particle to be added 
       */
      RCTruth& add ( const LHCb::Particle* p ) 
      { addObject ( p )  ; return *this ; }
      /** add additional particles to the 
       *  list of MCParticles to be matched 
       *  @param range new range of MC particles 
       */
      RCTruth& add ( const LoKi::Types::Range& range ) 
      { return add ( range.begin() , range.end() ) ; }
      /** add few additional particles to the list of 
       *  Particles  to be matched 
       *  @param first "begin" iterator of sequence 
       *  @param last   
       */
      template <class PARTICLE>
      RCTruth& add ( PARTICLE first  , 
                     PARTICLE last   ) 
      { 
        addObjects ( first , last ) ;
        return *this ;
      };      
    private:
      // default constructor is disabled 
      RCTruth();
      // assignement is disabled 
      RCTruth& operator=( const RCTruth& ) ;
    private:
      // MC-match object
      LoKi::MCMatch  m_match ; ///< MC-match object
    };
    
  }; // end of namespace LoKi::Particles

} ; // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSMCPARTICLES_H
// ============================================================================
