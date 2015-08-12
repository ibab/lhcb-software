// Include files

// local
#include "TrackAssociator.h"

// from LHCb
#include "Event/Track.h"
#include "Linker/LinkerWithKey.h"
#include "Event/MCParticle.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"
#include "Event/MuonCoord.h"
#include "Event/MCVertex.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackAssociator )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackAssociator::TrackAssociator( const std::string& name,
                                  ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm( name, pSvcLocator ),
  m_debugLevel(false),
  m_fractionOK(0.),
  m_nTotVelo(0.),
  m_nTotTT1(0.),
  m_nTotSeed(0.),
  m_nTotMuon(0.),
  m_muonLink(0,0,"")
{
  declareProperty( "TracksInContainer",
                   m_tracksInContainer = TrackLocation::Default );
  declareProperty( "LinkerOutTable", m_linkerOutTable = "" );
  declareProperty( "FractionOK"    , m_fractionOK = 0.70 );
  declareProperty( "DecideUsingMuons", m_decideUsingMuons = false );
  declareProperty( "UseUT",          m_useUT = false );
}

//=============================================================================
// Destructor
//=============================================================================
TrackAssociator::~TrackAssociator() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode TrackAssociator::initialize() {

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) { return sc; }

  // Set the path for the linker table Track - MCParticle
  if ( m_linkerOutTable == "" ) m_linkerOutTable = m_tracksInContainer;

  m_debugLevel = msgLevel( MSG::DEBUG );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackAssociator::execute() {

  // Retrieve the Tracks
  LHCb::Track::Range tracks = getIfExists<LHCb::Track::Range> ( m_tracksInContainer );
  
  // Retrieve the MCParticles
  MCParticles* mcParts = get<MCParticles> ( MCParticleLocation::Default );

  // Create the Linker table from Track to MCParticle
  // Linker table is stored in "Link/" + m_linkerOutTable
  // Sorted by decreasing weight, so first retrieved has highest weight
  LinkerWithKey<MCParticle,Track>
    myLinker( evtSvc(), msgSvc(), m_linkerOutTable );
  myLinker.reset();  //== reset it, in case we work from a DST where associations are already there.

  // Get the linker table VeloCluster => MCParticle
  LinkedTo<MCParticle,VeloCluster>
    veloLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );
  if( veloLink.notFound() ) {
    error() << "Unable to retrieve VeloCluster to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }

  // Get the linker table TTCluster => MCParticle
  LinkedTo<MCParticle,STCluster>
    ttLink( evtSvc(), msgSvc(), m_useUT ? LHCb::STClusterLocation::UTClusters : LHCb::STClusterLocation::TTClusters);
  if( ttLink.notFound() ) {
    error() << "Unable to retrieve "
            << (m_useUT ? "UTCluster" : "TTCluster")
            <<" to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }

  // Get the linker table ITCluster => MCParticle
  LinkedTo<MCParticle,STCluster>
    itLink(evtSvc(),msgSvc(),LHCb::STClusterLocation::ITClusters);
  if( itLink.notFound() ) {
    error() << "Unable to retrieve ITCluster to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }

  // Get the linker table OTCluster => MCParticle
  LinkedTo<LHCb::MCParticle>
    otLink(evtSvc(),msgSvc(),LHCb::OTTimeLocation::Default);
  if( otLink.notFound() ) {
    error() << "Unable to retrieve OTCluster to MCParticle linker table."
            << endreq;
    return StatusCode::FAILURE;
  }

  // Get the linker table MuonCoord => MCParticle
  if(m_decideUsingMuons) {
    m_muonLink = MuonLink(evtSvc(),msgSvc(),LHCb::MuonCoordLocation::MuonCoords);
    if( m_muonLink.notFound() ) {
      error() << "Unable to retrieve MuonCoord to MCParticle linker table."
              << endreq;
      return StatusCode::FAILURE;
    }
  }

  // Loop over the Tracks
  LHCb::Track::Range::const_iterator it;
  for( it = tracks.begin(); tracks.end() != it; ++it ) {
    const Track* tr = *it;
    m_nTotVelo = 0.;
    m_nTotTT1  = 0.;
    m_nTotSeed = 0.;
    m_nTotMuon = 0.;
    m_parts.clear();
    m_nVelo.clear();
    m_nTT1.clear();
    m_nSeed.clear();
    m_nMuon.clear();

    // Loop over the LHCbIDs of the Track
    static int nMeas;
    nMeas = 0;
    for( std::vector<LHCbID>::const_iterator iId = tr->lhcbIDs().begin();
         tr->lhcbIDs().end() != iId; ++iId )
    {
      if( (*iId).isVelo() ) {
        ++nMeas;
        VeloChannelID vID = (*iId).veloID();
        // Count number of Velo hits
        m_nTotVelo += 1.;
        MCParticle* mcParticle = veloLink.first( vID );
        if( m_debugLevel && 0 == mcParticle ) {
          debug() << "No MCParticle linked with VeloCluster " << vID << endreq;
        }
        while( 0 != mcParticle ) {
          if( mcParts != mcParticle->parent() ) {
            debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
          }
          else { countMCPart( mcParticle, 1., 0., 0., 0. ); }
          mcParticle = veloLink.next();
        }
        continue;
      }
      if( (*iId).isTT() || (*iId).isUT() ) {
        ++nMeas;
        m_nTotTT1 += 1.;
        STChannelID ttID = (*iId).stID();
        MCParticle* mcParticle = ttLink.first( ttID );
        if( m_debugLevel && 0 == mcParticle ) {
          if ( m_useUT ) debug() << "No MCParticle linked with UTCluster " << ttID << endreq;
          else           debug() << "No MCParticle linked with TTCluster " << ttID << endreq;
        }
        while( 0 != mcParticle ) {
          if( mcParts != mcParticle->parent() ) {
            debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
          }
          else { countMCPart( mcParticle, 0., 1., 0., 0. ); }
          mcParticle = ttLink.next();
        }
        continue;
      }
      if( (*iId).isIT() ) {
        ++nMeas;
        // Count number of IT+OT hits
        m_nTotSeed += 1.;
        STChannelID itID = (*iId).stID();
        MCParticle* mcParticle = itLink.first( itID );
        if( m_debugLevel && 0 == mcParticle ) {
          debug() << "No MCParticle linked with ITCluster " << itID << endreq;
        }
        while( 0 != mcParticle ) {
          if( mcParts != mcParticle->parent() ) {
            debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
          }
          else { countMCPart( mcParticle, 0., 0., 1., 0. ); }
          mcParticle = itLink.next();
        }
        continue;
      }
      if( (*iId).isOT() ) {
        ++nMeas;
        // Count number of IT+OT hits
        m_nTotSeed += 1.;
        OTChannelID otID = (*iId).otID();
        MCParticle* mcParticle = otLink.first( otID );
        if( m_debugLevel && 0 == mcParticle ) {
          debug() << "No MCParticle linked with OTTime " << otID << endreq;
        }
        while( 0 != mcParticle ) {
          if( mcParts != mcParticle->parent() ) {
            debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
          }
          else { countMCPart( mcParticle, 0., 0., 1., 0. ); }
          mcParticle = otLink.next();
        }
      }
      if(m_decideUsingMuons) {
        if( (*iId).isMuon() ) {
          ++nMeas;
          // Count number of Muon hits
          m_nTotMuon += 1.;
          MuonTileID muonID = (*iId).muonID();
          MCParticle* mcParticle = m_muonLink.first( muonID );
          if( m_debugLevel && 0 == mcParticle ) {
            debug() << "No MCParticle linked with MuonCoord " << muonID << endreq;
          }
          while( 0 != mcParticle ) {
            if( mcParts != mcParticle->parent() ) {
              debug() << " (other BX " <<  mcParticle->key() << ")" << endreq;
            }
            else { countMCPart( mcParticle, 0., 0., 0., 1. ); }
            mcParticle = m_muonLink.next();
          }
        }
      }
    }

    //== Handling of decays between Velo and T: Associate the hits also to the mother

    for ( unsigned int j1 = 0; m_parts.size() > j1; ++j1 ) {
      const MCVertex* vOrigin = m_parts[j1]->originVertex();
      if ( 0 == vOrigin ) continue;
      const MCParticle* mother = vOrigin->mother();
      if( 0 == mother ) continue;
      if ( vOrigin->type() != MCVertex::DecayVertex ) continue;
      for( unsigned int j2 = 0; m_parts.size() > j2; ++j2 ) {
        if ( mother == m_parts[j2] ) {
          debug() << "  *** Particle " << m_parts[j1]->key()
                  << "[" << m_parts[j1]->particleID().pid()
                  << "] (" << m_nVelo[j1] << "," << m_nTT1[j1] << "," << m_nSeed[j1] << ")"
                  << " is daughter at z=" << vOrigin->position().z()
                  << " of " << m_parts[j2]->key()
                  << "[" << m_parts[j2]->particleID().pid()
                  << "] (" << m_nVelo[j2] << "," << m_nTT1[j2] << "," << m_nSeed[j2] << ")"
                  << ". Merge hits to tag both." << endreq;
          
          //== Daughter hits are added to mother.
          m_nVelo[j2] += m_nVelo[j1];
          m_nTT1 [j2] += m_nTT1 [j1];
          m_nSeed[j2] += m_nSeed[j1];
          
          //== Mother hits overwrite Daughter hits
          m_nVelo[j1] = m_nVelo[j2];
          m_nTT1 [j1] = m_nTT1 [j2];
          m_nSeed[j1] = m_nSeed[j2];
        }
      }     
    }

    // Add parent muon hits to daughter MCParticle
    if(m_decideUsingMuons) {
      unsigned int j1, j2;
      for( j1 = 0; m_parts.size() > j1; ++j1 ) {
        if( m_nMuon[j1] < m_fractionOK * m_nTotMuon ) { continue; }
        const MCVertex* vOrigin = m_parts[j1]->originVertex();
        while( 0 != vOrigin ) {
          const MCParticle* mother = vOrigin->mother();
          if( 0 == mother ) break;
          for( j2 = 0; m_parts.size() > j2; ++j2 ) {
            if( m_nSeed[j2] < m_fractionOK * m_nTotSeed ) { continue; }
            if( mother == m_parts[j2] ) {
              m_nMuon[j2] += m_nMuon[j1];
              m_nMuon[j1] = m_nMuon[j2];
            }
          }
          vOrigin = mother->originVertex();
        }
      }
    }

    // For each MCParticle with a Measurement associated,
    //  Relate the Track to the MCParticles matching the below criteria,
    //  using as weight the ratio of ( total # Measurements associated
    //                                 to this MCParticle )
    //                           and ( total # Measurements )
    //   if total # Velo hits > 2,
    //   or if 0.70 <= ( # Velo hits associated to the MCParticle /
    //                   total # Velo hits )
    //  AND
    //   if total # IT+OT hits > 2,
    //   or if 0.70 <= ( # IT+OT hits associated to the MCParticle /
    //                   total # IT+OT hits )
    //  AND
    //   if # associated TT hits > ( total # TT hits - 2 ) ??
    //   or if total # Velo hits > 2 AND total # IT+OT hits > 2
    // When taking Muon hits into account, also:
    //  AND
    //   if 0.70 <= ( # Muon hits associated to the MCParticle /
    //                total # Muon hits )
    double ratio;
    for( unsigned int jj = 0; m_parts.size() > jj; ++jj ) {
      bool veloOK = true;
      if( 2 < m_nTotVelo ) {
        veloOK = false;
        ratio = m_nVelo[jj] / m_nTotVelo;
        if( m_fractionOK <= ratio ) { veloOK = true; }
      }
      bool seedOK = true;
      if( 2 < m_nTotSeed ) {
        seedOK = false;
        ratio =  m_nSeed[jj] / m_nTotSeed;
        if( m_fractionOK <= ratio ) { seedOK = true; }
      }
      bool TT1OK = m_nTT1[jj] > m_nTotTT1 - 2;
      if( 2 < m_nTotVelo && 2 < m_nTotSeed ) { TT1OK = true; }
      bool muonOK = true;
      if(m_decideUsingMuons) {
        ratio = m_nMuon[jj] / m_nTotMuon;
        if( m_fractionOK > ratio ) { muonOK = false; }
      }

      verbose() << " MC " << m_parts[jj]->key()
                << " Velo " <<  m_nVelo[jj] << "/" << m_nTotVelo
                << " TT1 " <<  m_nTT1[jj] << "/" << m_nTotTT1
                << " Seed " <<  m_nSeed[jj] << "/" << m_nTotSeed
                << endreq;

      //=== Decision. Use TT1. Fill Linker
      if( veloOK && seedOK && TT1OK && muonOK ) {
        double muons = 0.;
        if(m_decideUsingMuons) { muons = m_nMuon[jj]; }
        ratio = ( m_nVelo[jj] + m_nSeed[jj] + m_nTT1[jj] + muons ) / nMeas;
        myLinker.link( tr, m_parts[jj], ratio );
      }
    }
  } // End loop over Tracks

  return StatusCode::SUCCESS;
}

//=============================================================================
// Adjust the counters for this MCParticle, create one if needed
//=============================================================================
void TrackAssociator::countMCPart( const MCParticle* part,
                                   double incVelo,
                                   double incTT1,
                                   double incSeed,
                                   double incMuon ) {
  bool found = false;
  // Loop over the MCParticles vector to see whether part is already in m_parts
  for( unsigned int jj = 0; m_parts.size() > jj; ++jj ) {
    if( m_parts[jj] == part ) {
      m_nVelo[jj] += incVelo;
      m_nTT1[jj] += incTT1;
      m_nSeed[jj] += incSeed;
      m_nMuon[jj] += incMuon;
      // Prevent next if loop to run when this one already did the work
      found = true;
      // Stop looping when successful
      break;
    }
  }
  // If part an element of m_parts if it was not already and update counters
  if( !found ) {
    m_parts.push_back( part );
    m_nVelo.push_back( incVelo );
    m_nTT1.push_back( incTT1 );
    m_nSeed.push_back( incSeed );
    m_nMuon.push_back( incMuon );
  }
}
