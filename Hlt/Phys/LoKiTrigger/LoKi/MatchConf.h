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
    class GAUDI_API MatchConf 
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
      ( const std::string&       mTool    ,   //          ITrackMatch tool name  
        const std::string&       address  ,   // TES location of matched tracks
        const LHCb::Track::Types trType   ,   //                     Track type
        const bool               moveIDs  ,   //                 transfer IDs ? 
        const bool               moveAncs ,   //           transfer ancestors ? 
        const bool               moveInfo ,   //          transfer Extra Info ? 
        const std::string&       quality  ,   //               matching quality 
        const std::string&       quality2 ,   //              matching quality2 
        const double             maxQCut  = 1.e6 ,  // max matching quality cut 
        const double             maxQ2Cut = 1.e6 ) ;//max matching quality2 cut  
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
      /// quality 
      const std::string& quality  () const { return m_quality   ; }
      /// quality2 
      const std::string& quality2 () const { return m_quality2  ; }
      /// max quality  cut 
      double       maxQualityCut  () const { return m_maxQualityCut  ; } 
      /// max quality2 cut 
      double       maxQuality2Cut () const { return m_maxQuality2Cut ; }
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
      /// Quality 
      std::string m_quality  ;                //                        Quality 
      /// Quality2
      std::string m_quality2 ;                //                        Quality 
      /// max quality cut 
      double      m_maxQualityCut  ;          //                Max Quality Cut  
      /// max quality2 cut 
      double      m_maxQuality2Cut ;          //               Max Quality2 Cut  
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Hlt1
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
/// output operator to ostream  
std::ostream& operator<<( std::ostream&                s , 
                          const LoKi::Hlt1::MatchConf& o ) ;
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MATCHCONF_H
// ============================================================================
