
//-----------------------------------------------------------------------------
/** @file RichParticleProperties.cpp
 *
 *  Implementation file for tool : Rich::ParticleProperties
 *
 *  CVS Log :-
 *  $Id: RichParticleProperties.cpp,v 1.8 2009-07-30 12:14:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichParticleProperties.h"

// From LHCb 
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, ParticleProperties );

// Standard constructor
Rich::ParticleProperties::ParticleProperties ( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_pidTypes   ( Rich::particles()  )
{

  // declare interface
  declareInterface<IParticleProperties>(this);

  // PID types
  declareProperty( "ParticleTypes", m_pidTypesJO );

}

StatusCode Rich::ParticleProperties::initialize()
{

  // Sets up various tools and services
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  const IRefractiveIndex * refIndex;
  acquireTool( "RichRefractiveIndex", refIndex );

  // Retrieve particle property service
  LHCb::IParticlePropertySvc * ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

  // Retrieve particle masses
  m_particleMass[Rich::Electron] = ppSvc->find("e+" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Muon]     = ppSvc->find("mu+")->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Pion]     = ppSvc->find("pi+")->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Kaon]     = ppSvc->find("K+" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Proton]   = ppSvc->find("p+" )->mass()/Gaudi::Units::MeV;

  // cache squares of masses
  m_particleMassSq[Rich::Electron] = gsl_pow_2( m_particleMass[Rich::Electron] );
  m_particleMassSq[Rich::Muon]     = gsl_pow_2( m_particleMass[Rich::Muon]     );
  m_particleMassSq[Rich::Pion]     = gsl_pow_2( m_particleMass[Rich::Pion]     );
  m_particleMassSq[Rich::Kaon]     = gsl_pow_2( m_particleMass[Rich::Kaon]     );
  m_particleMassSq[Rich::Proton]   = gsl_pow_2( m_particleMass[Rich::Proton]   );

  // Informational Printout
  debug() << " Particle masses (MeV/c^2)     = " << m_particleMass << endmsg;

  // Setup momentum thresholds
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    const Rich::RadiatorType rad = static_cast<Rich::RadiatorType>(iRad);
    debug() << " Particle thresholds (MeV/c^2) : " << rad << " : ";
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
    {
      const double index = refIndex->refractiveIndex(rad);
      m_momThres[iRad][iHypo]  = m_particleMass[iHypo]/sqrt(index*index - 1.0);
      m_momThres2[iRad][iHypo] = m_momThres[iRad][iHypo]*m_momThres[iRad][iHypo];
      debug() << m_momThres[iRad][iHypo] << " ";
    }
    debug() << endmsg;
  }

  // release tool
  releaseTool(refIndex);

  // release service
  sc = release(ppSvc);

  // PID Types
  bool hasPion(true);
  if ( !m_pidTypesJO.empty() )
  {
    hasPion = false;
    m_pidTypes.clear();
    for ( std::vector<std::string>::const_iterator iS = m_pidTypesJO.begin();
          iS != m_pidTypesJO.end(); ++iS )
    {
      if      ( "electron"       == *iS ) { m_pidTypes.push_back(Rich::Electron); }
      else if ( "muon"           == *iS ) { m_pidTypes.push_back(Rich::Muon); }
      else if ( "pion"           == *iS ) { m_pidTypes.push_back(Rich::Pion); hasPion = true; }
      else if ( "kaon"           == *iS ) { m_pidTypes.push_back(Rich::Kaon); }
      else if ( "proton"         == *iS ) { m_pidTypes.push_back(Rich::Proton); }
      else if ( "belowThreshold" == *iS ) { m_pidTypes.push_back(Rich::BelowThreshold); }
      else
      {
        return Error( "Unknown particle type from options " + *iS );
      }
    }
  }
  info() << "Particle types considered = " << m_pidTypes << endmsg;
  if ( m_pidTypes.empty() ) return Error( "No particle types specified" );
  if ( !hasPion )           return Error( "Pion hypothesis must be included in list" );

  return sc;
}

StatusCode Rich::ParticleProperties::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

double Rich::ParticleProperties::beta( const double ptot,
                                       const Rich::ParticleIDType id ) const
{
  const double Esquare  = ptot*ptot + m_particleMassSq[id];
  return ( Esquare > 0 ? ptot/sqrt(Esquare) : 0 );
}

double Rich::ParticleProperties::mass( const Rich::ParticleIDType id ) const
{
  return m_particleMass[id];
}

double Rich::ParticleProperties::massSq( const Rich::ParticleIDType id ) const
{
  return m_particleMassSq[id];
}

double Rich::ParticleProperties::thresholdMomentum( const Rich::ParticleIDType id,
                                                    const Rich::RadiatorType rad ) const
{
  return m_momThres[rad][id];
}

double Rich::ParticleProperties::thresholdMomentumSq( const Rich::ParticleIDType id,
                                                      const Rich::RadiatorType rad ) const
{
  return m_momThres2[rad][id];
}

const Rich::Particles & Rich::ParticleProperties::particleTypes() const
{
  return m_pidTypes;
}
