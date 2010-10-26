// $Id$
// ============================================================================
#ifndef LOKI_UPGRADETOOL_H 
#define LOKI_UPGRADETOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
#include "Event/HltCandidate.h"
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
#include "LoKi/Listener.h"
// ============================================================================
class GaudiAlgorithm ;
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class UpgradeTool LoKi/UpgradeTool.h
     *  Simple replacement of HltTrackUpgradeTool 
     *  @see ITracksFromTrack
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-13
     */
    class UpgradeTool : public virtual LoKi::Listener
    {
    public:
      // ======================================================================
      /// create the tool from configuration info
      UpgradeTool ( const LoKi::Hlt1::UpgradeConf& info ) ;
      // ======================================================================
      /// copy constructor 
      UpgradeTool ( const UpgradeTool& right ) ;
      // ======================================================================
    protected:
      // ======================================================================
      /** upgrade the set of seeds          
       *  @param itracks input container of seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgrade 
      ( const LHCb::Track::ConstVector&  itracks , 
        LHCb::Track::ConstVector&        otracks ) const ;
      // ======================================================================
      /** upgrade one seed          
       *  @param itrack  input track/seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgrade 
      ( const LHCb::Track*        itrack , 
        LHCb::Track::ConstVector& otracks ) const ;
      // ======================================================================      
    public:
      // ======================================================================      
      /** upgrade the candidates          
       *  @param itrack  input track/seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgradeAll 
      ( const Hlt::Candidate::ConstVector& input  , 
        Hlt::Candidate::ConstVector&       output ) const ;
      // ======================================================================      
      /** upgrade the candidates          
       *  @param itrack  input track/seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgradeTracks 
      ( const Hlt::Candidate::ConstVector& input  , 
        Hlt::Candidate::ConstVector&       output ) const ;
      // ======================================================================      
      /** upgrade the candidates          
       *  @param itrack  input track/seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgradeMultiTracks 
      ( const Hlt::Candidate::ConstVector& input  , 
        Hlt::Candidate::ConstVector&       output ) const ;
      // ======================================================================      
      /** upgrade the candidates          
       *  @param itrack  input track/seeds 
       *  @param inedx   the track index to be upgraded 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode upgradeMultiTracks 
      ( const Hlt::Candidate::ConstVector& input  , 
        const unsigned int                 index  , 
        Hlt::Candidate::ConstVector&       output ) const ;
      // ======================================================================      
    private:
      // ======================================================================      
      /** upgrade the single track candidate 
       *  @param itrack  input track/seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode _i_upgrade_1track 
      ( const Hlt::Candidate*              input      , 
        Hlt::Candidate::ConstVector&       output     , 
        LHCb::Track::Container*            otracks    ) const ;
      // ======================================================================      
      /** upgrade the all tracks form multi-track candidate          
       *  @param itrack  input track/seeds 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode _i_upgrade_multi_track 
      ( const Hlt::Candidate*              input        , 
        Hlt::Candidate::ConstVector&       output       , 
        LHCb::Track::Container*            otracks      ) const ;
      // ======================================================================      
      /** upgrade tracsk by index in multitrack candidate 
       *  @param itrack  input track/seeds 
       *  @param inedx   the index of tarcj to be upgraded 
       *  @param otracks output container of upgraded tracks (cumulative) 
       *  @return status code 
       */
      StatusCode _i_upgrade_multi_track_j
      ( const Hlt::Candidate*              input        , 
        const unsigned int                 index        , // the index 
        Hlt::Candidate::ConstVector&       output       , 
        LHCb::Track::Container*            otracks      ) const ;
      // ======================================================================      
    public:
      // ======================================================================
      /// ITracksFromTrack tool name 
      const std::string& trTool   () const { return config().trTool   () ; }
      /// TES  location of upgraded tracks 
      const std::string& address  () const { return config().address  () ; }
      /// track type 
      LHCb::Track::Types trType   () const { return config().trType   () ; }
      /// owner ? 
      bool               owner    () const { return config().owner    () ; }
      /// transfer IDs ?
      bool               moveIDs  () const { return config().moveIDs  () ; }
      /// transfer Ancestors 
      bool               moveAncs () const { return config().moveAncs () ; }
      // transfer extra info? 
      bool               moveInfo () const { return config().moveInfo () ; }
      // pt-ordering ?
      bool               ptOrder  () const { return config().ptOrder  () ; }
      // ======================================================================
    public:
      // ======================================================================
      /// recoID 
      int recoID    () const { return m_recoID ; }
      /// configuration 
      const LoKi::Hlt1::UpgradeConf& config  () const { return m_config    ; }      
      /// configuration 
      operator const LoKi::Hlt1::UpgradeConf&() const { return  config () ; }      
      // ======================================================================      
    public:
      // ======================================================================
      /// access the tool 
      ITracksFromTrack* upgradeTool() const { return m_upgrade ; }
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
        LHCb::Track::ConstVector&  tracks  , 
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
        LHCb::Track::ConstVector&  tracks  , 
        LHCb::Track::Container*    otracks ) const ;
      /** find upgraded tracks in the TES container 
       *  @param seed   the seed to be upgraded 
       *  @param tracks container of output tracks (cumulative)
       *  @param otracks TES-container 
       *  @return number of added tracks 
       */
      size_t find
      ( const LHCb::Track*         seed    , 
        LHCb::Track::ConstVector& tracks  , 
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
    public:
      // ======================================================================
      /** handle the incidents:
       *  clear the involved pointers 
       */
      virtual void handle ( const Incident& /* incident */ )
      {
        // clear all pointers 
        m_hlt_candidates  = 0 ;
        m_hlt_stages      = 0 ;
        m_hlt_multitracks = 0 ;
        m_hlt_tracks      = 0 ;
      }
      // ======================================================================
    private:
      // ======================================================================      
      /// get the stored tracks 
      inline LHCb::Track::Container*     storedTracks      () const 
      {
        if ( !owner() ) { return 0 ; }
        if ( 0 == m_hlt_tracks ) { _createTracks () ; }
        return m_hlt_tracks ;
      }
      // ======================================================================      
    private:
      // ======================================================================
      template <class TYPE> 
      inline TYPE* _create ( typename TYPE::Container* cnt ) const 
      {
        TYPE* _new = new TYPE() ;
        cnt->push_back ( _new ) ;
        return _new ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /// get new candidate 
      inline Hlt::Candidate*  newCandidate     () const 
      {
        if ( 0 == m_hlt_candidates  ) { _createCandidates  () ; }
        return _create<Hlt::Candidate> ( m_hlt_candidates   ) ;
      }
      /// get new stage 
      inline Hlt::Stage*      newStage         () const 
      {
        if ( 0 == m_hlt_stages       ) { _createStages      () ; }
        return _create<Hlt::Stage>      ( m_hlt_stages     ) ;
      }
      /// get new multitarck
      inline Hlt::MultiTrack* newMultiTrack    () const 
      {
        if ( 0 == m_hlt_multitracks ) { _createMultiTracks () ; }
        return _create<Hlt::MultiTrack> ( m_hlt_multitracks ) ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// get the storage for tracks 
      LHCb::Track::Container*     _createTracks      () const ;
      /// get the storage for candidates 
      Hlt::Candidate::Container*  _createCandidates  () const ;
      /// get the storage for stages 
      Hlt::Stage::Container*      _createStages      () const ;
      /// get the storage for multitracks 
      Hlt::MultiTrack::Container* _createMultiTracks () const ;
      // ======================================================================
    private:
      // ======================================================================
      /// container of Hlt-candidates 
      mutable Hlt::Candidate::Container*  m_hlt_candidates  ;
      /// container of Hlt-stages 
      mutable Hlt::Stage::Container*      m_hlt_stages      ;      
      /// container of Hlt-multitracks 
      mutable Hlt::MultiTrack::Container* m_hlt_multitracks ;      
      /// container of tracks 
      mutable LHCb::Track::Container*     m_hlt_tracks      ;      
      // ======================================================================
    };  
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_UPGRADETOOL_H
// ============================================================================
