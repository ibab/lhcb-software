#include "GaudiKernel/ToolFactory.h"
#include "Event/Particle.h"
// kernel
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "VertexIsolation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexIsolation
// Converted from TupleToolVtxIsoln by Albert Puig
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( VertexIsolation )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  VertexIsolation::VertexIsolation( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent)
    : GaudiTool ( type, name , parent )
    , m_dva(0)
    , m_dist(0)
    , m_pVertexFit(0)
{
  declareInterface<IExtraInfoTool>(this);
  declareProperty("InputParticles", m_inputParticles,
                  "List of containers to check for extra particle vertexing") ;
  declareProperty("MaxDeltaChi2", m_deltaChi2 = 9.0,
                  "Maximum Delta chi2 for compatible particles") ;
  declareProperty("MaxChi2", m_chi2 = 9.0,
                  "Maximum chi2 for compatible particles") ;
}

//=============================================================================
// Destructor
//=============================================================================
VertexIsolation::~VertexIsolation() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode VertexIsolation::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  // Get DVAlgo
  m_dva = Gaudi::Utils::getIDVAlgorithm( contextSvc(), this ) ;
  if ( !m_dva )
  {
    Error("Couldn't get parent DVAlgorithm") ;
    return StatusCode::FAILURE ;
  }
    // Get distance calculator
  m_dist = m_dva->distanceCalculator();
  if ( !m_dist )
  {
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }
  // Get vertex fitter
  m_pVertexFit= m_dva->vertexFitter() ;
  if ( !m_pVertexFit ){
    Error("Unable to retrieve the IVertexFit tool");
    return StatusCode::FAILURE;
  }
  return sc;
}

