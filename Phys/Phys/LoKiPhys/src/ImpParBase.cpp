// $Id: ImpParBase.cpp,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
  InstanceCounter<LoKi::Vertices::ImpParBase> s_IPBCounter ;  
};
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
LoKi::Vertices::ImpParBase::ImpParBase
( const LHCb::Vertex*  vertex ,   
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
( const LHCb::Vertex*                    vertex ,   
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
  : LoKi::Vertices::ImpactParamTool ( tool ) 
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
