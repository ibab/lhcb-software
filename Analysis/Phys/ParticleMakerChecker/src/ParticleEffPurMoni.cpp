//-----------------------------------------------------------------------------
/** @file ParticleEffPurMoni.cpp
 *
 *  Implementation file for class : ParticleEffPurMoni
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
DECLARE_ALGORITHM_FACTORY( ParticleEffPurMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleEffPurMoni::ParticleEffPurMoni( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : DaVinciHistoAlgorithm ( name , pSvcLocator ),
    m_truthTool      ( NULL ),
    m_trSel          ( NULL ),
    m_mcRec          ( NULL ),
    m_mcSel          ( NULL ),
    m_particleLinker ( NULL ),
    m_protoCount     ( 0    ),
    m_partCount      ( 0    ),
    m_nEvts          ( 0    ),
    m_maxNameLength  ( 50   )
{
  declareProperty( "MinContributionInSummary",     m_minContrib       = 0.5   );
  declareProperty( "MinChargedProtoPMCAssWeight",  m_minAssWeightCh   = 0.0   );
  declareProperty( "MinNeutralProtoPMCAssWeight",  m_minAssWeightNu   = 0.0   );
  declareProperty( "MinCompositePartMCAssWeight",  m_minAssWeightComp = 0.0   );
  declareProperty( "UseReversePToMCPCharged",      m_useRMCPcharged   = false );
  declareProperty( "UseReversePToMCPNeutral",      m_useRMCPneutral   = true  );
  declareProperty( "UseFullMCDecayTreeClass",      m_fullMCTree       = true  );
  declareProperty( "MaxMCDecayTreeSize",           m_maxMCTreeSize    = 5     );
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
  const StatusCode sc = DaVinciHistoAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );
  // get an instance of the MC Reconstructible tool
  m_mcRec = tool<IMCReconstructible>( "MCReconstructible", "MCRecibleForParticleMoni" );
  // MCParticle selector
  m_mcSel = tool<IMCParticleSelector>( "MCParticleSelector" );

  m_ShortNames["/Event/Rec/ProtoP/Charged"] = "OfflineProtos";
  m_ShortNames["/Event/Hlt/ProtoP/Charged"] = "HltProtos";

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleEffPurMoni::execute()
{
  // Never abort a sequence
  setFilterPassed(true);

  // get the local particles
  const LHCb::Particle::Range particles = this->particles();
  if ( particles.empty() ) return StatusCode::SUCCESS;

  // count events
  ++m_nEvts;

  // clear the working maps
  m_partProtoMap.clear();
  m_np2mcp.clear();
  m_cp2mcp.clear();

  // Loop over particles to fill the map
  for ( LHCb::Particle::Range::const_iterator iP = particles.begin();
        iP != particles.end(); ++iP )
  {
    if ( msgLevel(MSG::DEBUG) )
    {
      const std::string loc = objectLocation(*iP);
      debug() << "Found input Particle at : " << loc << endmsg;
    }
    addParticle( *iP, *iP );
  }

  typedef std::set<std::string> Locations;
  // Proto TES locations found
  Locations protoLocations;
  // MCParticle locations found
  Locations mcPLocations;

  // Protos associated to each MCParticle
  typedef std::set<const LHCb::Particle *> PartSet;
  typedef std::map<const LHCb::Particle *, PartSet>  PartMap;
  typedef std::map<const LHCb::MCParticle*, PartMap> MCP2PartMap;
  MCP2PartMap mcp2Parts;

  // already used MCParticles for each each ProtoParticle location
  std::map< const std::string, std::set<const LHCb::MCParticle*> > usedMCPs;

  // Loop over the final list of Protos and associated Particles
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << m_partProtoMap.size()
            << " used ProtoParticles from input Particles" << endmsg;
  for ( ParticleProtoMap::const_iterator iPM = m_partProtoMap.begin();
        iPM != m_partProtoMap.end(); ++iPM )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "  -> ProtoParticle " << (*iPM).first->key() << endmsg;

    // select protoparticles
    if ( !selectProto((*iPM).first) ) continue;
    if ( msgLevel(MSG::DEBUG) ) debug() << "     -> selected " << endmsg;

    // store this proto TES location
    const std::string& protoTesLoc = objectLocation((*iPM).first);
    protoLocations.insert( protoTesLoc );

    // Get the MCParticle
    const LHCb::MCParticle * mcPart = mcParticle((*iPM).first);

    // Store the TES location for this MCParticle container
    if (mcPart) mcPLocations.insert( objectLocation(mcPart) );

    // Get the MCParticle reco type
    const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(mcPart);

    // ProtoParticle momentum
    const double ptot = momentum((*iPM).first);
    const double pt   = transverseMomentum((*iPM).first);

    // protoparticle type (long etc.)
    const std::string& protoType = protoParticleType((*iPM).first);

    // Loop over Particles for the Proto
    for ( ParticleHistory::Map::const_iterator iPart = (*iPM).second.begin();
          iPart != (*iPM).second.end(); ++iPart )
    {
      // Find its TES location
      const std::string& tesLoc = objectLocation( (*iPart).first.firstParticle );
      if ( msgLevel(MSG::DEBUG) )
        debug() << " -> Found Particle in " << tesLoc << endmsg
                << " -> " << (*iPart).first << endmsg;

      // is this a clone ?
      const std::string cloneKey(tesLoc+protoTesLoc);
      const bool isClone =
        ( usedMCPs[cloneKey].find(mcPart) != usedMCPs[cloneKey].end() );

      // add to list of seen MCPs
      usedMCPs[cloneKey].insert(mcPart);

      // count protos per particle TES
      ++((m_partProtoTESStats[tesLoc])[protoTesLoc]).nTotal;
      if ( mcPart ) ++((m_partProtoTESStats[tesLoc])[protoTesLoc]).nWithMC;

      // Proto Correlations
      if ( !isClone )
      {
        ((mcp2Parts[mcPart])[(*iPart).first.particle]).insert((*iPart).first.firstParticle);
      }

      // get the data for this TES location
      MCSummaryMap & mcMap = m_locMap[tesLoc];

      // get the summary for this reco particle type
      MCSummary & mcSum = mcMap[ FullPartName( (*iPart).first.properties->particle(),
                                               (*iPart).first.history,
                                               protoType,
                                               protoTesLoc ) ];

      // count the number reconstructed for this type
      ++(mcSum.nReco);

      // count the true types for this reco type
      MCTally & tally = (mcSum.trueMCType[mcRecType])[ mcParticleName(mcPart) ];
      if ( !isClone )
      {
        ++(tally.notclones);
        tally.histos.fillAll( mcPart ? mcPart->p()  : ptot,
                              mcPart ? mcPart->pt() : pt   );
      } else { ++(tally.clones); }
      // if configured to do so, include full tree info
      if ( m_fullMCTree && mcPart )
      {
        const std::string& tmpName = mcParticleNameTree(mcPart);
        if ( !isClone )
        {
          ++(tally.notclones_detailed[tmpName]);
          tally.histos_detailed[tmpName].fillAll( mcPart ? mcPart->p()  : ptot,
                                                  mcPart ? mcPart->pt() : pt   );
        } else { ++(tally.clones_detailed[tmpName]); }
      }

    } // loop over particles produced from this proto

  } // loop over all used protos

  // Count the total number of protos at each location used from TES
  for ( Locations::const_iterator iProtoLoc = protoLocations.begin();
        iProtoLoc != protoLocations.end(); ++iProtoLoc )
  {
    // already used MCParticles
    std::set<const LHCb::MCParticle *> usedMCPs2;
    // fill short location name
    shortProtoLoc(*iProtoLoc);
    // get the protos at this location
    const std::string & tesLoc = (*iProtoLoc);
    const LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( tesLoc, false );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Looping over " << protos->size() << " ProtoParticles at " << tesLoc << endmsg;
    for ( LHCb::ProtoParticles::const_iterator proto = protos->begin();
          proto != protos->end(); ++proto )
    {
      // is this proto selected ?
      if ( !selectProto(*proto) ) continue;

      // Get the MCParticle
      const LHCb::MCParticle * mcPart = mcParticle(*proto);

      // is this a clone ?
      const bool isClone = ( usedMCPs2.find(mcPart) != usedMCPs2.end() );

      // add to list of seen MCPs
      if ( mcPart ) usedMCPs2.insert(mcPart);

      // Get the MCParticle reco type
      const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(mcPart);

      // protoparticle type (long etc.)
      const std::string& protoType = protoParticleType(*proto);

      // count
      MCTally & tally =
        ((m_mcProtoCount[tesLoc])[protoType].trueMCType[mcRecType])[mcParticleName(mcPart)];
      // Momentum histogramming
      if ( !isClone )
      {
        ++(tally.notclones);
        tally.histos.fillAll ( mcPart ? mcPart->p()  : momentum(*proto),
                               mcPart ? mcPart->pt() : transverseMomentum(*proto) );
      } else { ++(tally.clones); }
      // if configured to do so, include full tree info
      if ( m_fullMCTree && mcPart )
      {
        const std::string& tmpName = mcParticleNameTree(mcPart);
        if ( !isClone )
        { 
          ++(tally.notclones_detailed[tmpName]);
          tally.histos_detailed[tmpName].fillAll( mcPart ? mcPart->p()  : momentum(*proto),
                                                  mcPart ? mcPart->pt() : transverseMomentum(*proto) );
        } else { ++(tally.clones_detailed[tmpName]); }
      }

    } // loop over all protos at one location in TES

  } // loop over TES locations

  // Loop over found MCParticle containers
  for ( Locations::const_iterator iMCPLoc = mcPLocations.begin();
        iMCPLoc != mcPLocations.end(); ++iMCPLoc )
  {
    // loop over MCParticles at this TES location
    const LHCb::MCParticles * mcPs = get<LHCb::MCParticles>( *iMCPLoc, false );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Looping over " << mcPs->size() << " MCParticles at " << *iMCPLoc << endmsg;
    for ( LHCb::MCParticles::const_iterator iMCP = mcPs->begin();
          iMCP != mcPs->end(); ++iMCP )
    {
      // Get the MCParticle reco type
      const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(*iMCP);

      // only consider reconstructable MCPs
      if ( mcRecType <= IMCReconstructible::NotReconstructible ) continue;

      // Save the info for this mcparticle
      const std::string& name     = mcParticleName(*iMCP);
      const std::string& fullname = mcParticleNameTree(*iMCP);
      MCTally & tally = (m_rawMCMap[mcRecType])[name];
      ++(tally.notclones);
      // Momentum histogramming
      tally.histos.fillAll ( (*iMCP)->p(), (*iMCP)->pt() );
      // if configured to do so, include full tree info
      if ( m_fullMCTree )
      {
        ++(tally.notclones_detailed[fullname]);
        tally.histos_detailed[fullname].fillAll( (*iMCP)->p(), (*iMCP)->pt() );
      }

    }
  }

  // Loop over found MCParticles with associated protos, if more than one proto location found
  debug() << "Starting Correlations ..." << endmsg;
  for ( MCP2PartMap::const_iterator iMCP = mcp2Parts.begin();
        iMCP != mcp2Parts.end(); ++iMCP )
  {
    // Pointer to MCParticle
    const LHCb::MCParticle * mcP = (*iMCP).first;
    if ( !mcP ) continue;

    if ( msgLevel(MSG::DEBUG) )
      debug() << "MCParticle " << mcP->key() << " ("
              << objectLocation(mcP) << ") has "
              << (*iMCP).second.size() << " Particles" << endmsg;
    
    // Need at least 2 particles for correlations ...
    if ( (*iMCP).second.size() > 1 )
    {

      // MCParticle Type
      const std::string& name      = mcParticleName(mcP);
      const std::string& fullname  = mcParticleNameTree(mcP);
      debug() << " -> MC Type " << fullname << endmsg;

      // Get the MCParticle reco type
      const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(mcP);

      // Loop over the pairs of protos
      PartMap::const_iterator ipart1_end = (*iMCP).second.end(); 
      --ipart1_end;
      for ( PartMap::const_iterator ipart1 = (*iMCP).second.begin();
            ipart1 != ipart1_end; ++ipart1 )
      {
        // get pointers to particles
        const LHCb::Particle * part1 = (*ipart1).first;
        // Location in TES for proto1
        const std::string& protoloc1  = objectLocation(part1->proto());
        // Type of proto1
        const std::string& proto1Type = protoParticleType(part1->proto());
        if ( msgLevel(MSG::DEBUG) )
          debug() << "  -> Part1 " << part1->key() << " " << objectLocation(part1) 
                  << " " << protoloc1 << endmsg;
        for ( PartSet::const_iterator iPset1 = (*ipart1).second.begin();
              iPset1 != (*ipart1).second.end(); ++iPset1 )
        {
          const LHCb::Particle * firstpart1 = *iPset1;
          // Location in TES for part1
          const std::string& partloc1 = objectLocation(firstpart1);
          if ( msgLevel(MSG::DEBUG) )
            debug() << "   -> First Part1 " << firstpart1->key() << " " << partloc1 << endmsg;
          // inner loop over protos
          PartMap::const_iterator ipart2 = ipart1; ++ipart2;
          for ( ; ipart2 != (*iMCP).second.end(); ++ipart2 )
          {
            // get pointers to particles
            const LHCb::Particle * part2 = (*ipart2).first;
            // Location in TES for proto2
            const std::string& protoloc2 = objectLocation(part2->proto());
            // Type of proto2
            const std::string& proto2Type = protoParticleType(part2->proto());
            if ( msgLevel(MSG::DEBUG) )
              debug() << "    -> Part2 " << part2->key() << " " << objectLocation(part2)
                      << " " << protoloc2 << endmsg;
            for ( PartSet::const_iterator iPset2 = (*ipart2).second.begin();
                  iPset2 != (*ipart2).second.end(); ++iPset2 )
            {
              const LHCb::Particle * firstpart2 = *iPset2;
              // Location in TES for part2
              const std::string& partloc2 = objectLocation(firstpart2);
              if ( msgLevel(MSG::DEBUG) )
                debug() << "     -> First Part2 " << firstpart2->key() << " " << partloc2 << endmsg;
              if ( partloc1 != partloc2 && proto1Type == proto2Type )
              {
                const std::string corname = ( corProtoLocation( protoloc1, protoloc2 ) + "-" +
                                              corPartLocation ( partloc1,  partloc2  ) );
                debug() << "      -> Correlation name " << corname << endmsg;
                // count
                MCTally & tally =
                  ((m_correlations[corname])[proto1Type].trueMCType[mcRecType])[name];
                ++(tally.notclones);
                // Momentum histogramming
                tally.histos.fillAll ( mcP->p(), mcP->pt() );
                if ( m_fullMCTree )
                {
                  ++(tally.notclones_detailed[fullname]);
                  tally.histos_detailed[fullname].fillAll ( mcP->p(), mcP->pt() );
                }
              }
            }
          } // proto2 loop
        }
      } // proto1 loop

    } // more than one associated particle
  } // loop over MCPs

  return StatusCode::SUCCESS;
}

std::string
ParticleEffPurMoni::mcParticleName( const LHCb::MCParticle * mcPart )
{
  // Get the particles properties for this MCParticle only
  const LHCb::ParticleProperty * mcProp = ( mcPart ? partProp(mcPart->particleID()) : NULL );
  // return
  return ( mcProp ? mcProp->particle() : "NoMC" );
}

std::string ParticleEffPurMoni::mcParticleNameTree( const LHCb::MCParticle * mcPart )
{
  if ( mcPart )
  {
    // Use full MC tree for this particle
    std::string name;
    unsigned int nTree(0); // Limit tree size in name
    while ( mcPart && nTree<m_maxMCTreeSize )
    {
      // Get the particles properties for this MCParticle only
      const LHCb::ParticleProperty * mcProp = partProp(mcPart->particleID());
      if (mcProp)
      {
        if ( !name.empty() ) name = " -> "+name;
        name = mcProp->particle()+name;
      }
      if ( !mcPart->mother() &&
           mcPart->originVertex() &&
           mcPart->originVertex()->isPrimary() )
      {
        name = "PV -> " + name;
      }
      mcPart = mcPart->mother();
      ++nTree;
    }
    //if ( name.size() > m_maxNameLength ) m_maxNameLength = name.size();
    return name;
  }
  return "";
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
ParticleEffPurMoni::mcParticle_forward( const LHCb::ProtoParticle * proto ) const
{
  const LHCb::MCParticle * mcP(NULL);
  if ( proto->track() )
  {
    // For charged tracks use the RICH tool due to problems with ProtoLinkers
    mcP = truthTool()->mcParticle( proto->track(), minMCWeight(proto) );
  }
  else
  {
    // get the appropriate linker
    ProtoParticle2MCLinker * links = protoLinker(proto);
    if ( links )
    {
      // Nastiness to deal with linker interface :(
      double bestWeight(-999999);
      const LHCb::MCParticle * tmpMCP(links->firstMCP(proto));
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
    }
  }
  return mcP;
}

const LHCb::MCParticle *
ParticleEffPurMoni::mcParticle_reverse( const LHCb::ProtoParticle * proto ) const
{
  // correct map
  P2MCP * pmap = ( 0 != proto->charge() ? &m_cp2mcp : &m_np2mcp );
  if ( pmap->empty() )
  {
    // fill the map
    // get the appropriate linker
    ProtoParticle2MCLinker * links = protoLinker(proto);
    if ( links )
    {
      // loop over all MCPs
      const LHCb::MCParticles * mcps = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
      for ( LHCb::MCParticles::const_iterator iMCP = mcps->begin();
            iMCP != mcps->end(); ++iMCP )
      {
        // right charge ?
        const int charge = (*iMCP)->particleID().threeCharge();
        if ( ( 0 == proto->charge() && 0 == charge ) ||
             ( 0 != proto->charge() && 0 != charge ) )
        {
          if ( m_mcSel->accept(*iMCP) )
          {
            // Nastiness to deal with linker interface :(
            double bestWeight(-999999);
            const LHCb::ProtoParticle *P(NULL), *tmpP(links->firstP(*iMCP));
            while ( NULL != tmpP )
            {
              if ( links->weightP() > bestWeight &&
                   links->weightP() > minMCWeight(proto) )
              {
                bestWeight = links->weightP();
                P = tmpP;
              }
              tmpP = links->nextP();
            }
            // store final best association
            (*pmap)[P] = *iMCP;
          }
        }
      }
    }
  }
  P2MCP::const_iterator p = pmap->find( proto );
  return ( p == pmap->end() ? NULL : p->second );
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

const LHCb::ParticleProperty *
ParticleEffPurMoni::partProp( const LHCb::ParticleID& id ) const
{
  const LHCb::ParticleProperty * prop = ppSvc()->find ( id );
  if ( NULL == prop )
  {
    std::ostringstream mess;
    mess << "LHCb::ParticleProperty missing for ParticleID " << id;
    Warning( mess.str(), StatusCode::SUCCESS, 1 );
  }
  return prop;
}

void
ParticleEffPurMoni::addParticle( const LHCb::Particle * particle,
                                 const LHCb::Particle * firstParticle,
                                 const unsigned int recurCount,
                                 const std::string & history,
                                 const bool toplevel ) const
{
  // protect against too many recursive calls...
  // (should never ever get this deep, but to just to be safe ...)
  if ( recurCount > 99999 )
  {
    Warning( "Recursive limit in addParticle reached. Aborting Particle tree scan" ).ignore();
    return;
  }

  // get the history for the Particle
  const LHCb::ParticleProperty * prop = partProp( particle->particleID() );
  // If not available, just abort
  if ( !prop ) { return; }

  // Does this particle have a ProtoParticle. I.e. its stable
  const LHCb::ProtoParticle * proto = particle->proto();

  // yes, so add to map
  if ( proto )
  {
    // down to a basic particle, so save with history
    ++(m_partProtoMap[proto])[ ParticleHistory( particle, firstParticle,
                                                history + prop->particle(),
                                                prop, toplevel ) ];
  }
  // if no proto, try and save descendants
  else if ( !particle->isBasicParticle() )
  {
    // loop over them and to add them instead
    const LHCb::Particle::ConstVector& daughters = particle->daughtersVector();
    for ( LHCb::Particle::ConstVector::const_iterator iP = daughters.begin();
          iP != daughters.end(); ++iP )
    {
      addParticle( *iP, firstParticle, recurCount+1,
                   history + prop->particle() + " -> ", false );
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
    case LHCb::Track::Long        : return "Charged Long Track";
    case LHCb::Track::Upstream    : return "Charged Upstream Track";
    case LHCb::Track::Downstream  : return "Charged Downstream Track";
    case LHCb::Track::Ttrack      : return "Charged Ttrack Track";
    case LHCb::Track::Velo        : return "Charged Velo Track";
    case LHCb::Track::VeloR       : return "Charged VeloR Track";
    default                       : return "Charged UNKNOWN";
    }
  }
  else
  {
    return "Neutral Particle";
  }
}

const std::string &
ParticleEffPurMoni::shortProtoLoc( const std::string & loc ) const
{
  if ( m_ShortNames.find(loc) == m_ShortNames.end() )
  {
    std::ostringstream loc2;
    loc2 << "Protos" << m_protoCount++;
    m_ShortNames[loc] = loc2.str();
  }
  return m_ShortNames[loc];
}

const std::string &
ParticleEffPurMoni::shortPartLoc( const std::string & loc ) const
{
  if ( m_ShortNames.find(loc) == m_ShortNames.end() )
  {
    std::ostringstream loc2;
    loc2 << "Particles" << m_partCount++;
    m_ShortNames[loc] = loc2.str();
  }
  return m_ShortNames[loc];
}

void ParticleEffPurMoni::printStats() const
{

  /** @todo Getting a little messy.
   *  Could do with a tidy up together with the associated utility classes
   */

  const std::string LINES(147,'=');
  const std::string lines(147,'-');

  // loop over Particle TES locations
  for ( LocationMap::iterator iLoc = m_locMap.begin();
        iLoc != m_locMap.end(); ++iLoc )
  {
    always() << LINES << endmsg;
    always() << " Particle Location : " << (*iLoc).first
             << " (" << shortPartLoc((*iLoc).first) << ")"
             << endmsg;

    // Print ProtoParticle Stats for these Particles
    std::set<StringPair> _corLocs;
    unsigned effN(0);
    for ( ProtoTESStatsMap::const_iterator iX = m_partProtoTESStats[(*iLoc).first].begin();
          iX != m_partProtoTESStats[(*iLoc).first].end(); ++iX )
    {
      always() << "  -> Used ProtoParticles        : " << iX->first << " "
               << eff( iX->second.nWithMC, iX->second.nTotal ) << "% with MC" << endmsg;
      for ( LocationMap::iterator iLoc2 = m_locMap.begin();
            iLoc2 != m_locMap.end(); ++iLoc2 )
      {
        if ( (*iLoc2).first == (*iLoc).first ) continue;
        always() << "    -> Correlated Particles     : " << (*iLoc2).first
                 << " (" << shortPartLoc((*iLoc2).first) << ")"
                 << endmsg;
        for ( ProtoTESStatsMap::const_iterator iXX = m_partProtoTESStats[(*iLoc2).first].begin();
              iXX != m_partProtoTESStats[(*iLoc2).first].end(); ++iXX )
        {
          always() << "      -> Used ProtoParticles    : " << iXX->first << " "
                   << eff( iXX->second.nWithMC, iXX->second.nTotal ) << "% with MC" << endmsg;
          const std::string corLoc = ( corProtoLocation(iX->first,iXX->first) + "-" +
                                       corPartLocation((*iLoc).first,(*iLoc2).first) );
          const std::string effS = "CorrEff"+boost::lexical_cast<std::string>(effN++);
          _corLocs.insert(StringPair(effS,corLoc));
          always() << "      -> '" << effS << "' = 100% * ("
                   << shortPartLoc((*iLoc).first) << "&&" << shortPartLoc((*iLoc2).first)
                   << ")/" << shortPartLoc((*iLoc).first) << endmsg;
        }
      }
    }
    always() << lines << endmsg;

    // loop over reco particle types for this location
    for ( MCSummaryMap::iterator iSum = (*iLoc).second.begin();
          iSum != (*iLoc).second.end(); ++iSum )
    {
      MCRecTypeMap & mcTypes = (*iSum).second.trueMCType;
      long nRecoTrue(0);
      for ( MCRecTypeMap::iterator iMCt = mcTypes.begin();
            iMCt != mcTypes.end(); ++iMCt )
      {
        nRecoTrue += ((*iMCt).second[(*iSum).first.particleName]).all();
      }
      //const bool primaryPart = ( (*iSum).first.decayTree == (*iSum).first.particleName );
      const bool primaryPart = true;
      std::ostringstream srecotitle;
      srecotitle << "  Reco. | " << (*iSum).second.nReco << " " << (*iSum).first.decayTree
                 << " | " << (*iSum).first.protoType;
      std::string recotitle = srecotitle.str();
      recotitle.resize(10+m_maxNameLength,' ');
      always() << recotitle << "|  % of sample";
      if ( primaryPart )
      {
        always() << "   | Proto->Part eff |  MC->Part eff  |  % MC Clones";
        for ( std::set<StringPair>::const_iterator iCorPartLoc = _corLocs.begin();
              iCorPartLoc != _corLocs.end(); ++iCorPartLoc )
        {
          always() << "   |   " << iCorPartLoc->first << "  ";
        }
      }
      always() << endmsg;
      if ( (*iSum).second.nReco > 0 )
      {
        // MC reconstructibility
        for ( MCRecTypeMap::iterator iMCT = (*iSum).second.trueMCType.begin();
              iMCT != (*iSum).second.trueMCType.end(); ++iMCT )
        {
          always() << "   -> MC Class | " << IMCReconstructible::text((*iMCT).first)
                   << endmsg;
          int suppressedContribs(0);
          // loop over the MC Types for this reco type
          for ( TypeTally::iterator iT = (*iMCT).second.begin();
                iT != (*iMCT).second.end(); ++iT )
          {
            if ( (100.0*(*iT).second.all())/(*iSum).second.nReco > m_minContrib )
            {
              // Main contribution
              MCTally & partTally = (*iT).second;
              std::ostringstream mcTs;
              mcTs << "     -> " << partTally.all() << " " << (*iT).first;
              std::string mcT = mcTs.str();
              mcT.resize(10+m_maxNameLength,' ');
              const MCTally & protoTally = ((m_mcProtoCount[(*iSum).first.protoTESLoc])[(*iSum).first.protoType].trueMCType[(*iMCT).first])[(*iT).first];
              const MCTally & mcTally = (m_rawMCMap[(*iMCT).first])[(*iT).first];
              const long nTotalMC = mcTally.all();
              always() << mcT
                       << "|" << eff( partTally.all(), (*iSum).second.nReco );

              // histo path
              std::ostringstream h_path;
              h_path << convertTitleToID((*iLoc).first) << "/"
                     << "Reco " << (*iSum).first.decayTree << "/"
                     << "Reco " << (*iSum).first.protoType << "/"
                     << "MC "   << IMCReconstructible::text((*iMCT).first) << "/"
                     << "MC "   << (*iT).first;

              if ( primaryPart )
              {
                makeAllPlots1( h_path.str(), partTally.histos, protoTally.histos, mcTally.histos );
                always() << " | " << eff( partTally.notclones, protoTally.notclones )
                         << " |"  << eff( partTally.notclones, nTotalMC )
                         << " |"  << eff( partTally.clones, nTotalMC );
                // correlations
                if ( !m_correlations.empty() )
                {
                  for ( std::set<StringPair>::const_iterator iCorPartLoc = _corLocs.begin();
                        iCorPartLoc != _corLocs.end(); ++iCorPartLoc )
                  {
                    const MCTally & corTally  = (m_correlations[iCorPartLoc->second])[(*iSum).first.protoType].trueMCType[(*iMCT).first][(*iT).first];
                    always() << " |" << eff( corTally.notclones, partTally.notclones );
                    makeAllPlots2( h_path.str(), partTally.histos, mcTally.histos, *iCorPartLoc, corTally.histos );
                  }
                }
              }
              always() << endmsg;

              // sub contributions
              int suppressedContribsC(0);
              for ( MCTally::Contributions::const_iterator iC = partTally.notclones_detailed.begin();
                    iC != partTally.notclones_detailed.end(); ++iC )
              {
                const unsigned long int nClones    = partTally.clones_detailed[(*iC).first];
                const unsigned long int nNotClones = (*iC).second;
                const unsigned long int nTotal     = nClones+nNotClones;
                if ( (100.0*nTotal)/(*iSum).second.nReco > m_minContrib )
                {
                  std::ostringstream mcTsC;
                  mcTsC << "       -> "<< nTotal << " " << (*iC).first;
                  std::string mcTC = mcTsC.str();
                  mcTC.resize(10+m_maxNameLength,' ');
                  const long nBkgTrue =
                    ((m_mcProtoCount[(*iSum).first.protoTESLoc])[(*iSum).first.protoType].trueMCType[(*iMCT).first])[(*iT).first].notclones_detailed[(*iC).first];
                  //const long nBkg = nBkgTrue + nClones;
                  const long nTotalMCC = (m_rawMCMap[(*iMCT).first])[(*iT).first].notclones_detailed[(*iC).first];;
                  always() << mcTC
                           << "|" << eff( nTotal, (*iSum).second.nReco );
                  if ( primaryPart )
                  {
                    makeAllPlots1( h_path.str()+"/"+(*iC).first,
                                   partTally.histos_detailed[(*iC).first],
                                   protoTally.histos_detailed[(*iC).first],
                                   mcTally.histos_detailed[(*iC).first] );
                    always() << " | " << eff( nNotClones, nBkgTrue )
                             << " |"  << eff( nNotClones, nTotalMCC )
                             << " |"  << eff( nClones, nTotalMCC );
                    // correlations
                    if ( !m_correlations.empty() )
                    {
                      for ( std::set<StringPair>::const_iterator iCorPartLoc = _corLocs.begin();
                            iCorPartLoc != _corLocs.end(); ++iCorPartLoc )
                      {
                        const MCTally & ccorTally = ((m_correlations[iCorPartLoc->second])[(*iSum).first.protoType].trueMCType[(*iMCT).first])[(*iT).first];
                        const long moo = ccorTally.notclones_detailed[(*iC).first];
                        always() << " |" << eff( moo, nNotClones );
                        makeAllPlots2( h_path.str()+"/"+(*iC).first,
                                       partTally.histos_detailed[(*iC).first],
                                       mcTally.histos_detailed[(*iC).first],
                                       *iCorPartLoc,
                                       ccorTally.histos_detailed[(*iC).first] );
                      }
                    }
                  }
                  always() << endmsg;

                } else { ++suppressedContribsC; }
              }
              if ( suppressedContribsC>0 )
              {
                always() << "       -> Suppressed " << suppressedContribsC << " contribution(s) below "
                         <<  m_minContrib << " %" << endmsg;
              }
            } else { ++suppressedContribs; }
          }
          if ( suppressedContribs>0 )
          {
            always() << "     -> Suppressed " << suppressedContribs << " contribution(s) below "
                     <<  m_minContrib << " %" << endmsg;
          }
        }
      }

    }
  }

  always() << LINES << endmsg;
}

