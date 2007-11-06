// $Id: PhysMCParticles.h,v 1.8 2007-11-06 10:53:15 cattanem Exp $
// ============================================================================
#ifndef LOKI_PHYSMCPARTICLES_H 
#define LOKI_PHYSMCPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Keeper.h"
#include "LoKi/Interface.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
// LoKiMC  
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/GenTypes.h"
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/PhysMCTypes.h"
#include "LoKi/MCMatch.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/HepMC2MC.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-20 
 */
// ============================================================================

namespace LoKi 
{
  /// forward declarations 
  class MCMatch ;
  // ==========================================================================
  namespace PhysMCParticles 
  { 
    // ========================================================================
    /** @class MCTruth PhysMCParticles.h LoKi/PhysMCParticles.h
     *  
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some LHCb::MCParticle
     *
     *  @code 
     * 
     *  // some sequence of mc-particle
     *  SEQUENCE mcps = ...  ;
     *  // get mc-matcher 
     *  MCMatch   mc =  ... ;
     *  // create the function 
     *  Cut cut = MCTRUTH( mc , mcps.begin() , mcps.end() ) ;
     *  
     *
     *  const LHCb::Particle* B = ... ;
     *  // use the predicate!
     * 
     *  const bool good = cut( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::MCTRUTH
     *  @see LoKi::MCMatchObj
     *  @see LoKi::MCMatch
     *  @see LHCb::Particle
     *  @see LHCb::MCParticle
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
      ( const LoKi::MCMatch&                       match   , 
        const LHCb::MCParticle::Vector&            range   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LoKi::MCMatch&                       match   , 
        const LHCb::MCParticle::ConstVector&       range   ) ;
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
        , LoKi::Keeper<LHCb::MCParticle> ( begin , end ) 
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
      ( const LHCb::MCParticle::Vector&   range   ,
        const LoKi::MCMatch&              match   ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range range of MC particles 
       */
      MCTruth
      ( const LHCb::MCParticle::ConstVector&       range   ,
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
        , LoKi::Keeper<LHCb::MCParticle> ( begin , end ) 
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
    // ========================================================================    
    /** @class GenMCTruth PhysMCParticles.h LoKi/PhysMCParticles.h
     *
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some HepMC::GenParticle
     *
     *  @code 
     * 
     *  // some sequence of HepMC-particles
     *  SEQUENCE hepmcps = ...  ;
     *  // get RC<-->MC-matcher 
     *  MCMatch   mc =  ... ;
     *  // get HepMC<-->MC matcher
     *  const LHCb::HepMC2MC* table = ... ;
     *  // create the function 
     *  Cut cut = GMCTRUTH( table , mc , hepmcps.begin() , hepmcps.end() ) ;
     *  
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the predicate!
     * 
     *  const bool good = cut( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::GMCTRUTH 
     *  @see HepMC::GenParticle
     *  @see LHCb::Particle
     *  @see LoKi::MCMathch
     *  @see LoKi::MCMathchObj
     *  @see LHCb::HepMC2MC 
     *  @author Vanya BELYAEV ibelyaev@physics.sye.edu
     *  @date   2003-01-28
     */
    class GenMCTruth 
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// constructor from HepMC::GenVertex
      GenMCTruth 
      ( const HepMC::GenVertex*             particle   , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// constructor from HepMC::GenParticle
      GenMCTruth 
      ( const HepMC::GenParticle*           particle   , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// constructor from container of HepMC::GenParticles
      GenMCTruth 
      ( const LoKi::GenTypes::GenContainer& particles  , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// constructor from range of HepMC::GenParticles
      GenMCTruth 
      ( const LoKi::Types::GRange&          particles  , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// templated constructor 
      template <class GENERATOR>
      GenMCTruth 
      ( GENERATOR                 first    ,
        GENERATOR                 last     ,
        const LHCb::HepMC2MC*     table    , 
        const LoKi::MCMatch&      match    ) 
        : LoKi::Predicate<const LHCb::Particle*> () 
        , m_table ( table ) 
        , m_match ( match ) 
        , m_hepmc () 
        , m_mc    () 
      { add ( first , last ) ; } ;
      /// MANDATORY: copy constructor 
      GenMCTruth ( const GenMCTruth& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~GenMCTruth() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual GenMCTruth* clone () const ; 
      /// MANDATORY: the only one essential method ("function")
      virtual result_type operator () ( argument ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    public:
      /// rebuild the internal storages 
      StatusCode rebuild() ;
      /// add HepMC::GenParticle
      GenMCTruth& add ( const HepMC::GenParticle*           p ) ;
      /// add HepMC::GenVertex
      GenMCTruth& add ( const HepMC::GenVertex*             p ) ;
      /// add LoKi::GenTypes::GenContainer
      GenMCTruth& add ( const LoKi::GenTypes::GenContainer& p ) ;
      /// add LoKi::Types::GRange 
      GenMCTruth& add ( const LoKi::Types::GRange&          p ) ;
      /// add a sequence of something 
      template <class OBJECT>
      GenMCTruth& add ( OBJECT first , 
                        OBJECT last  ) 
      {
        _add ( first , last ) ;
        rebuild() ;
        return *this ;
      } ;
    public:
      /// conversion operator to relation table:
      operator const LoKi::Interface<LHCb::HepMC2MC>&        () const 
      { return m_table ; }
      /// conversion operator to the HepMC-storage
      operator const LoKi::UniqueKeeper<HepMC::GenParticle>& () const 
      { return m_hepmc ; }
      // conversion operator to LoKi::MCMatch object 
      operator const LoKi::MCMatch&                          () const 
      { return m_match ; }
    protected:
      /// add HepMC::GenParticle
      void _add ( const HepMC::GenParticle*              p ) ;
      /// add HepMC::GenVertex
      void _add ( const HepMC::GenVertex*                p ) ;
      /// add LoKi::GenTypes::GenContainer
      void _add ( const LoKi::GenTypes::GenContainer&    p ) ;
      /// add LoKi::Types::GRange 
      void _add ( const LoKi::Types::GRange&             p ) ;
      /// add a sequence of something 
      template <class OBJECT>
      void _add ( OBJECT first , OBJECT last  ) 
      { for ( ; first != last ; ++first ) { _add ( *first ) ; } }
    private:
      // the first relation table HepMC -> MC  
      LoKi::Interface<LHCb::HepMC2MC>        m_table ;
      // the helper object for MC->RC relations 
      LoKi::MCMatch                          m_match ;
      // local storage of generator particles 
      LoKi::UniqueKeeper<HepMC::GenParticle> m_hepmc ;
      // local storage of MC-particles 
      LoKi::UniqueKeeper<LHCb::MCParticle>   m_mc    ;
    } ;
    // ========================================================================
    /** @class RCTruth PhysMCParticles.h LoKi/PhysMCParticles.h
     *  
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some LHCb::MCParticle
     *
     *  @see LoKi::MCMatchObj
     *  @see LoKi::MCMatch
     *  @see LHCb::Particle
     *  @see LHCb::MCParticle
     *  @see LoKi::Cuts::RCTRUTH
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
        , LoKi::Keeper<LHCb::Particle> ( begin , end )  
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
        , LoKi::Keeper<LHCb::Particle> ( begin , end )  
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
       *  @param p  particle to be added 
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
    // ========================================================================
    namespace Particles 
    {
      /// import the type into proepr namespace 
      typedef LoKi::PhysMCParticles::MCTruth    MCTruth    ;      
      /// import the type into proepr namespace 
      typedef LoKi::PhysMCParticles::GenMCTruth GenMCTruth ;
    }
    // ========================================================================
    namespace MCParticles 
    {
      /// import the type into proepr namespace 
      typedef LoKi::PhysMCParticles::RCTruth    RCTruth    ;
    }
    
  } // end of namespace LoKi::PhysMCParticles
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSMCPARTICLES_H
// ============================================================================
