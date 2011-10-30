// $Id$
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Particle.h"

// from ROOT
#include "Math/Boost.h"

// local
#include "P2VVAngleCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : P2VVAngleCalculator
//
// 2007-08-22 : Thomas Blake
// 2008-05-30 : Greig Cowan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( P2VVAngleCalculator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
P2VVAngleCalculator::P2VVAngleCalculator( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IP2VVAngleCalculator>(this);

}
//=============================================================================
// Destructor
//=============================================================================
P2VVAngleCalculator::~P2VVAngleCalculator() {} 

//=============================================================================

/// calculate the polar angle in the rest frame of the mother particle
double P2VVAngleCalculator::calculatePolarAngle( const Gaudi::LorentzVector& particleAxis,
                                            const Gaudi::LorentzVector& particleA,
                                            const Gaudi::LorentzVector& particleB ) {   
   
  const Gaudi::LorentzVector particleFrame = particleA + particleB ;
  
  ROOT::Math::Boost boost( particleFrame.BoostToCM() );
  
  const Gaudi::LorentzVector boostedAxis     = boost( particleAxis );
  const Gaudi::LorentzVector boostedParticle = boost( particleA );
  
  const Gaudi::XYZVector vecAxis     = Gaudi::XYZVector(boostedAxis).unit() ;
  const Gaudi::XYZVector vecParticle = Gaudi::XYZVector(boostedParticle).unit();
  
  double cosTheta = vecParticle.Dot(vecAxis) ;
  if (msgLevel(MSG::DEBUG)) debug() << "Theta_hel " << acos(cosTheta) << endmsg;
  return acos(cosTheta);
}

/// calculate the angle between two planes in the rest frame of the mother particle
double P2VVAngleCalculator::calculatePlaneAngle( const Gaudi::LorentzVector& particleFrame,
                                            const Gaudi::LorentzVector& particleA,
                                            const Gaudi::LorentzVector& particleB,
                                            const Gaudi::LorentzVector& particleC,
                                            const Gaudi::LorentzVector& particleD ) {
  
  ROOT::Math::Boost boost( particleFrame.BoostToCM() );

  const Gaudi::LorentzVector boostedAxis = boost( particleC+particleD );
  const Gaudi::LorentzVector boostedA   = boost( particleA );
  const Gaudi::LorentzVector boostedB   = boost( particleB );
  const Gaudi::LorentzVector boostedC = boost( particleC );
  const Gaudi::LorentzVector boostedD = boost( particleD );
  
  const Gaudi::XYZVector vecA = Gaudi::XYZVector( boostedA ).unit() ;
  const Gaudi::XYZVector vecB = Gaudi::XYZVector( boostedB ).unit() ;
  const Gaudi::XYZVector vecC = Gaudi::XYZVector( boostedC ).unit() ;
  const Gaudi::XYZVector vecD = Gaudi::XYZVector( boostedD ).unit() ;
  
  const Gaudi::XYZVector el = ( vecA.Cross( vecB ) ).unit() ;
  const Gaudi::XYZVector ek = ( vecC.Cross( vecD ) ).unit() ;
  const Gaudi::XYZVector ez = ( Gaudi::XYZVector( boostedAxis ) ).unit() ;
  
  double cosPhi = ( ek.Dot(el) );
  double sinPhi = ( el.Cross(ek) ).Dot( ez ) ;
  double phi    = acos( cosPhi ) ;  

  if (msgLevel(MSG::DEBUG)) debug() << "Phi_hel " << phi << endmsg;
  return ( sinPhi > 0.0 ? phi : -phi ) ;
}