void ParticleEffPurMoni::makeAllPlots1( const std::string hpath,
                                        const HistoHandle & partTally,
                                        const HistoHandle & protoTally,
                                        const HistoHandle & mcTally ) const
{
  makeEffHisto( hpath+"/MCParticle -> Particle Eff. Versus Ptot",
                partTally.effVp(), mcTally.effVp() );
  makeEffHisto( hpath+"/ProtoParticle -> Particle Eff. Versus Ptot",
                partTally.effVp(), protoTally.effVp() );
  makeEffHisto( hpath+"/MCParticle -> ProtoParticle Eff. Versus Ptot",
                protoTally.effVp(), mcTally.effVp() );
  makeEffHisto( hpath+"/MCParticle -> Particle Eff. Versus Pt",
                partTally.effVpt(), mcTally.effVpt() );
  makeEffHisto( hpath+"/ProtoParticle -> Particle Eff. Versus Pt",
                partTally.effVpt(), protoTally.effVpt() );
  makeEffHisto( hpath+"/MCParticle -> ProtoParticle Eff. Versus Pt",
                protoTally.effVpt(), mcTally.effVpt() );
  //makeEffHisto( hpath+"/MCParticle -> Particle Eff. Versus Ptot&Pt",
  //              partTally.effVpVpt(), mcTally.effVpVpt() );
  //makeEffHisto( hpath+"/ProtoParticle -> Particle Eff. Versus Ptot&Pt",
  //              partTally.effVpVpt(), protoTally.effVpVpt() );
  //makeEffHisto( hpath+"/MCParticle -> ProtoParticle Eff. Versus Ptot&Pt",
  //              protoTally.effVpVpt(), mcTally.effVpVpt() );
}

