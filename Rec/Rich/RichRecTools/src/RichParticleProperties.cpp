
/** @file RichParticleProperties.cpp
 *
 *  Implementation file for tool : RichParticleProperties
 *
 *  CVS Log :-
 *  $Id: RichParticleProperties.cpp,v 1.9 2004-10-13 09:52:41 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2004/07/27 20:15:30  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichParticleProperties.h"

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

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  IRichRefractiveIndex * refIndex;
  acquireTool( "RichRefractiveIndex", refIndex );

  // Retrieve particle property service
  IParticlePropertySvc * ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc" );

  // Retrieve particle masses
  m_particleMass[Rich::Electron] = ppSvc->find("e+" )->mass()/MeV;
  m_particleMass[Rich::Muon]     = ppSvc->find("mu+")->mass()/MeV;
  m_particleMass[Rich::Pion]     = ppSvc->find("pi+")->mass()/MeV;
  m_particleMass[Rich::Kaon]     = ppSvc->find("K+" )->mass()/MeV;
  m_particleMass[Rich::Proton]   = ppSvc->find("p+" )->mass()/MeV;

  // cache squares of masses
  m_particleMassSq[Rich::Electron] = gsl_pow_2( m_particleMass[Rich::Electron] );
  m_particleMassSq[Rich::Muon]     = gsl_pow_2( m_particleMass[Rich::Muon]     );
  m_particleMassSq[Rich::Pion]     = gsl_pow_2( m_particleMass[Rich::Pion]     );
  m_particleMassSq[Rich::Kaon]     = gsl_pow_2( m_particleMass[Rich::Kaon]     );
  m_particleMassSq[Rich::Proton]   = gsl_pow_2( m_particleMass[Rich::Proton]   );

  // release service
  release(ppSvc);

  // Informational Printout
  debug() << " Particle masses (MeV/c^2)     = " << m_particleMass << endreq;

  // Setup momentum thresholds
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    const Rich::RadiatorType rad = static_cast<Rich::RadiatorType>(iRad);
    debug() << " Particle thresholds (MeV/c^2) : " << rad << " : ";
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      const double index = refIndex->refractiveIndex(rad);
      m_momThres[iRad][iHypo] = m_particleMass[iHypo]/sqrt(index*index - 1.0);
      debug() << m_momThres[iRad][iHypo] << " ";
    }
    debug() << endreq;
  }

  // release tool
  releaseTool(refIndex);

  return StatusCode::SUCCESS;
}

StatusCode RichParticleProperties::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichParticleProperties::beta( RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) const
{
  const double momentum = segment->trackSegment().bestMomentum().mag();
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
