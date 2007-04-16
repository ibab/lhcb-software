// $Id: ImpParBase.cpp,v 1.5 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/12/01 08:17:55  ibelyaev
//  fix for VertexBase argument
//
// Revision 1.3  2006/10/27 13:39:32  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.2  2006/03/18 12:40:18  ibelyaev
//  fix a warning for Win32
//
// Revision 1.1  2006/02/19 21:49:12  ibelyaev
//  restructirisation + new funtions
//
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Vertex.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/InstanceCounter.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IGeomDispCalculator.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/ImpParBase.h"
// ============================================================================

// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Vertices::ImpParBase
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

// ============================================================================
namespace
{
  InstanceCounter<LoKi::Vertices::ImpParBase> s_IPBCounter ;  
};
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LHCb::VertexBase* vertex ,   
  IGeomDispCalculator* tool   )
  : LoKi::Vertices::ImpactParamTool ( tool    ) 
  , LoKi::Vertices::VertexHolder    ( vertex  )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LoKi::Point3D& point  ,   
  IGeomDispCalculator* tool   )
  : LoKi::Vertices::ImpactParamTool ( tool  ) 
  , LoKi::Vertices::VertexHolder    ( point )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LoKi::Vertices::VertexHolder& holder ,   
  IGeomDispCalculator*                tool   )
  : LoKi::Vertices::ImpactParamTool ( tool   ) 
  , LoKi::Vertices::VertexHolder    ( holder )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
// ============================================================================



// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LHCb::VertexBase* vertex ,   
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::Vertices::ImpactParamTool ( tool    ) 
  , LoKi::Vertices::VertexHolder    ( vertex  )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LoKi::Point3D&                   point  ,   
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::Vertices::ImpactParamTool ( tool  ) 
  , LoKi::Vertices::VertexHolder    ( point )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LoKi::Vertices::VertexHolder&    holder ,   
  const LoKi::Vertices::ImpactParamTool& tool   )
  : LoKi::Vertices::ImpactParamTool ( tool   ) 
  , LoKi::Vertices::VertexHolder    ( holder )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LoKi::Vertices::ImpParBase&   tool )
  : LoKi::AuxFunBase                ( tool )  
  , LoKi::Vertices::ImpactParamTool ( tool ) 
  , LoKi::Vertices::VertexHolder    ( tool )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
// ============================================================================
/// destructor 
// ============================================================================
LoKi::Vertices::ImpParBase::~ImpParBase() 
{
#ifdef LOKI_DEBUG
  s_IPBCounter.decrement();
#endif 
  //
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
