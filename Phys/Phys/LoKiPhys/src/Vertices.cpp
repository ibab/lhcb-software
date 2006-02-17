// $Id: Vertices.cpp,v 1.1.1.1 2006-02-17 19:17:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// Event
// ============================================================================
#include "Event/Vertex.h"
#include "Event/PrimVertex.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Geometry.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices.h"
#include "LoKi/Print.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Vertices
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================


// ============================================================================
LoKi::Vertices::IsPrimary* 
LoKi::Vertices::IsPrimary::clone() const 
{ return new IsPrimary( *this ) ; }
// ============================================================================
LoKi::Vertices::IsPrimary::result_type 
LoKi::Vertices::IsPrimary::operator() 
  ( LoKi::Vertices::IsPrimary::argument v ) const 
{
  if ( 0 != v ) { return  v->isPrimary() ; } // RETURN 
  Error ( " Invalid Vertex, return 'false'" ) ;
  return false ;                   // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::IsPrimary::fillStream
( std::ostream& s ) const 
{ return s << "PRIMARY" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::Technique* 
LoKi::Vertices::Technique::clone() const 
{ return new Technique( *this ) ; }
// ============================================================================
LoKi::Vertices::Technique::result_type 
LoKi::Vertices::Technique::operator() 
  ( LoKi::Vertices::Technique::argument v ) const 
{
  if ( 0 != v ) { return v -> technique() ; }                // RETURN 
  Error ( " Invalid Vertex, return -1000 " ) ;
  return -1000 ;                   // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::Technique::fillStream
( std::ostream& s ) const 
{ return s << "TECHNIQUE" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::VertexChi2* 
LoKi::Vertices::VertexChi2::clone() const 
{ return new VertexChi2( *this ) ; }
// ============================================================================
LoKi::Vertices::VertexChi2::result_type 
LoKi::Vertices::VertexChi2::operator() 
  ( LoKi::Vertices::VertexChi2::argument v ) const 
{
  if ( 0 != v ) { return v -> chi2 () ; }                    // RETURN 
  Error ( " Invalid Vertex, return 'InvalidChi2'" ) ;
  return LoKi::Constants::InvalidChi2 ;                      // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::VertexChi2::fillStream
( std::ostream& s ) const 
{ return s << "VCHI2" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::VertexDoF* 
LoKi::Vertices::VertexDoF::clone() const 
{ return new VertexDoF( *this ) ; }
// ============================================================================
LoKi::Vertices::VertexDoF::result_type 
LoKi::Vertices::VertexDoF::operator() 
  ( LoKi::Vertices::VertexDoF::argument v ) const 
{
  if ( 0 != v ) { return v -> nDoF () ; }                    // RETURN 
  Error ( " Invalid Vertex, return -1000 " ) ;
  return -1000 ;                                             // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::VertexDoF::fillStream
( std::ostream& s ) const 
{ return s << "VDOF" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::PositionX* 
LoKi::Vertices::PositionX::clone() const 
{ return new PositionX( *this ) ; }
// ============================================================================
LoKi::Vertices::PositionX::result_type 
LoKi::Vertices::PositionX::operator() 
  ( LoKi::Vertices::PositionX::argument v ) const 
{
  if ( 0 != v ) { return v ->  position () .X() ; }         // RETURN 
  Error ( " Invalid Vertex, return 'InvalidDistance'" ) ;
  return LoKi::Constants::InvalidDistance;                   // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::PositionX::fillStream
( std::ostream& s ) const 
{ return s << "VX" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::PositionY* 
LoKi::Vertices::PositionY::clone() const 
{ return new PositionY( *this ) ; }
// ============================================================================
LoKi::Vertices::PositionY::result_type 
LoKi::Vertices::PositionY::operator() 
  ( LoKi::Vertices::PositionY::argument v ) const 
{
  if ( 0 != v ) { return v ->  position () . Y () ; }         // RETURN 
  Error ( " Invalid Vertex, return 'InvalidDistance'" ) ;
  return LoKi::Constants::InvalidDistance;                   // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::PositionY::fillStream
( std::ostream& s ) const 
{ return s << "VY" ; }
// ============================================================================


// ============================================================================
LoKi::Vertices::PositionZ* 
LoKi::Vertices::PositionZ::clone() const 
{ return new PositionZ( *this ) ; }
// ============================================================================
LoKi::Vertices::PositionZ::result_type 
LoKi::Vertices::PositionZ::operator() 
  ( LoKi::Vertices::PositionZ::argument v ) const 
{
  if ( 0 != v ) { return v ->  position () . Z () ; }         // RETURN 
  Error ( " Invalid Vertex, return 'InvalidDistance'" ) ;
  return LoKi::Constants::InvalidDistance;                   // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::PositionZ::fillStream
( std::ostream& s ) const 
{ return s << "VZ" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::NumberOfOutgoing* 
LoKi::Vertices::NumberOfOutgoing::clone() const 
{ return new NumberOfOutgoing( *this ) ; }
// ============================================================================
LoKi::Vertices::NumberOfOutgoing::result_type 
LoKi::Vertices::NumberOfOutgoing::operator() 
  ( LoKi::Vertices::NumberOfOutgoing::argument v ) const 
{
  if ( 0 != v ) { return v ->outgoingParticles().size() ; }   // RETURN 
  Error ( " Invalid Vertex, return '0'" ) ;
  return 0 ;                                                  // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::NumberOfOutgoing::fillStream
( std::ostream& s ) const 
{ return s << "NPRONGS" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::HasInfo::HasInfo( const int info ) 
  : LoKi::Predicate<const LHCb::Vertex*>() 
  , m_info ( info ) 
{}
// ============================================================================
LoKi::Vertices::HasInfo::HasInfo
( const LoKi::Vertices::HasInfo& right ) 
  : LoKi::Predicate<const LHCb::Vertex*>( right ) 
  , m_info ( right.m_info )
{}
// ============================================================================
LoKi::Vertices::HasInfo::~HasInfo(){}
// ============================================================================
LoKi::Vertices::HasInfo* 
LoKi::Vertices::HasInfo::clone() const 
{ return new HasInfo( *this ) ; }
// ============================================================================
LoKi::Vertices::HasInfo::result_type 
LoKi::Vertices::HasInfo::operator() 
  ( LoKi::Vertices::HasInfo::argument v ) const 
{
  if ( 0 != v ) { return v -> hasInfo( m_info ) ; }      // RETURN 
  Error ( " Invalid Vertex, return 'false'" ) ;
  return false;                                           // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::HasInfo::fillStream
( std::ostream& s ) const 
{ return s << "HASVINFO[" << m_info << "]" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::Info::Info
( const int    key , 
  const double def , 
  const double bad )
  : LoKi::Function<const LHCb::Vertex*>() 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( bad ) 
{}
// ============================================================================
LoKi::Vertices::Info::Info
( const int    key , 
  const double def )
  : LoKi::Function<const LHCb::Vertex*>() 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( def ) 
{}
// ============================================================================
LoKi::Vertices::Info::Info
( const LoKi::Vertices::Info& right ) 
  : LoKi::Function<const LHCb::Vertex*>( right ) 
  , m_key  ( right.m_key  )
  , m_def  ( right.m_def  )
  , m_bad  ( right.m_bad  )
{}
// ============================================================================
LoKi::Vertices::Info::~Info(){}
// ============================================================================
LoKi::Vertices::Info* 
LoKi::Vertices::Info::clone() const 
{ return new Info( *this ) ; }
// ============================================================================
LoKi::Vertices::Info::result_type 
LoKi::Vertices::Info::operator() 
  ( LoKi::Vertices::Info::argument v ) const 
{
  if ( 0 != v ) { return v -> info( m_key , m_def ) ; }      // RETURN 
  Error ( " Invalid Vertex, return " 
          + boost::lexical_cast<std::string>( m_bad ) ) ;
  return m_bad;                                              // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::Info::fillStream
( std::ostream& s ) const 
{ 
  s << "VINFO[" << m_key << "," << m_def ;
  if ( m_bad != m_def ) { s << "," << m_bad ; }
  return s << "]" ;
}
// ============================================================================


// ============================================================================
LoKi::Vertices::NumberOfTracks* 
LoKi::Vertices::NumberOfTracks::clone() const 
{ return new NumberOfTracks( *this ) ; }
// ============================================================================
LoKi::Vertices::NumberOfTracks::result_type 
LoKi::Vertices::NumberOfTracks::operator() 
  ( LoKi::Vertices::NumberOfTracks::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( " Invalid Vertex, return '0'" ) ;
    return 0 ;                                           // RETURN 
  }
  if ( !v->isPrimary() ) { return 0 ; }                   // RETURN  
  const LHCb::PrimVertex* pv = 
    dynamic_cast<const LHCb::PrimVertex*>( v ) ;
  if ( 0 == pv ) { return 0 ; }                            // RETURN
  //
  return pv ->tracks().size() ;                            // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::NumberOfTracks::fillStream
( std::ostream& s ) const 
{ return s << "NTRACKS" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LHCb::Vertex* vertex ) 
  : LoKi::Function<const LHCb::Vertex*>()  
  , LoKi::Vertices::VertexHolder ( vertex )
{} ;
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Point3D&  point ) 
  : LoKi::Function<const LHCb::Vertex*>()  
  , LoKi::Vertices::VertexHolder ( point )
{} ;
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Vertices::VertexHolder& right ) 
  : LoKi::Function<const LHCb::Vertex*>()  
  , LoKi::Vertices::VertexHolder ( right  )
{} ;
// ============================================================================
LoKi::Vertices::VertexDistance::VertexDistance
( const LoKi::Vertices::VertexDistance& right ) 
  : LoKi::Function<const LHCb::Vertex*>( right )  
  , LoKi::Vertices::VertexHolder ( right  )
{} ;
// ============================================================================
LoKi::Vertices::VertexDistance::~VertexDistance(){}
// ============================================================================
LoKi::Vertices::VertexDistance*
LoKi::Vertices::VertexDistance::clone() const 
{ return new LoKi::Vertices::VertexDistance(*this) ; }
// ============================================================================
LoKi::Vertices::VertexDistance::result_type 
LoKi::Vertices::VertexDistance::operator() 
  ( LoKi::Vertices::VertexDistance::argument v ) const 
{ return distance( v ) ; }
// ============================================================================
LoKi::Vertices::VertexDistance::result_type 
LoKi::Vertices::VertexDistance::distance  
( LoKi::Vertices::VertexDistance::argument v ) const 
{
  if ( 0 != v ) {  return ( v->position() - this->position()).R() ; } //RETURN 
  Error ( " Invalid Vertex, return 'InvalidDistance'") ;
  return LoKi::Constants::InvalidDistance ;      // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Vertices::VertexDistance::fillStream
( std::ostream& s ) const 
{ return s << "VDIST" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LHCb::Vertex* vertex ) 
  : LoKi::Function<const LHCb::Vertex*>()  
  , LoKi::Vertices::VertexHolder ( vertex )
{} ;
// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LoKi::Point3D&  point ) 
  : LoKi::Function<const LHCb::Vertex*>()  
  , LoKi::Vertices::VertexHolder ( point )
{} ;
// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LoKi::Vertices::VertexHolder& right ) 
  : LoKi::Function<const LHCb::Vertex*>()  
  , LoKi::Vertices::VertexHolder ( right  )
{} ;
// ============================================================================
LoKi::Vertices::VertexChi2Distance::VertexChi2Distance
( const LoKi::Vertices::VertexChi2Distance& right ) 
  : LoKi::Function<const LHCb::Vertex*>( right )  
  , LoKi::Vertices::VertexHolder ( right  )
{} ;
// ============================================================================
LoKi::Vertices::VertexChi2Distance::~VertexChi2Distance(){}
// ============================================================================
LoKi::Vertices::VertexChi2Distance*
LoKi::Vertices::VertexChi2Distance::clone() const 
{ return new LoKi::Vertices::VertexChi2Distance(*this) ; }
// ============================================================================
LoKi::Vertices::VertexChi2Distance::result_type 
LoKi::Vertices::VertexChi2Distance::operator() 
  ( LoKi::Vertices::VertexChi2Distance::argument v ) const 
{ return chi2 ( v ) ; }
// ============================================================================
LoKi::Vertices::VertexChi2Distance::result_type 
LoKi::Vertices::VertexChi2Distance::chi2  
( LoKi::Vertices::VertexChi2Distance::argument v ) const 
{
  if     ( 0 == v    ) 
  {
    Error ( " Invalid Vertex, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;         // RETURN 
  }
  else if ( !valid() ) 
  {
    Error ( " Invalid Status, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;         // RETURN 
  }
  
  double _chi2 = LoKi::Constants::InvalidChi2 ;
  
  StatusCode sc = StatusCode::SUCCESS ;
  switch ( type() ) 
  {
  case _vertex  : 
    sc = LoKi::Geometry::chi2 
      ( v        -> position  () ,
        v        -> covMatrix () , 
        vertex() -> position  () , 
        vertex() -> covMatrix () , _chi2 ) ; break ;
  case _point   : 
    sc = LoKi::Geometry::chi2 
      ( point()                  ,
        v        -> position  () ,
        v        -> covMatrix () , _chi2 ) ; break ;
  default:
    sc = Error ( "Warning, invalid switch/case ") ; break ;
  } ;
  if ( sc.isFailure() ) 
  { Error ( "Evaluation is invalid, return " + 
            LoKi::Print::print ( _chi2 ) , sc ) ; }
  //
  return _chi2 ;
};
// ============================================================================
std::ostream& 
LoKi::Vertices::VertexChi2Distance::fillStream
( std::ostream& s ) const 
{ return s << "VDCHI2" ; }
// ============================================================================

// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::Vertex* vertex ) 
  : LoKi::Function<const LHCb::Vertex*>()
  , m_vertices  ( 1 , vertex      )
  , m_evaluator ( LoKi::Point3D() )
  , m_results   ( 1 )
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::Vertex::Vector& vs ) 
  : LoKi::Function<const LHCb::Vertex*>()
  , m_vertices  ( vs.begin() , vs.end() )
  , m_evaluator ( LoKi::Point3D() )
  , m_results   ( vs.size()       )
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::Vertex::ConstVector& vs ) 
  : LoKi::Function<const LHCb::Vertex*>()
  , m_vertices  ( vs.begin() , vs.end() )
  , m_evaluator ( LoKi::Point3D() )
  , m_results   ( vs.size()       )
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::PrimVertex::Vector& vs ) 
  : LoKi::Function<const LHCb::Vertex*>()
  , m_vertices  ( vs.begin() , vs.end() )
  , m_evaluator ( LoKi::Point3D() )
  , m_results   ( vs.size()       )
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::PrimVertex::ConstVector& vs ) 
  : LoKi::Function<const LHCb::Vertex*>()
  , m_vertices  ( vs.begin() , vs.end() )
  , m_evaluator ( LoKi::Point3D() )
  , m_results   ( vs.size()       )
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LHCb::PrimVertices* vs ) 
  : LoKi::Function<const LHCb::Vertex*>()
  , m_vertices  ( vs->begin() , vs->end() )
  , m_evaluator ( LoKi::Point3D() )
  , m_results   ( vs->size()      )
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::PhysTypes::VRange& vs ) 
  : LoKi::Function<const LHCb::Vertex*>()
  , m_vertices  ( vs.begin() , vs.end() )
  , m_evaluator ( LoKi::Point3D() )
  , m_results   ( vs.size()       )
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
} ;
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::MinVertexChi2Distance 
( const LoKi::Vertices::MinVertexChi2Distance& right ) 
  : LoKi::Function<const LHCb::Vertex*> ( right ) 
  , m_vertices  ( right.m_vertices  ) 
  , m_evaluator ( right.m_evaluator ) 
  , m_results   ( right.m_results   ) 
{
  Vrtxs::iterator end = std::remove
    ( m_vertices.begin() , m_vertices.end() , (const LHCb::Vertex*) 0 ) ;
  m_vertices.erase( end , m_vertices.end() ) ;
}
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::~MinVertexChi2Distance(){};
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance*
LoKi::Vertices::MinVertexChi2Distance::clone() const 
{ return new MinVertexChi2Distance(*this) ; }
// ============================================================================
LoKi::Vertices::MinVertexChi2Distance::result_type 
LoKi::Vertices::MinVertexChi2Distance::operator() 
  ( LoKi::Vertices::MinVertexChi2Distance::argument v ) const
{
  if     ( 0 == v    ) 
  {
    Error ( " Invalid Vertex, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;                  // RETURN 
  }
  if ( m_vertices.empty() ) 
  {
    Error ( " Empty list of vertices, return 'InvalidChi2'") ;
    return LoKi::Constants::InvalidChi2;                   // RETURN 
  }
  if ( m_vertices.size() != m_results.size() ) 
  { m_results = Results ( m_vertices.size() ) ; }
  
  // 1) reconfigure the evaluator 
  m_evaluator.setVertex( v ) ;
  // evaluate the chi2  
  m_evaluator.evaluate 
    ( m_vertices.begin() , m_vertices.end() , m_results.begin() ) ;
  // find the minimum
  Results::const_iterator imin = 
    std::min_element( m_results.begin() , m_results.end() ) ;
  //
  return (*imin) ;
};
// ============================================================================
std::ostream& 
LoKi::Vertices::MinVertexChi2Distance::fillStream
( std::ostream& s ) const 
{ return s << "MINVDCHI2" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

