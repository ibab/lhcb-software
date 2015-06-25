// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/VertexBase.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Geometry.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/VertexCast.h"
#include "LoKi/Vertices0.h"
// ============================================================================
// GSL
// ============================================================================
#include "gsl/gsl_cdf.h"
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
 *  with the campain of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16
 *
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 *
 */
// ============================================================================
LoKi::Vertices::IsPrimary*
LoKi::Vertices::IsPrimary::clone() const
{ return new IsPrimary( *this ) ; }
// ============================================================================
LoKi::Vertices::IsPrimary::result_type
LoKi::Vertices::IsPrimary::operator()
  ( LoKi::Vertices::IsPrimary::argument v ) const
{
  if ( 0 == v )
  {
    Error ( "Invalid VertexBase, return 'false'" ) ;
    return false ;                   // RETURN
  }
  if ( v -> isPrimary() ) { return true  ; }              // RETURN
  const LHCb::RecVertex* rv = dynamic_cast<const LHCb::RecVertex*>( v ) ;
  if ( 0 == rv          ) { return false ; }              // RETURN
  if ( rv-> isPrimary() ) { return true  ; }              // RETURN
  if ( LHCb::RecVertex::Primary == rv->technique() ) { return true ; } // RETURN
  Warning ( "non-primary LHCb::RecVertex, return 'false'" ) ;
  return false ;
}
// ============================================================================
std::ostream&
LoKi::Vertices::IsPrimary::fillStream
( std::ostream& s ) const
{ return s << "PRIMARY" ; }
// ============================================================================
LoKi::Vertices::Technique*
LoKi::Vertices::Technique::clone() const
{ return new Technique( *this ) ; }
// ============================================================================
LoKi::Vertices::Technique::result_type
LoKi::Vertices::Technique::operator()
  ( LoKi::Vertices::Technique::argument v ) const
{
  // cast
  const LHCb::Vertex* vertex = base2vertex ( v ) ;
  //
  if ( 0 != vertex ) { return vertex -> technique() ; }      // RETURN
  //
  const int errVal = -1000;
  Error ( "Invalid Vertex, return "+Gaudi::Utils::toString(errVal) ) ;
  return errVal ;                   // RETURN
}
// ============================================================================
std::ostream&
LoKi::Vertices::Technique::fillStream
( std::ostream& s ) const
{ return s << "TECHNIQUE" ; }
// ============================================================================
LoKi::Vertices::VertexChi2*
LoKi::Vertices::VertexChi2::clone() const
{ return new VertexChi2( *this ) ; }
// ============================================================================
LoKi::Vertices::VertexChi2::result_type
LoKi::Vertices::VertexChi2::operator()
  ( LoKi::Vertices::VertexChi2::argument v ) const
{
  if ( 0 != v ) { return chi2 ( *v ) ; }                    // RETURN
  Error ( "Invalid Vertex, return 'InvalidChi2'" ) ;
  return LoKi::Constants::InvalidChi2 ;                      // RETURN
}

// ============================================================================
std::ostream&
LoKi::Vertices::VertexChi2::fillStream
( std::ostream& s ) const
{ return s << "VCHI2" ; }
// ============================================================================
LoKi::Vertices::VertexChi2PerDoF*
LoKi::Vertices::VertexChi2PerDoF::clone() const
{ return new VertexChi2PerDoF( *this ) ; }
// ============================================================================
LoKi::Vertices::VertexChi2PerDoF::result_type
LoKi::Vertices::VertexChi2PerDoF::operator()
  ( LoKi::Vertices::VertexChi2PerDoF::argument v ) const
{
  if ( 0 != v ) { return chi2PerDoF ( *v ) ; }                    // RETURN
  Error ( "Invalid Vertex, return 'InvalidChi2'" ) ;
  return LoKi::Constants::InvalidChi2 ;                      // RETURN
}
// ============================================================================
std::ostream&
LoKi::Vertices::VertexChi2PerDoF::fillStream
( std::ostream& s ) const
{ return s << "VCHI2PDOF" ; }
// ============================================================================
LoKi::Vertices::VertexDoF*
LoKi::Vertices::VertexDoF::clone() const
{ return new VertexDoF( *this ) ; }
// ============================================================================
LoKi::Vertices::VertexDoF::result_type
LoKi::Vertices::VertexDoF::operator()
  ( LoKi::Vertices::VertexDoF::argument v ) const
{
  if ( 0 == v )
  {
    const int errVal = -1000;
    Error ( "Invalid Vertex, return "+Gaudi::Utils::toString(errVal) ) ;
    return errVal ;                                             // RETURN
  }
  //
  return std::max( v->nDoF() , 1 ) ;
}
// ============================================================================
std::ostream&
LoKi::Vertices::VertexDoF::fillStream
( std::ostream& s ) const
{ return s << "VDOF" ; }
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
  Error ( "Invalid Vertex, return 'InvalidDistance'" ) ;
  return LoKi::Constants::InvalidDistance;                   // RETURN
}
// ============================================================================
std::ostream&
LoKi::Vertices::PositionX::fillStream
( std::ostream& s ) const
{ return s << "VX" ; }
// ============================================================================
LoKi::Vertices::PositionY*
LoKi::Vertices::PositionY::clone() const
{ return new PositionY( *this ) ; }
// ============================================================================
LoKi::Vertices::PositionY::result_type
LoKi::Vertices::PositionY::operator()
  ( LoKi::Vertices::PositionY::argument v ) const
{
  if ( v ) { return v -> position () . Y () ; }         // RETURN
  Error ( "Invalid Vertex, return 'InvalidDistance'" ) ;
  return LoKi::Constants::InvalidDistance;                   // RETURN
}
// ============================================================================
std::ostream&
LoKi::Vertices::PositionY::fillStream
( std::ostream& s ) const
{ return s << "VY" ; }

