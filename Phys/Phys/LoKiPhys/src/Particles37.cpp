// $Id: $
// STD
#include <cmath>

// LoKi
#include <LoKi/Vertices0.h>
#include <LoKi/PhysHelpers.h>
#include <LoKi/PhysSources.h>
#include <LoKi/Particles37.h>

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

namespace LoKi {
namespace Particles {

//=============================================================================
// constructor from a vertex
//=============================================================================
DecayLengthSignificance::DecayLengthSignificance( const LHCb::VertexBase* vertex )
   : LoKi::BasicFunctors< const LHCb::Particle* >::Function()
   , LoKi::Vertices::VertexHolder( vertex )
{

}

//=============================================================================
// constructor from a point
//=============================================================================
DecayLengthSignificance::DecayLengthSignificance( const LoKi::Point3D& point )
   : LoKi::BasicFunctors< const LHCb::Particle* >::Function()
   , LoKi::Vertices::VertexHolder( point )
{

}

//=============================================================================
// constructor from a base class
//=============================================================================
DecayLengthSignificance::DecayLengthSignificance
( const LoKi::Vertices::VertexHolder& base )
   : LoKi::BasicFunctors< const LHCb::Particle* >::Function()
   , LoKi::Vertices::VertexHolder( base )
{

}

//=============================================================================
// copy constructor
//=============================================================================
DecayLengthSignificance::DecayLengthSignificance
( const DecayLengthSignificance& other )
   : LoKi::AuxFunBase( other ),
     LoKi::BasicFunctors< const LHCb::Particle* >::Function( other )
   , LoKi::Vertices::VertexHolder( other )
{

}

//=============================================================================
// MANDATORY: virtual destructor
//=============================================================================
DecayLengthSignificance::~DecayLengthSignificance()
{

} 

//=============================================================================
// MANDATORY: clone method ("virtual constructor")
//=============================================================================
DecayLengthSignificance* DecayLengthSignificance::clone() const
{ 
   return new DecayLengthSignificance( *this );
}

//=============================================================================
// The method where the calculation is done
//=============================================================================
DecayLengthSignificance::result_type DecayLengthSignificance::dls
( argument p ) const
{
   if( 0 == p ) {
      Error( " Invalid Particle, return 'InvalidDistance'");
      return LoKi::Constants::InvalidDistance;
   }

   // Get a pointer to the vertex we hold
   const LHCb::VertexBase* v = vertex();
   if( 0 == v ) {
      Error( " Invalid Vertex, return 'InvalidDistance'" );
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
   for ( size_t row = 0; row < 3; ++row ) {
      for ( size_t col = 0; col <= row; ++col ) {
         W( row, col ) +=  pCov( row, col )
            + a * a * pCov( row + 3, col + 3 )
            - a * ( pCov( row + 3, col ) + pCov( col + 3, row) );
      }
   }
 
   int OK = W.Invert() ;
   if( !OK ) { 
      Error( "Error inverting the updated covariance matrix." );
   }
 
   // Calculate the required numbers.
   double halfdChi2dLam2 = ROOT::Math::Similarity( W, dir );
   double decayLength    = ROOT::Math::Dot( dir , W * flight ) / halfdChi2dLam2;
   double decayLengthErr = std::sqrt( 1 / halfdChi2dLam2 );

   return decayLength / decayLengthErr;

   // Gaudi::Vector3 res = flight - decaylength * dir ;
   // chi2 = ROOT::Math::Similarity( W, res );

}

//=============================================================================
// MANDATORY: the only essential method 
//=============================================================================
DecayLengthSignificance::result_type DecayLengthSignificance::operator()
   ( argument p ) const
{
   return dls( p );
}

//=============================================================================
// OPTIONAL: nice printout 
//=============================================================================
std::ostream& DecayLengthSignificance::fillStream( std::ostream& stream ) const
{
   return stream << "DLS" ;
}


//=============================================================================
// default constructor
//=============================================================================
DecayLengthSignificanceDV::DecayLengthSignificanceDV()
   : LoKi::AuxDesktopBase(),
     DecayLengthSignificance( LoKi::Point3D() )
{

}

//=============================================================================
// copy constructor
//=============================================================================
DecayLengthSignificanceDV::DecayLengthSignificanceDV
( const DecayLengthSignificanceDV& other )
   : LoKi::AuxFunBase( other ),
     LoKi::AuxDesktopBase( other ),
     DecayLengthSignificance( other )
{

}

//=============================================================================
// MANDATORY: the only essential method 
//=============================================================================
DecayLengthSignificanceDV::result_type 
DecayLengthSignificanceDV::operator()
   ( DecayLengthSignificanceDV::argument p ) const
{
   if( 0 == p ) {
      Error( " Invalid Particle, return 'InvalidDistance'");
      return LoKi::Constants::InvalidDistance;
   }

   // Get the best vertex from desktop.
   const LHCb::VertexBase* pv = bestVertex( p );
   if( 0 == pv ) {
      Error( " Invalid Primary Vertex for Particle, return 'InvalidDistance'");
      return LoKi::Constants::InvalidDistance;
   }

   // Set our vertex to the best PV
   setVertex( pv );

   return dls( p ) ; 
}

} // namespace Particles
} // namespace LoKi
