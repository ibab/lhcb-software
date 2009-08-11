// $Id: MCMatcher.h,v 1.1 2009-08-11 18:26:59 ibelyaev Exp $
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
    /** @class MCMatcher LoKi/MCMatcher.h
     *  
     *  The special version of class LoKi::PhysMCParticles::MCTruth 
     *  to be used withing CombineParticles/DecayTree frameworks
     *
     *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-08-11
     */
    class MCMatcher  : public LoKi::PhysMCParticles::MCTruth
    {
    public:
      // ======================================================================
      /// the actual type for TES-locations for relation tables 
      typedef std::vector<std::string>  Locations ; // TES-locations of tables
      // ======================================================================
      typedef Decays::IMCDecay::Finder                                 Finder ;
      typedef Finder::iTree                                             iTree ;
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
      /** constructor from the decay and MC-truth matching tables
       *  @param decay the decay 
       *  @param locations the location of MCtruth relation tables 
       */
      MCMatcher 
      ( const iTree&      decay           , 
        const Locations&  locations       ) ;
      /** constructor from the decay and MC-truth matching tables
       *  @param decay the decay 
       */
      MCMatcher 
      ( const iTree&       decay          , 
        const std::string& location       ) ;
      /** constructor from the decay and MC-truth matching tables
       *  @param decay the decay 
       */
      MCMatcher 
      ( const iTree&       decay          , 
        const std::string& location1      ,
        const std::string& location2      ) ;
      /** constructor from the decay and MC-truth matching tyep for 
       *  protoparticles 
       */
      MCMatcher 
      ( const iTree&      decay           , 
        const ProtoPMatch matchType = All ) ;
      /** constructor from the decay and MC-truth matching tables
       *  @param decay the decay 
       *  @param locations the location of MCtruth relation tables 
       */
      MCMatcher 
      ( const std::string& decay           , 
        const Locations&   locations       ) ;
      /** constructor from the decay and MC-truth matching tables
       *  @param decay the decay 
       */
      MCMatcher 
      ( const std::string& decay           , 
        const std::string& location        ) ;
      /** constructor from the decay and MC-truth matching tables
       *  @param decay the decay 
       */
      MCMatcher 
      ( const std::string& decay           , 
        const std::string& location1       , 
        const std::string& location2       ) ;
      /** constructor from the decay and MC-truth matching tyep for 
       *  protoparticles 
       */
      MCMatcher 
      ( const std::string& decay           , 
        const ProtoPMatch  matchType = All ) ;
      /// MANDATORY: virtual destructor
      virtual ~MCMatcher () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual MCMatcher* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;  
      // ======================================================================
    private:
      // ======================================================================
      /// check the decay descriptor
      void checkFinder () ;                       // check the decay descriptor
      /// get the decay descriptor
      void getFinder ( const std::string& decay ) ; // get the decay descriptor
      /// loda the data 
      StatusCode load() const ;                                // load the data 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MCMatcher() ;                      // the default constructor is disabled
      /// no copy constructor 
      MCMatcher ( const MCMatcher& right ) ;             // no copy constructor
      /// no assigneement 
      MCMatcher& operator=( const MCMatcher& right ) ;        // no assignement 
      // ======================================================================
    private:
      // ======================================================================
      /// the decay finder
      Finder    m_finder      ;                             // the decay finder 
      /// TES locations of relation tables 
      Locations m_locations ;               // TES locations of relation tables 
      /// the algorithm
      mutable GaudiAlgorithm* m_alg ;                          // the algorithm
      // ======================================================================      
    };
    // ========================================================================
  } //                                   end of namespace LoKi::PhysMCParticles
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /// import the symbol into proper namespace 
    typedef LoKi::PhysMCParticles::MCMatcher                        MCMatcher ;
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCMATCHER_H
// ============================================================================