// ============================================================================
LoKi::Vertices::PositionZ*
LoKi::Vertices::PositionZ::clone() const
{ return new PositionZ( *this ) ; }
// ============================================================================
LoKi::Vertices::PositionZ::result_type
LoKi::Vertices::PositionZ::operator()
  ( LoKi::Vertices::PositionZ::argument v ) const
{
  if ( v ) { return v -> position () . Z () ; }         // RETURN
  Error ( "Invalid Vertex, return 'InvalidDistance'" ) ;
  return LoKi::Constants::InvalidDistance;                   // RETURN
}
// ============================================================================
std::ostream&
LoKi::Vertices::PositionZ::fillStream
( std::ostream& s ) const
{ return s << "VZ" ; }



// ============================================================================
LoKi::Vertices::PositionRho*
LoKi::Vertices::PositionRho::clone() const
{ return new PositionRho( *this ) ; }
// ============================================================================
LoKi::Vertices::PositionRho::result_type
LoKi::Vertices::PositionRho::operator()
  ( LoKi::Vertices::PositionRho::argument v ) const
{
  if ( v ) { return v -> position () . Rho () ; }         // RETURN
  Error ( "Invalid Vertex, return 'InvalidDistance'" ) ;
  return LoKi::Constants::InvalidDistance;                   // RETURN
}
// ============================================================================
std::ostream& LoKi::Vertices::PositionRho::fillStream
( std::ostream& s ) const
{ return s << "VRHO" ; }
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
  const LHCb::Vertex* vertex = base2vertex ( v ) ;
  //
  if ( vertex ) { return vertex -> outgoingParticles().size() ; }   // RETURN
  const int errVal = 0;
  Error ( "Invalid Vertex, return "+Gaudi::Utils::toString(errVal) ) ;
  return errVal ;                                                  // RETURN
}
// ============================================================================
std::ostream&
LoKi::Vertices::NumberOfOutgoing::fillStream
( std::ostream& s ) const
{ return s << "NPRONGS" ; }
// ============================================================================
LoKi::Vertices::HasInfo::HasInfo( const int info )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate()
  , m_info ( info )
{}
// ============================================================================
LoKi::Vertices::HasInfo::HasInfo
( const LoKi::Vertices::HasInfo& right )
  : LoKi::AuxFunBase                        ( right )
  , LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate( right )
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
  Error ( "Invalid Vertex, return 'false'" ) ;
  return false;                                           // RETURN
}
// ============================================================================
std::ostream&
LoKi::Vertices::HasInfo::fillStream
( std::ostream& s ) const
{ return s << "HASVINFO[" << m_info << "]" ; }
// ============================================================================
LoKi::Vertices::Info::Info
( const int    key ,
  const double def ,
  const double bad )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function()
  , m_key ( key )
  , m_def ( def )
  , m_bad ( bad )
{}
// ============================================================================
LoKi::Vertices::Info::Info
( const int    key ,
  const double def )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function()
  , m_key ( key )
  , m_def ( def )
  , m_bad ( def )
{}
// ============================================================================
LoKi::Vertices::Info::Info
( const LoKi::Vertices::Info& right )
  : LoKi::AuxFunBase                       ( right )
  , LoKi::BasicFunctors<const LHCb::VertexBase*>::Function( right )
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
  if ( v ) { return v -> info( m_key , m_def ) ; }      // RETURN
  Error ( "Invalid Vertex, return "
            + Gaudi::Utils::toString ( m_bad )  ) ;
  return m_bad;                                              // RETURN
}
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
LoKi::Vertices::NumberOfTracks*
LoKi::Vertices::NumberOfTracks::clone() const
{ return new NumberOfTracks( *this ) ; }
// ============================================================================
LoKi::Vertices::NumberOfTracks::result_type
LoKi::Vertices::NumberOfTracks::operator()
  ( LoKi::Vertices::NumberOfTracks::argument v ) const
{
  const LHCb::RecVertex* rv = base2rec ( v ) ;
  //
  if ( NULL == rv )
  {
    const int errVal = 0;
    Error ( "Invalid (Rec)Vertex, return "+Gaudi::Utils::toString(errVal) ) ;
    return errVal ;                                 // RETURN
  }
  //
  return ( ! rv->tracks() .empty() ? rv->tracks(). size() :
           ! rv->weights().empty() ? rv->weights().size() :
           0.5 * ( 3 + rv->nDoF() ) );
}
// ============================================================================
std::ostream&
LoKi::Vertices::NumberOfTracks::fillStream
( std::ostream& s ) const
{ return s << "NTRACKS" ; }
// ============================================================================

