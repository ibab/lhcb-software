// $Id: VertexHolder.cpp,v 1.1.1.1 2006-02-17 19:17:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Vertex.h"
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
#include "LoKi/Constants.h"
#include "LoKi/Print.h"
#include "LoKi/Exception.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/VertexHolder.h"
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


namespace
{
#ifdef LOKI_DEBUG 
  static InstanceCounter<LoKi::Vertices::ImpactParamTool> s_IPTCounter ;
  static InstanceCounter<LoKi::Vertices::ImpParBase>      s_IPBCounter ;  
#endif 
};


// ============================================================================
/// constructor from the 3Dpoint
// ============================================================================
LoKi::Vertices::VertexHolder::VertexHolder
( const LoKi::Point3D& point      ) 
  : AuxFunBase() 
  , m_type   ( _point ) 
  , m_vertex (  0     ) 
  , m_point  (  point ) 
{};
// ============================================================================
/// constructor from Vertex 
// ============================================================================
LoKi::Vertices::VertexHolder::VertexHolder
( const LHCb::Vertex*  vertex ) 
  : AuxFunBase() 
  , m_type   ( _vertex ) 
  , m_vertex (  vertex ) 
  , m_point  () 
{};
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::Vertices::VertexHolder::VertexHolder
( const LoKi::Vertices::VertexHolder& right ) 
  : AuxFunBase ( right          ) 
  , m_type     ( right.m_type   ) 
  , m_vertex   ( right.m_vertex ) 
  , m_point    ( right.m_point  ) 
{};
// ============================================================================
LoKi::Vertices::VertexHolder::~VertexHolder(){}
// ============================================================================
const LoKi::Point3D& 
LoKi::Vertices::VertexHolder::position() const 
{
  static const LoKi::Point3D s_error = 
    LoKi::Point3D ( 0 , 0 , LoKi::Constants::InvalidDistance ) ;
  static const std::string   s_print = "Vertex Information is invalid, return " 
    + LoKi::Print::toString( s_error ) ;
  if ( !valid() ) { Error ( s_print ) ; return s_error ; }    // RETURN 
  switch ( type() ) 
  {
  case _point  : return point()                ; break ; // RETURN 
  case _vertex : return vertex()->position()   ; break ; // RETURN
  default      : Error ( s_print ) ; return s_error ; break ; // RETURN   
  }
  Error ( s_print ) ;
  return s_error ;                                            // RETURN 
} ;
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
( const LoKi::Point3D&    point  ,   
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
( const LHCb::Vertex*               vertex ,   
  const LoKi::Vertices::ImpParBase& tool   )
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
( const LoKi::Point3D&              point  ,   
  const LoKi::Vertices::ImpParBase& tool   )
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
  const LoKi::Vertices::ImpParBase&   tool   )
  : LoKi::Vertices::ImpactParamTool ( tool   ) 
  , LoKi::Vertices::VertexHolder    ( holder )
{
#ifdef LOKI_DEBUG
  s_IPBCounter.increment();
#endif 
} ;
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