void ParticleEffPurMoni::makeAllPlots2( const std::string hpath,
                                        const HistoHandle & partTally,
                                        const HistoHandle & mcTally,
                                        const StringPair& corName,
                                        const HistoHandle & corTally ) const
{
  makeEffHisto( hpath+"/MCParticle -> "+corName.second+" Versus Ptot",
                corTally.effVp(), mcTally.effVp() );
  makeEffHisto( hpath+"/MCParticle -> "+corName.second+" Versus Pt",
                corTally.effVpt(), mcTally.effVpt() );
  //makeEffHisto( hpath+"/MCParticle -> "+corName.second+" Versus Ptot&Pt",
  //              corTally.effVpVpt(), mcTally.effVpVpt() );
  makeEffHisto( hpath+"/'"+corName.first+"' Versus Ptot",
                corTally.effVp(), partTally.effVp() );
  makeEffHisto( hpath+"/'"+corName.first+"' Versus Pt",
                corTally.effVpt(), partTally.effVpt() );
  //makeEffHisto( hpath+"/'"+corName.first+"' Versus Ptot&Pt",
  //              corTally.effVpVpt(), partTally.effVpVpt() );
}

void ParticleEffPurMoni::makeEffHisto( const std::string title,
                                       const EffVersusMomentum & top,
                                       const EffVersusMomentum & bot ) const
{
  AIDA::IProfile1D * h(NULL);
  // Loop over bins
  verbose() << "Filling histo " << title << endmsg;
  for ( unsigned int bin = 0; bin < top.nBins(); ++bin )
  {
    verbose() << " -> bin " << bin << " : "
              << top.data()[bin] << " / " << bot.data()[bin] << endmsg;
    const unsigned int total = bot.data()[bin];
    if ( total>0 )
    {
      const unsigned int selected = top.data()[bin];
      // Fill Profile correct number of times with 0 or 100
      for ( unsigned int i = 0; i < total; ++i )
      {
        h = profile1D( top.p(bin),
                       ( i < selected ? 100.0 : 0.0 ),
                       title, title,
                       top.minX(), top.maxX(), top.nBins() );
      }
    }
  }
  if ( h )
  {
    TProfile * histo = Gaudi::Utils::Aida2ROOT::aida2root(h);
    if (histo) histo->SetErrorOption("g");
  }
}

