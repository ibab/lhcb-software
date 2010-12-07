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
/** @file  LoKi/TrMatch.h
 *  
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-11-14
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
    /** @class Match LoKi/TrMatch.h
     *  Simple helper which represent the track matching concept  
     *  @see ITrackMatch 
     *  @see LoKi::Hlt1::MatchConf
     *  @see LoKi::Cuts::TC_MATCH
     *  @see LoKi::Hlt1::TC_MATCH2
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
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
      /** constructor 
       *  @param output (INPUT) name of output selection (can be anonymous) 
       *  @param track2 (INPUT) the second source tracks/candidates 
       *  @param config (INPUT) the actual configuration 
       */
      Match 
      ( const std::string&           output  ,   //   output selection name/key 
        const Source&                tracks2 ,   //   tracks to be matched with 
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /** constructor 
       *  @param output (INPUT) name of output selection (can be anonymous) 
       *  @param track2 (INPUT) the second source tracks/candidates 
       *  @param config (INPUT) the actual configuration 
       */
      Match 
      ( const std::string&           output  ,   //   output selection name/key 
        const std::string&           tracks2 ,   //   selection name  
        const LoKi::Hlt1::MatchConf& config  ) ; //          tool configuration 
      /// MANDATORY: virtual desctructor 
      virtual ~Match() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Match* clone() const ; 
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
      /// the output selection 
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
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
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
      virtual ~Match2() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Match2* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream&  fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Match2() ;                          // the default constructor is disabled 
      // ======================================================================
    };
    // ========================================================================
  } //                                              end of namespace LoKi::Hlt1 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRMATCH_H
// ============================================================================
