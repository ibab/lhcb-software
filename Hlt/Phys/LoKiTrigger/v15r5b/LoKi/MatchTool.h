// $Id$
// ============================================================================
#ifndef LOKI_MATCHTOOL_H 
#define LOKI_MATCHTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// TrackInterfaces
// ============================================================================
#include "TrackInterfaces/ITrackMatch.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/ITrack2CandidateMatch.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
#include "LoKi/MatchConf.h"
#include "LoKi/HelperTool.h"
// ============================================================================
/** @file  LoKi/MatchTool.h
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
 *  @date   2010-10-28
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
    /** @class MatchTool LoKi/MatchTool.h
     *  
     *  Helper class to implement track-match 
     *
     *  @author Vanya Belyaev
     *  @date   2010-10-28
     */
    class GAUDI_API MatchTool : public LoKi::Hlt1::HelperTool
    {
    public:
      // ======================================================================
      /// constructor 
      MatchTool 
      ( const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// MANDATORY: virtual desctructor 
      virtual ~MatchTool() {}
      // ======================================================================
    protected:
      // ======================================================================
      /** perform the track matching 
       *  @param tr1 the first  track 
       *  @param tr2 the second track 
       *  @return matched track (if any) 
       */
      const LHCb::Track* match 
      ( const LHCb::Track* tr1 , 
        const LHCb::Track* tr2 ) const ;
      // ======================================================================
      /** perform the track matching 
       *  @param tr1 the first  track 
       *  @param tr2 the second track 
       *  @return matched track (if any) 
       */
      const LHCb::Track* match 
      ( const LHCb::Track*    tr1 , 
        const Hlt::Candidate* tr2 ) const ;
      // ======================================================================
      /** perform the track matching 
       *  @param tr1 the first  track 
       *  @param tr2 the second candidate 
       *  @return treu in case of good matching 
       */
      bool matched 
      ( const LHCb::Track*    tr1 , 
        const Hlt::Candidate* tr2 ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MatchTool () ;                     // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// ITrackMatch tool name 
      const std::string& mTool    () const { return m_config.mTool    () ; }
      /// TES location of matched tracks 
      const std::string& address  () const { return m_config.address  () ; }
      /// track type 
      LHCb::Track::Types trType   () const { return m_config.trType   () ; }
      /// transfer IDs ?
      bool               moveIDs  () const { return m_config.moveIDs  () ; }
      /// transfer Ancestors 
      bool               moveAncs () const { return m_config.moveAncs () ; }
      /// transfer extra info? 
      bool               moveInfo () const { return m_config.moveInfo () ; }     
      // ======================================================================
    public:
      // ======================================================================
      /// get the configuration 
      const LoKi::Hlt1::MatchConf& config   () const { return m_config   ; }
      /// cast to the configuration 
      operator const LoKi::Hlt1::MatchConf& () const { return   config() ; }
      /// the matching tool
      const LoKi::Interface<ITrackMatch>&                match  () const 
      { return m_match  ; }
      /// the matching tool
      const LoKi::Interface<Hlt::ITrack2CandidateMatch>& match2 () const
      { return m_match2 ; }
      // ======================================================================
    public:
      // ======================================================================
      /// 'reco-ID'
      int recoID     () const { return m_recoID     ; }
      // ======================================================================
      /// invert arguments ?
      bool invert    () const { return m_invert     ; }
      void setInverted ( bool value ) { m_invert = value ; }
      // ======================================================================
    private:
      // ======================================================================
      /// tool configuration 
      LoKi::Hlt1::MatchConf            m_config  ;  //       tool configuration 
      // ======================================================================
    private:
      // ======================================================================
      /// the matching tool itself 
      mutable LoKi::Interface<ITrackMatch>                m_match    ;
      /// the matching tool itself 
      mutable LoKi::Interface<Hlt::ITrack2CandidateMatch> m_match2   ;
      /// "reco ID"
      mutable int  m_recoID ;                         //                reco ID 
      // invert arguments ? 
      mutable bool m_invert ;                        // invert arguments ?
      // ======================================================================
    } ;
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MATCHTOOL_H
// ============================================================================
