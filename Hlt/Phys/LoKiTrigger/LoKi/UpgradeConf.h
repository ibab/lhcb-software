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
// LoKi
// ============================================================================
#include  "LoKi/TrackTool.h"
// ============================================================================
/** @file  LoKi/UpgradeConf.h
 *  
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-11-12
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$
 */
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
     *  HltTrackUpgradeTool and it is assumes that the 
     *  internal dictionary from HltTrackUpgradeTool will 
     *  be implemented in python configurables using this objects
     *
     *  This file is part of LoKi project: 
     *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
     * 
     *  @see LoKi::Hlt1::TrUpgrade
     *  @see ITracksFromTrack
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @daet 2008-11-12
     */
    class GAUDI_API UpgradeConf : public LoKi::Hlt1::TrackTool 
    {
    public:
      // ======================================================================
      /** constructor 
       *  @param trTool the name of ITracksFromTrack tool 
       *  @param address TES location of the upgdarded tracks 
       *  @param trType   the track type for upgrade 
       *  @param owner    ownership ? 
       *  @param moveIDs  transfer IDs ?
       *  @param moveAncs transfer anscendents ?
       *  @param moveInfo transfer Extra Info ?
       *  @param ptOrder   order in pt ?
       */
      UpgradeConf 
      ( std::string         trTool   ,    //   ITrackFromTrack  tool name
        std::string         address  ,    //   TES location of the tracks 
        LHCb::Track::Types  trType   ,    //                   track type 
        bool                owner    ,    //                      owner ? 
        bool                moveIDs  ,    //               transfer IDs ? 
        bool                moveAncs ,    //       transfer anscendents ? 
        bool                moveInfo ,    //        transfer extra info ?
        bool                ptOrder  ) ;  //                   order pt ?
      // ======================================================================
      /** constructor 
       *  @param trTool the name of ITracksFromTrack tool 
       *  @param address TES location of the upgdarded tracks 
       *  @param trType   the track type for upgrade 
       *  @param owner    ownership ? 
       *  @param moveIDs  transfer IDs ?
       *  @param moveAncs transfer anscendents ?
       *  @param moveInfo transfer Extra Info ?
       *  @param ptOrder   order in pt ?
       */
      UpgradeConf 
      ( std::string         trTool   ,    //   ITrackFromTrack  tool name
        std::string         address  ,    //   TES location of the tracks 
        LHCb::Track::Types  trType   ,    //                   track type 
        bool                owner    ,    //                      owner ? 
        bool                moveIDs  ,    //               transfer IDs ? 
        bool                moveAncs ,    //       transfer anscendents ? 
        bool                moveInfo ,    //        transfer extra info ?
        bool                ptOrder  , 
        const LoKi::Functor<const LHCb::Track*,bool>& cut ) ;  //       functor
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
    /// output operator to ostream  
    inline std::ostream& operator<<
      ( std::ostream&                  s , 
        const LoKi::Hlt1::UpgradeConf& o ) { return o.fillStream ( s ) ; }
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_UPGRADECONF_H
// ============================================================================
