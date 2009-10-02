// $Id: Particles13.h,v 1.9 2009-10-02 15:06:13 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES13_H 
#define LOKI_PARTICLES13_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
namespace LHCb { class State ; }  
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-23 
 */
// ============================================================================
namespace LoKi
{
  namespace Particles 
  {
    // ========================================================================
    /** @class TrackChi2 
     *  The trivial funtion which evaluates LHCb::Track::chi2
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::chi2 
     *  @see LoKi::Cuts::TRCHI2 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackChi2
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TrackChi2() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackChi2* clone() const { return new TrackChi2(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;
    };
    // ========================================================================    
    /** @class TrackChi2PerDoF 
     *  The trivial funtion which evaluates LHCb::Track::chi2PerDoF
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::chi2PerDoF
     *  @see LoKi::Cuts::TRCHI2DOF
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackChi2PerDoF 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TrackChi2PerDoF() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackChi2PerDoF* clone() const 
      { return new TrackChi2PerDoF(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;
    };
    // ========================================================================
    /** @class TrackChi2Prob 
     *  The trivial function whcih evaluates the track's 
     *  \f$\chi^2\f$-probability.
     *
     *  The GSL routine <c>gsl_cdf_chisq_Q</c> is used for evaluation 
     *  @see LHCb::Track
     *  @see LoKi::TRPCHI2 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-10
     */
    class TrackChi2Prob 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~TrackChi2Prob () {}
      /// MANDATORY: clone method ("virtual constructor") 
      virtual  TrackChi2Prob* clone () const 
      { return new TrackChi2Prob ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the extractor of tracks: 
      LoKi::Extract::Particle2Track m_track ; // the extractor of tracks 
      // ======================================================================        
    };
    // ========================================================================
    /** @class TrackHasState
     *  The trivial funtion which evaluates LHCb::Track::hasStateAt
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::hasStateAt
     *  @see LoKi::Cuts::HASSTATE
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackHasState
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate 
    {
    public:
      /// constructor from the state location 
      TrackHasState ( const LHCb::State::Location location ) ;
      /// copy constructor
      TrackHasState ( const TrackHasState& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrackHasState() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackHasState* clone() const
      { return new TrackHasState(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      TrackHasState();
    private:
      // extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;
      // location of the state 
      LHCb::State::Location m_location ;
    } ;
    // ========================================================================
    /** @class TrackType
     *  The trivial function which evaluates LHCb::Track::type
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::Type
     *  @see LHCb::Track::type 
     *  @see LoKi::Cuts::TRTYPE
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackType
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TrackType() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackType* clone() const 
      { return new TrackType(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;
    };
    // ========================================================================
    /** @class TrackHistory
     *  The trivial function which evaluates LHCb::Track::history
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::History
     *  @see LHCb::Track::history
     *  @see LoKi::Cuts::TRHISTORY
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackHistory 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TrackHistory() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackHistory* clone() const 
      { return new TrackHistory(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;
    };
    // ========================================================================
    /** @class TrackHistoryFit
     *  The trivial function which evaluates LHCb::Track::historyFit
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::HistoryFit
     *  @see LHCb::Track::historyFit
     *  @see LoKi::Cuts::TRHISTFIT
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackHistoryFit
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TrackHistoryFit() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackHistoryFit* clone() const 
      { return new TrackHistoryFit(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;
    };
    // ========================================================================    
    /** @class TrackStatus
     *  The trivial function which evaluates LHCb::Track::status
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::Status
     *  @see LHCb::Track::status
     *  @see LoKi::Cuts::TRSTATUS
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackStatus
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~TrackStatus() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackStatus* clone() const 
      { return new TrackStatus(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;
    };
    // ========================================================================
    /** @class GhostProbability 
     *  The trivial function which evaluates "ghost probability"
     *  @see LoKi::Cuts::TRGHOSTPROB
     *  @see LoKi::Cuts::TRGP
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-10-02
     */
    class GhostProbability 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~GhostProbability() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  GhostProbability* clone() const 
      { return new GhostProbability(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;                // track extractor 
      // ======================================================================
    };
    // ========================================================================
    /** @class Track Likelihood
     *  The trivial function which evaluates "track likelihood"
     *  @see LoKi::Cuts::TRLIKELIHOOD
     *  @see LoKi::Cuts::TRLH
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-10-02
     */
    class TrackLikelihood
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~TrackLikelihood() {}
      /// MANDATORY: clone method ("vitual constructor")
      virtual  TrackLikelihood* clone() const 
      { return new TrackLikelihood(*this); }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// extractor of the track from the partice 
      LoKi::Extract::Particle2Track m_track ;                // track extractor 
      // ======================================================================
    };
    // ========================================================================
  } //                                         end of namespace LoKi::Particles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES12_H
// ============================================================================
