// $Id$
// ============================================================================
#ifndef LOKI_TRACKTYPES_H 
#define LOKI_TRACKTYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/BiFunctions.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::TrackTypes 
   *  helper namespace  to collect the types needed for "Track"-functions
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
   *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
   *
   *  @author Vanya BELAYEV ibelyaev@physics.syr.edu
   *  @date 2007-07-06
   */
  namespace TrackTypes 
  {
    // ========================================================================
    // the regular functors for Tracks
    // ========================================================================
    
    /// type of 'cuts' for Tracks       (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Predicate TrCuts ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Function  TrFunc ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::Assignable<TrCuts>::Type                     TrCut  ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::Assignable<TrFunc>::Type                     TrFun  ;
    
    /// type of 'map'       for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Map       TrMaps ;
    /// type of 'map'       for Tracks    (assignable)
    typedef LoKi::Assignable<TrMaps>::Type                     TrMap  ;
    
    /// type of 'pipe'      for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Pipe      TrPipes ;
    /// type of 'pipe'      for Tracks    (assignable)
    typedef LoKi::Assignable<TrPipes>::Type                    TrPipe  ;
    
    /// type of 'fun-val'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::FunVal    TrFunVals ;
    /// type of 'fun-val'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrFunVals>::Type                  TrFunVal  ;
    
    /// type of 'fun-val'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::CutVal    TrCutVals ;
    /// type of 'fun-val'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrCutVals>::Type                  TrCutVal  ;
    
    /// type of 'source'   for Tracks    (interface)
    typedef LoKi::BasicFunctors<const LHCb::Track*>::Source    TrSources  ;
    /// type of 'source'   for Tracks    (assignable)
    typedef LoKi::Assignable<TrSources>::Type                  TrSource   ;
    // ========================================================================
  } //                                        end of namespace LoKi::TrackTypes 
  // ==========================================================================
  namespace Types 
  {
    // ========================================================================
    // regular functions/predicates for tracks: "Tr"
    // ========================================================================
    /// type of 'cuts' for Tracks       (interface)
    typedef LoKi::TrackTypes::TrCuts              TrCuts  ;
    /// type of 'functions' for Tracks  (interface)
    typedef LoKi::TrackTypes::TrFunc              TrFunc  ;
    /// type of 'cuts' for Tracks       (assignable)
    typedef LoKi::TrackTypes::TrCut               TrCut   ;
    /// type of 'functions' for Tracks  (assignable)
    typedef LoKi::TrackTypes::TrFun               TrFun   ;

    // ========================================================================
    // functional part for Tracks :
    // ========================================================================

    /// type for "maps" for Track  (interface)
    typedef LoKi::TrackTypes::TrMaps     TrMaps    ;
    /// type for "maps" for Track  (assignable)
    typedef LoKi::TrackTypes::TrMap      TrMap     ;
    
    /// type for "pipes" for Track  (interface)
    typedef LoKi::TrackTypes::TrPipes    TrPipes   ;
    /// type for "pipes" for Track  (assignable)
    typedef LoKi::TrackTypes::TrPipe     TrPipe    ;
    
    /// type for "fun-vals" for Track  (interface)
    typedef LoKi::TrackTypes::TrFunVals  TrFunVals ;
    /// type for "fun-vals" for Track  (assignable)
    typedef LoKi::TrackTypes::TrFunVal   TrFunVal  ;
    
    /// type for "fun-vals" for Track  (interface)
    typedef LoKi::TrackTypes::TrCutVals  TrCutVals ;
    /// type for "fun-vals" for Track  (assignable)
    typedef LoKi::TrackTypes::TrCutVal   TrCutVal  ;
    
    /// type for "sources" for Track  (interface)
    typedef LoKi::TrackTypes::TrSources TrSources   ;
    /// type for "sources" for Track  (assignable)
    typedef LoKi::TrackTypes::TrSource  TrSource    ;
    
    // ========================================================================
  } //                                             end of namespace LoKi::Types 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  // the specialized printout 
  // ==========================================================================
  template <>
  std::ostream& 
  Constant<const LHCb::Track*,bool>::fillStream( std::ostream& s ) const ;
  // ==========================================================================  
} //                                                      end of namespace LoKi
// ============================================================================
#include "LoKi/Filters.h"
#include "LoKi/Dumper.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Functors 
  {
    // ========================================================================
    // the specific printout
    // ========================================================================
    template <>
    std::ostream& Empty<const LHCb::Track*>::fillStream ( std::ostream& s ) const ; 
    // ========================================================================
    // the specific printpout
    // ========================================================================
    template <>
    std::ostream& 
    Size<const LHCb::Track*>::fillStream ( std::ostream& s ) const  ;
    // ========================================================================
    template <>
    Dump_<const LHCb::Track*>::result_type 
    Dump_<const LHCb::Track*>::operator() 
      ( Dump_<const LHCb::Track*>::argument a ) const ;
    // ========================================================================
    template <>
    Dump1_<const LHCb::Track*,bool>::result_type 
    Dump1_<const LHCb::Track*,bool>::operator() 
      ( Dump1_<const LHCb::Track*,bool>::argument a ) const ;
    // ========================================================================
    template <>
    Dump1_<const LHCb::Track*,double>::result_type 
    Dump1_<const LHCb::Track*,double>::operator() 
      ( Dump1_<const LHCb::Track*,double>::argument a ) const ;
    // ========================================================================
  } //                                          end of namespace LoKi::Functors 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// a little bit of streamers 
// ============================================================================
GAUDI_API 
LHCb::Track::ConstVector
operator>> 
( const LHCb::Track::ConstVector&    input ,
  const LoKi::Types::TrCuts&         cuts  ) ;
// ============================================================================
GAUDI_API 
LHCb::Track::ConstVector
operator>> 
( const LHCb::Track::Range&          input ,
  const LoKi::Types::TrCuts&         cuts  ) ;
// ===========================================================================
GAUDI_API 
LHCb::Track::ConstVector
operator>> 
( const SmartRefVector<LHCb::Track>& input ,
  const LoKi::Types::TrCuts&         cuts  ) ;
// ===========================================================================
GAUDI_API 
LHCb::Track::ConstVector
operator>> 
( const LHCb::Track::Container*      input ,
  const LoKi::Types::TrCuts&         cuts  ) ;
// ===========================================================================
GAUDI_API 
LHCb::Track::ConstVector
operator>> 
( const LHCb::Track::Selection*      input ,
  const LoKi::Types::TrCuts&         cuts  ) ;
// ===========================================================================
// functions 
// ===========================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Track::ConstVector&    input ,
  const LoKi::Types::TrFunc&         cuts  ) ;
// ============================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Track::Range&          input ,
  const LoKi::Types::TrFunc&         cuts  ) ;
// ===========================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const SmartRefVector<LHCb::Track>& input ,
  const LoKi::Types::TrFunc&         cuts  ) ;
// ===========================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Track::Container*      input ,
  const LoKi::Types::TrFunc&         cuts  ) ;
// ===========================================================================
GAUDI_API 
std::vector<double>
operator>> 
( const LHCb::Track::Selection*      input ,
  const LoKi::Types::TrFunc&         cuts  ) ;
// ===========================================================================


// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TRACKTYPES_H
// ============================================================================
