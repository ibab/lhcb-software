//-----------------------------------------------------------------------------
/** @file ParticleEffPurMoni.cpp
 *
 *  Implementation file for class : ParticleEffPurMoni
 *
 *  CVS Log :-
 *  $Id: ParticleEffPurMoni.cpp,v 1.25 2008-07-11 23:48:32 jonrob Exp $
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
  : DVAlgorithm      ( name , pSvcLocator ),
    m_recurCount     ( 0    ),
    m_trSel          ( NULL ),
    m_mcRec          ( NULL ),
    m_mcSel          ( NULL ),
    m_particleLinker ( NULL ),
    m_protoCount     ( 0    ),
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
  //declareProperty( "MinimumHistoMomentum",       m_minP = 0*Gaudi::Units::GeV );
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
  // get an instance of the MC Reconstructible tool
  m_mcRec = tool<IMCReconstructible>( "MCReconstructible", "MCRecibleForParticleMoni" );
  // MCParticle selector
  m_mcSel = tool<IMCParticleSelector>( "MCParticleSelector" );

  m_protoShortNames["/Event/Rec/ProtoP/Charged"] = "OfflineProtos";
  m_protoShortNames["/Event/Hlt/ProtoP/Charged"] = "HltProtos";

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleEffPurMoni::execute()
{
  // get the desktop particles
  const LHCb::Particle::ConstVector & particles = desktop()->particles();

  // clear the working maps
  m_partProtoMap.clear();
  m_np2mcp.clear();
  m_cp2mcp.clear();

  // count events
  ++m_nEvts;

  // Loop over particles to fill the map
  for ( LHCb::Particle::ConstVector::const_iterator iP = particles.begin();
        iP != particles.end(); ++iP )
  {
    m_recurCount = 0;
    addParticle( *iP, *iP );
  }

  typedef std::set<std::string> Locations;
  // Proto TES locations found
  Locations protoLocations;
  // MCParticle locations found
  Locations mcPLocations;

  // Protos associated to each MCParticle
  typedef std::set<const LHCb::ProtoParticle *> ProtoSet;
  typedef std::map<const LHCb::MCParticle*, ProtoSet> MCP2ProtoSet;
  MCP2ProtoSet mcp2Protos;

  // already used MCParticles for each each ProtoParticle location
  std::map< std::string, std::set<const LHCb::MCParticle*> > usedMCPs;

  // Loop over the final list of Protos and associated Particles
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << m_partProtoMap.size()
            << " used ProtoParticles from input Particles" << endreq;
  for ( ParticleProtoMap::const_iterator iPM = m_partProtoMap.begin();
        iPM != m_partProtoMap.end(); ++iPM )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "  -> ProtoParticle " << (*iPM).first->key() << endreq;

    // select protoparticles
    if ( !selectProto((*iPM).first) ) continue;
    if ( msgLevel(MSG::DEBUG) ) debug() << "     -> selected " << endreq;

    // store this proto TES location
    const std::string& protoTesLoc = objectLocation((*iPM).first->parent());
    protoLocations.insert( protoTesLoc );

    // Get the MCParticle
    const LHCb::MCParticle * mcPart = mcParticle((*iPM).first);

    // is this a clone ?
    const bool isClone =
      ( usedMCPs[protoTesLoc].find(mcPart) != usedMCPs[protoTesLoc].end() );

    // add to list of seen MCPs
    usedMCPs[protoTesLoc].insert(mcPart);

    // Store the TES location for this MCParticle container
    if (mcPart) mcPLocations.insert( objectLocation(mcPart->parent()) );

    // Get the MCParticle reco type
    const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(mcPart);

    // ProtoParticle momentum
    const double ptot = momentum((*iPM).first);
    const double pt   = transverseMomentum((*iPM).first);

    // protoparticle type (long etc.)
    const std::string& protoType = protoParticleType((*iPM).first);

    // Loop over Particles for the Proto
    for ( ParticleHistory::Vector::const_iterator iPart = (*iPM).second.begin();
          iPart != (*iPM).second.end(); ++iPart )
    {
      // Find its TES location
      const std::string& tesLoc = objectLocation( (*iPart).firstParticle->parent() );
      if ( msgLevel(MSG::DEBUG) )
        debug() << " -> Found Particle in " << tesLoc << endreq
                << " -> " << *iPart << endreq;

      // count protos per particle TES
      ++((m_partProtoTESStats[tesLoc])[protoTesLoc]).nTotal;
      if ( mcPart ) ++((m_partProtoTESStats[tesLoc])[protoTesLoc]).nWithMC;

      // get the data for this TES location
      MCSummaryMap & mcMap = m_locMap[tesLoc];

      // get the summary for this reco particle type
      const FullPartName partName( (*iPart).properties->particle(),
                                   (*iPart).history,
                                   protoType,
                                   protoTesLoc );
      MCSummary & mcSum = mcMap[ partName ];

      // count the number reconstructed for this type
      ++mcSum.nReco;

      // count the true types for this reco type
      MCTally & tally = (mcSum.trueMCType[mcRecType])[ mcParticleName(mcPart) ];
      ++(tally.all);
      if ( isClone ) ++(tally.clones);
      // if configured to do so, include full tree info
      if ( m_fullMCTree && mcPart )
      {
        const std::string tmpName = mcParticleNameTree(mcPart);
        ++(tally.all_detailed[tmpName]);
        if ( isClone ) ++(tally.clones_detailed[tmpName]);
      }
      // Momentum histogramming
      if ( !isClone )
      {
        tally.effVp().fill   ( mcPart ? mcPart->p()  : ptot );
        tally.effVpt().fill  ( mcPart ? mcPart->pt() : pt   );
        tally.effVpVpt().fill( mcPart ? mcPart->p()  : ptot,
                               mcPart ? mcPart->pt() : pt   );
      }

    } // loop over particles produced from this proto

  } // loop over all used protos

  // Count the total number of protos at each location used from TES
  for ( Locations::const_iterator iProtoLoc = protoLocations.begin();
        iProtoLoc != protoLocations.end(); ++iProtoLoc )
  {
    // already used MCParticles
    std::set<const LHCb::MCParticle *> usedMCPs;
    // fill short location name
    shortProtoLoc(*iProtoLoc);
    // get the protos at this location
    const std::string & tesLoc = (*iProtoLoc);
    const LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( tesLoc );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Looping over " << protos->size() << " ProtoParticles at " << tesLoc << endreq;
    for ( LHCb::ProtoParticles::const_iterator proto = protos->begin();
          proto != protos->end(); ++proto )
    {
      // is this proto selected ?
      if ( !selectProto(*proto) ) continue;

      // Get the MCParticle
      const LHCb::MCParticle * mcPart = mcParticle(*proto);

      // is this a clone ?
      const bool isClone = ( usedMCPs.find(mcPart) != usedMCPs.end() );

      // add to list of seen MCPs
      if ( mcPart ) usedMCPs.insert(mcPart);

      // Get the MCParticle reco type
      const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(mcPart);

      // protoparticle type (long etc.)
      const std::string& protoType = protoParticleType(*proto);

      // count
      MCTally & tally =
        ((m_mcProtoCount[tesLoc])[protoType].trueMCType[mcRecType])[mcParticleName(mcPart)];
      ++(tally.all);
      if (isClone) ++(tally.clones);
      // if configured to do so, include full tree info
      if ( m_fullMCTree && mcPart )
      {
        const std::string& tmpName = mcParticleNameTree(mcPart);
        ++(tally.all_detailed[tmpName]);
        if (isClone) ++(tally.clones_detailed[tmpName]);
      }

      // Momentum histogramming
      if ( !isClone )
      {
        tally.effVp().fill    ( mcPart ? mcPart->p()  : momentum(*proto)           );
        tally.effVpt().fill   ( mcPart ? mcPart->pt() : transverseMomentum(*proto) ); 
        tally.effVpVpt().fill ( mcPart ? mcPart->p()  : momentum(*proto),
                                mcPart ? mcPart->pt() : transverseMomentum(*proto) );
      }

      // Proto Correlations
      if ( !isClone && mcPart ) mcp2Protos[mcPart].insert( *proto );

    } // loop over all protos at one location in TES

  } // loop over TES locations

  // Loop over found MCParticle containers
  for ( Locations::const_iterator iMCPLoc = mcPLocations.begin();
        iMCPLoc != mcPLocations.end(); ++iMCPLoc )
  {
    // loop over MCParticles at this TES location
    const LHCb::MCParticles * mcPs = get<LHCb::MCParticles>( *iMCPLoc );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Looping over " << mcPs->size() << " MCParticles at " << *iMCPLoc << endreq;
    for ( LHCb::MCParticles::const_iterator iMCP = mcPs->begin();
          iMCP != mcPs->end(); ++iMCP )
    {
      // Get the MCParticle reco type
      const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(*iMCP);

      // only consider reconstructable MCPs
      if ( mcRecType <= IMCReconstructible::NotReconstructible ) continue;

      // make sure this MCParticle is in the list
      //mcp2Protos[*iMCP];

      // Save the info for this mcparticle
      const std::string& name     = mcParticleName(*iMCP);
      const std::string& fullname = mcParticleNameTree(*iMCP);
      MCTally & tally = (m_rawMCMap[mcRecType])[name];
      ++(tally.all);
      // if configured to do so, include full tree info
      if ( m_fullMCTree ) { ++(tally.all_detailed[fullname]); }

      // Momentum histogramming
      tally.effVp().fill    ( (*iMCP)->p()  );
      tally.effVpt().fill   ( (*iMCP)->pt() ); 
      tally.effVpVpt().fill ( (*iMCP)->p(), (*iMCP)->pt() );
    }
  }

  // Loop over found MCParticles with associated protos, if more than one proto location found
  if ( protoLocations.size() > 1 )
  {
    for ( MCP2ProtoSet::const_iterator iMCP = mcp2Protos.begin();
          iMCP != mcp2Protos.end(); ++iMCP )
    {
      const LHCb::MCParticle * mcP = (*iMCP).first;
      if ( !mcP ) continue;
      const std::string& name      = mcParticleName(mcP);
      const std::string& fullname  = mcParticleNameTree(mcP);
      // Get the MCParticle reco type
      const IMCReconstructible::RecCategory mcRecType = m_mcRec->reconstructible(mcP);

      // Loop over the pairs of protos
      for ( ProtoSet::const_iterator proto1 = (*iMCP).second.begin();
            proto1 != (*iMCP).second.end(); ++proto1 )
      {
        // Location in TES for proto1
        const std::string& loc1 = objectLocation((*proto1)->parent());
        // Type of proto1
        const std::string& proto1Type = protoParticleType(*proto1);
        // inner loop over protos
        ProtoSet::const_iterator proto2 = proto1; ++proto2;
        for ( ; proto2 != (*iMCP).second.end(); ++proto2 )
        {
          // Location in TES for proto2
          const std::string& loc2 = objectLocation((*proto2)->parent());
          // Type of proto2
          const std::string& proto2Type = protoParticleType(*proto2);
          if ( loc1 != loc2 && proto1Type == proto2Type )
          {
            m_corProtoMap[loc1] = loc2;
            m_corProtoMap[loc2] = loc1;
            const std::string& corname = ( loc1<loc2 ? loc1+"&"+loc2 : loc2+"&"+loc1 );
            // count
            MCTally & tally =
              ((m_correlations[corname])[proto1Type].trueMCType[mcRecType])[name];
            ++(tally.all);
            if ( m_fullMCTree ) { ++(tally.all_detailed[fullname]); }
            // Momentum histogramming
            tally.effVp().fill ( mcP->p()  );
            tally.effVpt().fill( mcP->pt() );
          }
        } // proto2 loop
      } // proto1 loop

    } // loop over MCPs
  } // more than one proto location

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

std::string ParticleEffPurMoni::mcParticleName( const LHCb::MCParticle * mcPart )
{
  // Get the particles properties for this MCParticle only
  const ParticleProperty * mcProp = ( mcPart ? partProp(mcPart->particleID()) : NULL );
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
      const ParticleProperty * mcProp = partProp(mcPart->particleID());
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
ParticleEffPurMoni::mcParticle_reverse( const LHCb::ProtoParticle * proto ) const
{
  // correct map
  P2MCP * pmap = ( 0 != proto->charge() ? &m_cp2mcp : &m_np2mcp );
  if ( pmap->empty() )
  {
    // fill the map
    // get the appropriate linker
    ProtoParticle2MCLinker * links = protoLinker(proto);
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

const ParticleProperty *
ParticleEffPurMoni::partProp( const LHCb::ParticleID id ) const
{
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

  // get the history for the Particle
  const ParticleProperty * prop = partProp( particle->particleID() );
  // If not available, just abort
  if ( !prop ) { return; }

  // Does this particle have a ProtoParticle. I.e. its stable
  const LHCb::ProtoParticle * proto = particle->proto();

  // yes, so add to map
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
    const LHCb::Particle::ConstVector& daughters = particle->daughtersVector();
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
  if ( m_protoShortNames.find(loc) == m_protoShortNames.end() )
  {
    std::ostringstream loc2;
    loc2 << "PLoc" << m_protoCount++;
    m_protoShortNames[loc] = loc2.str();
  }
  return m_protoShortNames[loc];
}

void ParticleEffPurMoni::printStats() const
{

  /** @todo Getting a little messy.
   *  Could do with a tidy up together with the associated utility classes
   */

  const std::string LINES(147,'=');
  const std::string lines(147,'-');

  const Rich::PoissonEffFunctor eff("%7.2f +-%5.2f");

  always() << LINES << endreq;

  // loop over Particle TES locations
  for ( LocationMap::iterator iLoc = m_locMap.begin();
        iLoc != m_locMap.end(); ++iLoc )
  {
    always() << " Particle Location : " << (*iLoc).first << endreq;
    for ( ProtoTESStatsMap::const_iterator iX = m_partProtoTESStats[(*iLoc).first].begin();
          iX != m_partProtoTESStats[(*iLoc).first].end(); ++iX )
    {
      always() << "  -> Used ProtoParticles        : " << iX->first << " "
               << eff( iX->second.nWithMC, iX->second.nTotal ) << "% with MC" << endreq;
      always() << "  -> Correlated ProtoParticles  : " << m_corProtoMap[iX->first] << endreq;
      always() << "    -> 'ProtoCorr%' = 100% * (" << shortProtoLoc(iX->first) << "&" << shortProtoLoc(m_corProtoMap[iX->first])
               << ")/" << shortProtoLoc(iX->first) << endreq;
    }
    always() << lines << endreq;

    // loop over reco particle types for this location
    for ( MCSummaryMap::iterator iSum = (*iLoc).second.begin();
          iSum != (*iLoc).second.end(); ++iSum )
    {
      MCRecTypeMap & mcTypes = (*iSum).second.trueMCType;
      long nRecoTrue(0);
      for ( MCRecTypeMap::iterator iMCt = mcTypes.begin();
            iMCt != mcTypes.end(); ++iMCt )
      {
        nRecoTrue += ((*iMCt).second[(*iSum).first.particleName]).all;
      }
      const bool primaryPart = ( (*iSum).first.decayTree == (*iSum).first.particleName );
      //const bool primaryPart = true;
      std::ostringstream srecotitle;
      srecotitle << "  Reco. | " << (*iSum).second.nReco << " " << (*iSum).first.decayTree
                 << " | " << (*iSum).first.protoType;
      std::string recotitle = srecotitle.str();
      recotitle.resize(10+m_maxNameLength,' ');
      always() << recotitle << "|  % of sample";
      if ( primaryPart )
      {
        always() << "   | Proto->Part eff |  MC->Part eff  |  % MC Clones";
        if ( !m_correlations.empty() ) always() << "   |  ProtoCorr%";
      }
      always() << endreq;
      if ( (*iSum).second.nReco > 0 )
      {
        // MC reconstructibility
        for ( MCRecTypeMap::iterator iMCT = (*iSum).second.trueMCType.begin();
              iMCT != (*iSum).second.trueMCType.end(); ++iMCT )
        {
          always() << "   -> MC Class | " << IMCReconstructible::text((*iMCT).first)
                   << endreq;
          int suppressedContribs(0);
          // loop over the MC Types for this reco type
          for ( TypeTally::iterator iT = (*iMCT).second.begin();
                iT != (*iMCT).second.end(); ++iT )
          {
            if ( (100.0*(*iT).second.all)/(*iSum).second.nReco > m_minContrib )
            {
              // Main contribution
              MCTally & tally = (*iT).second;
              std::ostringstream mcTs;
              mcTs << "     -> " << tally.all << " " << (*iT).first;
              std::string mcT = mcTs.str();
              mcT.resize(10+m_maxNameLength,' ');
              const MCTally & protoTally = ((m_mcProtoCount[(*iSum).first.protoTESLoc])[(*iSum).first.protoType].trueMCType[(*iMCT).first])[(*iT).first];
              const long nBkgTrue = protoTally.all;
              const MCTally & mcTally = (m_rawMCMap[(*iMCT).first])[(*iT).first];
              const long nTotalMC = mcTally.all;
              always() << mcT
                       << "|" << eff( tally.all, (*iSum).second.nReco );
              if ( primaryPart )
              {
                always() << " | " << eff( tally.all, nBkgTrue )
                         << " |"  << eff( tally.all-tally.clones, nTotalMC )
                         << " |"  << eff( tally.clones, nTotalMC );
                // correlations
                if ( !m_correlations.empty() )
                {
                  const std::string & loc1    = (*iSum).first.protoTESLoc;
                  const std::string & loc2    = m_corProtoMap[loc1];
                  const std::string & corname = ( loc1<loc2 ? loc1+"&"+loc2 : loc2+"&"+loc1 );
                  const MCTally & corTally = (m_correlations[corname])[(*iSum).first.protoType].trueMCType[(*iMCT).first][(*iT).first];
                  always() << " |" << eff( corTally.all, tally.all );
                }
              }
              always() << endreq;

              // histograms
              if ( primaryPart )
              {
                // make a histo (not yet for composites....)
                std::ostringstream h_path;
                h_path
                  << convertTitleToID((*iLoc).first) << "/"
                  << "Reco " << (*iSum).first.decayTree << "/"
                  << "Reco " << (*iSum).first.protoType << "/"
                  << "MC "   << IMCReconstructible::text((*iMCT).first) << "/"
                  << "MC "   << (*iT).first;
                makeEffHisto( h_path.str()+"/MCParticle -> Particle Eff. Versus Ptot",
                              tally.effVp(), mcTally.effVp() );
                makeEffHisto( h_path.str()+"/ProtoParticle -> Particle Eff. Versus Ptot",
                              tally.effVp(), protoTally.effVp() );
                makeEffHisto( h_path.str()+"/MCParticle -> ProtoParticle Eff. Versus Ptot",
                              protoTally.effVp(), mcTally.effVp() );
                makeEffHisto( h_path.str()+"/MCParticle -> Particle Eff. Versus Pt",
                              tally.effVpt(), mcTally.effVpt() );
                makeEffHisto( h_path.str()+"/ProtoParticle -> Particle Eff. Versus Pt",
                              tally.effVpt(), protoTally.effVpt() );
                makeEffHisto( h_path.str()+"/MCParticle -> ProtoParticle Eff. Versus Pt",
                              protoTally.effVpt(), mcTally.effVpt() );
                makeEffHisto( h_path.str()+"/MCParticle -> Particle Eff. Versus P&Pt",
                              tally.effVpVpt(), mcTally.effVpVpt() );
                makeEffHisto( h_path.str()+"/ProtoParticle -> Particle Eff. Versus P&Pt",
                              tally.effVpVpt(), protoTally.effVpVpt() );
                makeEffHisto( h_path.str()+"/MCParticle -> ProtoParticle Eff. Versus P&Pt",
                              protoTally.effVpVpt(), mcTally.effVpVpt() );
                if ( !m_correlations.empty() )
                {
                  const std::string & loc1    = (*iSum).first.protoTESLoc;
                  const std::string & loc2    = m_corProtoMap[loc1];
                  const std::string & corname = ( loc1<loc2 ? loc1+"&"+loc2 : loc2+"&"+loc1 );
                  const MCTally & corTally = (m_correlations[corname])[(*iSum).first.protoType].trueMCType[(*iMCT).first][(*iT).first];
                  const std::string & sloc1   = shortProtoLoc(loc1);
                  const std::string & sloc2   = shortProtoLoc(loc2);
                  const std::string & scorname = ( sloc1<sloc2 ? sloc1+"&"+sloc2 : sloc2+"&"+sloc1 );
                  makeEffHisto( h_path.str()+"/ MCParticle -> "+scorname+" Versus Ptot",
                                corTally.effVp(), mcTally.effVp() );
                  makeEffHisto( h_path.str()+"/ MCParticle -> "+scorname+" Versus Pt",
                                corTally.effVpt(), mcTally.effVpt() );
                  makeEffHisto( h_path.str()+"/ MCParticle -> "+scorname+" Versus P&Pt",
                                corTally.effVpVpt(), mcTally.effVpVpt() );
                }
              }

              // sub contributions
              int suppressedContribsC(0);
              for ( MCTally::Contributions::const_iterator iC = tally.all_detailed.begin();
                    iC != tally.all_detailed.end(); ++iC )
              {
                if ( (100.0*(*iC).second)/(*iSum).second.nReco > m_minContrib )
                {
                  std::ostringstream mcTsC;
                  mcTsC << "       -> "<< (*iC).second << " " << (*iC).first;
                  std::string mcTC = mcTsC.str();
                  mcTC.resize(10+m_maxNameLength,' ');
                  const long nBkgTrue =
                    ((m_mcProtoCount[(*iSum).first.protoTESLoc])[(*iSum).first.protoType].trueMCType[(*iMCT).first])[(*iT).first].all_detailed[(*iC).first];
                  const long nTotalMC = (m_rawMCMap[(*iMCT).first])[(*iT).first].all_detailed[(*iC).first];;
                  always() << mcTC
                           << "|" << eff( (*iC).second, (*iSum).second.nReco );
                  if ( primaryPart )
                  {
                    const unsigned long int nClones = tally.clones_detailed[(*iC).first];
                    always() << " | " << eff( (*iC).second, nBkgTrue )
                             << " |"  << eff( (*iC).second-nClones, nTotalMC )
                             << " |"  << eff( nClones, nTotalMC );
                    // correlations
                    if ( !m_correlations.empty() )
                    {
                      const std::string & loc1    = (*iSum).first.protoTESLoc;
                      const std::string & loc2    = m_corProtoMap[loc1];
                      const std::string & corname = ( loc1<loc2 ? loc1+"&"+loc2 : loc2+"&"+loc1 );
                      const long moo = ((m_correlations[corname])[(*iSum).first.protoType].trueMCType[(*iMCT).first])[(*iT).first].all_detailed[(*iC).first];
                      always() << " |" << eff( moo, (*iC).second );
                    }
                  }
                  always() << endreq;
                } else { ++suppressedContribsC; }
              }
              if ( suppressedContribsC>0 )
              {
                always() << "       -> Suppressed " << suppressedContribsC << " contribution(s) below "
                         <<  m_minContrib << " %" << endreq;
              }
            } else { ++suppressedContribs; }
          }
          if ( suppressedContribs>0 )
          {
            always() << "     -> Suppressed " << suppressedContribs << " contribution(s) below "
                     <<  m_minContrib << " %" << endreq;
          }
        }
      }

    }

    always() << lines << endreq;
  }

  always() << LINES << endreq;
}

