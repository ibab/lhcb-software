// $Id: RichFunctionalRayleighScatter.cpp,v 1.1 2003-08-26 14:40:18 jonrob Exp $

// local
#include "RichFunctionalRayleighScatter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichFunctionalRayleighScatter
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichFunctionalRayleighScatter>          s_factory ;
const        IToolFactory& RichFunctionalRayleighScatterFactory = s_factory ;

// Standard constructor
RichFunctionalRayleighScatter::RichFunctionalRayleighScatter ( const std::string& type,
                                                               const std::string& name,
                                                               const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_eVToMicron   ( 0 ),
    m_AeroClarity  ( 0 ) {

  declareInterface<IRichRayleighScatter>(this);

}

StatusCode RichFunctionalRayleighScatter::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Get Rich Detector elements
  SmartDataPtr<IDetectorElement> Rich1DE( detSvc(), "/dd/Structure/LHCb/Rich1" );

  // Rayleigh scattering parameters
  m_eVToMicron = ( h_Planck/(joule*s) * c_light/(m/s) / e_SI ) / nanometer ;
  m_AeroClarity = Rich1DE->userParameterAsDouble("AerogelClarity")/cm;

  // Informational Printout
  msg << MSG::DEBUG
      << " Using analytic implementation" << endreq
      << " eV to mm conversion factor   = " << m_eVToMicron << endreq
      << " Aerogel clarity              = " << m_AeroClarity << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichFunctionalRayleighScatter::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichFunctionalRayleighScatter::photonScatteredProb( const RichRecSegment * segment,
                                                           const double energy ) {

  // check this is aerogel
  if ( segment->trackSegment().radiator() != Rich::Aerogel ) return 0;

  // check energy is valid
  if ( energy <= 0 ) return 0;

  // check path length is valid
  double path = segment->trackSegment().pathLength();
  if ( path <= 0 ) return 0;

  // compute and return prob
  double lambda   = m_eVToMicron/energy;
  double scatLeng = lambda*lambda*lambda*lambda/m_AeroClarity;

  return 1 - (scatLeng/path)*(1-exp(-1*path/scatLeng));
}
