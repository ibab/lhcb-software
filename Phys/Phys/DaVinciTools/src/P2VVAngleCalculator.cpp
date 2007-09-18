// $Id: P2VVAngleCalculator.cpp,v 1.1 2007-09-18 13:50:28 pkoppenb Exp $
// Include files 

// from Gaudi
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
  return acos(cosTheta);
}
 
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

  return ( sinPhi > 0.0 ? phi : -phi ) ;
}
