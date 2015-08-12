// $Id$
// Include files

// local
#include "NeutralCCChangePIDTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NeutralCCChangePIDTool
//
// 2006-12-15 : Patrick Spradlin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( NeutralCCChangePIDTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  NeutralCCChangePIDTool::NeutralCCChangePIDTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
    : GaudiTool ( type, name , parent )
    , m_ppSvc(0)
{
  declareInterface<IChangePIDTool>(this);
}

//=============================================================================
// Destructor
//=============================================================================
NeutralCCChangePIDTool::~NeutralCCChangePIDTool() {}


//=============================================================================
// Tool initialization method
//=============================================================================
StatusCode NeutralCCChangePIDTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();      /// standard initizization
  if( !sc )
    return Warning( "Failed to initialize base class GaudiTool", sc );

  /// Get reference to the ParticlePropertySvc
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);

  return sc;
}


//=============================================================================
// Change the PID of a single particle.
// It returns by value a Particle that has the conjugate PID of the input
//   particle, but is otherwise equal to the input particle.
// It ONLY works for Particles with PIDs which are charge neutral and not
//   self conjugate.  If these conditions are not satisfied by the input
//   particle, the method raises an exception.
//=============================================================================
LHCb::Particle NeutralCCChangePIDTool::changePID( const LHCb::Particle &oldpart )
{

  const LHCb::ParticleProperty *oldpp = m_ppSvc->find( oldpart.particleID() );

  /// Test for validity of ParticleProperty and for charge neutrality
  if( isNeutralPP(oldpp) )
  {
    /// Retrieve anti-particle ParticleProperty
    const LHCb::ParticleProperty *newpp = oldpp->antiParticle();

    /// Test for validity of ParticleProperty and for non-self-conjugate
    if( newpp && newpp != oldpp )
    {
      LHCb::Particle newpart = oldpart;   /// Local copy
      newpart.setParticleID( LHCb::ParticleID( newpp->pythiaID()) ); /// Change PID

      /// Verbosity
      verbose() << "Returning " << newpart.particleID().pid()
                << " with momentum " << newpart.momentum() << " m="
                << newpart.measuredMass() << endreq;
      verbose() << " copied from " << oldpart.particleID().pid()
                << " with momentum " << oldpart.momentum() << " m="
                << oldpart.measuredMass() << endreq;

      return newpart;     /// Return by value
    }
  }

  /// Failure
  fatal() << " Input particle has an invalid ID.  Throwing an exception."  << endmsg;
  Exception( "invalid PID" );
  return LHCb::Particle() ; // just to avoid a compiler warning
}


//=============================================================================
// Change the PID of each member of a vector of particles
// It returns by value a vector of Particles that have the conjugate PID of
//   the input particles, but are otherwise equal to the input particles.
// The implementation calls the single particle changePID() method for each
//   input particle, hence an exception will be raised if any one of the
//   input Particles is not neutral or has a self-conjugate PID.
//=============================================================================
std::vector<LHCb::Particle>
NeutralCCChangePIDTool::changePID( const LHCb::Particle::ConstVector &inparts )
{
  std::vector<LHCb::Particle> outparts;

  LHCb::Particle::ConstVector::const_iterator i;
  for( i = inparts.begin(); i != inparts.end(); i++ )
    outparts.push_back( changePID(*(*i)) );

  verbose() << " Reassigned " << outparts.size()
            << " Particles from an input list of " << inparts.size() << endmsg;

  return outparts;
}


//=============================================================================