void ParticleEffPurMoni::makeEffHisto( const std::string title,
                                       const EffVersusMomentum & top,
                                       const EffVersusMomentum & bot ) const
{
  AIDA::IProfile1D * h(NULL);
  // Loop over bins
  debug() << "Filling histo " << title << endreq;
  for ( unsigned int bin = 0; bin < top.nBins(); ++bin )
  {
    debug() << " -> bin " << bin << " : "
            << top.data()[bin] << " / " << bot.data()[bin] << endreq;
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
    histo->SetErrorOption("g");
  }
}

void ParticleEffPurMoni::makeEffHisto( const std::string title,
                                       const EffVersusMomentum2D & top,
                                       const EffVersusMomentum2D & bot ) const
{
  AIDA::IProfile2D * h(NULL);
  // Loop over bins
  debug() << "Filling histo " << title << endreq;
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
    histo->SetErrorOption("g");
  }
}

// Access the charged ProtoParticle Linker
ProtoParticle2MCLinker *
ParticleEffPurMoni::chargedProtoLinker(const LHCb::ProtoParticle * proto) const
{
  if (!proto) return NULL;
  const std::string loc = objectLocation(proto->parent());
  ProtoParticle2MCLinker *& linker = m_chargedProtoLinker[loc];
  if ( !linker )
  {
    linker =
      new ProtoParticle2MCLinker( this,
                                  Particle2MCMethod::ChargedPP,
                                  std::vector<std::string>(1,loc) );
  }
  return linker;
}

