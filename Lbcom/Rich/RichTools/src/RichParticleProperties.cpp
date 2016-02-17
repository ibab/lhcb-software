
//-----------------------------------------------------------------------------
/** @file RichParticleProperties.cpp
 *
 *  Implementation file for tool : Rich::ParticleProperties
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
#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, ParticleProperties )

// Standard constructor
Rich::ParticleProperties::ParticleProperties ( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
  : Rich::ToolBase   ( type, name, parent        ),
    m_particleMass   ( Rich::NParticleTypes, 0.0 ),
    m_particleMassSq ( Rich::NParticleTypes, 0.0 ),
    m_pidTypes       ( Rich::particles()         )
{
  // declare interface
  declareInterface<IParticleProperties>(this);
  // PID types
  declareProperty( "ParticleTypes", m_pidTypesJO );
  //setProperty( "OutputLevel", 2 );
}

StatusCode Rich::ParticleProperties::initialize()
{

  // Sets up various tools and services
  StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRefractiveIndex", m_refIndex );

  // Retrieve particle property service
  LHCb::IParticlePropertySvc * ppSvc = 
    svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

  // Retrieve particle masses
  m_particleMass[Rich::Electron] = ppSvc->find("e+" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Muon]     = ppSvc->find("mu+")->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Pion]     = ppSvc->find("pi+")->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Kaon]     = ppSvc->find("K+" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Proton]   = ppSvc->find("p+" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Deuteron] = ppSvc->find("deuteron" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::BelowThreshold] = boost::numeric::bounds<double>::highest();

  // cache squares of masses
  m_particleMassSq[Rich::Electron] = std::pow( m_particleMass[Rich::Electron] , 2 );
  m_particleMassSq[Rich::Muon]     = std::pow( m_particleMass[Rich::Muon]     , 2 );
  m_particleMassSq[Rich::Pion]     = std::pow( m_particleMass[Rich::Pion]     , 2 );
  m_particleMassSq[Rich::Kaon]     = std::pow( m_particleMass[Rich::Kaon]     , 2 );
  m_particleMassSq[Rich::Proton]   = std::pow( m_particleMass[Rich::Proton]   , 2 );
  m_particleMassSq[Rich::Deuteron] = std::pow( m_particleMass[Rich::Deuteron] , 2 );
  m_particleMassSq[Rich::BelowThreshold] = boost::numeric::bounds<double>::highest();

  // release service
  sc = release(ppSvc);

  // PID Types
  if ( !m_pidTypesJO.empty() )
  {
    m_pidTypes.clear();
    for ( const auto& S : m_pidTypesJO )
    {
      if      ( "electron"       == S ) { m_pidTypes.push_back(Rich::Electron); }
      else if ( "muon"           == S ) { m_pidTypes.push_back(Rich::Muon); }
      else if ( "pion"           == S ) { m_pidTypes.push_back(Rich::Pion); }
      else if ( "kaon"           == S ) { m_pidTypes.push_back(Rich::Kaon); }
      else if ( "proton"         == S ) { m_pidTypes.push_back(Rich::Proton); }
      else if ( "deuteron"       == S ) { m_pidTypes.push_back(Rich::Deuteron); }
      else if ( "belowThreshold" == S ) { m_pidTypes.push_back(Rich::BelowThreshold); }
      else
      {
        return Error( "Unknown particle type from options " + S );
      }
    }
  }
  // sort the list to ensure strict (increasing) mass ordering.
  std::sort( m_pidTypes.begin(), m_pidTypes.end() );
  // is pion in the list ?
  const bool hasPion = std::find( m_pidTypes.begin(),
                                  m_pidTypes.end(),
                                  Rich::Pion ) != m_pidTypes.end();
  info() << "Particle types considered = " << m_pidTypes << endmsg;
  if ( m_pidTypes.empty() ) return Error( "No particle types specified" );
  if ( !hasPion )           return Error( "Pion hypothesis must be enabled" );

  // Informational Printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " Particle masses (MeV/c^2)     = " << m_particleMass << endmsg;
    for ( const auto& pid : m_pidTypes )
    {
      debug() << pid << " Momentum Thresholds (GeV/c) :"
        //<< " Aero=" << thresholdMomentum(pid,Rich::Aerogel)/Gaudi::Units::GeV
              << " R1Gas=" << thresholdMomentum(pid,Rich::Rich1Gas)/Gaudi::Units::GeV
              << " R2Gas=" << thresholdMomentum(pid,Rich::Rich2Gas)/Gaudi::Units::GeV
              << endmsg;
    }
  }

  return sc;
}

double Rich::ParticleProperties::beta( const double ptot,
                                       const Rich::ParticleIDType id ) const
{
  const double Esquare = ptot*ptot + m_particleMassSq[id];
  return ( Rich::BelowThreshold != id && Esquare > 0 ? ptot/std::sqrt(Esquare) : 0 );
}

double Rich::ParticleProperties::mass( const Rich::ParticleIDType id ) const
{
  return m_particleMass[id];
}

double Rich::ParticleProperties::massSq( const Rich::ParticleIDType id ) const
{
  return m_particleMassSq[id];
}

double
Rich::ParticleProperties::thresholdMomentum( const Rich::ParticleIDType id,
                                             const Rich::RadiatorType rad ) const
{
  double thresP = boost::numeric::bounds<double>::highest();
  if ( Rich::BelowThreshold != id )
  {
    const auto index = m_refIndex->refractiveIndex(rad);
    thresP = ( index > 1.0 ? mass(id) / std::sqrt((index*index)-1.0) : 
               boost::numeric::bounds<double>::highest() );
  }
  return thresP;
}

double
Rich::ParticleProperties::thresholdMomentum( const Rich::ParticleIDType id,
                                             const LHCb::RichTrackSegment& trSeg ) const
{
  double thresP = boost::numeric::bounds<double>::highest();
  if ( Rich::BelowThreshold != id )
  {
    const auto index = m_refIndex->refractiveIndex( trSeg.radIntersections(),
                                                    trSeg.avPhotonEnergy() );
    thresP = ( index > 1.0 ? mass(id) / std::sqrt((index*index)-1.0) : 
               boost::numeric::bounds<double>::highest() );
  }
  return thresP;
}

const Rich::Particles & Rich::ParticleProperties::particleTypes() const
{
  return m_pidTypes;
}
