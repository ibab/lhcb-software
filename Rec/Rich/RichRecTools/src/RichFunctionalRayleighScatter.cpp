// $Id: RichFunctionalRayleighScatter.cpp,v 1.5 2004-03-16 13:45:02 jonesc Exp $

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

  debug() << "Initialize" << endreq;

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Get Rich1 Detector element
  DeRich1 * Rich1DE = getDet<DeRich1>( DeRich1Location::Default );

  // Rayleigh scattering parameters
  m_eVToMicron  = ( h_Planck/(joule*s) * c_light/(m/s) / e_SI ) / nanometer ;
  m_AeroClarity = Rich1DE->userParameterAsDouble( "AerogelClarity" )/cm;

  // Informational Printout
  debug() << " Using analytic implementation" << endreq
          << " eV to mm conversion factor   = " << m_eVToMicron << endreq
          << " Aerogel clarity              = " << m_AeroClarity << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichFunctionalRayleighScatter::finalize() {

  debug() << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichFunctionalRayleighScatter::photonScatteredProb( const RichRecSegment * segment,
                                                    const double energy ) const {

  // check this is aerogel
  if ( segment->trackSegment().radiator() != Rich::Aerogel ) return 0;

  // check energy is valid
  if ( energy <= 0 ) return 0;

  // check path length is valid
  const double path = segment->trackSegment().pathLength();
  if ( path <= 0 ) return 0;

  // compute and return prob
  const double lambda   = m_eVToMicron/energy;
  const double scatLeng = lambda*lambda*lambda*lambda/m_AeroClarity;

  return 1 - (scatLeng/path)*(1-exp(-1*path/scatLeng));
}
