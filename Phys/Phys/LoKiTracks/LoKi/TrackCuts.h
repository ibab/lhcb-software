// $Id$
// ============================================================================
#ifndef LOKI_TRACKCUTS_H
#define LOKI_TRACKCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTypes.h"
#include "LoKi/Tracks.h"
#include "LoKi/Primitives.h"
#include "LoKi/Monitoring.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    // Suppress Intel compiler warnings about missing default constructor
    // In this case the compiler generated constructor is fine, since there are
    // no member data to be initialised
#ifdef __INTEL_COMPILER
#pragma warning(disable:854)
#pragma warning(push)
#endif
    // ======================================================================== 
    /** @var TrTYPE 
     *  Simple function which evaluates LHCb::Track::type
     * 
     *  @code
     * 
     *    const LHCb::Track& t = ... ;
     *    
     *    TrCut isLong = LHCb::Track::Long == TrTYPE ;
     *
     *    const bool longTrack = isLong ( t ) ;
     *   
     *  @endcode 
     *  
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LoKi::Tracks::Type
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-27
     */
    const LoKi::Tracks::Type                                           TrTYPE ;
    // ========================================================================
    /** @var TrALL 
     *  trivial predicate which always return true 
     *
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::BasicFunctors<const LHCb::Track*>::BooleanConstant TrALL ( true ) ;
    // ========================================================================
    /** @var TrBACKWARD 
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool backward = TrBACKWARD ( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag         TrBACKWARD ( LHCb::Track::Backward ) ;
    // ========================================================================
    /** @var TrCHI2
     *  the simple function which returns LHCb::Track::Chi2
     *  @see LHCb::Track
     *  @see LHCb::Track::chi2
     *  @see LoKi::Tracks::Chi2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-27
     */
    const LoKi::Tracks::Chi2                                           TrCHI2 ; 
    // ========================================================================
    /** @var TrCHI2PDOF
     *  the simple function which returns LHCb::Track::chi2PerDoF
     *  @see LHCb::Track
     *  @see LHCb::Track::chi2PerDoF
     *  @see LoKi::Tracks::Chi2PerDoF
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-27
     */
    const LoKi::Tracks::Chi2PerDoF                                 TrCHI2PDOF ; 
    // ========================================================================
    /** @var TrCLONE
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool clone = TrCLONE ( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag              TrCLONE ( LHCb::Track::Clone ) ;
    // ========================================================================
    /** @typedef TrCOUNTER
     *  Simple monitoring counter for predicates
     *
     *  @code 
     *
     *  // some predicate to be monitored
     *  TrCut cut = ... ;
     *
     *  // Create the monitored predicate using the generic counter
     *  // either local counter, or the counter from IStatSvc or ICounterSvc
     *  StatEntity* counter = ... ; ///< the generic counter
     *  TrCut mon = TrCOUNTER ( cut , counter ) ;
     *
     *  for ( ... ) 
     *    {
     *      ...
     *      const LHCb::Track& t = ... ;
     *      const bool result = mon( t ) ; ///< use the predicate
     *      ...
     *    }
     *
     *  ...
     *  info () 
     *     << " Monitoring results : "                       << endreq 
     *     << " NEntries:  #" << counter->entries ()         << endreq 
     *     << " Efficiency:(" << counter->eff     ()   
     *     << "+="            << counter->effErr  () << ")%" << endreq ;
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored predicate:
     *  @code 
     *
     *  TrCut cut = ... ;
     *
     *  TrCut mon = monitor ( cut , counter("SomeEffCounter") ) ;
     *  
     *  @endcode 
     *
     *  The substitution of the predicate by monitored predicate 
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some predicate to be monitored
     *  TrCut cut = ... ;
     *
     *  if ( monitoring ) 
     *    {
     *       cut = monitor ( cut , "Efficiency1" ) ;
     *    }
     *
     *  @endcode
     *
     *
     *  @attention The string representation of the object 
     *             is delegated to the underlying prediate,  
     *             therefore the object is NOT recontructable 
     *             from its string representations. It is done 
     *             on purpose to avoid the disturbing of ids. 
     * 
     *  @see LoKi::Monitoring::Counter
     *  @see LoKi::monitor
     *  @see StatEntity
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */        
    typedef LoKi::Monitoring::Counter<const LHCb::Track*,bool>      TrCOUNTER ;
    // ========================================================================
    /** @var TrDOWNSTREAM
     *  simple predicate to check if the track has a type LHCb::Track::Downstream
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isDown = TrDOWNSTREAM ( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::Downstream
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrDOWNSTREAM ( TrTYPE , LHCb::Track::Downstream ) ;
    // ========================================================================
    /** @typedef TrEQUALTO
     *  helper function to check the equality of another function to some value
     *  @see LoKi::EqualToValue 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    typedef LoKi::EqualToValue<const LHCb::Track*,double>           TrEQUALTO ;
    // ======================================================================== 
    /** @var TrFALSE
     *  trivial predicate which always return false 
     *
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::BasicFunctors<const LHCb::Track*>::BooleanConstant TrFALSE ( false ) ;
    // ========================================================================
    /** @typedef TrFILTER  
     *  Simple predicate (wrapper) for ITrackSelector tool
     *  
     *  @code 
     * 
     *    const LHCb::Track&    track = ... ;
     *    // create the function:
     *    const TrCut accept = TrFILTER   ( "mytooltypename") ;
     *    // use it!
     *    const bool OK = accept ( track ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::Filter
     *  @see LoKi::Cuts::TrSELECTOR
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-01-20
     */
    typedef LoKi::Tracks::Filter                                   TrFILTER ;
    // ========================================================================
    /** @typedef TrHASINFO 
     *  Trivial predicate which evaluates LHCb::Track::hasInfo
     *  function
     *
     *  @code 
     * 
     *  const LHCb::Track& t = ... ;
     *  TrCut hasIndex = TrHASINFO( <index> ) ;
     * 
     *  const bool good = hasIndex( p ) ;
     *
     *  @endcode 
     *
     * 
     *  @see LHCb::Track
     *  @see LoKi::Tracks::HasInfo
     *  @see LoKi::ExtraInfo::CheckInfo
     *  @see LoKi::ExtraInfo::hasInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */    
    typedef LoKi::Tracks::HasInfo                                   TrHASINFO ;
    // ========================================================================
    /** @var TrHASSTATE
     *  Simple predicate which evaluates LHCb::Track::hasStateAt
     *
     *  @code 
     * 
     *  const LHCb::Track& t = ... ;
     *  TrCut hasAtTT = TeHASSTATE ( LHCb::State::AtTT ) ;
     * 
     *  const bool good = hasAtTT ( p ) ;
     *
     *  @endcode 
     *  @see LHCb::Track
     *  @see LHCb::Track::hasStateAt
     *  @see LHCb::State
     *  @see LHCb::State::Location
     *  @see LoKi::Tracks::HasStateAt
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */    
    typedef LoKi::Tracks::HasStateAt                               TrHASSTATE ;
    // ========================================================================
    /** @var TrINTES
     *  trivial predicate which checks if the object registered in TES
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const bool inTES = TrINTES ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::InTES
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::InTES                                         TrINTES ;
    // ========================================================================
    /** @typedef TrINFO 
     *  Trivial function which evaluates LHCb::Track::info
     *  
     *  @code 
     * 
     *  const LHCb::Track& t = ... ;
     *
     *  const int index = ... ;
     *
     *  TrFun info = TrINFO( index , -1000 ) ;
     * 
     *  const double result = info( t ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::info
     *  @see LoKi::ExtraInfo::GetInfo
     *  @see LoKi::ExtraInfo::info
     *  @see LoKi::Tracks::Info
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    typedef LoKi::Tracks::Info                                         TrINFO ;
    // ========================================================================
    /** @var TrINVALID
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool invalid = TrINVALID ( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag          TrINVALID ( LHCb::Track::Invalid ) ;
    // ========================================================================
    /** @var TrIPSELECTED
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool ok = TrIPSELECTED ( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag    TrIPSELECTED ( LHCb::Track::IPSelected ) ;
    // ========================================================================
    /** @typedef TrISFLAG
     *  Check is the track has a given flag:
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   // create the predicate:
     *   const TrCut isClone = TrISFLAG ( LHCb::Track::Clone ) ;
     *
     *   cons bool clone = isClone ( track ) ;
     *
     *  @endcode
     *  
     *  @see LoKi::Cuts::TrBACKWARD
     *  @see LoKi::Cuts::TrINVALID
     *  @see LoKi::Cuts::TrCLONE
     *  @see LoKi::Cuts::TrUSED
     *  @see LoKi::Cuts::TrIPSELECTED
     *  @see LoKi::Cuts::TrPIDSELECTED
     *  @see LoKi::Cuts::TrSELECTED
     *  @see LoKi::Cuts::TrL0CANDIDATE
     *  @see LoKi::Tracks::Momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    typedef LoKi::Tracks::CheckFlag                                  TrISFLAG ;
    // ========================================================================
    /** @typedef TrISONTRACK
     *  Simple predicate whcih evaluates LHCb::Track::isOnTrack
     * 
     *  @code
     *
     *   const LHCb::Track& t = ... ;
     *  
     *   const LHCb::LHCbID& id = ... ;
     *   TrCut ok = TrISONTRACK ( id ) ;
     *
     *   const bool good = ok ( t ) ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-27
     */
    typedef LoKi::Tracks::IsOnTrack                               TrISONTRACK ;
    // ========================================================================
    /** @var TrKEY
     *  trivial estimator of the key 
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const double key = TrKEY ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::Key
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::Key                                             TrKEY ;
    // ========================================================================
    /** @var TrL0CANDIDATE
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool ok = TrL0CANDIDATE( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag  TrL0CANDIDATE ( LHCb::Track::L0Candidate ) ;
    // ========================================================================
    /** @var TrLONG
     *  simple predicate to check if the track has a type LHCb::Track:Long
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isLong = TrLONG ( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::Long
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrLONG ( TrTYPE , LHCb::Track::Long ) ;
    // ========================================================================
    /** @var TrMUON
     *  simple predicate to check if the track has a type LHCb::Track:Muon
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isMuon = TrMUON ( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::Muon
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrMUON ( TrTYPE , LHCb::Track::Muon ) ;
    // ========================================================================
    /** @var TrNONE
     *  trivial predicate which always return false 
     *
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::BasicFunctors<const LHCb::Track*>::BooleanConstant TrNONE ( false ) ;
    // ========================================================================
    /** @var TrONE 
     *  trivial function which always return 1 
     *
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::BasicFunctors<const LHCb::Track*>::Constant       TrONE ( 1 ) ;
    // ======================================================================== 
    /** @var TrP
     *  trivial estimator of the momentum of the track
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const double p = TrP ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::Momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::Momentum                                          TrP ;
    // ========================================================================
    /** @var TrPX
     *  trivial estimator of the x-component momentum of the track
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const double px = TrPX ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::Momentum
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-18
     */
    const LoKi::Tracks::MomentumX                                       TrPX ;
    // ========================================================================
    /** @var TrPY
     *  trivial estimator of the y-component momentum of the track
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const double py = TrPY ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::MomentumY
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-18
     */
    const LoKi::Tracks::MomentumY                                       TrPY ;
    // ========================================================================
    /** @var TrPZ
     *  trivial estimator of the z-component momentum of the track
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const double pz = TrPZ ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::MomentumZ
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-18
     */
    const LoKi::Tracks::MomentumZ                                       TrPZ ;
    // ========================================================================
    /** @var TrPIDSELECTED
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool ok = TrPIDSELECTED ( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag  TrPIDSELECTED ( LHCb::Track::PIDSelected ) ;
    // ========================================================================
    /** @typedef TrPLOT
     *  Simple monitoring histogram for the functions
     *
     *  @code 
     *
     *  // some function to be monitored
     *  TrFun fun = ... ;
     *
     *  // Create the monitored function using the histogram:
     *  AIDA::IHistogram1D* histo = ... ;
     *  TrFun mon = PLOT ( fun , histo ) ;
     *
     *  for ( ... ) 
     *    {
     *      ...
     *      const LHCb::Track& t = ... ;
     *      const double result = mon( t ) ; ///< use the function
     *      ...
     *    }
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored function
     *  @code 
     *
     *  // some function to be monitored
     *  TrFun fun = ... ;
     *
     *  AIDA::IHistogram1D* histo = ... ;
     *  TrFun mon = monitor ( fun , histo ) ;
     *  
     *  @endcode 
     *
     *  The substitution of the function by monitored function 
     *  cound be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some function to be monitored
     *  TrFun fun = ... ;
     *
     *  if ( monitoring ) 
     *    {
     *       AIDA::IHistogram1D* histo = ... ;
     *       fun = monitor ( fun , histo ) ;
     *    }
     *
     *  @endcode
     * 
     *  @attention The string representation of the object 
     *             is delegated to the underlying function,  
     *             therefore the object is NOT recontructable 
     *             from its string representations. It is done 
     *             on purpose to avoid the disturbing of ids. 
     * 
     *  @see LoKi::Monitoring::Plot
     *  @see LoKi::monitor
     *  @see AIDA::IHistogram1D
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */        
    typedef LoKi::Monitoring::Plot<const LHCb::Track*,double>          TrPLOT ;
    // ========================================================================
    /** @var TrPROBCHI2
     *  the simple function which returns LHCb::Track::probChi2
     *  @see LHCb::Track
     *  @see LHCb::Track::probChi2
     *  @see LoKi::Tracks::ProbChi2
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-07-27
     */
    const LoKi::Tracks::ProbChi2                                   TrPROBCHI2 ; 
    // ========================================================================
    /** @var TrPT
     *  trivial estimator of the tarnsverse momentum of the track
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const double pt = TrPT ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::TransverseMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::GhostProb                                   TrGHOSTPROB ; 
    // ========================================================================
    /** @var TrGHOSTPROB
     *  trivial estimator of the ghost probability of the track
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const double pt = TrGHOSTPROB ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::GhostProb
     *  @author Sascha Stahl sascha.stahl@cern.ch
     *  @date   2016-01-12
     */
    const LoKi::Tracks::TransverseMomentum                               TrPT ;
    // =========================================================================
    /** @var TrHAST
     *  Check if track is of a type that goes thro T
     
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const bool ok = TrHAST ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::HasT
     *  @see LHCb::Track::hasT
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-18
     */
    const LoKi::Tracks::HasT                                           TrHAST ;
    // ========================================================================
    /** @var TrHASVELO
     *  Check if track is of a type that goes thro Velo
     
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const bool ok = TrHASVELO ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::HasVelo
     *  @see LHCb::Track::hasVelo
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-18
     */
    const LoKi::Tracks::HasVelo                                     TrHASVELO ;
    // ========================================================================
    /** @var TrHASTT
     *  Check if track is of a type that goes thro TT
     
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const bool ok = TrHASTT ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::HasTT
     *  @see LHCb::Track::hasTT 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-03-18
     */
    const LoKi::Tracks::HasTT                                         TrHASTT ;
    // ========================================================================
    /** @var TrQ
     *  trivial estimator of the charge of the track 
     *
     *  @code
     *
     *   const LHCb::Track& track = ... ;
     *
     *   const bool positive = 0 < TrQ ( track ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Tracks::Charge
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::Tracks::Charge                                            TrQ ;
    // ========================================================================
    /** @var TrSELECTED
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool ok = TrSELECTED ( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag        TrSELECTED ( LHCb::Track::Selected ) ;
    // ========================================================================
    /** @typedef TrSELECTOR
     *  Simple predicate (wrapper) for ITrackSelector tool
     *  
     *  @code 
     * 
     *    const LHCb::Track&    track = ... ;
     *    // get the tool: 
     *    const ITrackSelector* tool = ... ;
     *    //create the function:
     *    const TrCut accept = TrSELECTOR ( tool ) ;
     *    // use it!
     *    const bool OK = accept ( track ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::Selector
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    typedef LoKi::Tracks::Selector                                 TrSELECTOR ;
    // ========================================================================
    /** @typedef TrSINFO 
     *  "Smart-Extra-Info" function, 
     *  it checks the information with the given key in "Extra Info",
     *  for the missing information the supplied function will 
     *  be evaluated and (optinaly) the informatin will be added   
     *  to "extra info"
     *  
     *  @code 
     * 
     *  const LHCb::Track& t = ... ;
     *
     *  const int index = ... ;
     *
     *  const TrFrun& fun = ... ;                   
     *
     *  TrFun sinfo = TrSINFO( index , fun , true ) ;
     * 
     *  const double result = sinfo( t ) ;
     *
     *  @endcode 
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::info
     *  @see LoKi::ExtraInfo::GetSmartInfo
     *  @see LoKi::ExtraInfo::info
     *  @see LoKi::ExtraInfo::hasInfo
     *  @see LoKi::ExtraInfo::addInfo
     *  @see LoKi::Tracks::SmartInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-14
     */
    typedef LoKi::Tracks::SmartInfo                                   TrSINFO ;
    // ========================================================================
    /** @typedef TrSSWITCH
     *  Simple "switch"
     *  The function evaluates the predicate and returns 
     *  one of the predefined values, depending on result on 
     *  predicate evaluation.
     *  In some sense it is a conversion of "predicate" to "function"
     * 
     *  @code
     *
     *   const LHCb::Track& = ...  ;
     *
     *   TrFun fun = TrSSWITCH( TrPT > 1000 , 1 , -1 ) 
     *
     *   const double value = fun( p ) ;
     *
     *  @endcode 
     *
     *  For this example function returns 1 if track has Pt>1GeV 
     *  and -1 otherwise 
     *
     *  @see LoKi::SipleSwitch 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    typedef LoKi::SimpleSwitch<const LHCb::Track*>                  TrSSWITCH ;
    // ========================================================================    
    /** @typedef TrSTAT
     *  Simple monitoring counter for the functions
     *
     *  @code 
     *
     *  // some function to be monitored
     *  TrFun fun = ... ;
     *
     *  // Create the monitored function using the generic counter
     *  // either local counter, or the counter from IStatSvc or ICounterSvc
     *  StatEntity* counter = ... ; ///< the generic counter
     *  TrFun mon = TrSTAT ( fun , counter ) ;
     *
     *  for ( ... ) 
     *    {
     *      ...
     *      const LHCb::Track& t = ... ;
     *      const double result = mon( t ) ; ///< use the function
     *      ...
     *    }
     *
     *  ...
     *  info () 
     *     << " Monitoring results : "                 << endreq 
     *     << " NEntries:  #" << counter->entries  ()  << endreq 
     *     << " TotalSum:   " << counter->flag     ()  << endreq 
     *     << " Mean+-RMS:  " << counter->flagMean () 
     *     << "+="            << counter->flagRMS  ()  << endreq      
     *     << " Min/Max:    " << counter->flagMin  ()  
     *     << "/"             << counter->flagMax  ()  << endreq ;
     *
     *  @endcode
     *
     *  The alternative (recommended) way to create the monitored function
     *  @code 
     *
     *  // some function to be monitored
     *  TrFun fun = ... ;
     *
     *  TrFun mon = monitor ( fun , counter("SomeCounter") ) ;
     *  
     *  @endcode 
     *
     *  The substitution of the function by monitored function 
     *  could be done "on-flight" without the disturbing of the
     *  actual processing:
     *  @code
     *
     *  // some function to be monitored
     *  TrFun fun = ... ;
     *
     *  if ( monitoring ) 
     *    {
     *       fun = monitor ( fun , "MonitoredFun" ) ;
     *    }
     *
     *  @endcode
     *
     *  @attention The string representation of the object 
     *             is delegated to the underlying function,  
     *             therefore the object is NOT recontructable 
     *             from its string representations. It is done 
     *             on purpose to avoid the disturbing of ids. 
     * 
     *  @see LoKi::Monitoring::Counter
     *  @see LoKi::monitor
     *  @see StatEntity
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */        
    typedef LoKi::Monitoring::Counter<const LHCb::Track*,double>       TrSTAT ;
    // ========================================================================
    /** @typedef TrSWITCH
     *  "switch"
     *  The function evaluates the predicate and returns 
     *  the values of one of the predefined functions, 
     *  depending on result on predicate evaluation.
     * 
     *  @code
     *
     *
     *   const LHCb::Track& = ...  ;
     *
     *   TrFun fun = TrSWITCH( TrQ >= 0 , TrPT , -1*TrPT ) 
     *
     *   const double value = fun( p ) ;
     *
     *  @endcode 
     *
     *  For this example function returns pt  for tracks of positive charge 
     *  and -1*pt otherwise 
     *  @see LoKi::Cuts::TrQ 
     *  @see LoKi::Cuts::TrPT 
     *  @see LoKi::Switch 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    typedef LoKi::Switch<const LHCb::Track*>                         TrSWITCH ;
    // ========================================================================
    /** @var TrTRUE
     *  trivial predicate which always return true 
     *
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::BasicFunctors<const LHCb::Track*>::BooleanConstant TrTRUE ( true ) ;
    // ========================================================================
    /** @var TrTTRACK
     *  simple predicate to check if the track has a type LHCb::Track::Ttrack
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isT = TrTTRACK( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::Ttrack
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrTTRACK ( TrTYPE , LHCb::Track::Ttrack ) ;
    // ========================================================================
    /** @var TrUNKNOWN
     *  simple predicate to check if the track has a type LHCb::Track::TypeUnknown
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isUnknown = TrUNKNOWN ( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::TypeUnknown
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrUNKNOWN ( TrTYPE , LHCb::Track::TypeUnknown ) ;
    // ========================================================================
    /** @var TrUPSTREAM
     *  simple predicate to check if the track has a type LHCb::Track::Upstream
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isUp = TrUPSTREAM ( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::Upstream
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrUPSTREAM ( TrTYPE , LHCb::Track::Upstream ) ;
    // ========================================================================
    /** @var TrUSED
     *  simple objects which check the "Flag" of the track:
     *
     *  @code 
     *   
     *    const LHCb::Track& track = ... ;
     * 
     *    const bool used = TrUSED ( track ) ;
     *
     *  @endcode 
     *  @see LoKi::Cuts::TrISFLAG
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::Tracks::CheckFlag                TrUSED ( LHCb::Track::Used ) ;
    // ========================================================================
    /** @var TrVELO 
     *  simple predicate to check if the track has a type LHCb::Track:Velo
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isVelo = TrVELO ( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::Velo
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrVELO ( TrTYPE , LHCb::Track::Velo ) ;
    // ========================================================================
    /** @var TrVELOR
     *  simple predicate to check if the track has a type LHCb::Track:VeloR
     *
     *  @code
     *
     *  const LHCb::Track&t = ... ;
     *  const bool isVeloR = TrVELOR ( t ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::Track::type
     *  @see LHCb::Track::Types
     *  @see LHCb::Track::VeloR
     *  @see LoKi::Cuts::TrTYPE
     *  @see LoKi::EqualToValue
     *  @see LoKi::Tracks::CheckFlag
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */     
    const LoKi::EqualToValue<const LHCb::Track*,double> 
    TrVELOR ( TrTYPE , LHCb::Track::VeloR ) ;
    // ========================================================================
    /** @var TrZERO
     *  trivial function which always return 0 
     *
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-08
     */
    const LoKi::BasicFunctors<const LHCb::Track*>::Constant TrZERO ( 0 ) ;
    // ========================================================================
    /** @typedef TrSTATEZ 
     *  simple evaluator for Z-position of the certain state 
     *  @code
     * 
     *  const TrSTATEZ fun = TrSTATEZ ( LHCb::State::FirstMeasurement ) ;
     *  const LHCb::Track&t = ... ;
     *
     *  const double value =  fun ( t ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::StateZ
     *  @see LoKi::Cuts::TsFIRSTHITZ
     *
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    typedef LoKi::Tracks::StateZ                                     TrSTATEZ ;
    // ========================================================================
    /** @var TrFIRSTHITZ 
     *  simple evaluator for Z-position of the firts measuremnet 
     *  @code
     * 
     *  const LHCb::Track&t = ... ;
     *
     *  const double value =  TrFIRSTHITZ ( t ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::StateZ
     *  @see LoKi::Cuts::TsSTATEZ 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-02
     */
    const LoKi::Tracks::StateZ TrFIRSTHITZ ( LHCb::State::FirstMeasurement ) ;
    // ========================================================================
    /** @typedef TrCOV2 
     *  Get the element of track covariance matrix 
     *
     *  @code 
     *
     *    // use th first state 
     *    const TrCOV2 covXX_1 = TrCOV2( 0 , 0  ) ;
     *
     *    // use the specific state:
     *    const TrCOV2 covXX_2 = TrCOV2(  LHCb::State::AtTT  , 0 , 0  ) ; 
     *
     *    // use the state,closest to some z : 
     *    const double z = ... ;
     *    const TrCOV2 covXX_3 = TrCOV2(  z  , 0 , 0  ) ; 
     *
     *    const LHCb::Track* track  = ... ;
     * 
     *    const double c1 = covXX_1 ( track ) ;
     *    const double c2 = covXX_2 ( track ) ;
     *    const double c3 = covXX_3 ( track ) ;
     *
     *  @endcode
     *
     *  @see LHCb::Track
     *  @see LHCb::State
     *  @see LHCb::State
     *  @see Gaudi::TrackSymMatrix
     *  @see LoKi::Tracks::Cov2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cer.ch
     *  @date 2010-12-09
     */
    typedef LoKi::Tracks::Cov2                                         TrCOV2 ;
    // ========================================================================
    /** @var TrPATQUALITY 
     *  Accessor to LHCb::Track::PatQuality
     */
    const LoKi::Tracks::Info TrPATQUALITY ( LHCb::Track::PatQuality , -1000 ) ;
    // ========================================================================
    /** @var TrMATCHCHI2
     *  Accessor to LHCb::Track::MatchChi2
     */
    const LoKi::Tracks::Info TrMATCHCHI2 ( LHCb::Track::MatchChi2 , -1000 ) ;
    // ========================================================================
    /** @var TrFITVELOCHI2
     *  Accessor to LHCb::Track::FitVeloChi2
     */
    const LoKi::Tracks::Info TrFITVELOCHI2 ( LHCb::Track::FitVeloChi2 , -1000 ) ;
    // ========================================================================
    /** @var TrFITVELONDOF
     *  Accessor to LHCb::Track::FitVeloNDof
     */
    const LoKi::Tracks::Info TrFITVELONDOF ( LHCb::Track::FitVeloNDoF , -1000 ) ;
    // ========================================================================
    /** @var TrFITTCHI2
     *  Accessor to LHCb::Track::FitTChi2
     */
    const LoKi::Tracks::Info TrFITTCHI2 ( LHCb::Track::FitTChi2 , -1000 ) ;
    // ========================================================================
    /** @var TrFITTNDOF
     *  Accessor to LHCb::Track::FitTNDof
     */
    const LoKi::Tracks::Info TrFITTNDOF ( LHCb::Track::FitTNDoF , -1000 ) ;
    // ========================================================================
    /** @var TrFITMATCHCHI2
     *  Accessor to LHCb::Track::FitMatchChi2
     */
    const LoKi::Tracks::Info TrFITMATCHCHI2 ( LHCb::Track::FitMatchChi2 , -1000 ) ;
    // ========================================================================
    /** @var TrTSALIKELIHOOD
     *  Accessor to LHCb::Track::TsaLikelihood
     */
    const LoKi::Tracks::Info TrTSALIKELIHOOD ( LHCb::Track::TsaLikelihood , -1000 ) ;
    // ========================================================================
    /** @var TrCLONEDIST
     *  Accessor to LHCb::Track::CloneDist
     */
    const LoKi::Tracks::Info TrCLONEDIST ( LHCb::Track::CloneDist , 1 * Gaudi::Units::km ) ;
    // ========================================================================
    /** @var TrNVELOMISS 
     *  @see Hlt::MissedVeloHits
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-01-28
     */
    const LoKi::Tracks::NVeloMissed                               TrNVELOMISS ;
    // ========================================================================
    /** @var TrNTHITS 
     *  get effective number of T-hits for Johannes:  2x#IT + #OT
     *  @see LoKi::Tracks::NTHits
     *  @see LoKi::Cuts::TrTNORMIDC
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-02-02
     */
    const LoKi::Tracks::NTHits                                       TrNTHITS ;
    // ========================================================================
    /** @var TrNTHITS 
     *  get effective number of T-hits for Vava:  2x#IT + #OT
     *  @see LoKi::Tracks::NTHits
     *  @see LoKi::Cuts::TrNTHITS
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-02-02
     */
    const LoKi::Tracks::NTHits                                     TrTNORMIDC ;
    // ========================================================================
    /** @typedef  Tr_FASTDOCATOBEAMLINE
     *  Fast DOCA to beam line using TTrDOCA and BEAMSPOT, similar to the
     *  BEAMSPOTRHO cut
     *  @see LoKi::Cuts::TTrDOCA
     *  @see HltUtils::closestDistanceMod
     *  @see LoKi::Tracks::DistanceOfClosestApproach
     *  @see LoKi::BeamSpot
     *  @see LoKi::Cuts::BEAMSPOT
     *  @see LoKi::Vertices::BeamSpotRho
     *  @see LoKi::Cuts::BEAMSPOTRHO
     *  @author Pieter David pieter.david@cern.ch
     *  @date 2012-02-24
     */
    typedef LoKi::Tracks::FastDOCAToBeamLine            Tr_FASTDOCATOBEAMLINE ;
    // ========================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop) 
#endif
    // ========================================================================
 } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRACKCUTS_H
// ============================================================================
