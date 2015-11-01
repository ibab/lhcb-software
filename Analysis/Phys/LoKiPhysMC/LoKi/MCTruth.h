// $Id$
// ============================================================================
#ifndef LOKI_MCTRUTH_H 
#define LOKI_MCTRUTH_H 1
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
namespace LoKi 
{
  // =========================================================================
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
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
      , public LoKi::Keeper<LHCb::MCParticle>
    {
    public:
      // ======================================================================
      /// the actual type of Keeper 
      typedef LoKi::Keeper<LHCb::MCParticle>                           Keeper ;
      // ======================================================================
    public:      
      // ======================================================================
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
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
        , LoKi::Keeper<LHCb::MCParticle> ( begin , end ) 
        , m_match     ( match       )
      {}
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
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
        , LoKi::Keeper<LHCb::MCParticle> ( begin , end ) 
        , m_match ( match )
      {}
      /// copy constructor 
      MCTruth( const MCTruth& right ) ;
      /// MANDATORY: virual destructor 
      virtual ~MCTruth() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual MCTruth* clone () const ; 
      /// MANDATORY: the only one essential method ("function")
      virtual result_type operator () ( argument p ) const { return match ( p ) ; }
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
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
      { addObjects ( first, last ) ; return *this ; } 
      // ======================================================================
    public:
      // ======================================================================
      /// implicit convertion to match object
      operator const MCMatch& () const { return match () ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// explicit  convertion to match object
      const MCMatch& match ()   const { return m_match   ; }
      // ======================================================================
      bool match ( argument p ) const ;
      // ======================================================================
      /// default constructor is protected
      MCTruth() ;                           // default constructor is protected
      /// set new mc match object
      void setMatch ( const LoKi::MCMatch& m ) const ;
      /// get the keeper 
      Keeper& storage () const ;
      // ======================================================================
    private:
      // ======================================================================
      // assignement is disabled 
      MCTruth& operator=( const MCTruth& ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// match-object 
      LoKi::MCMatch m_match     ;                            // MC-match-object 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                   end of namespace LOKi::PhysMCParticles 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCTRUTH_H
// ============================================================================
