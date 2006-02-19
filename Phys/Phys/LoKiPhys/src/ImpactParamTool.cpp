// $Id: ImpactParamTool.cpp,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IGeomDispCalculator.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/InstanceCounter.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Exception.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/ImpactParamTool.h"
// ============================================================================

// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Vertices::VertexHolder 
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
 *  @date 2006-02-16 
 */
// ============================================================================

// ============================================================================
namespace
{
  InstanceCounter<LoKi::Vertices::ImpactParamTool> s_IPTCounter ;
};
// ============================================================================

// ============================================================================
LoKi::Vertices::ImpactParamTool::ImpactParamTool 
( IGeomDispCalculator* tool ) 
  : LoKi::AuxFunBase () 
  , m_tool           ( tool )
{
  if ( 0 == m_tool ) 
  { throw LoKi::Exception("ImpactParamTool():  Tool* points to NULL "  ,
                          StatusCode::FAILURE , __FILE__ , __LINE__ ); }
  m_tool -> addRef  ();
  //
#ifdef LOKI_DEBUG
  s_IPTCounter.increment();
#endif 
  //
};
// ============================================================================

// ============================================================================
/// copy 
// ============================================================================
LoKi::Vertices::ImpactParamTool::ImpactParamTool
( const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::AuxFunBase ( tool ) 
  , m_tool           ( tool.m_tool )
{
  if( 0 == m_tool ) 
    { throw LoKi::Exception
        ("ImpactParamTool( copy ):  Tool* points to NULL ",
         StatusCode::FAILURE , __FILE__ , __LINE__ ); }
  m_tool -> addRef  ();
  //
#ifdef LOKI_DEBUG
  s_IPTCounter.increment();
#endif 
  //
};
// ============================================================================
/// assignement 
// ============================================================================
LoKi::Vertices::ImpactParamTool& 
LoKi::Vertices::ImpactParamTool::operator=
( const LoKi::Vertices::ImpactParamTool& right ) 
{
  if ( &right == this ) { return *this ; }
  IGeomDispCalculator* _tmp = m_tool ;
  m_tool = right.m_tool ;
  if ( 0 != m_tool ) { m_tool -> addRef  () ; }
  if ( 0 !=  _tmp  ) { _tmp   -> release () ;}
  //
  if ( 0 == m_tool ) 
  { throw LoKi::Exception 
      ("ImpactParamTool&operator=: Tool* points to NULL "  ,
       StatusCode::FAILURE , __FILE__ , __LINE__ ); }
  //
  return *this ;
} ;
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
LoKi::Vertices::ImpactParamTool::~ImpactParamTool()
{
  if ( 0 == m_tool ) 
  { throw LoKi::Exception 
      ("~ImpactParamTool(): Tool* points to NULL "  ,
       StatusCode::FAILURE , __FILE__ , __LINE__ ); }
  m_tool -> release ();
  //
#ifdef LOKI_DEBUG
  s_IPTCounter.decrement();
#endif 
  //
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