//=============================================================================
// Fill Cone Info structure
//=============================================================================
StatusCode VertexIsolation::calculateExtraInfo( const LHCb::Particle *top,
                                                const LHCb::Particle *part )
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Fill" << endmsg;

  // Check the particle
  if ( top->isBasicParticle() || isPureNeutralCalo(top) )
  {
    Error("Cannot calculate isolation for basic or calorimetric particles!") ;
    return StatusCode::FAILURE ;
  }

  // Check the vertex of the particle
  const LHCb::Vertex* vtx = top->endVertex() ;
  if ( !vtx ) {
    Error("Can't retrieve the vertex!") ;
    return StatusCode::FAILURE ;
  }
  const double originalVtxChi2 = vtx->chi2() ;

  // Put all basic particles we are considering in m_particlesToVertex
  // -- Clear the vector with the particles in the specific decay
  m_particlesToVertex.clear();
  // -- Add the mother (prefix of the decay chain) to the vector
  if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  m_particlesToVertex.push_back( top );
  // -- Save all basic particles that belong to the mother vertex in the vector m_particlesToVertex
  findDaughters2Vertex( top );

  // -- Get vector of particles excluding the signal
  if ( m_inputParticles.empty() ) m_inputParticles.push_back("/Event/Phys/StdNoPIDsPions") ;
  LHCb::Particle::ConstVector partsToCheck ;
  for ( std::vector<std::string>::iterator iLocation = m_inputParticles.begin();
        iLocation != m_inputParticles.end();
        iLocation++ )
  {
    // Check there's actually particles
    if ( !exist<LHCb::Particle::Range>(*iLocation+"/Particles") )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "No particles at " << *iLocation << " !!!!!" << endreq;
      continue;
    }
    // Get the particles
    LHCb::Particle::Range particles = get<LHCb::Particle::Range>(*iLocation+"/Particles") ;
    if (msgLevel(MSG::DEBUG)) debug() << "Got " << particles.size() << " particles from " << *iLocation << endreq ;
    // Loop over the particles and take the ones we can use for vertexing
    for ( LHCb::Particle::Range::const_iterator iParticle = particles.begin();
          iParticle != particles.end();
          iParticle++  )
    {
      // Ignore if no proto
      if ( !(*iParticle)->proto() )        continue ;
      // Also ignore if neutral
      if ( isPureNeutralCalo(*iParticle) ) continue ;
      // Check that the proto of the particle does not match the signal
      bool isSignal = false ;
      for ( LHCb::Particle::ConstVector::const_iterator iSignal = m_particlesToVertex.begin();
            iSignal != m_particlesToVertex.end();
            iSignal++ )
      {
        if ( (*iSignal)->proto() == (*iParticle)->proto() ) { isSignal = true ; break ; }
      }
      if ( isSignal ) continue ;
      // Check for duplicated particles
      bool isAlreadyIn = false ;
      for ( LHCb::Particle::ConstVector::const_iterator iSavedParticle = partsToCheck.begin();
            iSavedParticle != partsToCheck.end();
            iSavedParticle++ )
      {
        if ( (*iSavedParticle)->proto() == (*iParticle)->proto() ) { isAlreadyIn = true ; break ; }
      }
      if ( isAlreadyIn ) continue ;
      partsToCheck.push_back( *iParticle ) ;
    }
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "Number of particles to check excluding signal, particles with same proto and gammas = " << partsToCheck.size() << endreq;
  // Now let's do some vertexing
  if( !part )
  {
    if ( msgLevel(MSG::WARNING) ) warning() << "Found an invalid particle" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling isolation variables" << endmsg;

  IsolationResult isolationOneTrack = getIsolation(originalVtxChi2, partsToCheck) ;
  IsolationResult isolationTwoTracks ;
  double smallestMassOneTrack  = 0.0 ;
  double smallestMassTwoTracks = 0.0 ;
  if ( isolationOneTrack.bestParticle )
  {
    Gaudi::LorentzVector cand = part->momentum() + isolationOneTrack.bestParticle->momentum();
    smallestMassOneTrack = cand.mass();
    // Second fit
    // Add the extra particle to the particles to vertex
    m_particlesToVertex.push_back((isolationOneTrack.bestParticle)) ;
    // Build list with particles to check removing bestParticle
    LHCb::Particle::ConstVector partsToCheckForTwoTracks ;
    for ( LHCb::Particle::ConstVector::const_iterator iSavedParticle = partsToCheck.begin();
          iSavedParticle != partsToCheck.end();
          iSavedParticle++ )
    {
      if ( (*iSavedParticle)->proto() != (isolationOneTrack.bestParticle)->proto() ) partsToCheckForTwoTracks.push_back(*iSavedParticle) ;
    }
    // Compute isolation adding one further track
    isolationTwoTracks = getIsolation(originalVtxChi2, partsToCheckForTwoTracks) ;
    // Cleanup the list of signal tracks
    m_particlesToVertex.pop_back() ;
    if ( isolationTwoTracks.bestParticle )
    {
      Gaudi::LorentzVector cand = part->momentum() + isolationOneTrack.bestParticle->momentum() + isolationTwoTracks.bestParticle->momentum();
      smallestMassTwoTracks = cand.mass();
    }
  }

  // Save values
  m_nPartDeltaChi2Win              = isolationOneTrack.nCompatibleDeltaChi2 ;
  m_nPartChi2Win                   = isolationOneTrack.nCompatibleChi2 ;
  m_smallestDeltaChi2OneTrack      = isolationOneTrack.smallestDeltaChi2;
  m_smallestChi2OneTrack           = isolationOneTrack.smallestChi2;
  m_smallestDeltaChi2MassOneTrack  = smallestMassOneTrack;
  m_smallestDeltaChi2TwoTracks     = isolationTwoTracks.smallestDeltaChi2;
  m_smallestChi2TwoTracks          = isolationTwoTracks.smallestChi2;
  m_smallestDeltaChi2MassTwoTracks = smallestMassTwoTracks;

  // We're done!
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Get (recursively) the particles to vertex in the decay chain
//=============================================================================
//bool  VertexIsolation::getIsolation( const double originalVtxChi2,
//                                     LHCb::Particle::ConstVector &extraParticles )
VertexIsolation::IsolationResult VertexIsolation::getIsolation( const double originalVtxChi2, LHCb::Particle::ConstVector &extraParticles )
{
  int             nCompatibleDeltaChi2 = 0 ;
  int             nCompatibleChi2      = 0 ;
  double          smallestChi2         = -1 ;
  double          smallestDeltaChi2    = -1 ;
  LHCb::Particle *bestParticle         = NULL ;
  StatusCode sc = true ;
  for ( LHCb::Particle::ConstVector::const_iterator iExtraPart = extraParticles.begin() ;
        iExtraPart != extraParticles.end() ;
        iExtraPart++ )
  {
    LHCb::Vertex vtxWithExtraTrack ;
    // Temporarily add the extra track to the partcles to vertex vector
    m_particlesToVertex.push_back(*iExtraPart) ;
    // Fit
    StatusCode  sc2 = m_pVertexFit->fit(vtxWithExtraTrack, m_particlesToVertex) ;

    // Remove the extra track
    m_particlesToVertex.pop_back() ;
    if ( !sc ) Warning("Failed to fit vertex").ignore() ;
    else
    {
      double deltaChi2 = vtxWithExtraTrack.chi2() - originalVtxChi2 ;
      if ( msgLevel(MSG::DEBUG) ) debug() << "Fitted vertex adding track has Delta chi2 = " << deltaChi2  << " chi2 = " << vtxWithExtraTrack.chi2() << endmsg ;
      // Get values
      if ( (m_deltaChi2 > 0.0) && (deltaChi2 < m_deltaChi2) )      nCompatibleDeltaChi2++ ;
      if ( (m_chi2 > 0.0) && (vtxWithExtraTrack.chi2() < m_chi2) ) nCompatibleChi2++ ;
      if ( (smallestChi2) < 0 || (smallestChi2 > vtxWithExtraTrack.chi2()) ) smallestChi2 = vtxWithExtraTrack.chi2() ;
      if ( (smallestDeltaChi2 < 0) || (smallestDeltaChi2 > deltaChi2) )
      {
        smallestDeltaChi2 = deltaChi2 ;
        bestParticle = (LHCb::Particle*) (*iExtraPart) ;
      }
    }

  }
  IsolationResult res ;
  res.nCompatibleChi2      = nCompatibleChi2 ;
  res.nCompatibleDeltaChi2 = nCompatibleDeltaChi2 ;
  res.smallestChi2         = smallestChi2 ;
  res.smallestDeltaChi2    = smallestDeltaChi2 ;
  res.bestParticle         = bestParticle ;
  return res ;
}

//=============================================================================
// Get (recursively) the particles to vertex in the decay chain
//=============================================================================
void VertexIsolation::findDaughters2Vertex( const LHCb::Particle *top )
{
  // -- Get the daughters of the top particle
  LHCb::Particle::ConstVector daughters = top->daughtersVector();
  // -- Fill all the daugthers in m_particlesToVertex
  for(  LHCb::Particle::ConstVector::const_iterator idau = daughters.begin() ; idau != daughters.end() ; ++idau)
  {
    // -- If the particle is stable, save it in the vector, or...
    if( (*idau)->isBasicParticle() )
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      if ( (*idau)->proto() && !isPureNeutralCalo(*idau) ) m_particlesToVertex.push_back( (*idau) );
    }
    else
    { // -- if it is not stable, call the function recursively
      m_particlesToVertex.push_back( (*idau) );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      findDaughters2Vertex( (*idau) );
    }
  }
}

