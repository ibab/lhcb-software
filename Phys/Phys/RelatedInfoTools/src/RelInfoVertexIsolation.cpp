#include "Event/Particle.h"
// kernel
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/RelatedInfoNamed.h"

// local
#include "RelInfoVertexIsolation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoVertexIsolation
// Converted from TupleToolVtxIsoln by Albert Puig
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoVertexIsolation )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoVertexIsolation::RelInfoVertexIsolation( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent)
: GaudiTool ( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);
  declareProperty("InputParticles", m_inputParticles,
                  "List of containers to check for extra particle vertexing") ;
  declareProperty("MaxChi2", m_chi2 = 9.0,
                  "Maximum chi2 for compatible particles") ;
  declareProperty("Variables", m_variables,
                  "List of variables to store (store all if empty)");
}

//=============================================================================
// Destructor
//=============================================================================
RelInfoVertexIsolation::~RelInfoVertexIsolation() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode RelInfoVertexIsolation::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // If no input particle container is specified, put StdNoPIDsPions by default
  if ( m_inputParticles.empty() )
  {
    m_inputParticles.push_back("/Event/Phys/StdNoPIDsPions") ;
  }

  m_keys.clear();

  if ( m_variables.empty() )
  {

    if ( msgLevel(MSG::DEBUG) ) debug() << "List of variables empty, adding all" << endmsg;
    m_keys.push_back( RelatedInfoNamed::VTXISONUMVTX );
    m_keys.push_back( RelatedInfoNamed::VTXISODCHI2ONETRACK );
    m_keys.push_back( RelatedInfoNamed::VTXISODCHI2MASSONETRACK );
    m_keys.push_back( RelatedInfoNamed::VTXISODCHI2TWOTRACK );
    m_keys.push_back( RelatedInfoNamed::VTXISODCHI2MASSTWOTRACK );

  } else {

    for ( const auto& var : m_variables )
    {
      short int key = RelatedInfoNamed::indexByName( var );
      if (key != RelatedInfoNamed::UNKNOWN) {
        m_keys.push_back( key );
        debug() << "Adding variable " << var << ", key = " << key << endmsg;
      } else {
        warning() << "Unknown variable " << var << ", skipping" << endmsg;
      }
    }

  }

  return sc;
}

