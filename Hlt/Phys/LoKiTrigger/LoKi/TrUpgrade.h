// $Id$
// ============================================================================
#ifndef LOKI_TRUPGRADE_H 
#define LOKI_TRUPGRADE_H 1
// ============================================================================
// $URL$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
#include "LoKi/UpgradeTool.h"
#include "LoKi/TriggerTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ============================================================
    /** @class Upgrade LoKi/TrUpgrade.h
     *  Simple functor which "upgrades" the set of tracks.
     *  The upgraded tracks are registeres in Hlt Data Service 
     *  @see ITracksFromnTrack
     *  @see LoKi::Hlt1::UpgradeTool
     *  @see LoKi::Hlt1::UpgradeConf
     *  @see LoKi::Cuts::TrUPGRADE 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-11
     */
    class Upgrade 
      : public LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe 
      , public LoKi::Hlt1::UpgradeTool 
    {
    public:
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      Upgrade 
      ( const std::string&             output  ,   // output selection name/key 
        const LoKi::Hlt1::UpgradeConf& config  ) ; //             configuration 
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      Upgrade 
      ( const std::string&             output  ,   // output selection name/key 
        const LoKi::Hlt1::UpgradeTool& config  ) ; //             configuration 
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~Upgrade() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Upgrade* clone () const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Upgrade() ;                        // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// output selection 
      const Gaudi::StringKey&   output  () const { return m_sink.output  () ; }
      const std::string& selName () const { return m_sink.selName () ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// "sink":  register the selection for Hlt Data Service 
      LoKi::Hlt1::Sink m_sink ;                                     // the sink
      // ======================================================================      
    };
    // ============================================================
    /** @class TrUpgrade LoKi/TrUpgrade.h
     *  Simple functor which "upgrades" the set of tracks.
     *  The upgraded tracks are registeres in Hlt Data Service 
     *  @see ITracksFromnTrack
     *  @see LoKi::Hlt1::UpgradeTool
     *  @see LoKi::Hlt1::UpgradeConf
     *  @see LoKi::Cuts::TrUPGRADE 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-11
     */
    class TrUpgrade 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Pipe 
      , public LoKi::Hlt1::UpgradeTool 
    {
    public:
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      TrUpgrade 
      ( const std::string&             output  ,   // output selection name/key 
        const LoKi::Hlt1::UpgradeConf& config  ) ; //             configuration 
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      TrUpgrade 
      ( const std::string&             output  ,   // output selection name/key 
        const LoKi::Hlt1::UpgradeTool& config  ) ; //             configuration 
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~TrUpgrade() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrUpgrade* clone () const { return new TrUpgrade(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrUpgrade() ;                     // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// output selection 
      const Gaudi::StringKey&   output  () const { return m_sink.output  () ; }
      const std::string& selName () const { return m_sink.selName () ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// "sink":  register the selection for Hlt Data Service 
      LoKi::Hlt1::TrRegister m_sink ;                               // the sink
      // ======================================================================      
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TRUPGRADE_H
// ============================================================================
