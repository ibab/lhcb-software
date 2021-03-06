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
#include "LoKi/HelperTool.h"
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
    class UpgradeTool : public LoKi::Hlt1::HelperTool
    {
    public:
      // ======================================================================
      /// the copy constructor is default
      UpgradeTool ( const UpgradeTool& ) = default;
      // ======================================================================
      /// the assignment operator is default
      UpgradeTool& operator=( const UpgradeTool& ) = default;
      // ======================================================================
      /// create the tool from configuration info
      UpgradeTool ( const LoKi::Hlt1::UpgradeConf& info ) ;
      // ======================================================================
      /// create the tool from configuration info and complement cache location
      UpgradeTool ( const LoKi::Hlt1::UpgradeConf& info,
                    const std::string& complement ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is private
      UpgradeTool ();
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
      StatusCode upgradeTracks 
      ( const Hlt::Candidate::ConstVector& input  , 
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
    public:
      // ======================================================================
      /// ITracksFromTrack tool name 
      const std::string& trTool     () const { return config().trTool   () ; }
      /// TES  location of upgraded tracks 
      const std::string& address    () const { return config().address  () ; }
      /// track type 
      LHCb::Track::Types trType     () const { return config().trType   () ; }
      /// owner ? 
      bool               owner      () const { return config().owner    () ; }
      /// transfer IDs ?
      bool               moveIDs    () const { return config().moveIDs  () ; }
      /// transfer Ancestors 
      bool               moveAncs   () const { return config().moveAncs () ; }
      // transfer extra info? 
      bool               moveInfo   () const { return config().moveInfo () ; }
      // pt-ordering ?
      bool               ptOrder    () const { return config().ptOrder  () ; }
      /// TES  location of complement cache
      const std::string& complement () const { return m_complement         ; }
      // ======================================================================
    public:
      // ======================================================================
      /// configuration 
      const LoKi::Hlt1::UpgradeConf& config  () const { return m_config    ; }      
      /// configuration 
      operator const LoKi::Hlt1::UpgradeConf&() const { return  config ()  ; }      
      // ======================================================================      
    public:
      // ======================================================================
      /// access the tool 
      ITracksFromTrack* upgradeTool() const { return m_upgrade ; }
      // ======================================================================
    protected:
      // ======================================================================
      /** upgrade one track. 
       *  - else if it is not yet reconstucted upgrade it
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
      // ======================================================================
    private:
      // ======================================================================      
      /// the actual configuration of the tool 
      LoKi::Hlt1::UpgradeConf m_config ;       // the configuration of the tool 
      // ======================================================================
      /// Complement cache location
      std::string m_complement ;                            // complement cache
      // ======================================================================
    private:
      // ======================================================================
      /// the recontruction/upgrade tool itself 
      mutable LoKi::Interface<ITracksFromTrack> m_upgrade ; // the upgrade tool
      // ======================================================================      
    };  
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::UpgradeTool& o ) ;
    // ========================================================================
  }  
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_UPGRADETOOL_H
// ============================================================================