// ============================================================================
// MANDATORY: virtual destructor:
// ============================================================================
LoKi::Vertices::Chi2Prob*
LoKi::Vertices::Chi2Prob::clone() const
{ return new LoKi::Vertices::Chi2Prob ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method:
// ============================================================================
LoKi::Vertices::Chi2Prob::result_type
LoKi::Vertices::Chi2Prob::operator()
  ( LoKi::Vertices::Chi2Prob::argument v ) const
{
  if ( NULL == v )
  {
    Error ( "Invalid Vertex, return 'LoKi::Constants::InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN
  }
  //
  const int nDoF = v -> nDoF () ;
  // use GSL for computations:
  return gsl_cdf_chisq_Q ( v -> chi2 ()  , nDoF ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Vertices::Chi2Prob::fillStream ( std::ostream& s ) const
{ return s << "VPCHI2" ; }
// ============================================================================




// ============================================================================
LoKi::Vertices::IsVertex*
LoKi::Vertices::IsVertex::clone() const
{ return new IsVertex( *this ) ; }
// ============================================================================
LoKi::Vertices::IsVertex::result_type
LoKi::Vertices::IsVertex::operator()
  ( LoKi::Vertices::IsVertex::argument v ) const
{
  if ( NULL == v )
  {
    Warning ( "LHCb::VertexBase points to NULL, return 'false'" ) ;
    return false ;                                         // RETURN
  }
  //
  return NULL != dynamic_cast<const LHCb::Vertex*>( v ) ;
}
// ============================================================================
std::ostream& LoKi::Vertices::IsVertex::fillStream ( std::ostream& s ) const
{ return s << "ISVERTEX" ; }

// ============================================================================
LoKi::Vertices::IsRecVertex*
LoKi::Vertices::IsRecVertex::clone() const
{ return new IsRecVertex( *this ) ; }
// ============================================================================
LoKi::Vertices::IsRecVertex::result_type
LoKi::Vertices::IsRecVertex::operator()
  ( LoKi::Vertices::IsRecVertex::argument v ) const
{
  if ( NULL == v )
  {
    Warning ( "LHCb::VertexBase points to NULL, return 'false'" ) ;
    return false ;                                         // RETURN
  }
  //
  return NULL != dynamic_cast<const LHCb::RecVertex*>( v ) ;
}
// ============================================================================
std::ostream& LoKi::Vertices::IsRecVertex::fillStream ( std::ostream& s ) const
{ return s << "ISRECVERTEX" ; }


// ============================================================================
// VCOV2
// ============================================================================
// constructor from the indices
// ============================================================================
LoKi::Vertices::Cov2::Cov2
( const std::size_t i ,
  const std::size_t j )
  :  LoKi::BasicFunctors<const LHCb::VertexBase*>::Function ()
  ,  m_i ( std::max ( i , j ) )
  ,  m_j ( std::min ( i , j ) )
{
  Assert ( m_i < 3 , "Invalid matrix index" ) ;
  Assert ( m_j < 3 , "Invalid matrix index" ) ;
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Vertices::Cov2::~Cov2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::Cov2*
LoKi::Vertices::Cov2::clone () const
{ return new LoKi::Vertices::Cov2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Vertices::Cov2::result_type
LoKi::Vertices::Cov2::operator()
  ( LoKi::Vertices::Cov2::argument v ) const
{
  if ( NULL == v )
  {
    Error ( "Invalid Vertex, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN
  }
  //
  return v->covMatrix() ( m_i , m_j ) ;
  //
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Vertices::Cov2::fillStream ( std::ostream& s ) const
{ return s << "VCOV2(" << m_i << "," << m_j << ") " ; }
// ============================================================================





// ============================================================================
// The END
// ============================================================================
