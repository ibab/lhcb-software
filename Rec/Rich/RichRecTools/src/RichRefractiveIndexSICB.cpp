// $Id: RichRefractiveIndexSICB.cpp,v 1.1 2003-08-26 14:40:20 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichRefractiveIndexSICB.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichRefractiveIndexSICB
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRefractiveIndexSICB>          s_factory ;
const        IToolFactory& RichRefractiveIndexSICBFactory = s_factory ;

// Standard constructor
RichRefractiveIndexSICB::RichRefractiveIndexSICB ( const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichRefractiveIndex>(this);

  // Define job option parameters
}

StatusCode RichRefractiveIndexSICB::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);

  // Get the Quantum Eff
  // For time being assume only one reference curve for all HPDs
  m_referenceQE = m_richDetInt->hpdQuantumEff();
  if ( !m_referenceQE ) {
    msg << MSG::ERROR << "Failed to acquire QE function" << endreq;
    return StatusCode::FAILURE;
  }

  // release det interface
  releaseTool( m_richDetInt );

  // Initialise W function for expected signal calculation
  // get from sellmeir tool !!!
  m_selF1[Rich::Aerogel] = 2.653;
  m_selF1[Rich::C4F10]   = 12305.3;
  m_selF1[Rich::CF4]     = 7626.16;
  m_selF2[Rich::Aerogel] = 13.075;
  m_selF2[Rich::C4F10]   = 0.0;
  m_selF2[Rich::CF4]     = 0.0;
  m_selE1[Rich::Aerogel] = 10.666;
  m_selE1[Rich::C4F10]   = 18.938;
  m_selE1[Rich::CF4]     = 20.003;
  m_selE2[Rich::Aerogel] = 18.125;
  m_selE2[Rich::C4F10]   = 50.0;
  m_selE2[Rich::CF4]     = 50.0;
  m_molW[Rich::Aerogel]  = 0;
  m_molW[Rich::C4F10]    = 138.0;
  m_molW[Rich::CF4]      = 88.0;
  m_rho[Rich::Aerogel]   = 0;
  m_rho[Rich::C4F10]     = 0.00964;
  m_rho[Rich::CF4]       = 0.00366;

  // Informational Printout
  msg << MSG::DEBUG << " Using hardcoded SICB implementation" << endreq;

  return sc;
}

StatusCode RichRefractiveIndexSICB::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichRefractiveIndexSICB::refractiveIndex( const Rich::RadiatorType radiator,
                                                 double energy ) {

  double index = 0;
  double fe =
    m_selF1[radiator]/(m_selE1[radiator]*m_selE1[radiator] - energy*energy) +
    m_selF2[radiator]/(m_selE2[radiator]*m_selE2[radiator] - energy*energy);
  if ( Rich::Aerogel == radiator ) {
    index = sqrt( fe + 1 );
  } else {
    double cfe = 0.3738*(m_rho[radiator]/m_molW[radiator])*fe;
    index = sqrt( (1+2*cfe)/(1-cfe) );
  }

  return index;
}

double RichRefractiveIndexSICB::refractiveIndex( RichRecSegment * segment ) {
  return refractiveIndex( segment->trackSegment().radiator() );
}

double RichRefractiveIndexSICB::refractiveIndex( const Rich::RadiatorType radiator ) {

  double meanEnergy = m_referenceQE->meanX( m_referenceQE->minX(),
                                            m_referenceQE->maxX() ) / eV;
  return refractiveIndex( radiator, meanEnergy );
}
