// $Id$
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/MCVertex.h"
#include "Event/MCParticle.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCVertices.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace LoKi::MCVertices
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
 *  @date 2001-01-23
 */
// ============================================================================
LoKi::MCVertices::TypeOfMCVertex*
LoKi::MCVertices::TypeOfMCVertex::clone() const
{ return new TypeOfMCVertex() ; }
// ============================================================================
LoKi::MCVertices::TypeOfMCVertex::result_type
LoKi::MCVertices::TypeOfMCVertex::operator()
  ( LoKi::MCVertices::TypeOfMCVertex::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return -1000000." ) ;
    return -1000000.0 ;
  }
  return v->type() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::TypeOfMCVertex::fillStream
( std::ostream& s ) const
{ return s << "MCVTYPE" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::TimeOfFlight*
LoKi::MCVertices::TimeOfFlight::clone() const
{ return new TimeOfFlight( *this ) ; }
// ============================================================================
LoKi::MCVertices::TimeOfFlight::result_type
LoKi::MCVertices::TimeOfFlight::operator()
  ( LoKi::MCVertices::TimeOfFlight::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return -1000 * ms " ) ;
    return -1000. * Gaudi::Units::ms ;
  }
  return v->time() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::TimeOfFlight::fillStream
( std::ostream& s ) const
{ return s << "MCVTOF" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::VertexPositionX*
LoKi::MCVertices::VertexPositionX::clone() const
{ return new VertexPositionX(*this) ; }
// ============================================================================
LoKi::MCVertices::VertexPositionX::result_type
LoKi::MCVertices::VertexPositionX::operator()
  ( LoKi::MCVertices::VertexPositionX::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return v->position().x() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::VertexPositionX::fillStream
( std::ostream& s ) const
{ return s << "MCVX" ; }
// ============================================================================


// ============================================================================
LoKi::MCVertices::VertexPositionY*
LoKi::MCVertices::VertexPositionY::clone() const
{ return new VertexPositionY( *this ) ; }
// ============================================================================
LoKi::MCVertices::VertexPositionY::result_type
LoKi::MCVertices::VertexPositionY::operator()
  ( LoKi::MCVertices::VertexPositionY::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return v->position().y() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::VertexPositionY::fillStream
( std::ostream& s ) const
{ return s << "MCVY" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::VertexPositionZ*
LoKi::MCVertices::VertexPositionZ::clone() const
{ return new VertexPositionZ(*this) ; }
// ============================================================================
LoKi::MCVertices::VertexPositionZ::result_type
LoKi::MCVertices::VertexPositionZ::operator()
  ( LoKi::MCVertices::VertexPositionZ::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  return v->position().z() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::VertexPositionZ::fillStream
( std::ostream& s ) const
{ return s << "MCVZ" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::VertexTime*
LoKi::MCVertices::VertexTime::clone() const
{ return new VertexTime(*this) ; }
// ============================================================================
LoKi::MCVertices::VertexTime::result_type
LoKi::MCVertices::VertexTime::operator()
  ( LoKi::MCVertices::VertexTime::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return 'InvalidTime'" ) ;
    return LoKi::Constants::InvalidTime ;
  }
  return v->time() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::VertexTime::fillStream
( std::ostream& s ) const
{ return s << "MCVTIME" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::Primary*
LoKi::MCVertices::Primary::clone() const
{ return new Primary(*this) ; }
// ============================================================================
LoKi::MCVertices::Primary::result_type
LoKi::MCVertices::Primary::operator()
  ( LoKi::MCVertices::Primary::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return 'false' " ) ;
    return false ;
  }
  return v->isPrimary() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::Primary::fillStream
( std::ostream& s ) const
{ return s << "MCISPRIMARY" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::Decay*
LoKi::MCVertices::Decay::clone() const
{ return new Decay(*this) ; }
// ============================================================================
LoKi::MCVertices::Decay::result_type
LoKi::MCVertices::Decay::operator()
  ( LoKi::MCVertices::Primary::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return 'false' " ) ;
    return false ;
  }
  return v->isDecay() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::Decay::fillStream
( std::ostream& s ) const
{ return s << "MCISDECAY" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::MCVertexDistance::MCVertexDistance
( const LoKi::Point3D& point )
  : LoKi::BasicFunctors<const LHCb::MCVertex*>::Function()
  , m_point ( point )
{}
// ============================================================================
LoKi::MCVertices::MCVertexDistance::MCVertexDistance
( const LHCb::MCVertex* point )
  : LoKi::BasicFunctors<const LHCb::MCVertex*>::Function()
  , m_point ()
{
  if ( 0 == point )
  {
    Error ( "MCVertex* points to NULL!") ;
    m_point = LoKi::Point3D( -1.0 * Gaudi::Units::km ,
                             -1.0 * Gaudi::Units::km ,
                             -1.0 * Gaudi::Units::km ) ;
  }
  else { m_point = point->position(); }
}
// ============================================================================
LoKi::MCVertices::MCVertexDistance::MCVertexDistance
( const LoKi::MCVertices::MCVertexDistance& right )
  : LoKi::AuxFunBase                      ( right )
  , LoKi::BasicFunctors<const LHCb::MCVertex*>::Function( right )
  , m_point ( right.m_point )
{}
// ============================================================================
LoKi::MCVertices::MCVertexDistance::~MCVertexDistance(){}
// ============================================================================
LoKi::MCVertices::MCVertexDistance*
LoKi::MCVertices::MCVertexDistance::clone() const
{ return new MCVertexDistance ( *this ) ; }
// ============================================================================
LoKi::MCVertices::MCVertexDistance::result_type
LoKi::MCVertices::MCVertexDistance::operator()
  ( LoKi::MCVertices::MCVertexDistance::argument v ) const
{
  if ( 0 == v )
  {
    Error("MCVertex* points to NULL, return -1.0 * km " );
    return -1.0 * Gaudi::Units::km ;
  }
  return  ( v->position() - m_point ).R() ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::MCVertexDistance::fillStream
( std::ostream& s ) const
{ return s << "MCVDIST" ; }
// ============================================================================

// ============================================================================
LoKi::MCVertices::MCVFunAsMCFun::MCVFunAsMCFun
( const LoKi::MCTypes::MCVFunc& fun )
  : LoKi::BasicFunctors<const LHCb::MCParticle*>::Function ()
  , m_fun ( fun )
{}
// ============================================================================
LoKi::MCVertices::MCVFunAsMCFun::~MCVFunAsMCFun(){}
// ============================================================================
LoKi::MCVertices::MCVFunAsMCFun*
LoKi::MCVertices::MCVFunAsMCFun::clone() const
{ return new MCVFunAsMCFun ( *this ) ; }
// ============================================================================
LoKi::MCVertices::MCVFunAsMCFun::result_type
LoKi::MCVertices::MCVFunAsMCFun::operator()
  ( LoKi::MCVertices::MCVFunAsMCFun::argument p ) const
{
  if ( 0 == p )
  {
    Error ( "MCParticle* points to NULL, return -1000000.");
    return -1000000 ;
  };
  const LHCb::MCVertex* vertex = p->originVertex() ;
  if ( 0 == vertex ) { Warning ( "'origin' MCVertex* points to NULL") ;}
  //
  return m_fun( vertex ) ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::MCVFunAsMCFun::fillStream
( std::ostream& s ) const
{ return s << "MCVXFUN(" << m_fun << ")" ; }
// ============================================================================


// ============================================================================
LoKi::MCVertices::Key*
LoKi::MCVertices::Key::clone() const
{ return new Key(*this) ; }
// ============================================================================
LoKi::MCVertices::Key::result_type
LoKi::MCVertices::Key::operator()
  ( LoKi::MCVertices::Key::argument v ) const
{
  if ( 0 == v )
  {
    Error (  " MCVertex* points to NULL, return -1000 " ) ;
    return -1000 ;
  }
  return v -> key () ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::Key::fillStream
( std::ostream& s ) const
{ return s << "MCVKEY" ; }
// ============================================================================


// ============================================================================
LoKi::MCVertices::MCVPFunAsMCFun::MCVPFunAsMCFun
( const LoKi::MCTypes::MCVFunc& fun )
  : LoKi::MCVertices::MCVFunAsMCFun ( fun )
{}
// ============================================================================
LoKi::MCVertices::MCVPFunAsMCFun::~MCVPFunAsMCFun(){}
// ============================================================================
LoKi::MCVertices::MCVPFunAsMCFun*
LoKi::MCVertices::MCVPFunAsMCFun::clone() const
{ return new MCVPFunAsMCFun ( *this ) ; }
// ============================================================================
LoKi::MCVertices::MCVPFunAsMCFun::result_type
LoKi::MCVertices::MCVPFunAsMCFun::operator()
  ( LoKi::MCVertices::MCVPFunAsMCFun::argument p ) const
{
  //
  if ( 0 == p )
  {
    Error ( "MCParticle* points to NULL, return -1000000.");
    return -1000000 ;
  }
  //
  const LHCb::MCVertex* vertex = p->primaryVertex() ;
  if ( 0 == vertex ) { Warning ( "'primary' MCVertex* points to NULL") ;}
  //
  return func()( vertex ) ;
}
// ============================================================================
std::ostream& LoKi::MCVertices::MCVPFunAsMCFun::fillStream
( std::ostream& s ) const
{ return s << "MCVPXFUN("<< func() << ")" ; }
// ============================================================================


// ============================================================================
/* constructor from MC Vertex fuction
 *  @param func vertex function tobe applied
 *  @param index vertex index :
 *         -1   - the last vertex  in the list
 *         -2   - the origin  vertex
 *         -3   - the mother  vertex
 *         -4   - the primary vertex
 */
// ============================================================================
LoKi::MCVertices::MCVFunction::MCVFunction
( const LoKi::MCTypes::MCVFunc& func  ,
  const int                     index )
  : LoKi::MCVertices::MCVFunAsMCFun ( func )
  , m_case  ( true  )
  , m_index ( index )
  , m_cut   ( LoKi::Constant<const LHCb::MCVertex*,bool>( false ) )
{}
// ============================================================================
// constructor from MC Vertex fuction and vertex selection
// ============================================================================
LoKi::MCVertices::MCVFunction::MCVFunction
( const LoKi::MCTypes::MCVFunc& func ,
  const LoKi::MCTypes::MCVCuts& cuts )
  : LoKi::MCVertices::MCVFunAsMCFun ( func )
  , m_case  ( false  )
  , m_index ( 0      )
  , m_cut   ( cuts   )
{}
// ============================================================================
// constructor from MC Vertex fuction and vertex selection
// ============================================================================
LoKi::MCVertices::MCVFunction::MCVFunction
( const LoKi::MCTypes::MCVCuts& cuts ,
  const LoKi::MCTypes::MCVFunc& func )
  : LoKi::MCVertices::MCVFunAsMCFun ( func )
  , m_case  ( false  )
  , m_index ( 0      )
  , m_cut   ( cuts   )
{}
// ============================================================================
// virtual descructor
// ============================================================================
LoKi::MCVertices::MCVFunction::~MCVFunction(){}
// ============================================================================
// clone method (mandatory!)
// ============================================================================
LoKi::MCVertices::MCVFunction*
LoKi::MCVertices::MCVFunction::clone() const
{ return new LoKi::MCVertices::MCVFunction ( *this ) ; }
// ============================================================================
std::ostream& LoKi::MCVertices::MCVFunction::fillStream
( std::ostream& s ) const
{
  if ( m_case )
  { return s << "MCVFUN("<< func() << "," << m_index << ")" ; }
  //
  return   s << "MCVFUN("<< func() << "," << m_cut   << ")" ;
}
// ============================================================================
// the only one essential method
// ============================================================================
LoKi::MCVertices::MCVFunction::result_type
LoKi::MCVertices::MCVFunction::operator()
  ( LoKi::MCVertices::MCVFunction::argument p ) const
{
  //
  if ( 0 == p )
  {
    Warning ( "MCParticle* points to NULL, return -1e+9 ");
    return -1.e+9 ;
  }
  //
  const LHCb::MCVertex* v = 0 ;
  //
  if      ( m_case && Origin   == m_index  )
  {
    //
    v = p -> originVertex() ;
    if ( 0 == v ) { Warning ( " Origin     MCVertex* points to NULL" ) ; }
    //
    return func() ( v ) ;
  }
  else if ( m_case && Primary == m_index )
  {
    //
    v = p -> primaryVertex() ;
    if ( 0 == v ) { Warning ( " Primary    MCVertex* points to NULL" ) ; }
    //
    return func() ( v ) ;
  }
  //
  typedef SmartRefVector<LHCb::MCVertex> EV ;
  const EV& evs = p->endVertices() ;
  //
  if (  evs.empty() ) { Warning ( " Empty EndVertices list " ) ; }
  //
  if      ( m_case && Last == m_index )
  {
    if ( !evs.empty() ) { v = evs.back() ; }
    return func() ( v ) ;
  }
  else if ( m_case &&  0 <= m_index && m_index < (long) evs.size() )
  {
    v = evs[ m_index ] ;
    return func() ( v ) ;
  }
  else if ( m_case )
  {
    Error ( " Invalid EndVertices index" ) ;
    return func() ( v ) ;
  }
  // now we deal with cuts:
  EV::const_iterator igood = std::find_if ( evs.begin() , evs.end() , m_cut  ) ;
  //
  if ( evs.end() != igood ){ v = *igood ; }
  else { Error ( " No proper vertex is found " ) ; }
  //
  return func() ( v ) ;
  //
}

// some default destructors
LoKi::MCVertices::TypeOfMCVertex::~TypeOfMCVertex() {}
LoKi::MCVertices::TimeOfFlight::~TimeOfFlight() {}
LoKi::MCVertices::VertexTime::~VertexTime() {}
LoKi::MCVertices::VertexPositionX::~VertexPositionX() {}
LoKi::MCVertices::VertexPositionY::~VertexPositionY() {}
LoKi::MCVertices::VertexPositionZ::~VertexPositionZ() {}
LoKi::MCVertices::Primary::~Primary() {}
LoKi::MCVertices::Decay::~Decay() {}
LoKi::MCVertices::Key::~Key() {}


// ============================================================================
// The END
// ============================================================================
