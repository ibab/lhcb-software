// $Id: MCVertices.cpp,v 1.6 2007-04-16 16:16:08 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/10/27 13:35:46  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.4  2006/05/17 16:23:24  jpalac
// *** empty log message ***
//
// Revision 1.3  2006/02/18 18:10:57  ibelyaev
//  fix a typo
//
// Revision 1.2  2006/02/07 17:14:03  ibelyaev
//  regular update
//
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::TypeOfMCVertex::fillStream
( std::ostream& s ) const
{ return s << "MCVTYPE" ; } ;
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::TimeOfFlight::fillStream
( std::ostream& s ) const
{ return s << "MCVTOF" ; } ;
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::VertexPositionX::fillStream
( std::ostream& s ) const
{ return s << "MCVX" ; } ;
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::VertexPositionY::fillStream
( std::ostream& s ) const
{ return s << "MCVY" ; } ;
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::VertexPositionZ::fillStream
( std::ostream& s ) const
{ return s << "MCVZ" ; } ;
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::VertexTime::fillStream
( std::ostream& s ) const
{ return s << "MCVTIME" ; } ;
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::Primary::fillStream
( std::ostream& s ) const
{ return s << "MCISPRIMARY" ; } ;
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::Decay::fillStream
( std::ostream& s ) const
{ return s << "MCISDECAY" ; } ;
// ============================================================================

// ============================================================================
LoKi::MCVertices::MCVertexDistance::MCVertexDistance 
( const LoKi::Point3D& point ) 
  : LoKi::Function<const LHCb::MCVertex*>() 
  , m_point ( point )
{}
// ============================================================================
LoKi::MCVertices::MCVertexDistance::MCVertexDistance 
( const LHCb::MCVertex* point ) 
  : LoKi::Function<const LHCb::MCVertex*>() 
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
  , LoKi::Function<const LHCb::MCVertex*> ( right ) 
  , m_point ( right.m_point )
{};
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::MCVertexDistance::fillStream
( std::ostream& s ) const
{ return s << "MCVDIST" ; } ;
// ============================================================================

// ============================================================================
LoKi::MCVertices::MCVFunAsMCFun::MCVFunAsMCFun
( const LoKi::MCTypes::MCVFunc& fun ) 
  : LoKi::Function<const LHCb::MCParticle*> ()
  , m_fun ( fun ) 
{};
// ============================================================================
LoKi::MCVertices::MCVFunAsMCFun::MCVFunAsMCFun
( const LoKi::MCVertices::MCVFunAsMCFun& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::Function<const LHCb::MCParticle*> ( right )
  , m_fun ( right.m_fun ) 
{};
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
};
// ============================================================================
std::ostream& LoKi::MCVertices::MCVFunAsMCFun::fillStream
( std::ostream& s ) const
{ return s << "MCVXFUN" ; } ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