// Access the neutral ProtoParticle Linker
ProtoParticle2MCLinker *
ParticleEffPurMoni::neutralProtoLinker(const LHCb::ProtoParticle * proto) const
{
  if (!proto) return NULL;
  const std::string loc = objectLocation(proto->parent());
  ProtoParticle2MCLinker *& linker = m_neutralProtoLinker[loc];
  if ( !linker )
  {
    linker =
      new ProtoParticle2MCLinker( this,
                                  Particle2MCMethod::NeutralPP,
                                  std::vector<std::string>(1,loc) );
  }
  return linker;
}

// Access the Particle Linker appropriate for the given Particle
Particle2MCLinker *
ParticleEffPurMoni::particleLinker( const LHCb::Particle * /* part */ ) const
{
  if ( !m_particleLinker )
  { m_particleLinker =
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
  for ( ProtoLinkerTESMap::iterator iLC = m_chargedProtoLinker.begin();
        iLC != m_chargedProtoLinker.end(); ++iLC )
  { delete iLC->second; iLC->second = NULL; }
  for ( ProtoLinkerTESMap::iterator iLN = m_neutralProtoLinker.begin();
        iLN != m_neutralProtoLinker.end(); ++iLN )
  { delete iLN->second; iLN->second = NULL; }
  // finalize base class
  return DVAlgorithm::finalize();
}

//=============================================================================

