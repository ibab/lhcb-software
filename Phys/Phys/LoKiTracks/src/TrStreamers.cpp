// $Id: $
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <set>
#include <algorithm>
#include <functional>
// ============================================================================
// local
// ============================================================================
#include "LoKi/TrackTypes.h"
// ============================================================================
/** @file
 *
 *  implementation of some streamers
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
 *  with the smear campaign of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-05-24
 *
 *                    $Revision: 124207 $
 *  Last modification $Date: 2011-06-02 18:05:29 +0200 (Thu, 02 Jun 2011) $
 *                 by $Author: ibelyaev $
 */
// ============================================================================
namespace
{
  // ==========================================================================
  template <class TRACK>
  inline
  LHCb::Track::ConstVector
  _get_ ( TRACK                      first ,
          TRACK                      last  ,
          const LoKi::Types::TrCuts& cuts  )
  {
    //
    LHCb::Track::ConstVector trks ; trks.reserve ( last - first ) ;
    std::copy_if ( first ,
                   last  ,
                   std::back_inserter ( trks ) , std::cref(cuts) ) ;
    //
    return trks ;
  }
  // ==========================================================================
  template <class TRACKS>
  inline
  LHCb::Track::ConstVector
  _get_ ( const TRACKS*              tracks ,
          const LoKi::Types::TrCuts& cuts   )
  {
    //
    LHCb::Track::ConstVector trks ;
    if ( !tracks || tracks->empty() ) { return trks ; }
    //
    trks.reserve ( tracks -> size  () ) ;
    std::copy_if ( tracks -> begin () ,
                   tracks -> end   () ,
                   std::back_inserter ( trks ) , std::cref(cuts) ) ;
    //
    return trks ;
  }
  // ==========================================================================
  template <class TRACK>
  inline
  std::vector<double>
  _get_ ( TRACK                      first ,
          TRACK                      last  ,
          const LoKi::Types::TrFunc& func  )
  {
    //
    std::vector<double> r ; r.reserve ( last - first ) ;
    //
    LoKi::apply ( first ,
                  last  , func , std::back_inserter ( r ) ) ;
    //
    return r ;
  }
  // ==========================================================================
  template <class TRACKS>
  inline
  std::vector<double>
  _get_ ( const TRACKS*              tracks ,
          const LoKi::Types::TrFunc& func   )
  {
    std::vector<double> r ;
    if ( 0 == tracks || tracks -> empty() ) { return r ; }
    //
    r.reserve   ( tracks->size() ) ;
    //
    LoKi::apply ( tracks -> begin ()  ,
                  tracks -> end   ()  , func , std::back_inserter ( r ) ) ;
    //
    return r ;
  }
  // ==========================================================================
}
// ============================================================================
LHCb::Track::ConstVector
operator>>
( const LHCb::Track::ConstVector&    input ,
  const LoKi::Types::TrCuts&         cuts  )
{ return _get_ ( input.begin() , input.end() , cuts ) ; }
// ============================================================================
LHCb::Track::ConstVector
operator>>
( const LHCb::Track::Range&          input ,
  const LoKi::Types::TrCuts&         cuts  )
{ return _get_ ( input.begin() , input.end() , cuts ) ; }
// ===========================================================================
LHCb::Track::ConstVector
operator>>
( const SmartRefVector<LHCb::Track>& input ,
  const LoKi::Types::TrCuts&         cuts  )
{ return _get_ ( input.begin() , input.end() , cuts ) ; }
// ===========================================================================
LHCb::Track::ConstVector
operator>>
( const LHCb::Track::Container*      input ,
  const LoKi::Types::TrCuts&         cuts  )
{ return _get_ ( input , cuts ) ; }
// ===========================================================================
LHCb::Track::ConstVector
operator>>
( const LHCb::Track::Selection*      input ,
  const LoKi::Types::TrCuts&         cuts  )
{ return _get_ ( input , cuts ) ; }
// ===========================================================================
// functions
// ===========================================================================
std::vector<double>
operator>>
( const LHCb::Track::ConstVector&    input ,
  const LoKi::Types::TrFunc&         cuts  )
{ return _get_ ( input.begin() , input.end() , cuts ) ; }
// ============================================================================
std::vector<double>
operator>>
( const LHCb::Track::Range&          input ,
  const LoKi::Types::TrFunc&         cuts  )
{ return _get_ ( input.begin() , input.end() , cuts ) ; }
// ===========================================================================
std::vector<double>
operator>>
( const SmartRefVector<LHCb::Track>& input ,
  const LoKi::Types::TrFunc&         cuts  )
{ return _get_ ( input.begin() , input.end() , cuts ) ; }
// ===========================================================================
std::vector<double>
operator>>
( const LHCb::Track::Container*      input ,
  const LoKi::Types::TrFunc&         cuts  )
{ return _get_ ( input , cuts ) ; }
// ===========================================================================
std::vector<double>
operator>>
( const LHCb::Track::Selection*      input ,
  const LoKi::Types::TrFunc&         cuts  )
{ return _get_ ( input , cuts ) ; }
// ===========================================================================

// ============================================================================
// The END
// ============================================================================
