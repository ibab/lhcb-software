// $Id: MCMatcher.h,v 1.3 2009-09-03 13:50:41 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCMATCHER_H 
#define LOKI_MCMATCHER_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiMC
// ============================================================================
#include "LoKi/IMCDecay.h"
// ============================================================================
// LoKiPhysMC
// ============================================================================
#include "LoKi/MCTruth.h"
// ============================================================================
// forward declaration
// ============================================================================
class GaudiAlgorithm ;
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /// forward declarations 
  class MCMatch ;                                       // forward declarations
  // ==========================================================================
  namespace PhysMCParticles 
  {
    // ========================================================================
    /** @class MCMatcherBase LoKi/MCMatcher.h
     *  
     *  The special version of class LoKi::PhysMCParticles::MCTruth 
     *  to be used withing CombineParticles/DecayTree frameworks
     *
     *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-08-11
     */
    class MCMatcherBase  : public LoKi::PhysMCParticles::MCTruth
    {
    public:
      // ======================================================================
      /// the actual type for TES-locations for relation tables 
      typedef std::vector<std::string>  Locations ; // TES-locations of tables
      // ======================================================================
      typedef Decays::iNode                                             iNode ;
      // ======================================================================
      enum ProtoPMatch {
        All              ,  // Neutral , Charged + Upstream  
        Charged          ,  //           Charged + Upstream 
        ChargedLong      ,  //           Charged 
        Neutral      
      } ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the list of TES-locations
      MCMatcherBase ( const Locations&   locations  ) ;
      /// constructor from the list of TES-location
      MCMatcherBase ( const std::string& location   ) ;
      /// constructor from the list of TES-locations
      MCMatcherBase ( const std::string& location1  , 
                      const std::string& location2  ) ;
      /// constructor from the list of TES-locations
      MCMatcherBase ( const std::string& location1  , 
                      const std::string& location2  , 
                      const std::string& location3  ) ;
      /// constructor from protopartcile  matching flag
      MCMatcherBase ( const ProtoPMatch& protoMatch ) ;                
      // ======================================================================      
      /// virtual destructor
      virtual ~MCMatcherBase() ;
      // ======================================================================      
    protected:
      // ======================================================================
      /// load the data 
      StatusCode load() const ;                                // load the data 
      /// get MC-particles 
      virtual StatusCode getMCParticles 
      ( const LHCb::MCParticle::Container& cnt ) const = 0 ;
      /// clear all storages 
      void clearAll () const ;
      /// get all locations 
      const Locations& locations() const { return m_locations ; }
      /// get the algorithm
      const GaudiAlgorithm* alg() const { return m_alg ; }
      /// set the algorithm
      void setAlg ( const GaudiAlgorithm* a ) const { m_alg = a ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MCMatcherBase () ;                 // the default constructor is disabled
      /// no copy constructor 
      MCMatcherBase ( const MCMatcherBase& right ) ;     // no copy constructor
      /// no assigneement 
      MCMatcherBase& operator=( const MCMatcherBase& right ) ;// no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// TES locations of relation tables 
      Locations m_locations ;               // TES locations of relation tables 
      /// the algorithm
      mutable const GaudiAlgorithm* m_alg ;                    // the algorithm
      // ======================================================================      
    };
    // ========================================================================
    /** @class MCSelMatch 
     */
    class MCSelMatch : public MCMatcherBase 
    {
    public:
      // ======================================================================
      /// the actual type of MC-selector 
      typedef LoKi::Types::MCCuts                                      MCCuts ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from MC-selector and TES-locations 
       *  @param selector Monte Carlo selector
       *  @param locations TES-locations of relation tables 
       */
      MCSelMatch
      ( const MCCuts&      cuts      , 
        const Locations&   locations ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location TES-location of relation tables 
       */
      MCSelMatch
      ( const MCCuts&      cuts      , 
        const std::string& location  ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location1 TES-location of relation tables 
       *  @param location2 TES-location of relation tables 
       */
      MCSelMatch
      ( const MCCuts&      cuts      , 
        const std::string& location1 ,
        const std::string& location2 ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location1 TES-location of relation tables 
       *  @param location2 TES-location of relation tables 
       *  @param location3 TES-location of relation tables 
       */
      MCSelMatch
      ( const MCCuts&      cuts      , 
        const std::string& location1 ,
        const std::string& location2 ,
        const std::string& location3 ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param protoMatch the key for ProtoParticles match
       */
      MCSelMatch
      ( const MCCuts&      cuts       , 
        const ProtoPMatch& protoMatch ) ;
      // ======================================================================
      /// MANDATORY: virtual destrcutor
      virtual ~MCSelMatch () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCSelMatch* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream&  ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// get MC-particles 
      virtual StatusCode getMCParticles 
      ( const LHCb::MCParticle::Container& cnt ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MCSelMatch () ;                        // default constructor is disabled 
      /// no copy constructor 
      MCSelMatch ( const MCSelMatch& right ) ;           // no copy constructor
      /// no assigneement 
      MCSelMatch& operator=( const MCSelMatch& right ) ;      // no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual selector of Monte Carlo particles 
      LoKi::Types::MCCut m_cuts ;      // the selector of Monte Carlo particles
      // ======================================================================
    } ;
    // ========================================================================
    class MCTreeMatch : public MCMatcherBase 
    {
    public:
      // ======================================================================
      // the actual type of decay finder 
      typedef Decays::IMCDecay::Finder                                 Finder ;
      // the actual type of decay descriptor 
      typedef Finder::iTree                                             iTree ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param locations TES-locations of relation tables 
       */
      MCTreeMatch
      ( const iTree&       decay     , 
        const Locations&   locations ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location TES-location of relation tables 
       */
      MCTreeMatch
      ( const iTree&       decay     , 
        const std::string& location  ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location1 TES-location of relation tables 
       *  @param location2 TES-location of relation tables 
       */
      MCTreeMatch
      ( const iTree&       decay     , 
        const std::string& location1 ,
        const std::string& location2 ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location1 TES-location of relation tables 
       *  @param location2 TES-location of relation tables 
       *  @param location3 TES-location of relation tables 
       */
      MCTreeMatch
      ( const iTree&       decay     , 
        const std::string& location1 ,
        const std::string& location2 ,
        const std::string& location3 ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param protoMatch the key for ProtoParticles match
       */
      MCTreeMatch
      ( const iTree&       decay      , 
        const ProtoPMatch& protoMatch ) ;
      // ======================================================================
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param locations TES-locations of relation tables 
       */
      MCTreeMatch
      ( const std::string& decay     , 
        const Locations&   locations ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location TES-location of relation tables 
       */
      MCTreeMatch
      ( const std::string& decay     , 
        const std::string& location  ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location1 TES-location of relation tables 
       *  @param location2 TES-location of relation tables 
       */
      MCTreeMatch
      ( const std::string& decay     , 
        const std::string& location1 ,
        const std::string& location2 ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param location1 TES-location of relation tables 
       *  @param location2 TES-location of relation tables 
       *  @param location3 TES-location of relation tables 
       */
      MCTreeMatch
      ( const std::string& decay     , 
        const std::string& location1 ,
        const std::string& location2 ,
        const std::string& location3 ) ;
      /** constructor from decay descriptor and TES-locations of tables 
       *  @param decay the decay desriptor 
       *  @param protoMatch the key for ProtoParticles match
       */
      MCTreeMatch
      ( const std::string& decay      , 
        const ProtoPMatch& protoMatch ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~MCTreeMatch() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCTreeMatch* clone() const ;
      ///
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream&  ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// get MC-particles 
      virtual StatusCode getMCParticles 
      ( const LHCb::MCParticle::Container& cnt ) const ;
      // ======================================================================
      /// check the validness of finder 
      void checkFinder () ;
      /// get the decay finder 
      void getFinder   ( const std::string& decay ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MCTreeMatch () ;                       // default constructor is disabled 
      /// no copy constructor 
      MCTreeMatch ( const MCTreeMatch& right ) ;         // no copy constructor
      /// no assigneement 
      MCTreeMatch& operator=( const MCTreeMatch& right ) ;    // no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual decay finder 
      Finder m_finder ;                              // the actual decay finder
      // ======================================================================
    } ;
    // ========================================================================
    class MCNodeMatch : public MCMatcherBase 
    {
    public:
      // ======================================================================
      // the actual type of decay node 
      typedef Decays::iNode                                             iNode ;
      // the actual type of decay node 
      typedef Decays::Node                                               Node ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from node descriptor and TES-locations of tables 
      MCNodeMatch
      ( const iNode&       decay     , 
        const Locations&   locations ) ;
      /// constructor from node descriptor and TES-locations of tables 
      MCNodeMatch
      ( const iNode&       node      , 
        const std::string& location  ) ;
      /// constructor from node descriptor and TES-locations of tables 
      MCNodeMatch
      ( const iNode&       decay     , 
        const std::string& location1 ,
        const std::string& location2 ) ;
      /// constructor from node descriptor and TES-locations of tables 
      MCNodeMatch
      ( const iNode&       node      , 
        const std::string& location1 ,
        const std::string& location2 ,
        const std::string& location3 ) ;
      /// constructor from decay descriptor and TES-locations of tables 
      MCNodeMatch
      ( const iNode&       node       , 
        const ProtoPMatch& protoMatch ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~MCNodeMatch() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MCNodeMatch* clone() const ;
      ///
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream&  ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// get MC-particles 
      virtual StatusCode getMCParticles 
      ( const LHCb::MCParticle::Container& cnt ) const ;
      // ======================================================================
      /// check the validness of node 
      void checkNode () ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      MCNodeMatch () ;                       // default constructor is disabled 
      /// no copy constructor 
      MCNodeMatch ( const MCNodeMatch& right ) ;         // no copy constructor
      /// no assigneement 
      MCNodeMatch& operator=( const MCNodeMatch& right ) ;    // no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual decay node
      Node m_node ;                                    // the actual decay node
      // ======================================================================
    } ;
    // ========================================================================
  } //                                   end of namespace LoKi::PhysMCParticles
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /// import the symbol into proper namespace 
    typedef LoKi::PhysMCParticles::MCSelMatch                     MCSelMatch  ;
    typedef LoKi::PhysMCParticles::MCTreeMatch                    MCTreeMatch ;
    typedef LoKi::PhysMCParticles::MCNodeMatch                    MCNodeMatch ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef MCSELMATCH
     *  helper symbol (for CombineParticles/FilterDesktop framework) 
     *  to perform MC-truth match
     *
     *  @code
     *  
     *    Cut cut = MCSELMATCH ( "pi+" == MCABSID ) ;
     *
     *  @endcode 
     *
     *  @see CombineParticles 
     *  @see FilterDesktop 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-08-12
     */
    typedef LoKi::PhysMCParticles::MCSelMatch                      MCSELMATCH ;
    // ========================================================================
    /** @typedef MCTREEMATCH
     *  helper symbol (for CombineParticles/FilterDesktop framework) 
     *  to perform MC-truth match
     *
     *  @code
     *  
     *    Cut cut = MCTREEMATCH ( "B0 -> pi+ pi-") ;
     *
     *  @endcode 
     *
     *  @see CombineParticles 
     *  @see FilterDesktop 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-08-12
     */
    typedef LoKi::PhysMCParticles::MCTreeMatch                    MCTREEMATCH ;
    // ========================================================================
    /** @typedef MCNODEMATCH
     *  helper symbol (for CombineParticles/FilterDesktop framework) 
     *  to perform MC-truth match
     *
     *  @code
     *  
     *    Cut cut = MCNODEMATCH ( StableCharged ) ;
     *
     *  @endcode 
     *
     *  @see CombineParticles 
     *  @see FilterDesktop 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-08-12
     */
    typedef LoKi::PhysMCParticles::MCNodeMatch                    MCNODEMATCH ;
    // ========================================================================
    /** @typedef MCMATCH
     *  helper symbol (for CombineParticles/FilterDesktop framework) 
     *  to perform MC-truth match
     *
     *  @code
     *  
     *    Cut cut = MCMATCH ( "B0 -> pi+ pi-") ;
     *
     *  @endcode 
     *
     *  @see CombineParticles 
     *  @see FilterDesktop 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-08-12
     */
    typedef LoKi::PhysMCParticles::MCTreeMatch                        MCMATCH ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCMATCHER_H
// ============================================================================
