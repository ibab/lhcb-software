// $Id: TrUpgrade.h,v 1.2 2008-11-13 22:11:03 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_TRUPGRADE_H 
#define LOKI_TRUPGRADE_H 1
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
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ============================================================
    /** @class TrUpgrade LoKi/TrUpgrade.h
     *  Simple functor which "upgrades" the set of tracks.
     *  Th eupgraded tracks are registeres in Hlt Data Service 
     *  @see ITracksFromnTrack
     *  @see LoKi::Hlt1::UpgradeTool
     *  @see LoKi::Hlt1::UpgradeConf
     *  @see LoKi::Cuts::TrUPGRADE 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-11
     */
    class TrUpgrade 
      : public LoKi::BasicFunctors<LHCb::Track*>::Pipe 
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
      /** constructor from all configuration parameters 
       *  @param output    the output selection name 
       *  @param trTool    the name of ITracksFromTrack tool 
       *  @param trType    the track type for upgrade 
       *  @param owner     owner ? 
       *  @param moveIDs   transfer IDs ?
       *  @param moveAncs  transfer anscendents ?
       *  @param moveInfo  transfer Extra Info ?
       *  @param ptOrder   order in pt ?
       */
      TrUpgrade 
      ( const std::string&        output   ,    //    output selection name/key 
        const std::string&        trTool   ,    //   ITrackFromTrack  tool name
        const std::string&        address  ,    //   TES location of the tracks 
        const LHCb::Track::Types  trType   ,    //                   track type 
        const bool                owner    ,    //                      owner ?
        const bool                moveIDs  ,    //               transfer IDs ? 
        const bool                moveAncs ,    //       transfer anscendents ? 
        const bool                moveInfo ,    //        transfer extra info ?
        const bool                ptOrder  ) ;  //                   order pt ?
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
      const stringKey&   output  () const { return m_sink.output  () ; }
      const std::string& selName () const { return m_sink.selName () ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// "sink":  register the selection for Hlt Data Service 
      LoKi::Hlt1::TrRegister m_sink ;                               // the sink
      // ======================================================================      
    };
    // ========================================================================
  } // end of namespace LoKi::Hlt1 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRUPGRADE_H
// ============================================================================
