
//-----------------------------------------------------------------------------
/** @file ParticleEffPurMoni.cpp
 *
 *  Implementation file for class : ParticleEffPurMoni
 *
 *  CVS Log :-
 *  $Id: ParticleEffPurMoni.cpp,v 1.6 2007-02-25 12:13:05 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2007-002-21
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ParticleEffPurMoni.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleEffPurMoni );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleEffPurMoni::ParticleEffPurMoni( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : DVAlgorithm   ( name , pSvcLocator ),
    m_recurCount  ( 0    ),
    m_trSel       ( NULL ),
    m_particleLinker     ( NULL ),
    m_chargedProtoLinker ( NULL ),
    m_neutralProtoLinker ( NULL ),
    m_nEvts       ( 0    )
{
  declareProperty( "MinContributionInSummary",     m_minContrib       = 0.05 );
  declareProperty( "MinChargedProtoPMCAssWeight",  m_minAssWeightCh   = 0.9  );
  declareProperty( "MinNeutralProtoPMCAssWeight",  m_minAssWeightNu   = 0.9  );
  declareProperty( "MinCompositePartMCAssWeight",  m_minAssWeightComp = 0.9  );
}

//=============================================================================
// Destructor
//=============================================================================
ParticleEffPurMoni::~ParticleEffPurMoni() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticleEffPurMoni::initialize()
{
  const StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleEffPurMoni::execute()
{
  // get the desktop particles
  const LHCb::Particle::ConstVector & particles = desktop()->particles();

  // clear the working map
  m_partProtoMap.clear();
  // count events
  ++m_nEvts;

  // Loop over particles to fill the map
  for ( LHCb::Particle::ConstVector::const_iterator iP = particles.begin();
        iP != particles.end(); ++iP )
  {
    m_recurCount = 0;
    addParticle( *iP, *iP );
  }

  // Proto TES locations found
  std::map<std::string,bool> protoLocations;

  // Loop over the final list of Protos and associated Particles
  debug() << "Found " << m_partProtoMap.size() << " used ProtoParticles from input Particles" << endreq;
  for ( ParticleProtoMap::const_iterator iPM = m_partProtoMap.begin();
        iPM != m_partProtoMap.end(); ++iPM )
  {
    debug() << "  -> ProtoParticle " << (*iPM).first->key() << endreq;

    // select protoparticles
    if ( !selectProto((*iPM).first) ) continue;
    debug() << "     -> selected " << endreq;

    // store this proto TES location
    const std::string protoTesLoc = objectLocation((*iPM).first->parent());
    protoLocations[ protoTesLoc ] = true;

    // Get the MCParticle
    const LHCb::MCParticle * mcPart = mcParticle((*iPM).first);

    // Get the particles properties for this MCParticle
    const ParticleProperty * mcProp = ( mcPart ? partProp(mcPart->particleID()) : NULL );

    // Loop over Particles for the Proto
    for ( ParticleHistory::Vector::const_iterator iPart = (*iPM).second.begin();
          iPart != (*iPM).second.end(); ++iPart )
    {
      // Find its TES location
      const std::string tesLoc = objectLocation( (*iPart).firstParticle->parent() );
      debug() << " -> Found Particle in " << tesLoc << endreq
              << " -> " << *iPart << endreq;

      // get the data for this TES location
      MCSummaryMap & mcMap = m_locMap[tesLoc];

      // get the summary for this reco particle type
      const FullPartName partName( (*iPart).properties->particle(),
                                   (*iPart).history,
                                   protoParticleType((*iPM).first),
                                   protoTesLoc );
      MCSummary & mcSum = mcMap[ partName ];

      // count the number reconstructed for this type
      ++mcSum.nReco;

      // count the true types for this reco type
      ++mcSum.trueMCType[ mcProp ? mcProp->particle() : "NoMC" ];

    } // loop over particles produced from this proto

  } // loop over all used protos

  // Count the total number of protos at each location used from TES
  for ( std::map<std::string,bool>::const_iterator iProtoLoc = protoLocations.begin();
        iProtoLoc != protoLocations.end(); ++iProtoLoc )
  {
    // get the protos at this location
    const std::string & tesLoc = (*iProtoLoc).first;
    const LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( tesLoc );
    debug() << "Looping over " << protos->size() << " ProtoParticles at " << tesLoc << endreq;
    for ( LHCb::ProtoParticles::const_iterator proto = protos->begin();
          proto != protos->end(); ++proto )
    {
      // is this proto selected ?
      if ( !selectProto(*proto) ) continue;

      // count the total number of selected protos at this TES location
      ++(m_protoTesStats[tesLoc].nTotal);

      // Get the MCParticle
      const LHCb::MCParticle * mcPart = mcParticle(*proto);

      // Get the particles properties for this MCParticle
      const ParticleProperty * mcProp = ( mcPart ? partProp(mcPart->particleID()) : NULL );

      // count the total number of selected protos at this TES location with MC
      if ( NULL != mcProp ) ++(m_protoTesStats[tesLoc].nWithMC);

      // count
      ++(m_mcProtoCount[tesLoc])[protoParticleType(*proto)].trueMCType[ mcProp ? mcProp->particle() : "NoMC" ];

    } // loop over all protos at one location in TES

  } // loop over TES locations

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

bool ParticleEffPurMoni::selectProto( const LHCb::ProtoParticle * proto ) const
{
  bool OK(true);
  if ( 0 != proto->charge() )
  {
    // charged Proto selection
    const LHCb::Track * track = proto->track();
    if ( !track || !m_trSel->accept(*track) ) OK = false;
  }
  // at the moment, no neutral selection
  return OK;
}

const LHCb::MCParticle *
ParticleEffPurMoni::mcParticle( const LHCb::ProtoParticle * proto ) const
{
  // get the appropriate linker
  ProtoParticle2MCLinker * links = protoLinker(proto);
  // Nastiness to deal with linker interface :(
  double bestWeight(-999999);
  const LHCb::MCParticle *mcP(NULL), *tmpMCP(links->firstMCP(proto));
  while ( NULL != tmpMCP )
  {
    if ( links->weight() > bestWeight &&
         links->weight() > minMCWeight(proto) )
    {
      bestWeight = links->weight();
      mcP = tmpMCP;
    }
    tmpMCP = links->next();
  }
  return mcP;
}

const LHCb::MCParticle *
ParticleEffPurMoni::mcParticle( const LHCb::Particle * part ) const
{
  if ( part->isBasicParticle() )
  {
    return mcParticle ( part->proto() );
  }
  else
  {
    // get the appropriate linker
    Particle2MCLinker * links = particleLinker(part);
    // Nastiness to deal with linker interface :(
    double bestWeight(-999999);
    const LHCb::MCParticle *mcP(NULL), *tmpMCP(links->firstMCP(part));
    while ( NULL != tmpMCP )
    {
      if ( links->weight() > bestWeight &&
           links->weight() > minMCWeight(part) )
      {
        bestWeight = links->weight();
        mcP = tmpMCP;
      }
      tmpMCP = links->next();
    }
    return mcP;
  }
}

const ParticleProperty *
ParticleEffPurMoni::partProp( const LHCb::ParticleID id ) const
{
  //const ParticleProperty * prop = ppSvc()->findByPythiaID( id.pid() );
  const ParticleProperty * prop = ppSvc()->findByStdHepID( id.pid() );
  if ( NULL == prop )
  {
    std::ostringstream mess;
    mess << "ParticleProperty missing for ParticleID " << id;
    Warning( mess.str(), StatusCode::SUCCESS, 1 );
  }
  return prop;
}

void
ParticleEffPurMoni::addParticle( const LHCb::Particle * particle,
                                 const LHCb::Particle * firstParticle,
                                 const std::string & history,
                                 const bool toplevel ) const
{
  // protect against too many recursive calls... (should never ever get this deep)
  ++m_recurCount;
  if ( m_recurCount > 9999 )
  {
    Warning( "Recursive limit in addParticle reached. Aborting Particle tree scan" );
    return;
  }

  // Does this particle have a ProtoParticle. I.e. its stable
  const LHCb::ProtoParticle * proto = particle->proto();

  // get the history for the Particle
  const ParticleProperty * prop = partProp( particle->particleID() );
  // If not available, just abort
  if ( !prop ) { return; }

  // add to map
  if ( proto )
  {
    // down to a basic particle, so save with history
    m_partProtoMap[proto].push_back( ParticleHistory( particle, firstParticle,
                                                      history + prop->particle(),
                                                      prop, toplevel ) );
  }
  // if no proto, try and save descendants
  else if ( !particle->isBasicParticle() )
  {
    // loop over them and to add them instead
    const LHCb::Particle::ConstVector daughters = particle->daughtersVector();
    for ( LHCb::Particle::ConstVector::const_iterator iP = daughters.begin();
          iP != daughters.end(); ++iP )
    {
      addParticle( *iP, firstParticle, history + prop->particle() + " -> ", false );
    }
  }
}

std::string ParticleEffPurMoni::protoParticleType( const LHCb::ProtoParticle * proto ) const
{
  if ( 0 != proto->charge() )
  {
    const LHCb::Track * track = proto->track();
    if ( !track ) Exception( "NULL Track pointer for charged ProtoParticle" );
    switch ( track->type() )
    {
    case LHCb::Track::Long        : return "Charged : Long Track";
    case LHCb::Track::Upstream    : return "Charged : Upstream Track";
    case LHCb::Track::Downstream  : return "Charged : Downstream Track";
    case LHCb::Track::Ttrack      : return "Charged : Ttrack Track";
    case LHCb::Track::Velo        : return "Charged : Velo Track";
    case LHCb::Track::VeloR       : return "Charged : VeloR Track";
    default                       : return "Charged : UNKNOWN TYPE";
    }
  }
  else
  {
    return "Neutral Particle";
  }
}

void ParticleEffPurMoni::printStats() const
{

  const std::string & LINES =
    "================================================================================================";
  const std::string & lines =
    "------------------------------------------------------------------------------------------------";

  const Rich::PoissonEffFunctor eff("%6.2f +-%5.2f");

  always() << LINES << endreq;

  // loop over ProtoParticles used
  for ( ProtoTESStatsMap::iterator iProtoTES = m_protoTesStats.begin();
        iProtoTES != m_protoTesStats.end(); ++iProtoTES )
  {
    debug() << lines << endreq
            << " ProtoParticle Location : " << (*iProtoTES).first << endreq
            << "  -> Fraction with MC "
            << eff( (*iProtoTES).second.nWithMC,(*iProtoTES).second.nTotal )
            << endreq;
  }
  debug() << LINES << endreq;

  // loop over Particle TES locations
  for ( LocationMap::iterator iLoc = m_locMap.begin();
        iLoc != m_locMap.end(); ++iLoc )
  {
    always()  << " Particle Location : " << (*iLoc).first << endreq
              << lines << endreq;
    // loop over reco particle types for this location
    std::string lastType;
    for ( MCSummaryMap::iterator iSum = (*iLoc).second.begin();
          iSum != (*iLoc).second.end(); ++iSum )
    {
      const long nRecoTrue  = (*iSum).second.trueMCType[(*iSum).first.particleName];
      if ( lastType != (*iSum).first.protoType )
      {
        always() << "  " << (*iSum).first.protoType << endreq;
        lastType = (*iSum).first.protoType;
      }
      always() << "    Reco. Tree : " << (*iSum).first.decayTree
               << " : " << nRecoTrue << " Particles" << endreq;
      const bool primaryPart = ( (*iSum).first.decayTree == (*iSum).first.particleName );
      if ( (*iSum).second.nReco > 0 )
      {
        int suppressedContribs(0);
        // loop over the MC Types for this reco type
        for ( TypeTally::const_iterator iT = (*iSum).second.trueMCType.begin();
              iT != (*iSum).second.trueMCType.end(); ++iT )
        {
          std::string mcT = (*iT).first;
          mcT.resize(15,' ');
          if ( (100*(*iT).second)/(*iSum).second.nReco > m_minContrib )
          {
            const long nBkgTrue =
              (m_mcProtoCount[(*iSum).first.protoTESLoc])[(*iSum).first.protoType].trueMCType[(*iT).first];
            always() << "     -> True MC Type : " << mcT
                     << " " << eff( (*iT).second, (*iSum).second.nReco )
                     << " % of sample";
            if ( primaryPart ) always() << " : ID eff. = " << eff( (*iT).second, nBkgTrue ) << " %";
            always() << endreq;
          } else { ++suppressedContribs; }
        }
        if ( suppressedContribs>0 )
        {
          always() << "     -> Suppressed " << suppressedContribs << " contribution(s) below "
                   <<  m_minContrib << " %" << endreq;
        }
      }

    }

    always() << lines << endreq;
  }

  always() << LINES << endreq;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ParticleEffPurMoni::finalize()
{
  // Print out summary info
  if ( m_nEvts > 0 ) printStats();
  // cleanup
  if ( m_particleLinker     ) { delete m_particleLinker;     m_particleLinker     = NULL; }
  if ( m_chargedProtoLinker ) { delete m_chargedProtoLinker; m_chargedProtoLinker = NULL; }
  if ( m_neutralProtoLinker ) { delete m_neutralProtoLinker; m_neutralProtoLinker = NULL; }
  // finalize base class
  return DVAlgorithm::finalize();
}

//=============================================================================
