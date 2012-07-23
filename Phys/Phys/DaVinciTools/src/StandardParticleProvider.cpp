
// local
#include "StandardParticleProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StandardParticleProvider
//
// 2012-02-26 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( StandardParticleProvider )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  StandardParticleProvider::StandardParticleProvider( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
    : GaudiTool ( type, name , parent )
{
  declareInterface<IStandardParticleProvider>(this);
  m_pidToTESMap = boost::assign::map_list_of
    ( 11,   "Phys/StdAllNoPIDsElectrons/Particles" )
    ( 13,   "Phys/StdAllNoPIDsMuons/Particles"     )
    ( 211,  "Phys/StdAllNoPIDsPions/Particles"     )
    ( 321,  "Phys/StdAllNoPIDsKaons/Particles"     )
    ( 2212, "Phys/StdAllNoPIDsProtons/Particles"   );
  declareProperty( "ParticleLocations", m_pidToTESMap );
}

//=============================================================================
// Destructor
//=============================================================================
StandardParticleProvider::~StandardParticleProvider() {}

//=============================================================================

StatusCode StandardParticleProvider::initialize()
{
  // Initialise base class
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return sc;
}

//=============================================================================

// Method that handles various Gaudi "software events"
void StandardParticleProvider::handle ( const Incident& )
{
  // only one incident type subscribed to, so no need to check type
  m_pidToParts.clear();
}

//=============================================================================

const LHCb::Particle * 
StandardParticleProvider::particle( const LHCb::ProtoParticle * proto,
                                    const LHCb::ParticleID& pid ) const
{
  const LHCb::Particle * part = NULL;
  if ( !proto ) return part;

  /// Find or build the map for the given PID type
  const ProtoToPartMap * pptop = NULL;
  PIDToPartsMap::const_iterator iP = m_pidToParts.find( pid.abspid() );
  if ( UNLIKELY( iP == m_pidToParts.end() ) )
  {
    if ( fillParticleMap(pid.abspid()) )
    {
      pptop = &(m_pidToParts[pid.abspid()]);
    } 
  }
  else
  {
    pptop = &(iP->second);
  }

  // Find the Particle in the Map
  if ( pptop )
  {
    ProtoToPartMap::const_iterator iPPToP = pptop->find(proto);
    if ( iPPToP != pptop->end() )
    {
      part = iPPToP->second;
    }
  }

  return part;
}

//=============================================================================

bool StandardParticleProvider::fillParticleMap( const unsigned int pid ) const
{
  bool OK = false;

  // Find the Particles for this PID type
  PIDToTESMap::const_iterator iPartLoc = m_pidToTESMap.find(pid);
  const LHCb::Particles * parts = 
    ( iPartLoc != m_pidToTESMap.end() ?
      getIfExists<LHCb::Particles>(evtSvc(),iPartLoc->second) :
      NULL );

  // If found, fill the map
  if ( parts )
  {
    ProtoToPartMap & pptop = m_pidToParts[pid];
    for ( LHCb::Particles::const_iterator iP = parts->begin();
          iP != parts->end(); ++iP )
    {
      if ( *iP && (*iP)->proto() )
      {
        pptop[(*iP)->proto()] = *iP;
      }
    }
    OK = true;
  }

  return OK;
}

//=============================================================================
