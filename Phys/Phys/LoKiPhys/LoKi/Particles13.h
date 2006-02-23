// $Id: Particles13.h,v 1.1 2006-02-23 21:14:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_PARTICLES13_H 
#define LOKI_PARTICLES13_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysExtract.h"
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-23 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  {
    
    /** @struct TrackChi2 
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
    struct TrackChi2 
      : public LoKi::Function<const LHCb::Particle*> 
    {
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
    
    /** @struct TrackChi2PerDoF 
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
    struct TrackChi2PerDoF 
      : public LoKi::Function<const LHCb::Particle*> 
    {
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

    
    /** @class TrackHasState
     *  The trivial funtion which evaluates LHCb::Track::hasSatteAt(...)
     *
     *  @see LHCb::Particle
     *  @see LHCb::Track
     *  @see LHCb::Track::hasStateAt
     *  @see LoKi::Cuts::TRNSTATES
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class TrackHasState
      : public LoKi::Predicate<const LHCb::Particle*> 
    {
    public:
      /// constructor from the state location 
      TrackHasState ( int location ) ;
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
      int m_location ;
    } ;
    
    /** @struct TrackType
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
    struct TrackType
      : public LoKi::Function<const LHCb::Particle*> 
    {
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
    

    /** @struct TrackHistory
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
    struct TrackHistory
      : public LoKi::Function<const LHCb::Particle*> 
    {
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

    /** @struct TrackHistoryFit
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
    struct TrackHistoryFit
      : public LoKi::Function<const LHCb::Particle*> 
    {
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
    
    /** @struct TrackStatus
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
    struct TrackStatus
      : public LoKi::Function<const LHCb::Particle*> 
    {
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

  } ; // end of namespace Particles
} ; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES12_H
// ============================================================================
