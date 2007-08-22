// $Id: SelectVertex.cpp,v 1.3 2007-08-22 18:16:51 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/select.h"
#include "LoKi/Functions.h"
#include "LoKi/Math.h"
#include "LoKi/Algs.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/SelectVertex.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::SelectVertex
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
 *  <b>"Python-based Interactive Environment for Smart and Friendly 
 *   Physics Analysis"</b>
 *
 *  The package has been designed with the kind help from
 *  Pere MATO and Andrey TSAREGORODTSEV. 
 *  And it is based on the 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  "C++ ToolKit for Smart and Friendly Physics Analysis"
 *
 *  By usage of this code one clearly states the disagreement 
 *   with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 * 
 *  @date 2006-11-27 
 *  @author Vanya  BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
LHCb::VertexBase*               LoKi::SelectVertex::selectMin 
( const LHCb::VertexBase::Vector&            vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::VertexBase::Vector::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMin 
( const LHCb::VertexBase::ConstVector&       vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::VertexBase::ConstVector::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMin 
( const LHCb::VertexBase::Container*         vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::VertexBase::Vector::const_iterator ipv = 
    LoKi::select_min ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMin 
( const LoKi::Keeper<LHCb::VertexBase>&      vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Keeper<LHCb::VertexBase>::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMin 
( const LoKi::UniqueKeeper<LHCb::VertexBase>&vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::UniqueKeeper<LHCb::VertexBase>::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
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

// ============================================================================
LHCb::Vertex*               LoKi::SelectVertex::selectMin 
( const LHCb::Vertex::Vector&                vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::Vertex::Vector::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMin 
( const LHCb::Vertex::ConstVector&           vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::Vertex::ConstVector::const_iterator ipv = 
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
  LHCb::Vertex::Vector::const_iterator ipv = 
    LoKi::select_min ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMin 
( const LoKi::Keeper<LHCb::Vertex>&          vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Keeper<LHCb::Vertex>::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMin 
( const LoKi::UniqueKeeper<LHCb::Vertex>&    vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::UniqueKeeper<LHCb::Vertex>::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================


// ============================================================================
LHCb::RecVertex*               LoKi::SelectVertex::selectMin 
( const LHCb::RecVertex::Vector&             vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::RecVertex::Vector::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMin 
( const LHCb::RecVertex::ConstVector&        vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::RecVertex::ConstVector::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMin 
( const LHCb::RecVertex::Container*          vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::RecVertex::Vector::const_iterator ipv = 
    LoKi::select_min ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMin 
( const LoKi::Keeper<LHCb::RecVertex>&       vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Keeper<LHCb::RecVertex>::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMin 
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::UniqueKeeper<LHCb::RecVertex>::const_iterator ipv = 
    LoKi::select_min ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================

// ============================================================================
LHCb::VertexBase*               LoKi::SelectVertex::selectMax 
( const LHCb::VertexBase::Vector&            vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::VertexBase::Vector::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMax 
( const LHCb::VertexBase::ConstVector&       vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::VertexBase::ConstVector::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMax 
( const LHCb::VertexBase::Container*         vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::VertexBase::Vector::const_iterator ipv = 
    LoKi::select_max ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMax 
( const LoKi::Keeper<LHCb::VertexBase>&      vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Keeper<LHCb::VertexBase>::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::VertexBase*               LoKi::SelectVertex::selectMax 
( const LoKi::UniqueKeeper<LHCb::VertexBase>&vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::UniqueKeeper<LHCb::VertexBase>::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
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

// ============================================================================
LHCb::Vertex*               LoKi::SelectVertex::selectMax 
( const LHCb::Vertex::Vector&                vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::Vertex::Vector::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMax 
( const LHCb::Vertex::ConstVector&           vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::Vertex::ConstVector::const_iterator ipv = 
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
  LHCb::Vertex::Vector::const_iterator ipv = 
    LoKi::select_max ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMax 
( const LoKi::Keeper<LHCb::Vertex>&          vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Keeper<LHCb::Vertex>::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::Vertex*               LoKi::SelectVertex::selectMax 
( const LoKi::UniqueKeeper<LHCb::Vertex>&    vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::UniqueKeeper<LHCb::Vertex>::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================


// ============================================================================
LHCb::RecVertex*               LoKi::SelectVertex::selectMax 
( const LHCb::RecVertex::Vector&             vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::RecVertex::Vector::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMax 
( const LHCb::RecVertex::ConstVector&        vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LHCb::RecVertex::ConstVector::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMax 
( const LHCb::RecVertex::Container*          vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  if ( 0 == vs ) { return 0 ; }
  LHCb::RecVertex::Vector::const_iterator ipv = 
    LoKi::select_max ( vs->begin() , vs->end() , fun , cut ) ;
  return vs->end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMax 
( const LoKi::Keeper<LHCb::RecVertex>&       vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::Keeper<LHCb::RecVertex>::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================
const LHCb::RecVertex*               LoKi::SelectVertex::selectMax 
( const LoKi::UniqueKeeper<LHCb::RecVertex>& vs       , 
  const LoKi::Types::VFunc&                  fun      , 
  const LoKi::Types::VCuts&                  cut      ) 
{
  LoKi::UniqueKeeper<LHCb::RecVertex>::const_iterator ipv = 
    LoKi::select_max ( vs.begin() , vs.end() , fun , cut ) ;
  return vs.end() == ipv ? 0 : *ipv ;
} 
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
