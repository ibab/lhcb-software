// $Id: RichParticleProperties.cpp,v 1.1 2003-08-06 11:08:12 jonrob Exp $

// local
#include "RichParticleProperties.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichParticleProperties
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichParticleProperties>          s_factory ;
const        IToolFactory& RichParticlePropertiesFactory = s_factory ;

// Standard constructor
RichParticleProperties::RichParticleProperties ( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichParticleProperties>(this);

}

StatusCode RichParticleProperties::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichRefractiveIndex", m_refIndex );

  // Retrieve particle property service
  IParticlePropertySvc * ppSvc = 0;
  if ( !serviceLocator()->service( "ParticlePropertySvc", ppSvc ) ) {
    msg << MSG::ERROR << "ParticlePropertySvc not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Rayleigh scattering parameters
  m_rayleighPara[0] = 1.24; // Transform from eV to microns
  m_rayleighPara[1] = 0.0008;

  // Retrieve particle masses
  m_particleMass.push_back( ppSvc->find("e+" )->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("mu+")->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("pi+")->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("K+" )->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("p+" )->mass()/MeV );
  ppSvc->release();
  // cache squares of masses
  m_particleMassSq.push_back( m_particleMass[ Rich::Electron ] *
                              m_particleMass[ Rich::Electron ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Muon ] *
                              m_particleMass[ Rich::Muon ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Pion ] *
                              m_particleMass[ Rich::Pion ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Kaon ] *
                              m_particleMass[ Rich::Kaon ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Proton ] *
                              m_particleMass[ Rich::Proton ] );

  // Setup momentum thresholds
  {for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      double refIndex = m_refIndex->refractiveIndex((Rich::RadiatorType)iRad);
      m_momThres[iRad][iHypo] = m_particleMass[iHypo]/sqrt(refIndex*refIndex - 1.0);
    }
  }}

  // Informational Printout
  msg << MSG::DEBUG
      << " Particle masses (MeV)      = " << m_particleMass << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichParticleProperties::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_refIndex );

  // Execute base class method
  return RichRecToolBase::finalize();
}


double RichParticleProperties::beta( RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) 
{
  double momentum = segment->trackSegment().bestMomentumMag();
  double Esquare = momentum*momentum + m_particleMassSq[id];
  return (Esquare > 0.0 ? momentum/sqrt(Esquare) : 0.0);
}

double RichParticleProperties::mass( const Rich::ParticleIDType id ) 
{
  return m_particleMass[id];
}

double RichParticleProperties::massSq( const Rich::ParticleIDType id ) 
{
  return m_particleMassSq[id];
}

double RichParticleProperties::thresholdMomentum( const Rich::ParticleIDType id,
                                                  const Rich::RadiatorType rad ) 
{
  return m_momThres[rad][id];
}
