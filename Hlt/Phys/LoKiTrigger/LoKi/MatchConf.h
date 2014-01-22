// $Id$
// ============================================================================
#ifndef LOKI_MATCHCONF_H 
#define LOKI_MATCHCONF_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTool.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1 
  {
    // ========================================================================
    /** @class MatchConf MatchConf.h LoKi/MatchConf.h
     *  Simple helper class to store the configuration information for 
     *  "track matching tool"
     *  @see ITrackMatch 
     *  @author Vanay BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-11-14
     */
    class GAUDI_API MatchConf : public LoKi::Hlt1::TrackTool
    {
    public:
      // ======================================================================
      /** costructor 
       *  @param mTool ITrackMatch tool name 
       *  @param address  TES location of "matched" tracks 
       *  @param trType   track type
       *  @param moveIDs  transfer IDs ?
       *  @param moveAncs transfer ancestors ? 
       *  @param moveInfo transfer extra info ? 
       *  @param quality  matching quality 
       *  @param quality2 matching quality2
       *  @param maxQCut  max matching quality  cut 
       *  @param maxQ2Cut max matching quality2 cut 
       */
      MatchConf 
      ( std::string       mTool    ,   //          ITrackMatch tool name  
        std::string       address  ,   // TES location of matched tracks
        LHCb::Track::Types trType   ,   //                     Track type
        bool               moveIDs  ,   //                 transfer IDs ? 
        bool               moveAncs ,   //           transfer ancestors ? 
        bool               moveInfo );  //          transfer Extra Info ? 
      // ======================================================================
      /** constructor 
       *  @param mTool ITrackMatch tool name 
       *  @param address  TES location of "matched" tracks 
       *  @param trType   track type
       *  @param moveIDs  transfer IDs ?
       *  @param moveAncs transfer ancestors ? 
       *  @param moveInfo transfer extra info ? 
       */
      MatchConf 
      ( std::string        mTool    ,   //          ITrackMatch tool name  
        std::string        address  ,   // TES location of matched tracks
        LHCb::Track::Types trType   ,   //                     Track type
        bool               moveIDs  ,   //                 transfer IDs ? 
        bool               moveAncs ,   //           transfer ancestors ? 
        bool               moveInfo ,   //          transfer Extra Info ? 
        const LoKi::Functor<const LHCb::Track*,bool>& cut ); //          filter 
      // ======================================================================
    public:
      // ======================================================================
      /// ITrackMatch tool name 
      const std::string& mTool    () const { return m_mTool     ; }
      /// TES location of matched tracks 
      const std::string& address  () const { return m_address   ; }
      /// track type 
      LHCb::Track::Types trType   () const { return m_trType    ; }
      /// transfer IDs ?
      bool               moveIDs  () const { return m_moveIDs   ; }
      /// transfer Ancestors 
      bool               moveAncs () const { return m_moveAncs  ; }
      /// transfer extra info? 
      bool               moveInfo () const { return m_moveInfo  ; }     
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MatchConf () ;                     // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      std::ostream& fillStream ( std::ostream& s ) const ;
      std::string   toString () const ;  
      // ======================================================================
    private:
      // ======================================================================
      /// ITrackMatch tool name 
      std::string m_mTool     ;               //          ITrackMatch tool name 
      /// TES location of matched tracks 
      std::string m_address   ;               // TES location of matches tracks
      /// Track Type 
      LHCb::Track::Types m_trType ;           //                     Track Type 
      /// transfer IDs ? 
      bool m_moveIDs  ;                       //                 transfer IDs ? 
      /// transfer ancestors ? 
      bool m_moveAncs ;                       //             transfer ancestors
      /// transfer extra info ? 
      bool m_moveInfo ;                       //          transfer extra info ? 
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1
  // ==========================================================================
  /// output operator to ostream  
  inline 
  std::ostream& operator<<( std::ostream&                s , 
                            const LoKi::Hlt1::MatchConf& o ) 
  { return o.fillStream ( s ) ; }
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MATCHCONF_H
// ============================================================================
