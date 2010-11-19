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
#include "LoKi/MatchTool.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class Match LoKi/TrMatch.h
     *  Simple helper which represent the track matching concept  
     *  @see ITrackMatch 
     *  @see LoKi::Hlt1::MatchConf
     *  @see LoKi::Cuts::TC_MATCH
     *  @see LoKi::Hlt1::TC_MATCH2
     *  @author Vanya BELYAEV Ivab.Belyaev@nikhef.nl
     *  @date   2008-11-14
     */
    class Match 
      : public virtual LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe 
      , public         LoKi::Hlt1::MatchTool 
    {
    protected:
      // ======================================================================
      /// the actual type of track source
      typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      Match 
      ( const std::string&           output  ,   //   output selection name/key 
        const Source&                tracks2 ,   //   tracks to be matched with 
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// constructor 
      Match 
      ( const std::string&           output  ,   //   output selection name/key 
        const std::string&           tracks2 ,   //   selection name  
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// MANDATORY: virtual desctructor 
      virtual ~Match() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Match* clone() const { return new Match(*this) ; }
      /// MANDATORY: the only essential method
      virtual  result_type   operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Match() ;                          // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// the second source 
      const Source&      source () const { return m_source2.func() ; }
      // ======================================================================
      /// the second sourcee 
      const std::string& output () const { return m_sink.output() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// souce of tracks to be matched 
      LoKi::Assignable<Source>::Type m_source2 ; //            source of tracks 
      // ======================================================================
      /// 'sink': the functor which register the selection in Hlt Data Svc
      LoKi::Hlt1::Sink               m_sink    ;  //                      'sink'
      // ======================================================================
    };
    // ========================================================================
    /** @class Match2 LoKi/TrMatch.h
     *  Simple helper which represent the track matching concept  
     *  @see ITrackMatch 
     *  @see LoKi::Hlt1::MatchConf
     *  @see LoKi::Cuts::TC_MATCH
     *  @see LoKi::Hlt1::TC_MATCH2
     *  @author Vanya BELYAEV Ivab.Belyaev@nikhef.nl
     *  @date   2008-11-14
     */
    class Match2 : public Match  
    { 
    protected:
      // ======================================================================
      /// the actual type of track source
      typedef LoKi::BasicFunctors<const Hlt::Candidate*>::Source Source ;
      // ======================================================================      
    public:
      // ======================================================================
      /// constructor 
      Match2 
      ( const std::string&           output  ,   //   output selection name/key 
        const Source&                tracks2 ,   //   tracks to be matched with 
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// constructor 
      Match2 
      ( const std::string&           output  ,   //   output selection name/key 
        const std::string&           tracks2 ,   //   tracks to be matched with 
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// MANDATORY: virtual desctructor 
      virtual ~Match2() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Match2* clone() const { return new Match2(*this) ; }
      /// OPTIONAL: nice printout 
      virtual std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Match2() ;                          // the default constructor is disabled 
      // ======================================================================
    };
//     // ========================================================================
//     /** @class TrMatch TrMatch.h LoKi/TrMatch.h
//      *  Simple helper whicch represent the track matching concept  
//      *  @see ITrackMatch 
//      *  @see LoKi::Hlt1::MatchConf
//      *  @see LoKi::Cuts::TrMATCH
//      *  @see LoKi::Hlt1::TrMATCH2
//      *  @author Vanya BELYAEV Ivab.Belyaev@nikhef.nl
//      *  @date   2008-11-14
//      */
//     class TrMatch 
//       : public virtual LoKi::BasicFunctors<const LHCb::Track*>::Pipe 
//       , public LoKi::Hlt1::MatchTool 
//     {
//     protected:
//       // ======================================================================
//       /// the actual type of track source
//       typedef LoKi::BasicFunctors<const LHCb::Track*>::Source TrSource ;
//       // ======================================================================
//     public:
//       // ======================================================================
//       /// constructor 
//       TrMatch 
//       ( const std::string&           output  ,   //   output selection name/key 
//         const TrSource&              tracks2 ,   //   tracks to be matched with 
//         const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
//       /// MANDATORY: virtual desctructor 
//       virtual ~TrMatch() {}
//       /// MANDATORY: clone method ("virtual constructor")
//       virtual  TrMatch* clone() const { return new TrMatch(*this) ; }
//       /// MANDATORY: the only essential method
//       virtual  result_type   operator() ( argument a ) const ;
//       /// OPTIONAL: nice printout 
//       virtual std::ostream&  fillStream ( std::ostream& s ) const ;
//       // ======================================================================
//     private:
//       // ======================================================================
//       /// the default constructor is disabled 
//       TrMatch() ;                        // the default constructor is disabled 
//       // ======================================================================
//     public:
//       // ======================================================================
//       /// the second source 
//       const TrSource& tracks2() const { return m_tracks2.func() ; }
//       // ======================================================================
//     private:
//       // ======================================================================
//       /// souce of tracks to be matched 
//       LoKi::Assignable<TrSource>::Type m_tracks2 ; //          source of tracks 
//       // ======================================================================
//       /// 'sink': the functor which register the selection in Hlt Data Svc
//       LoKi::Hlt1::TrRegister         m_sink   ;  //                      'sink'
//       // ======================================================================
//     };
//     // ========================================================================
//     /** @class TrMatch2 TrMatch.h LoKi/TrMatch.h
//      *  Simple helper which represent the track matching concept  
//      *  @see ITrackMatch 
//      *  @see LoKi::Hlt1::MatchConf
//      *  @see LoKi::Hlt1::TrMatch
//      *  @see LoKi::Cuts::TrMATCH2
//      *  @author Vanya BELYAEV Ivab.Belyaev@nikhef.nl
//      *  @date   2008-11-14
//      */
//     class TrMatch2 : public TrMatch
//     {
//     public :
//       // ======================================================================
//       /// constructor 
//       TrMatch2 
//       ( const std::string&           output  ,   //   output selection name/key 
//         const TrSource&              tracks2 ,   //   tracks to be matched with 
//         const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
//       /// MANDATORY: virtual desctructor 
//       virtual ~TrMatch2() {}
//       /// MANDATORY: clone method ("virtual constructor")
//       virtual  TrMatch2* clone() const { return new TrMatch2(*this) ; }
//       /// OPTIONAL: nice printout 
//       virtual std::ostream&  fillStream ( std::ostream& s ) const ;
//       // ======================================================================
//     private:
//       // ======================================================================
//       /// the default constructor is disabled 
//       TrMatch2() ;                       // the default constructor is disabled 
//       // ======================================================================
//     } ;
//     // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRMATCH_H
// ============================================================================
