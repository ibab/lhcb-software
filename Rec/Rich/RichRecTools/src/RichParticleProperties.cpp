// $Id: RichParticleProperties.cpp,v 1.5 2004-02-02 14:27:00 jonesc Exp $

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

  // Retrieve particle masses
  m_particleMass[Rich::Electron] = ppSvc->find("e+" )->mass()/MeV;
  m_particleMass[Rich::Muon]     = ppSvc->find("mu+")->mass()/MeV;
  m_particleMass[Rich::Pion]     = ppSvc->find("pi+")->mass()/MeV;
  m_particleMass[Rich::Kaon]     = ppSvc->find("K+" )->mass()/MeV;
  m_particleMass[Rich::Proton]   = ppSvc->find("p+" )->mass()/MeV;
 
  // cache squares of masses
  m_particleMassSq[Rich::Electron] = gsl_pow_2( m_particleMass[Rich::Electron] );
  m_particleMassSq[Rich::Muon]     = gsl_pow_2( m_particleMass[Rich::Muon] );
  m_particleMassSq[Rich::Pion]     = gsl_pow_2( m_particleMass[Rich::Pion] );
  m_particleMassSq[Rich::Kaon]     = gsl_pow_2( m_particleMass[Rich::Kaon] );
  m_particleMassSq[Rich::Proton]   = gsl_pow_2( m_particleMass[Rich::Proton] );

  // release service
  ppSvc->release();
  
  // Informational Printout
  msg << MSG::DEBUG
      << " Particle masses (MeV/c^2)     = " << m_particleMass << endreq;

  // Setup momentum thresholds
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    const Rich::RadiatorType rad = static_cast<Rich::RadiatorType>(iRad);
    msg << MSG::DEBUG << " Particle thresholds (MeV/c^2) = Rad " << rad << " : ";
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      const double refIndex = m_refIndex->refractiveIndex(rad);
      m_momThres[iRad][iHypo] = m_particleMass[iHypo]/sqrt(refIndex*refIndex - 1.0);
      msg << m_momThres[iRad][iHypo] << " ";
    }
    msg << endreq;
  }

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
                                     const Rich::ParticleIDType id ) const
{
  const double momentum = segment->trackSegment().bestMomentumMag();
  const double Esquare = momentum*momentum + m_particleMassSq[id];
  return ( Esquare > 0 ? momentum/sqrt(Esquare) : 0 );
}

double RichParticleProperties::mass( const Rich::ParticleIDType id ) const
{
  return m_particleMass[id];
}

double RichParticleProperties::massSq( const Rich::ParticleIDType id ) const
{
  return m_particleMassSq[id];
}

double RichParticleProperties::thresholdMomentum( const Rich::ParticleIDType id,
                                                  const Rich::RadiatorType rad ) const
{
  return m_momThres[rad][id];
}
