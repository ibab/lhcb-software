// $Id: RichTabulatedRayleighScatter.cpp,v 1.2 2004-02-02 14:27:05 jonesc Exp $

// local
#include "RichTabulatedRayleighScatter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTabulatedRayleighScatter
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedRayleighScatter>          s_factory ;
const        IToolFactory& RichTabulatedRayleighScatterFactory = s_factory ;

// Standard constructor
RichTabulatedRayleighScatter::RichTabulatedRayleighScatter ( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_rayScatL( 0 ) {

  declareInterface<IRichRayleighScatter>(this);

  declareProperty( "RayScatLengthLocation", 
                   m_rayScatLoc = "/dd/Materials/RichMaterialTabProperties/AerogelRayleighPT" );

}

StatusCode RichTabulatedRayleighScatter::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  SmartDataPtr<TabulatedProperty> tab( detSvc(), m_rayScatLoc );
  if ( !tab ) {
    msg << MSG::ERROR << "Failed to load Rayleigh scattering length from "
        << m_rayScatLoc << endreq;
    return StatusCode::FAILURE;
  }
  m_rayScatL = new Rich1DTabProperty( tab );

  // Informational Printout
  msg << MSG::DEBUG
      << " Using XML tabulated implementation" << endreq
      << " Ray. Scat. length location   = " << m_rayScatLoc << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedRayleighScatter::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // clean up
  if ( m_rayScatL ) {  delete m_rayScatL; m_rayScatL = 0; }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double 
RichTabulatedRayleighScatter::photonScatteredProb( const RichRecSegment * segment,
                                                   const double energy ) const {

  // check this is aerogel
  if ( segment->trackSegment().radiator() != Rich::Aerogel ) return 0;

  // check energy is valid
  if ( energy <= 0 ) return 0;

  // check path length is valid
  const double path = segment->trackSegment().pathLength();
  if ( path <= 0 ) return 0;

  // compute and return prob
  const double scatLeng = (*m_rayScatL)[energy*eV];
  return ( 1 - (scatLeng/path)*(1. - exp(-1*path/scatLeng)) );
}
