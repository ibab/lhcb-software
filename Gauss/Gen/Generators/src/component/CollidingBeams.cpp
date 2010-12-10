// $Id: CollidingBeams.cpp,v 1.5 2007-02-08 17:46:06 gcorti Exp $
// Include files 

// local
#include "CollidingBeams.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

// From Kernel
#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CollidingBeams
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( CollidingBeams );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CollidingBeams::CollidingBeams( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_angleSmear( 0. ) {
    declareInterface< IBeamTool >( this ) ;
    declareProperty( "VerticalCrossingAngle" , 
                     m_verticalXAngle = 0. * Gaudi::Units::milliradian ) ;
    declareProperty( "HorizontalCrossingAngle" , 
                     m_horizontalXAngle = 0.285 * Gaudi::Units::milliradian ) ;
    declareProperty( "Emittance" , 
                     m_emittance = 0.503 * 1.e-9 * Gaudi::Units::radian * Gaudi::Units::m  ) ;
    declareProperty( "BetaStar" , m_betaStar = 10. * Gaudi::Units::m ) ;
    declareProperty( "BeamMomentum" , m_beamMomentum = 7. * Gaudi::Units::TeV ) ;
    declareProperty( "VerticalBeamLineAngle",
                     m_vBeamLineAngle = 0. * Gaudi::Units::milliradian ) ;
    declareProperty( "HorizontalBeamLineAngle",
                     m_hBeamLineAngle = 0. * Gaudi::Units::milliradian ) ;
                     
}

//=============================================================================
// Destructor 
//=============================================================================
CollidingBeams::~CollidingBeams( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode CollidingBeams::initialize( ) {
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
    m_angleSmear = sqrt( m_emittance / m_betaStar) ;
  else m_angleSmear = 0.0 ;

  info() << "Colliding Beam Parameters: " << endmsg ;
  info() << "Beam Momentum (TeV): " << m_beamMomentum / Gaudi::Units::TeV << endmsg ;
  info() << "Vertical crossing angle (microradian): " 
          << m_verticalXAngle / ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg ;
  info() << "Horizontal crossing angle (microradian): " 
          << m_horizontalXAngle / ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg ;
  info() << "Beam Line rotated (x,y) in microradian: "
         << m_hBeamLineAngle / ( Gaudi::Units::milliradian * 1.e-3 ) << ", "
         << m_vBeamLineAngle / ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg;
  debug() << "Emittance (1.e-9 rad.m): " 
          << m_emittance / ( 1.e-9 * Gaudi::Units::rad * Gaudi::Units::m ) << endmsg ;
  debug() << "Beta_star (m): " << m_betaStar / Gaudi::Units::m << endmsg ;
  debug() << "Angular beam smearing (microradian): " 
          << m_angleSmear / ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg ;
  debug() << "Beam1 horizontal and vertical angle in LHCb (microrad): "
          << (m_horizontalXAngle + m_hBeamLineAngle) / 
             ( Gaudi::Units::milliradian * 1.e-3 ) << ", "
          << (m_verticalXAngle + m_vBeamLineAngle) /
             ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg;  
  debug() << "Beam2 horizontal and vertical angle in LHCb (microrad): "
          << - (m_horizontalXAngle - m_hBeamLineAngle) / 
             ( Gaudi::Units::milliradian * 1.e-3 ) << ", "
          << - (m_verticalXAngle - m_vBeamLineAngle) /
             ( Gaudi::Units::milliradian * 1.e-3 ) << endmsg;  
  

  return sc ;
}

//=============================================================================
// Mean value of the beam momentum
//=============================================================================
void CollidingBeams::getMeanBeams( Gaudi::XYZVector & pBeam1 , 
                                   Gaudi::XYZVector & pBeam2 )
  const {
  double p1x, p1y, p1z, p2x, p2y, p2z ;
  p1x = m_beamMomentum * sin( m_horizontalXAngle + m_hBeamLineAngle ) ;
  p1y = m_beamMomentum * sin( m_verticalXAngle + m_vBeamLineAngle  ) ;
  p1z = m_beamMomentum ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;

  p2x = m_beamMomentum * sin( m_horizontalXAngle - m_hBeamLineAngle ) ;
  p2y = m_beamMomentum * sin( m_verticalXAngle - m_vBeamLineAngle  ) ;
  p2z = -m_beamMomentum ;
  pBeam2.SetXYZ( p2x, p2y, p2z ) ;
}

//=============================================================================
// Current value of the smeared beams
//=============================================================================
void CollidingBeams::getBeams( Gaudi::XYZVector & pBeam1 , 
                               Gaudi::XYZVector & pBeam2 ) {
  double p1x, p1y, p1z, p2x, p2y, p2z ;
  p1x = m_beamMomentum * sin( m_horizontalXAngle + m_hBeamLineAngle +
                              m_gaussianDist() * m_angleSmear ) ;
  p1y = m_beamMomentum * sin( m_verticalXAngle + m_vBeamLineAngle +
                              m_gaussianDist() * m_angleSmear ) ;
  p1z = m_beamMomentum ;
  pBeam1.SetXYZ( p1x, p1y, p1z ) ;

  p2x = m_beamMomentum * sin( m_horizontalXAngle - m_hBeamLineAngle + 
                              m_gaussianDist() * m_angleSmear ) ;
  p2y = m_beamMomentum * sin( m_verticalXAngle - m_vBeamLineAngle +
                              m_gaussianDist() * m_angleSmear ) ;
  p2z = -m_beamMomentum ;
  pBeam2.SetXYZ( p2x, p2y, p2z ) ;
}
