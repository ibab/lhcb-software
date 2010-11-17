// $Id$
// ============================================================================
#ifndef LOKI_UPGRADECONF_H 
#define LOKI_UPGRADECONF_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// TrackEvent 
// ============================================================================
#include  "Event/Track.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1 
  {
    // ========================================================================
    /** @class UpgradeConf
     *  The helper class to keep all information needed for
     *  TrUpgrade functor
     * 
     *  Essentially it mimics the configuration of tool in 
     *  HltTrackUpgradeTool. and itis assumed that the 
     *  internal dictionary from HltTrackUpgradeTool will 
     *  be implemented in python configurables using this objects
     *
     *  @see LoKi::Hlt1::TrUpgrade
     *  @see ITracksFromTrack
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @daet 2008-11-12
     */
    class GAUDI_API UpgradeConf
    {
    public:
      // ======================================================================
      /** constructor 
       *  @param trTool the name of ITracksFromTrack tool 
       *  @param address TES lolcation of the upgdarded tracks 
       *  @param trType   the track type for upgrade 
       *  @param owner    ownership ? 
       *  @param moveIDs  transfer IDs ?
       *  @param moveAncs transfer anscendents ?
       *  @param moveInfo transfer Extra Info ?
       *  @param ptOrder   order in pt ?
       */
      UpgradeConf 
      ( const std::string&        trTool   ,    //   ITrackFromTrack  tool name
        const std::string&        address  ,    //   TES location of the tracks 
        const LHCb::Track::Types  trType   ,    //                   track type 
        const bool                owner    ,    //                      owner ? 
        const bool                moveIDs  ,    //               transfer IDs ? 
        const bool                moveAncs ,    //       transfer anscendents ? 
        const bool                moveInfo ,    //        transfer extra info ?
        const bool                ptOrder  ) ;  //                   order pt ?
      // ======================================================================
    public:
      // ======================================================================
      /// ITracksFromTrack tool name 
      const std::string& trTool   () const { return m_trTool    ; }
      /// TES  location of upgraded tracks 
      const std::string& address  () const { return m_address   ; }
      /// track type 
      LHCb::Track::Types trType   () const { return m_trType    ; }
      /// owner ?
      bool               owner    () const { return m_owner     ; }
      /// transfer IDs ?
      bool               moveIDs  () const { return m_moveIDs   ; }
      /// transfer Ancestors 
      bool               moveAncs () const { return m_moveAncs  ; }
      /// transfer extra info? 
      bool               moveInfo () const { return m_moveInfo  ; }
      /// pt-ordering ?
      bool               ptOrder  () const { return m_ptOrder   ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      UpgradeConf () ;                   // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      std::ostream& fillStream ( std::ostream& s ) const ;
      std::string   toString () const ;  
      // ======================================================================
    private:
      // ======================================================================
      /// tool name 
      std::string m_trTool ;                 //         Track->Tracks tool name 
      /// TES location of upgraded tracks 
      std::string m_address ;                // TES location of upgraded tracks 
      /// track type 
      LHCb::Track::Types m_trType ;          //                      track type 
      /// owner ? 
      bool m_owner    ;                      //                         owner ?  
      /// transfer IDs ? 
      bool m_moveIDs  ;                      //                  transfer IDs ? 
      /// transfer ancestors ? 
      bool m_moveAncs ;                      //              transfer ancestors
      /// transfer extra info ? 
      bool m_moveInfo ;                      //           transfer extra info ? 
      /// pt-ordering 
      bool m_ptOrder  ;                      //                     pt-ordering 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
/// output operator to ostream  
std::ostream& operator<<( std::ostream&                  s , 
                          const LoKi::Hlt1::UpgradeConf& o ) ;
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_UPGRADECONF_H
// ============================================================================