void ParticleEffPurMoni::makeEffHisto( const std::string title,
                                       const EffVersusMomentum2D & top,
                                       const EffVersusMomentum2D & bot ) const
{
  AIDA::IProfile2D * h(NULL);
  // Loop over bins
  verbose() << "Filling histo " << title << endmsg;
  for ( unsigned int binx = 0; binx < top.nBinsX(); ++binx )
  {
    for ( unsigned int biny = 0; biny < top.nBinsY(); ++biny )
    {
      const unsigned int total = (bot.data()[binx])[biny];
      if ( total>0 )
      {
        const unsigned int selected = (top.data()[binx])[biny];
        // Fill Profile correct number of times with 0 or 100
        for ( unsigned int i = 0; i < total; ++i )
        {
          h = profile2D( top.x(binx), top.y(biny),
                         ( i < selected ? 100.0 : 0.0 ),
                         title, title,
                         top.minX(), top.maxX(),
                         top.minY(), top.maxY(),
                         top.nBinsX(), top.nBinsY() );
        }
      }
    }
  }
  if ( h )
  {
    TProfile2D * histo = Gaudi::Utils::Aida2ROOT::aida2root(h);
    if (histo) histo->SetErrorOption("g");
  }
}

ProtoParticle2MCLinker *
ParticleEffPurMoni::protoLinker( const LHCb::ProtoParticle * proto ) const
{
  if (!proto) return NULL;
  const std::string& loc = objectLocation(proto);
  ProtoParticle2MCLinker *& linker = m_protoLinker[loc];
  if ( !linker )
  {
    linker =
      new ProtoParticle2MCLinker( this,
                                  ( 0 != proto->charge() ?
                                    Particle2MCMethod::ChargedPP : Particle2MCMethod::NeutralPP ),
                                  std::vector<std::string>(1,loc) );
  }
  return linker;
}

// Access the Particle Linker appropriate for the given Particle
Particle2MCLinker *
ParticleEffPurMoni::particleLinker( const LHCb::Particle * /* part */ ) const
{
  if ( !m_particleLinker )
  {
    m_particleLinker =
      new Particle2MCLinker ( this,
                              Particle2MCMethod::Composite,
                              std::vector<std::string>(1,"") );
  }
  return m_particleLinker;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ParticleEffPurMoni::finalize()
{
  // Print out summary info
  if ( m_nEvts > 0 ) printStats();
  // cleanup
  if ( m_particleLinker ) { delete m_particleLinker; m_particleLinker = NULL; }
  for ( ProtoLinkerTESMap::iterator iLC = m_protoLinker.begin();
        iLC != m_protoLinker.end(); ++iLC )
  { delete iLC->second; iLC->second = NULL; }
  // finalize base class
  return DaVinciHistoAlgorithm::finalize();
}

//=============================================================================

