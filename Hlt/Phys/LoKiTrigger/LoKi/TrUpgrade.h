// $Id: TrUpgrade.h,v 1.1 2008-11-13 13:14:41 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_TRUPGRADE_H 
#define LOKI_TRUPGRADE_H 1
// ============================================================================
// Include files
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/ITrackUpgrade.h"
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITracksFromTrack.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/Hlt1.h"
#include "LoKi/UpgradeConf.h"
// ============================================================================
class GaudiAlgorithm ;
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
     *  @see ITrackUpgrade
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-11
     */
    class TrUpgrade :  public LoKi::BasicFunctors<LHCb::Track*>::Pipe 
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
      /// access the tool 
      const LoKi::Interface<ITracksFromTrack>& upgrade() const 
      { return m_upgrade ; }
      // ======================================================================
    public:
      // ======================================================================
      /// ITracksFromTrack tool name 
      const std::string& trTool   () const { return m_config.trTool   () ; }
      /// TES  location of upgraded tracks 
      const std::string& address  () const { return m_config.address  () ; }
      /// track type 
      LHCb::Track::Types trType   () const { return m_config.trType   () ; }
      /// owner ? 
      bool               owner    () const { return m_config.owner    () ; }
      /// transfer IDs ?
      bool               moveIDs  () const { return m_config.moveIDs  () ; }
      /// transfer Ancestors 
      bool               moveAncs () const { return m_config.moveAncs () ; }
      // transfer extra info? 
      bool               moveInfo () const { return m_config.moveInfo () ; }
      // pt-ordering ?
      bool               ptOrder  () const { return m_config.ptOrder  () ; }
      // ======================================================================
    public:
      // ======================================================================
      /// recoID 
      int recoID    () const { return m_recoID ; }
      /// output selection 
      const stringKey& output () const { return m_sink.output () ; }
      // configuration 
      const LoKi::Hlt1::UpgradeConf& config () const { return m_config ; }      
      // ======================================================================      
    private:
      // ======================================================================
      /// perform proper initialization
      void init () const;                                        // initialize
      // ======================================================================
      /** upgrade/recontruct one track 
       *  @param seed   the seed to be upgraded 
       *  @param tracks container of output tracks 
       *  @param otracks TES-container 
       *  @return status code 
       */
      StatusCode reco 
      ( const LHCb::Track*         seed    , 
        std::vector<LHCb::Track*>& tracks  , 
        LHCb::Track::Container*    otracks ) const ;
      /** find upgraded tracks in the TES container 
       *  @param seed   the seed to be upgraded 
       *  @param tracks container of output tracks 
       *  @param otracks TES-container 
       *  @return number of tracks 
       */
      size_t find
      ( const LHCb::Track*         seed    , 
        std::vector<LHCb::Track*>& tracks  , 
        LHCb::Track::Container*    otracks ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the configuration 
      LoKi::Hlt1::UpgradeConf m_config ;                  // the coinfiguration 
      // ======================================================================
      /// "sink":  register the selection for Hlt Data Service 
      LoKi::Hlt1::TrRegister m_sink ;                               // the sink
      // ID of the reconstruction/upgrade tool 
      mutable int m_recoID ;                    // ID of the recontruction tool 
      // ======================================================================      
    private:
      // ======================================================================      
      /// the recontruction/upgrade tool itself 
      mutable LoKi::Interface<ITracksFromTrack> m_upgrade ; // the upgrade tool
      // ======================================================================      
      // the algorithm 
      mutable GaudiAlgorithm* m_alg ;
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