// angle between L+ in dileption restframe and the z-axis
double P2VVAngleCalculator::calculateThetaTr( const Gaudi::LorentzVector& particleA, 
                                               const Gaudi::LorentzVector& particleB,
                                               const Gaudi::LorentzVector& particleC, 
                                               const Gaudi::LorentzVector& particleD)
{
  
   // J/Psi rest frame
   const Gaudi::LorentzVector particleFrame1 = particleA + particleB ;
     
   // phi rest frame
   const Gaudi::LorentzVector particleFrame2 = particleC + particleD ;
   
   ROOT::Math::Boost boost( particleFrame1.BoostToCM() );
      
   const Gaudi::LorentzVector boostedAxis     = boost( particleFrame2 );
   const Gaudi::LorentzVector boostedParticleA = boost( particleA );
   const Gaudi::LorentzVector boostedParticleC = boost( particleC );
  
   const Gaudi::XYZVector vecXAxis = Gaudi::XYZVector(boostedAxis).unit() ;
   const Gaudi::XYZVector vecPartA = Gaudi::XYZVector(boostedParticleA).unit();
   const Gaudi::XYZVector vecPartC = Gaudi::XYZVector(boostedParticleC).unit();
   Gaudi::XYZVector vecZAxis = (vecXAxis.Cross(vecPartC) ).unit();
   Gaudi::XYZVector vecYAxis = (vecXAxis.Cross(vecZAxis) ).unit();
  
   if ( vecYAxis.Dot(vecPartC) > 0.0 ){
      vecZAxis = -vecZAxis;
      vecYAxis = -vecYAxis;
   }

   double cosTheta_tr = vecPartA.Dot(vecZAxis) ;
   if (msgLevel(MSG::DEBUG)) debug() << "Theta_tr " << acos(cosTheta_tr) << endmsg;
   
   return acos(cosTheta_tr);
   
}

double P2VVAngleCalculator::calculatePhiTr( const Gaudi::LorentzVector& particleA,
					    const Gaudi::LorentzVector& particleB,
					    const Gaudi::LorentzVector& particleC,
					    const Gaudi::LorentzVector& particleD ) 
{

   // J/Psi rest frame
   const Gaudi::LorentzVector particleFrame1 = particleA + particleB ;
   // phi rest frame
   const Gaudi::LorentzVector particleFrame2 = particleC + particleD ;
  
   ROOT::Math::Boost boost( particleFrame1.BoostToCM() );
   
   const Gaudi::LorentzVector boostedAxis     = boost( particleFrame2 );
   const Gaudi::LorentzVector boostedParticleA = boost( particleA );
   const Gaudi::LorentzVector boostedParticleC = boost( particleC );
  
   const Gaudi::XYZVector vecXAxis = Gaudi::XYZVector(boostedAxis).unit() ;
   const Gaudi::XYZVector vecPartA = Gaudi::XYZVector(boostedParticleA).unit();
   const Gaudi::XYZVector vecPartC = Gaudi::XYZVector(boostedParticleC).unit();
   Gaudi::XYZVector vecZAxis = (vecXAxis.Cross(vecPartC) ).unit();
   Gaudi::XYZVector vecYAxis = (vecXAxis.Cross(vecZAxis) ).unit();
  
   if ( vecYAxis.Dot(vecPartC) > 0.0 ){
      vecZAxis = -vecZAxis;
      vecYAxis = -vecYAxis;
   }
   
   double _sinphi = vecPartA.Dot( vecYAxis );
   double _cosphi = vecPartA.Dot( vecXAxis );
   double Phi_tr = atan( _sinphi / _cosphi );
  
  //Solve sign ambiguity:
  if ( vecPartA.Dot( vecYAxis ) > 0 && vecPartA.Dot( vecXAxis ) < 0 )
     Phi_tr = Phi_tr + Gaudi::Units::pi;
  if ( vecPartA.Dot( vecYAxis ) < 0 && vecPartA.Dot( vecXAxis ) < 0 ) 
     Phi_tr = Phi_tr - Gaudi::Units::pi;

  if (msgLevel(MSG::DEBUG)) debug() << "Phi_tr " << Phi_tr << endmsg;
  return Phi_tr;
   
}




