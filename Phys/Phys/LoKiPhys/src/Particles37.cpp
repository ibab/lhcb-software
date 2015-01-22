// $Id$
// ============================================================================
// Inclde files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Vertices0.h"
#include "LoKi/PhysHelpers.h"
#include "LoKi/PhysSources.h"
#include "LoKi/Particles37.h"
#include "LoKi/GetTools.h"
// ============================================================================
/** @file  Particles36.cpp
 *  Functors which calculate decay length significance.
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-03
 */
// ============================================================================
// constructor from a vertex
// ============================================================================
LoKi::Particles::DecayLengthSignificance::DecayLengthSignificance
( const LHCb::VertexBase* vertex )
  : LoKi::BasicFunctors< const LHCb::Particle* >::Function()
  , LoKi::Vertices::VertexHolder( vertex )
{}
// ============================================================================
// constructor from a point
// ============================================================================
LoKi::Particles::DecayLengthSignificance::DecayLengthSignificance
( const LoKi::Point3D& point )
  : LoKi::AuxFunBase ( std::tie ( point ) ) 
  , LoKi::BasicFunctors< const LHCb::Particle* >::Function()
  , LoKi::Vertices::VertexHolder( point )
{}
// ============================================================================
// constructor from a base class
// ============================================================================
LoKi::Particles::DecayLengthSignificance::DecayLengthSignificance
( const LoKi::Vertices::VertexHolder& base )
  : LoKi::BasicFunctors< const LHCb::Particle* >::Function()
  , LoKi::Vertices::VertexHolder( base )
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::DecayLengthSignificance::~DecayLengthSignificance(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::DecayLengthSignificance* 
LoKi::Particles::DecayLengthSignificance::clone() const
{ return new DecayLengthSignificance( *this ); }
// ============================================================================
// The method where the calculation is done
// ============================================================================
LoKi::Particles::DecayLengthSignificance::result_type 
LoKi::Particles::DecayLengthSignificance::dls
( LoKi::Particles::DecayLengthSignificance::argument p ) const
{
  if( 0 == p ) 
  {
    Error ( "Invalid Particle, return 'InvalidDistance'");
    return LoKi::Constants::InvalidDistance;
  }
  
  // Get a pointer to the vertex we hold
  const LHCb::VertexBase* v = vertex();
  if( 0 == v ) {
    Warning( "Invalid Vertex, return 'InvalidDistance'" );
    return LoKi::Constants::InvalidDistance;
  }
  
  // Calculate the distance between the particle and the vertex we hold.
  Gaudi::XYZVector f = p->referencePoint() - position();
  Gaudi::Vector3 flight( f.x(), f.y(), f.z() );
  
  // Get the 3 momentum vectors for following calculations.
  Gaudi::XYZVector p3 = p->momentum().Vect();
  double p3mag = p3.R() ; 
  Gaudi::XYZVector d = p3.Unit();
  Gaudi::Vector3 dir( d.x(), d.y(), d.z() );
  
  // Get the covariance matrix of the vertex.
  Gaudi::SymMatrix3x3 W = v->covMatrix();
  
  // Get the particle covariance matrix.
   const Gaudi::SymMatrix7x7 pCov = p->covMatrix();
   
   // Project the momentum of the particle onto its distance to the vertex
   double a = ROOT::Math::Dot( dir, flight ) / p3mag;
   
   // Update the covariance matrix
   for ( size_t row = 0; row < 3; ++row ) 
   {
     for ( size_t col = 0; col <= row; ++col ) 
     {
       W( row, col ) +=  pCov( row, col )
         + a * a * pCov( row + 3, col + 3 )
         - a * ( pCov( row + 3, col ) + pCov( col + 3, row) );
     }
   }
   
   int OK = W.Invert() ;
   if( !OK ) 
   { 
     Warning( "Error inverting the updated covariance matrix." );
   }
   
   // Calculate the required numbers.
   double halfdChi2dLam2 = ROOT::Math::Similarity( W, dir );
   double decayLength    = ROOT::Math::Dot( dir , W * flight ) / halfdChi2dLam2;
   double decayLengthErr = std::sqrt( 1 / halfdChi2dLam2 );
   
   return decayLength / decayLengthErr;
   
   // Gaudi::Vector3 res = flight - decaylength * dir ;
   // chi2 = ROOT::Math::Similarity( W, res );
   
}
// ============================================================================
// MANDATORY: the only essential method 
// ============================================================================
LoKi::Particles::DecayLengthSignificance::result_type 
LoKi::Particles::DecayLengthSignificance::operator()
  ( LoKi::Particles::DecayLengthSignificance::argument p ) const
{ return dls ( p ); }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayLengthSignificance::fillStream( std::ostream& stream ) const
{ return stream << "DLS" ; }
// ============================================================================
// default constructor
// ============================================================================
LoKi::Particles::DecayLengthSignificanceDV::DecayLengthSignificanceDV()
  : LoKi::AuxDesktopBase()
  , LoKi::Particles::DecayLengthSignificance( LoKi::Point3D() )
{}
// ============================================================================
// MANDATORY: the only essential method 
// ============================================================================
LoKi::Particles::DecayLengthSignificanceDV::result_type 
LoKi::Particles::DecayLengthSignificanceDV::operator()
  ( LoKi::Particles::DecayLengthSignificanceDV::argument p ) const
{
   if( 0 == p ) 
   {
     Error( "Invalid Particle, return 'InvalidDistance'");
     return LoKi::Constants::InvalidDistance;
   }
   
   // Get the best vertex from desktop.
   const LHCb::VertexBase* pv = bestVertex( p );
   if( 0 == pv ) 
   {
     Warning( "Invalid Primary Vertex for Particle, return 'InvalidDistance'");
     return LoKi::Constants::InvalidDistance;
   }
   
   // Set our vertex to the best PV
   setVertex( pv );
   
   return dls( p ) ; 
}

// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::PathDistance::PathDistance 
( const LHCb::VertexBase*                        pv     , 
  const IDistanceCalculator*                     tool   ) 
  : LoKi::Particles::ImpPar ( pv , tool ) 
{}
// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::PathDistance::PathDistance 
(  const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::Particles::ImpPar ( tool ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::PathDistance::~PathDistance(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PathDistance*
LoKi::Particles::PathDistance::clone() const
{ return new LoKi::Particles::PathDistance ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PathDistance::result_type 
LoKi::Particles::PathDistance::operator() 
  ( LoKi::Particles::PathDistance::argument p ) const 
{
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  double chi2     = LoKi::Constants::InvalidChi2     ;
  //
  const StatusCode sc = path ( p , distance , error , chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return 'InvalidDistance'", sc, 0 ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 >= error || 
       LoKi::Constants::InvalidChi2     == chi2     || 0 >  chi2  ) 
  { 
    Warning ( "Invalid values from path, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  return distance ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PathDistance::fillStream( std::ostream& s ) const 
{ return s <<  "PATHDIST" ; }



// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::PathDistanceChi2::PathDistanceChi2 
( const LHCb::VertexBase*                        pv     , 
  const IDistanceCalculator*                     tool   ) 
  : LoKi::Particles::PathDistance ( pv , tool ) 
{}
// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::PathDistanceChi2::PathDistanceChi2
(  const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::Particles::PathDistance ( tool ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::PathDistanceChi2::~PathDistanceChi2 (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PathDistanceChi2*
LoKi::Particles::PathDistanceChi2::clone() const
{ return new LoKi::Particles::PathDistanceChi2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PathDistanceChi2::result_type 
LoKi::Particles::PathDistanceChi2::operator() 
  ( LoKi::Particles::PathDistanceChi2::argument p ) const 
{
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  double chi2     = LoKi::Constants::InvalidChi2     ;
  //
  StatusCode sc = path ( p , distance , error , chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 >= error || 
       LoKi::Constants::InvalidChi2     == chi2     || 0 >  chi2  ) 
  { 
    Warning ( "Invalid values from path, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return chi2 ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PathDistanceChi2::fillStream( std::ostream& s ) const 
{ return s <<  "PROJDISTCHI2" ; }



// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::PathDistanceSignificance::PathDistanceSignificance 
( const LHCb::VertexBase*                        pv     , 
  const IDistanceCalculator*                     tool   ) 
  : LoKi::Particles::PathDistanceChi2 ( pv , tool ) 
{}
// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::PathDistanceSignificance::PathDistanceSignificance
(  const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::Particles::PathDistanceChi2 ( tool ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::PathDistanceSignificance::~PathDistanceSignificance (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PathDistanceSignificance*
LoKi::Particles::PathDistanceSignificance::clone() const
{ return new LoKi::Particles::PathDistanceSignificance ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PathDistanceSignificance::result_type 
LoKi::Particles::PathDistanceSignificance::operator() 
  ( LoKi::Particles::PathDistanceSignificance::argument p ) const 
{
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  double chi2     = LoKi::Constants::InvalidChi2     ;
  //
  StatusCode sc = path ( p , distance , error , chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 >= error || 
       LoKi::Constants::InvalidChi2     == chi2     || 0 >  chi2  ) 
  { 
    Warning ( "Invalid values from path, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return distance / error  ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PathDistanceSignificance::fillStream( std::ostream& s ) const 
{ return s <<  "PDS" ; }






// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::ProjectedDistance::ProjectedDistance
( const LHCb::VertexBase*                        pv     , 
  const IDistanceCalculator*                     tool   ) 
  : LoKi::Particles::PathDistance ( pv , tool ) 
{}
// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::ProjectedDistance::ProjectedDistance
(  const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::Particles::PathDistance ( tool ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::ProjectedDistance::~ProjectedDistance (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ProjectedDistance*
LoKi::Particles::ProjectedDistance::clone() const
{ return new LoKi::Particles::ProjectedDistance ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProjectedDistance::result_type 
LoKi::Particles::ProjectedDistance::operator() 
  ( LoKi::Particles::ProjectedDistance::argument p ) const 
{ return projected ( p ) ; }
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ProjectedDistance::fillStream( std::ostream& s ) const 
{ return s <<  "PROJDIST" ; }




// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificance::ProjectedDistanceSignificance
( const LHCb::VertexBase*                        pv     , 
  const IDistanceCalculator*                     tool   ) 
  : LoKi::Particles::ProjectedDistance ( pv , tool ) 
{}
// ============================================================================
// constructor from the primary vertex & tool 
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificance::ProjectedDistanceSignificance
(  const LoKi::Vertices::ImpParBase& tool ) 
  : LoKi::Particles::ProjectedDistance ( tool ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificance::~ProjectedDistanceSignificance (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificance*
LoKi::Particles::ProjectedDistanceSignificance::clone() const
{ return new LoKi::Particles::ProjectedDistanceSignificance ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificance::result_type 
LoKi::Particles::ProjectedDistanceSignificance::operator() 
  ( LoKi::Particles::ProjectedDistanceSignificance::argument p ) const 
{ 
  //
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  //
  StatusCode sc = projected ( p , distance , error ) ;
  //
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 >= error ) 
  { 
    Warning ( "Invalid values from path, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return distance / error  ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ProjectedDistanceSignificance::fillStream( std::ostream& s ) const 
{ return s <<  "PROJDS" ; }


// ============================================================================
// constructor from the toolnickname 
// ============================================================================
LoKi::Particles::PathDistanceWithBestPV::PathDistanceWithBestPV
(  const std::string& geo ) 
  : LoKi::AuxFunBase ( std::tie ( geo ) ) 
  , LoKi::Particles::ImpParWithTheBestPV  ( geo ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::PathDistanceWithBestPV::~PathDistanceWithBestPV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PathDistanceWithBestPV*
LoKi::Particles::PathDistanceWithBestPV::clone() const
{ return new LoKi::Particles::PathDistanceWithBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PathDistanceWithBestPV::result_type 
LoKi::Particles::PathDistanceWithBestPV::operator() 
  ( LoKi::Particles::PathDistanceWithBestPV::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Warning ( "End-vertex points to NULL, return 'InvalidDistance'" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  //
  setVertex ( pv ) ;
  //
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  double chi2     = LoKi::Constants::InvalidChi2     ;
  //
  StatusCode sc = path ( p , distance , error , chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return InvalidDistance" , sc ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 >= error ||
       LoKi::Constants::InvalidChi2     == chi2     || 0 > chi2    ) 
  { 
    Warning ( "Invalid values from path, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;
  }
  //
  return distance ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PathDistanceWithBestPV::fillStream( std::ostream& s ) const 
{
  //
  s << "BPVPATHDIST" ;
  if ( !geo().empty() ) { s << "_(" << Gaudi::Utils::toString ( geo() ) << ")" ; }
  //
  return s ;
}


// ============================================================================
// constructor from the toolnickname 
// ============================================================================
LoKi::Particles::PathDistanceChi2WithBestPV::PathDistanceChi2WithBestPV
(  const std::string& geo ) 
  : LoKi::AuxFunBase ( std::tie ( geo ) ) 
  , LoKi::Particles::PathDistanceWithBestPV  ( geo ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::PathDistanceChi2WithBestPV::~PathDistanceChi2WithBestPV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PathDistanceChi2WithBestPV*
LoKi::Particles::PathDistanceChi2WithBestPV::clone() const
{ return new LoKi::Particles::PathDistanceChi2WithBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PathDistanceChi2WithBestPV::result_type 
LoKi::Particles::PathDistanceChi2WithBestPV::operator() 
  ( LoKi::Particles::PathDistanceChi2WithBestPV::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Warning ( "End-vertex points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  //
  setVertex ( pv ) ;
  //
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  double chi2     = LoKi::Constants::InvalidChi2     ;
  //
  StatusCode sc = path ( p , distance , error , chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 > error || 
       LoKi::Constants::InvalidChi2     == chi2     || 0 > chi2  ) 
  { 
    Warning ( "Invalid values from path, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return chi2  ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PathDistanceChi2WithBestPV::fillStream( std::ostream& s ) const 
{
  //
  s << "BPVPATHDISTCHI2" ;
  if ( !geo().empty() ) { s << "_(" << Gaudi::Utils::toString ( geo() ) << ")" ; }
  //
  return s ;
}




// ============================================================================
// constructor from the toolnickname 
// ============================================================================
LoKi::Particles::PathDistanceSignificanceWithBestPV::PathDistanceSignificanceWithBestPV
(  const std::string& geo ) 
  : LoKi::AuxFunBase ( std::tie ( geo ) ) 
  , LoKi::Particles::PathDistanceChi2WithBestPV  ( geo ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::PathDistanceSignificanceWithBestPV::~PathDistanceSignificanceWithBestPV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PathDistanceSignificanceWithBestPV*
LoKi::Particles::PathDistanceSignificanceWithBestPV::clone() const
{ return new LoKi::Particles::PathDistanceSignificanceWithBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PathDistanceSignificanceWithBestPV::result_type 
LoKi::Particles::PathDistanceSignificanceWithBestPV::operator() 
  ( LoKi::Particles::PathDistanceSignificanceWithBestPV::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Warning ( "End-vertex points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  //
  setVertex ( pv ) ;
  //
  return projected ( p ) ; 
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PathDistanceSignificanceWithBestPV::fillStream
( std::ostream& s ) const 
{
  //
  s << "BPVPDS" ;
  if ( !geo().empty() ) { s << "_(" << Gaudi::Utils::toString ( geo() ) << ")" ; }
  //
  return s ;
}








// ============================================================================
// constructor from the toolnickname 
// ============================================================================
LoKi::Particles::ProjectedDistanceWithBestPV::ProjectedDistanceWithBestPV
(  const std::string& geo ) 
  : LoKi::AuxFunBase ( std::tie ( geo ) ) 
  , LoKi::Particles::PathDistanceWithBestPV ( geo ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::ProjectedDistanceWithBestPV::~ProjectedDistanceWithBestPV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ProjectedDistanceWithBestPV*
LoKi::Particles::ProjectedDistanceWithBestPV::clone() const
{ return new LoKi::Particles::ProjectedDistanceWithBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProjectedDistanceWithBestPV::result_type 
LoKi::Particles::ProjectedDistanceWithBestPV::operator() 
  ( LoKi::Particles::ProjectedDistanceWithBestPV::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Warning ( "End-vertex points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  //
  setVertex ( pv ) ;
  //
  
  
  
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  double chi2     = LoKi::Constants::InvalidChi2     ;
  //
  StatusCode sc = path ( p , distance , error , chi2 ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return InvalidChi2" , sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 > error || 
       LoKi::Constants::InvalidChi2     == chi2     || 0 > chi2  ) 
  { 
    Warning ( "Invalid values from path, return InvalidChi2" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return distance / error ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ProjectedDistanceWithBestPV::fillStream
( std::ostream& s ) const 
{
  //
  s << "BPVPROJDIST" ;
  if ( !geo().empty() ) { s << "_(" << Gaudi::Utils::toString ( geo() ) << ")" ; }
  //
  return s ;
}






// ============================================================================
// constructor from the toolnickname 
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificanceWithBestPV::ProjectedDistanceSignificanceWithBestPV
(  const std::string& geo ) 
  : LoKi::AuxFunBase ( std::tie ( geo ) ) 
  , LoKi::Particles::ProjectedDistanceWithBestPV ( geo ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificanceWithBestPV::~ProjectedDistanceSignificanceWithBestPV(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificanceWithBestPV*
LoKi::Particles::ProjectedDistanceSignificanceWithBestPV::clone() const
{ return new LoKi::Particles::ProjectedDistanceSignificanceWithBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProjectedDistanceSignificanceWithBestPV::result_type 
LoKi::Particles::ProjectedDistanceSignificanceWithBestPV::operator() 
  ( LoKi::Particles::ProjectedDistanceSignificanceWithBestPV::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( NULL == pv ) 
  {
    Warning ( "End-vertex points to NULL, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;                    // RETURN 
  }
  //
  setVertex ( pv ) ;
  //
  double distance = LoKi::Constants::InvalidDistance ;
  double error    = LoKi::Constants::InvalidDistance ;
  //
  const StatusCode sc = projected ( p , distance , error ) ;
  //
  if ( sc.isFailure() ) 
  {
    Warning ( "Error from path, return InvalidChi2", sc ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  if ( LoKi::Constants::InvalidDistance == distance || 
       LoKi::Constants::InvalidDistance == error    || 0 >= error ) 
  { 
    Warning ( "Invalid values from path, return 'InvalidChi2'" ) ;
    return LoKi::Constants::InvalidChi2 ;
  }
  //
  return distance / error ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ProjectedDistanceSignificanceWithBestPV::fillStream
( std::ostream& s ) const 
{
  //
  s << "BPVPROJDS" ;
  if ( !geo().empty() ) { s << "_(" << Gaudi::Utils::toString ( geo() ) << ")" ; }
  //
  return s ;
}


// ============================================================================
// The END 
// ============================================================================
