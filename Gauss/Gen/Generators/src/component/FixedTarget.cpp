// $Id: FixedTarget.cpp,v 1.3 2007-01-12 15:17:37 ranjard Exp $
// Include files 

// local
#include "FixedTarget.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedTarget
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( FixedTarget );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedTarget::FixedTarget( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_angleSmear( 0. ) {
    declareInterface< IBeamTool >( this ) ;
    declareProperty( "VerticalCrossingAngle" , 
                     m_verticalXAngle = 0. * Gaudi::Units::milliradian ) ;
    declareProperty( "HorizontalCrossingAngle" , 
                     m_horizontalXAngle = 0.285 * Gaudi::Units::milliradian ) ;
    declareProperty( "Emittance" , m_emittance = 0.503 * 1.e-9 * Gaudi::Units::radian * Gaudi::Units::m ) ;
    declareProperty( "BetaStar" , m_betaStar = 10. * Gaudi::Units::m ) ;
    declareProperty( "BeamMomentum" , m_beamMomentum = 7. * Gaudi::Units::TeV ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedTarget::~FixedTarget( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedTarget::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Initialize the number generator
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  
  sc = m_gaussianDist.initialize( randSvc , Rndm::Gauss( 0. , 1. ) )  ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize Gaussian random generator" , sc ) ;
  release( randSvc ) ;

  // set the value of angular smearing
  if ( 0.0 != m_betaStar ) 
    m_angleSmear = sqrt( m_emittance / m_betaStar ) ;
  else m_angleSmear = 0.0 ;

  debug() << "Collision with fixed target" << endmsg ;
  debug() << "Beam Momentum (TeV): " << m_beamMomentum / Gaudi::Units::TeV << endmsg ;
  debug() << "Vertical crossing angle (microradian): " 
          << m_verticalXAngle / ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg ;
  debug() << "Horizontal crossing angle (microradian): " 
          << m_horizontalXAngle / ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg ;
  debug() << "Emittance (1.e-9 rad.m): " 
          << m_emittance / ( 1.e-9 * Gaudi::Units::rad * Gaudi::Units::m ) << endmsg ;
  debug() << "Beta_star (m): " << m_betaStar / Gaudi::Units::m << endmsg ;
  debug() << "Angular beam smearing (microradian): " 
          << m_angleSmear / ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg ;

  return sc ;
}

//=============================================================================
// Mean value of the beam momentum
//=============================================================================
void FixedTarget::getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                                Gaudi::XYZVector & pBeam2 )
  const {
  double p1x, p1y, p1z ;
  p1x = m_beamMomentum * sin( m_horizontalXAngle ) ;
  p1y = m_beamMomentum * sin( m_verticalXAngle   ) ;
  p1z = m_beamMomentum ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;

  pBeam2.SetXYZ( 0., 0., 0. ) ;
}

//=============================================================================
// Current value of the smeared beams
//=============================================================================
void FixedTarget::getBeams( Gaudi::XYZVector & pBeam1 , 
                            Gaudi::XYZVector & pBeam2 ) {
  double p1x, p1y, p1z ;
  p1x = m_beamMomentum * sin( m_horizontalXAngle + 
                              m_gaussianDist() * m_angleSmear ) ;
  p1y = m_beamMomentum * sin( m_verticalXAngle + 
                              m_gaussianDist() * m_angleSmear ) ;
  p1z = m_beamMomentum ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;

  pBeam2.SetXYZ( 0., 0., 0. ) ;
}
