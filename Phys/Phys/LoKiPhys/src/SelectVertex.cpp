// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Math.h"
#include "LoKi/Algs.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/SelectVertex.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::SelectVertex
 *  @see LoKi::SelectVertex
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
 *   with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 * 
 *  @date 2006-11-27 
 *  @author Vanya  BELYAEV ibelyaev@physics.syr.edu
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// Min
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMin 
( const LoKi::Types::VRange&                 vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Types::VRange::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMin 
( const LHCb::Vertex::Container*             vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::Vertex::Container::const_iterator ipv = 
    LoKi::select_min ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMin 
( const LHCb::RecVertex::Container*          vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::RecVertex::Container::const_iterator ipv = 
    LoKi::select_min ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
// Max
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMax
( const LoKi::Types::VRange&                 vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Types::VRange::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMax
( const LHCb::Vertex::Container*             vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::Vertex::Container::const_iterator ipv = 
    LoKi::select_max ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMax
( const LHCb::RecVertex::Container*          vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::RecVertex::Container::const_iterator ipv = 
    LoKi::select_max ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
}
// ============================================================================
// The END 
// ============================================================================
