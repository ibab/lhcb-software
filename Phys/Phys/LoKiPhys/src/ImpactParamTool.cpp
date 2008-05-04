// $Id: ImpactParamTool.cpp,v 1.6 2008-05-04 15:26:25 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Exception.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/ImpactParamTool.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
LoKi::Vertices::ImpactParamTool::ImpactParamTool 
( const IDistanceCalculator* tool ) 
  : LoKi::AuxFunBase () 
  , m_tool ( tool )
{
  //if ( 0 == m_tool.getObject() ) 
  //{ throw LoKi::Exception("ImpactParamTool():  Tool* points to NULL "  ,
  //                        StatusCode::FAILURE , __FILE__ , __LINE__ ); }
}
// ============================================================================
LoKi::Vertices::ImpactParamTool::ImpactParamTool 
( const LoKi::Interface<IDistanceCalculator>& tool ) 
  : LoKi::AuxFunBase () 
  , m_tool ( tool )
{
  //if ( 0 == m_tool.getObject() ) 
  //{ throw LoKi::Exception("ImpactParamTool():  Tool* points to NULL "  ,
  //                        StatusCode::FAILURE , __FILE__ , __LINE__ ); }
}
// ============================================================================
//  copy 
// ============================================================================
LoKi::Vertices::ImpactParamTool::ImpactParamTool
( const LoKi::Vertices::ImpactParamTool& tool ) 
  : LoKi::AuxFunBase ( tool ) 
  , m_tool           ( tool.m_tool )
{
  //if ( 0 == m_tool.getObject()  ) 
  //{ throw LoKi::Exception
  //    ("ImpactParamTool( copy ):  Tool* points to NULL ",
  //     StatusCode::FAILURE , __FILE__ , __LINE__ ); }
}
// ============================================================================
//  virtual destructor
// ============================================================================
LoKi::Vertices::ImpactParamTool::~ImpactParamTool(){}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
