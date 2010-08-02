// $Id$
// ============================================================================
#ifndef LOKI_TRMATCH_H 
#define LOKI_TRMATCH_H 1
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
#include "TrackInterfaces/ITrackMatch.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Hlt1.h"
#include "LoKi/MatchConf.h"
// ============================================================================
// forward declarations 
// ============================================================================
class GaudiAlgorithm ;
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class TrMatch TrMatch.h LoKi/TrMatch.h
     *  Simple helper whicch represent the track matching concept  
     *  @see ITrackMatch 
     *  @see LoKi::Hlt1::MatchConf
     *  @see LoKi::Cuts::TrMATCH
     *  @see LoKi::Hlt1::TrMatch2
     *  @author Vanya BELYAEV Ivab.Belyaev@nikhef.nl
     *  @date   2008-11-14
     */
    class TrMatch : public LoKi::BasicFunctors<const LHCb::Track*>::Pipe 
    {
    protected:
      // ======================================================================
      /// the actual type of track source
      typedef LoKi::BasicFunctors<const LHCb::Track*>::Source TrSource ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      TrMatch 
      ( const std::string&           output  ,   //   output selection name/key 
        const TrSource&              tracks2 ,   //   tracks to be matched with 
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// MANDATORY: virtual desctructor 
      virtual ~TrMatch() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrMatch* clone() const { return new TrMatch(*this) ; }
      /// MANDATORY: the only essential method
      virtual  result_type   operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrMatch() ;                        // the default constructor is disabled 
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
      /// quality 
      const std::string& quality  () const { return m_config.quality  () ; }
      /// quality2 
      const std::string& quality2 () const { return m_config.quality2 () ; }
      /// max quality  cut 
      double maxQualityCut  () const { return m_config.maxQualityCut  () ; } 
      /// max quality2 cut 
      double maxQuality2Cut () const { return m_config.maxQuality2Cut () ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the configuration 
      const LoKi::Hlt1::MatchConf& config   () const { return m_config   ; }
      /// cast to the configuration 
      operator const LoKi::Hlt1::MatchConf& () const { return   config() ; }
      /// the matching tool
      const LoKi::Interface<ITrackMatch>& match () const { return m_match ; }
      /// the second source 
      const TrSource& tracks2() const { return m_tracks2.func() ; }
      // ======================================================================
    public:
      // ======================================================================
      /// ID of "quality"
      int qualityID  () const { return m_qualityID  ; }
      /// ID of "quality2"
      int qualityID2 () const { return m_qualityID2 ; }
      /// 'reco-ID'
      int recoID     () const { return m_recoID     ; }
      // ======================================================================
    private:
      // ======================================================================
      /// initialize the functor 
      void init () const ;                            // initialize the functor 
      /// get access to the algorihtm 
      const GaudiAlgorithm* alg() const { return m_alg ; } //    the algorihtm 
      // ======================================================================
    private:
      // ======================================================================
      /// souce of tracks to be matched 
      LoKi::Assignable<TrSource>::Type m_tracks2 ; //          source of tracks 
      /// tool configuration 
      LoKi::Hlt1::MatchConf            m_config  ;  //       tool configuration 
      // ======================================================================
      /// 'sink': the functor which register the selection in Hlt Data Svc
      LoKi::Hlt1::TrRegister         m_sink   ;  //                      'sink'
      // ======================================================================
    private:
      // ======================================================================
      /// ID of "quality"
      mutable int m_qualityID  ;                 //             ID of "quality"
      /// ID of "quality2"
      mutable int m_qualityID2 ;                 //            ID of "quality2"
      // ======================================================================
    private:
      // ======================================================================
      /// the matching tool itself 
      mutable LoKi::Interface<ITrackMatch> m_match ;  //      the matching tool 
      /// "reco ID"
      mutable int m_recoID ;                          //                reco ID 
      // ======================================================================
    private:
      // ======================================================================
      /// the algorithm
      mutable const GaudiAlgorithm* m_alg     ;  //              the algorithm
      // ======================================================================
    protected:
      // ======================================================================
      /// invert ?
      bool invert() const { return m_invert ; }                     // invert ?
      // ======================================================================
      /// set the invert flag
      void setInvert ( const bool invert ) { m_invert = invert ; }
      // ======================================================================
    private:
      // ======================================================================
      /// invert flag ?
      bool m_invert ;                                          // invert flag ?
      // ======================================================================
    };
    // ========================================================================
    /** @class TrMatch2 TrMatch.h LoKi/TrMatch.h
     *  Simple helper which represent the track matching concept  
     *  @see ITrackMatch 
     *  @see LoKi::Hlt1::MatchConf
     *  @see LoKi::Hlt1::TrMatch
     *  @see LoKi::Cuts::TrMATCH2
     *  @author Vanya BELYAEV Ivab.Belyaev@nikhef.nl
     *  @date   2008-11-14
     */
    class TrMatch2 : public TrMatch
    {
    public :
      // ======================================================================
      /// constructor 
      TrMatch2 
      ( const std::string&           output  ,   //   output selection name/key 
        const TrSource&              tracks2 ,   //   tracks to be matched with 
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// MANDATORY: virtual desctructor 
      virtual ~TrMatch2() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrMatch2* clone() const { return new TrMatch2(*this) ; }
      /// OPTIONAL: nice printout 
      virtual std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TrMatch2() ;                       // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Hlt1 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRMATCH_H
// ============================================================================
