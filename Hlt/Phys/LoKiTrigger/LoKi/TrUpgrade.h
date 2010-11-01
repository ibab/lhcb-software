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
    /** @class Upgrade
     *  Simple functor which "upgrades" the candidates.
     *  The upgraded candidates are registered in Hlt Data Service 
     *  Both track & mutlitrack candidated are upgraded 
     *  @see ITracksFromTrack
     *  @see LoKi::Hlt1::UpgradeTool
     *  @see LoKi::Hlt1::UpgradeConf
     *  @see LoKi::Cuts::TC_UPGRADE_ALL
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
    protected:
      // ======================================================================
      /// "sink":  register the selection for Hlt Data Service 
      LoKi::Hlt1::Sink m_sink ;                                     // the sink
      // ======================================================================      
    };
    // ============================================================
    /** @class UpgradeTracks
     *  Simple functor which "upgrades" the candidates.
     *  The upgraded candidates are registered in Hlt Data Service 
     *  Only "track" candidates are upgraded 
     *  @see ITracksFromTrack
     *  @see LoKi::Hlt1::UpgradeTool
     *  @see LoKi::Hlt1::UpgradeConf
     *  @see LoKi::Cuts::TC_UPGRADE_TR
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-26
     */
    class UpgradeTracks : public Upgrade 
    {
    public:
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      UpgradeTracks
      ( const std::string&             output  ,   // output selection name/key 
        const LoKi::Hlt1::UpgradeConf& config  ) ; //             configuration 
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      UpgradeTracks 
      ( const std::string&             output  ,   // output selection name/key 
        const LoKi::Hlt1::UpgradeTool& config  ) ; //             configuration 
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~UpgradeTracks () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  UpgradeTracks* clone () const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      UpgradeTracks() ;                  // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
    /** @class UpgradeMultiTracks
     *  Simple functor which "upgrades" the candidates.
     *  The upgraded candidates are registered in Hlt Data Service 
     *  Only "track" candidates are upgraded 
     *  @see ITracksFromTrack
     *  @see LoKi::Hlt1::UpgradeTool
     *  @see LoKi::Hlt1::UpgradeConf
     *  @see LoKi::Cuts::TC_UPGRADE_MT
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-26
     */
    class UpgradeMultiTracks : public Upgrade 
    {
    public:
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       *  @param index   teh tarck index to upgrade, -1: upgrade all 
       */
      UpgradeMultiTracks
      ( const std::string&             output  ,   // output selection name/key 
        const int                      index   ,   //   track index for upgrade 
        const LoKi::Hlt1::UpgradeConf& config  ) ; //             configuration 
      // ======================================================================
      /** constructor from all configuration parameters 
       *  @param output  the output selection name 
       *  @param config  the tool configuration 
       */
      UpgradeMultiTracks 
      ( const std::string&             output  ,   // output selection name/key 
        const int                      index   ,   //   track index for upgrade 
        const LoKi::Hlt1::UpgradeTool& config  ) ; //             configuration 
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~UpgradeMultiTracks () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  UpgradeMultiTracks* clone () const ; 
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      UpgradeMultiTracks () ;            // the default constructor is disabled 
      // ======================================================================
    private:      
      // ======================================================================
      /// track index for upgrade: -1 : all tracks 
      int m_index ;                 // track index for upgrade: -1 : all tracks 
      // ======================================================================
    };
//     // ========================================================================
//     // Obsolete??
//     // ========================================================================
//     /** @class TrUpgrade LoKi/TrUpgrade.h
//      *  Simple functor which "upgrades" the set of tracks.
//      *  The upgraded tracks are registeres in Hlt Data Service 
//      *  @see ITracksFromTrack
//      *  @see LoKi::Hlt1::UpgradeTool
//      *  @see LoKi::Hlt1::UpgradeConf
//      *  @see LoKi::Cuts::TrUPGRADE 
//      *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
//      *  @date   2008-11-11
//      */
//     class TrUpgrade 
//       : public LoKi::BasicFunctors<const LHCb::Track*>::Pipe 
//       , public LoKi::Hlt1::UpgradeTool 
//     {
//     public:
//       // ======================================================================
//       /** constructor from all configuration parameters 
//        *  @param output  the output selection name 
//        *  @param config  the tool configuration 
//        */
//       TrUpgrade 
//       ( const std::string&             output  ,   // output selection name/key 
//         const LoKi::Hlt1::UpgradeConf& config  ) ; //             configuration 
//       // ======================================================================
//       /** constructor from all configuration parameters 
//        *  @param output  the output selection name 
//        *  @param config  the tool configuration 
//        */
//       TrUpgrade 
//       ( const std::string&             output  ,   // output selection name/key 
//         const LoKi::Hlt1::UpgradeTool& config  ) ; //             configuration 
//       // ======================================================================
//       /// MANDATORY: virtual destructor 
//       virtual ~TrUpgrade() {}
//       /// MANDATORY: clone method ("virtual constructor")
//       virtual  TrUpgrade* clone () const { return new TrUpgrade(*this) ; }
//       /// MANDATORY: the only one essential method 
//       virtual result_type operator() ( argument a ) const ;
//       /// OPTIONAL: nice printout 
//       virtual std::ostream& fillStream ( std::ostream& s ) const ;
//       // ======================================================================
//     private:
//       // ======================================================================
//       /// the default constructor is disabled 
//       TrUpgrade() ;                     // the default constructor is disabled 
//       // ======================================================================
//     public:
//       // ======================================================================
//       /// output selection 
//       const Gaudi::StringKey&   output  () const { return m_sink.output  () ; }
//       const std::string& selName () const { return m_sink.selName () ; }
//       // ======================================================================      
//     private:
//       // ======================================================================
//       /// "sink":  register the selection for Hlt Data Service 
//       LoKi::Hlt1::TrRegister m_sink ;                               // the sink
//       // ======================================================================      
//     };
//     // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef TC_UPGRADE_ALL 
     *  functor to upgrade *ALL* tarcks& multitracks 
     *
     *  @see LoKi::Hlt1::Upgrade 
     *  @see LoKi::Hlt1::UpgradeTracks
     *  @see LoKi::Hlt1::UpgradeMultiTracks
     *  @see LoKi::Cuts::TC_UPGRADE_TR 
     *  @see LoKi::Cuts::TC_UPGRADE_MT 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-26
     */
    typedef LoKi::Hlt1::Upgrade                                TC_UPGRADE_ALL ;
    // ========================================================================
    /** @typedef TC_UPGRADE_TR
     *  functor to upgrade single tracks
     *  @see LoKi::Hlt1::UpgradeTracks
     *  @see LoKi::Hlt1::Upgrade 
     *  @see LoKi::Hlt1::UpgradeMultiTracks
     *  @see LoKi::Cuts::TC_UPGRADE_ALL
     *  @see LoKi::Cuts::TC_UPGRADE_MT 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-26
     */
    typedef LoKi::Hlt1::UpgradeTracks                           TC_UPGRADE_TR ;
    // ========================================================================
    /** @typedef TC_UPGRADE_MT
     *  functor to upgrade multitracks tracks
     *  @see LoKi::Hlt1::UpgradeMultiTracks
     *  @see LoKi::Hlt1::UpgradeTracks
     *  @see LoKi::Hlt1::Upgrade 
     *  @see LoKi::Cuts::TC_UPGRADE_ALL
     *  @see LoKi::Cuts::TC_UPGRADE_TR
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-26
     */
    typedef LoKi::Hlt1::UpgradeMultiTracks                      TC_UPGRADE_MT ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TRUPGRADE_H
// ============================================================================