int VertexIsolation::getFirstIndex(void) {
  return LHCb::Particle::NumPartWithinDeltaChi2Window ;
}

int VertexIsolation::getNumberOfParameters(void) {
  return 5 ; // This tool returns 5 parameters
  //return 8 ; // This tool returns 8 parameters
}

int VertexIsolation::getInfo(int index, double &value, std::string &name) {
  // Get ExtraInfo according to definitions in Particle.xml
  switch( index ) {
  case LHCb::Particle::NumVtxWithinChi2WindowOneTrack : value = (double) m_nPartChi2Win;          name = "NumVtxWithinChi2WindowOneTrack"; break;
  case LHCb::Particle::SmallestDeltaChi2OneTrack      : value = m_smallestDeltaChi2OneTrack;      name = "SmallestDeltaChi2OneTrack";      break;
  //case LHCb::Particle::SmallestChi2OneTrack           : value = m_smallestChi2OneTrack;           name = "SmallestChi2OneTrack";           break;
  case LHCb::Particle::SmallestDeltaChi2MassOneTrack  : value = m_smallestDeltaChi2MassOneTrack;  name = "SmallestDeltaChi2MassOneTrack";  break;
  case LHCb::Particle::SmallestDeltaChi2TwoTracks     : value = m_smallestDeltaChi2TwoTracks;     name = "SmallestDeltaChi2TwoTracks";     break;
  //case LHCb::Particle::SmallestChi2TwoTracks          : value = m_smallestChi2TwoTracks;          name = "SmallestChi2TwoTracks";          break;
  case LHCb::Particle::SmallestDeltaChi2MassTwoTracks : value = m_smallestDeltaChi2MassTwoTracks; name = "SmallestDeltaChi2MassTwoTracks"; break;
  default                                             : return 0 ;
  }
  // All is good
  return 1 ;

}
