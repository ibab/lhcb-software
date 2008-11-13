// $Id: UpgradeTool.h,v 1.1 2008-11-13 22:11:03 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_UPGRADETOOL_H 
#define LOKI_UPGRADETOOL_H 1
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
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITracksFromTrack.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/TrackTypes.h"
#include "LoKi/UpgradeConf.h"
// ============================================================================
class GaudiAlgorithm ;
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class UpgradeTool UpgradeTool.h LoKi/UpgradeTool.h
     *  Simple replacement of HltTrackUpgradeTool 
     *  @see ITracksFromTrack
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-13
     */
    class UpgradeTool : public virtual LoKi::AuxFunBase 
    {
    public:
      // ======================================================================
      /// create the tool from configuration info 
      UpgradeTool ( const LoKi::Hlt1::UpgradeConf& info ) ;
      // ======================================================================
    protected:
      // ======================================================================
      /** upgrade the set of seeds          
       *  @param itracks input container of seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgrade 
      ( const std::vector<LHCb::Track*>& itracks , 
        std::vector<LHCb::Track*>&       otracks ) const ;
      // ======================================================================
      /** upgrade one seed          
       *  @param itrack  input track/seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgrade 
      ( const LHCb::Track* itrack , 
        std::vector<LHCb::Track*>&       otracks ) const ;
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
      /// configuration 
      const LoKi::Hlt1::UpgradeConf& config () const { return m_config    ; }      
      /// configuration 
      operator const LoKi::Hlt1::UpgradeConf&() const { return  config () ; }      
      // ======================================================================      
    public:
      // ======================================================================
      /// access the tool 
      const LoKi::Interface<ITracksFromTrack>& upgrade() const 
      { return m_upgrade ; }
      // ======================================================================
    private:
      // ======================================================================
      /** upgrade one track. 
       *  - If the seed  has "right" type, just copy it it output
       *  - else if it is not yet reconstucted ( info(recoID() ), upgrade it
       *  - else find the tracks in "otracks", which have seed as ancestor
       * @param seed     the track to be upgraded
       * @param output   list of upgraded tracks (cumulative)
       * @param otrack   TES container 
       */
      StatusCode iupgrade 
      ( const LHCb::Track*         seed    , 
        std::vector<LHCb::Track*>& output  , 
        LHCb::Track::Container*    otracks ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /** upgrade/recontruct one track 
       *  @param seed   the seed to be upgraded 
       *  @param tracks container of output tracks (cumulative)
       *  @param otracks TES-container 
       *  @return status code 
       */
      StatusCode reco 
      ( const LHCb::Track*         seed    , 
        std::vector<LHCb::Track*>& tracks  , 
        LHCb::Track::Container*    otracks ) const ;
      /** find upgraded tracks in the TES container 
       *  @param seed   the seed to be upgraded 
       *  @param tracks container of output tracks (cumulative)
       *  @param otracks TES-container 
       *  @return number of added tracks 
       */
      size_t find
      ( const LHCb::Track*         seed    , 
        std::vector<LHCb::Track*>& tracks  , 
        LHCb::Track::Container*    otracks ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// initialize the object 
      void init () const ; // initialize the object 
      // ======================================================================      
    protected:
      // ======================================================================
      /// get access to the algorithm 
      const GaudiAlgorithm* alg() const { return m_alg ; }
      // ======================================================================
    private:
      // ======================================================================      
      /// the actual configuration of the tool 
      LoKi::Hlt1::UpgradeConf m_config ;       // the configuration of the tool 
      /// ID of the reconstruction/upgrade tool 
      mutable int m_recoID ;                    // ID of the recontruction tool 
      // ======================================================================
    private:
      // ======================================================================
      /// the recontruction/upgrade tool itself 
      mutable LoKi::Interface<ITracksFromTrack> m_upgrade ; // the upgrade tool
      // ======================================================================      
      /// the algorithm 
      mutable const GaudiAlgorithm* m_alg ; // the algorithm 
      // ======================================================================
    };  
    // ========================================================================
  } // end of namespace LoKi::Hlt1
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_UPGRADETOOL_H
// ============================================================================