//=============================================================================
// Fill Cone Info structure
//=============================================================================
StatusCode RelInfoVertexIsolation::calculateRelatedInfo( const LHCb::Particle *top,
                                                         const LHCb::Particle *part )
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Fill" << endmsg;

  // Let's not allow the tool to be run on basic particles or photons
  if ( part->isBasicParticle() || isPureNeutralCalo(part) )
  {
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Trying to fill isolation for basic particle. Exiting..." << endmsg;
    m_map.clear();
    return StatusCode::SUCCESS;
  }

  // Get the vertex
  const LHCb::Vertex* vtx = part->endVertex() ;
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "vertex for P, ID " << part->particleID().pid()
            << " = " << vtx << " at " << vtx->position() << endmsg;
  }

  if ( !vtx )
    return Error("Can't retrieve the vertex!") ;

  const double originalVtxChi2 = vtx->chi2() ;

  // -- Clear the vector with the particles in the specific decay
  m_signalFinalState.clear();
  m_particlesToVertex.clear();
  // -- Add the mother (prefix of the decay chain) to the vector
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  // -- Save all basic particles that belong to the mother vertex in the vector m_signalFinalState
  findSignalFinalState( part );

  // -- Get vector of particles excluding the signal
  LHCb::Particle::ConstVector partsToCheck ;
  for ( const auto& location : m_inputParticles )
  {
    // Get the particles
    LHCb::Particle::Range particles = getIfExists<LHCb::Particle::Range>(location+"/Particles") ;
    if (msgLevel(MSG::DEBUG))
      debug() << "Got " << particles.size() << " particles from " << location << endmsg ;
    if ( particles.empty() ) continue ;

    // Loop over the particles and take the ones we can use for vertexing
    for ( const auto* particle : particles )
    {
      // Ignore if no proto
      if ( !particle->proto() )        continue ;
      // Also ignore if neutral
      if ( isPureNeutralCalo(particle) ) continue ;
      // Check that the proto of the particle does not match the signal
      bool isSignal = false ;
      for ( auto iSignal = m_signalFinalState.begin(), iSignalEnd = m_signalFinalState.end();
            iSignal != iSignalEnd;
            ++iSignal )
      {
        const LHCb::ProtoParticle* pSignal = (*iSignal)->proto() ;
        if ( pSignal && (pSignal == particle->proto() ) )
        {
          isSignal = true ;
          break ;
        }
      }
      if ( isSignal ) continue ;

      // Check for duplicated particles
      bool isAlreadyIn = false ;
      for ( const auto* savedParticle : partsToCheck )
      {
        const LHCb::ProtoParticle* pSaved = savedParticle->proto() ;
        if ( pSaved && ( pSaved == particle->proto() ) )
        {
          isAlreadyIn = true ;
          break ;
        }
      }
      if ( !isAlreadyIn ) partsToCheck.push_back( particle ) ;
    }
  }
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Number of particles to check excluding signal, particles with same proto and gammas = "
            << partsToCheck.size() << endmsg;
  // Now let's do some vertexing
  if( !part )
  {
    return Warning( "Found an invalid particle" );
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling isolation variables" << endmsg;

  // Get particles to vertex
  m_particlesToVertex = part->daughtersVector() ;

  if (msgLevel(MSG::DEBUG)) debug() << "Particles to vertex = " <<  m_particlesToVertex.size()  << endmsg;

  IsolationResult isolationOneTrack = getIsolation(originalVtxChi2, partsToCheck) ;
  IsolationResult isolationTwoTracks ;
  double smallestMassOneTrack  = 0.0 ;
  double smallestMassTwoTracks = 0.0 ;
  if ( isolationOneTrack.bestParticle )
  {
    const Gaudi::LorentzVector cand = part->momentum() + isolationOneTrack.bestParticle->momentum();
    smallestMassOneTrack = cand.mass();
    // Second fit
    // Add the extra particle to the particles to vertex
    m_particlesToVertex.push_back( isolationOneTrack.bestParticle ) ;
    // Build list with particles to check removing bestParticle
    LHCb::Particle::ConstVector partsToCheckForTwoTracks ;
    for ( const auto* savedParticle : partsToCheck )
    {
      if ( savedParticle->proto() != (isolationOneTrack.bestParticle)->proto() )
        partsToCheckForTwoTracks.push_back(savedParticle) ;
    }
    // Compute isolation adding one further track
    isolationTwoTracks = getIsolation(originalVtxChi2, partsToCheckForTwoTracks) ;
    // Cleanup the list of signal tracks
    //   m_particlesToVertex.pop_back() ;
    if ( isolationTwoTracks.bestParticle )
    {
      const Gaudi::LorentzVector cand = ( part->momentum() + isolationOneTrack.bestParticle->momentum() +
                                          isolationTwoTracks.bestParticle->momentum() );
      smallestMassTwoTracks = cand.mass();
    }
  }

  // Save values
  m_nPartChi2Win                   = isolationOneTrack.nCompatibleChi2 ;
  m_smallestDeltaChi2OneTrack      = isolationOneTrack.smallestDeltaChi2;
  m_smallestChi2OneTrack           = isolationOneTrack.smallestChi2;
  m_smallestDeltaChi2MassOneTrack  = smallestMassOneTrack;
  m_smallestDeltaChi2TwoTracks     = isolationTwoTracks.smallestDeltaChi2;
  m_smallestChi2TwoTracks          = isolationTwoTracks.smallestChi2;
  m_smallestDeltaChi2MassTwoTracks = smallestMassTwoTracks;

  m_map.clear();

  for ( const auto key : m_keys )
  {

    float value = 0;
    switch (key) {
    case RelatedInfoNamed::VTXISONUMVTX            : value = (float) m_nPartChi2Win; break;
    case RelatedInfoNamed::VTXISODCHI2ONETRACK     : value = m_smallestDeltaChi2OneTrack; break;
    case RelatedInfoNamed::VTXISODCHI2MASSONETRACK : value = m_smallestDeltaChi2MassOneTrack; break;
    case RelatedInfoNamed::VTXISODCHI2TWOTRACK     : value = m_smallestDeltaChi2TwoTracks; break;
    case RelatedInfoNamed::VTXISODCHI2MASSTWOTRACK : value = m_smallestDeltaChi2MassTwoTracks; break;
    default: value = 0.; break;
    }
    debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endmsg;
    m_map.insert( std::make_pair( key, value) );
  }

  // We're done!
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Get (recursively) the particles to vertex in the decay chain
//=============================================================================
//bool  RelInfoVertexIsolation::getIsolation( const double originalVtxChi2,
//                                     LHCb::Particle::ConstVector &extraParticles )
RelInfoVertexIsolation::IsolationResult
RelInfoVertexIsolation::getIsolation( const double originalVtxChi2,
                                      LHCb::Particle::ConstVector &extraParticles )
{
  int             nCompatibleChi2      = 0 ;
  double          smallestChi2         = -1 ;
  double          smallestDeltaChi2    = -1 ;
  LHCb::Particle *bestParticle         = NULL ;

  IsolationResult res ;

  // Get DVAlgo
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { Error("Could not get parent DVAlgorithm").ignore(); }
  else
  {

    for ( const auto* extraPart : extraParticles )
    {
      LHCb::Vertex vtxWithExtraTrack ;
      // Temporarily add the extra track to the partcles to vertex vector
      m_particlesToVertex.push_back(extraPart) ;
      // Fit
      const StatusCode sc = dva->vertexFitter()->fit(vtxWithExtraTrack,m_particlesToVertex) ;
      // Remove the extra track
      m_particlesToVertex.pop_back() ;
      if ( !sc )
      {
        if ( msgLevel(MSG::DEBUG) ) debug() << "Failed to fit vertex" << endmsg ;
      }
      else
      {
        // Compute the relevant variables
        const double newChi2 = vtxWithExtraTrack.chi2() ;
        const double deltaChi2 = newChi2 - originalVtxChi2 ;

        // Here we found a reasonably compatible particle
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Fitted vertex adding track has Delta chi2 = " << deltaChi2
                    << " chi2 = " << newChi2 << endmsg ;
        // A chi2 of -1 means that the fit was not good,
        // so the particle was not compatible.
        if ( newChi2 <= 0 ) continue;

        // Get values of deltas, n particles, etc.
        if ( (m_chi2 > 0.0) && (newChi2 < m_chi2) ) nCompatibleChi2++ ;
        if ( (smallestChi2) < 0 || (smallestChi2 > newChi2) ) smallestChi2 = newChi2 ;
        if ( (smallestDeltaChi2 < 0) || (smallestDeltaChi2 > deltaChi2) )
        {
          smallestDeltaChi2 = deltaChi2 ;
          bestParticle = (LHCb::Particle*) (extraPart) ;
        }
      }
    }

    res.nCompatibleChi2      = nCompatibleChi2 ;
    res.smallestChi2         = smallestChi2 ;
    res.smallestDeltaChi2    = smallestDeltaChi2 ;
    res.bestParticle         = bestParticle ;

  }

  return res ;
}

//=============================================================================
// Get (recursively) the particles to vertex in the decay chain
//=============================================================================
void RelInfoVertexIsolation::findSignalFinalState( const LHCb::Particle *particle )
{
  // -- Fill all the daugthers in m_signalFinalState
  for ( const auto* dau : particle->daughtersVector() )
  {
    // Recurse (will not do anything for basic particles)
    findSignalFinalState( dau );
    // -- If the particle has proto and is not a photon, add to the list of final states
    if ( dau->proto() && !isPureNeutralCalo(dau) ) m_signalFinalState.push_back( dau );
  }
}

LHCb::RelatedInfoMap* RelInfoVertexIsolation::getInfo(void)
{
  return &m_map;
}

