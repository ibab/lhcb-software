// Include files 

// local
#include "PatTrack2MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatTrack2MCParticle
//
// 2005-06-01 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatTrack2MCParticle )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatTrack2MCParticle::PatTrack2MCParticle( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "FractionOK" , m_fractionOK     = 0.70 );
  declareProperty( "Locations"  , m_locations        );
  m_locations.push_back( LHCb::TrackLocation::RZVelo );
  m_locations.push_back( LHCb::TrackLocation::Velo );
  m_locations.push_back( LHCb::TrackLocation::Forward );
  m_locations.push_back( LHCb::TrackLocation::Tsa );
  m_locations.push_back( LHCb::TrackLocation::Seed );
  m_locations.push_back( LHCb::TrackLocation::Match );
  m_locations.push_back( LHCb::TrackLocation::Downstream );
  m_locations.push_back( LHCb::TrackLocation::Tsa );
  m_locations.push_back( LHCb::TrackLocation::VeloTT );
  m_locations.push_back( LHCb::TrackLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PatTrack2MCParticle::~PatTrack2MCParticle() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatTrack2MCParticle::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_lhcbLinks = "Pat/LHCbID";

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatTrack2MCParticle::execute() {

  debug() << "==> Execute" << endmsg;

  debug() << "Loading LHCbID Links from " << m_lhcbLinks << endmsg;
  LinkedTo<LHCb::MCParticle> lhcbIdLink( evtSvc(), msgSvc(), m_lhcbLinks );

  //== Process the list of locations

  for ( std::vector<std::string>::iterator itS = m_locations.begin();
        m_locations.end() != itS; ++itS ) {
    const std::string & name =  (*itS);
    if ( !exist<LHCb::Tracks>( name ) ) {
      debug() << "Container " << name << " not found." << endmsg;
      continue;
    }
    LHCb::Tracks* container = get<LHCb::Tracks>( name );
    debug() << "Processing " << container->size() << " Tracks from " << name << endmsg;
    LinkerWithKey<LHCb::MCParticle,LHCb::Track>
      trackLink( evtSvc(), msgSvc(), name );
    for ( LHCb::Tracks::const_iterator itT = container->begin();
          container->end() != itT; ++itT ) {
      associateTrack( *itT, lhcbIdLink, trackLink );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Associate a track
//=========================================================================
void PatTrack2MCParticle::associateTrack ( const LHCb::Track* tr,
                                           LinkedTo<LHCb::MCParticle>& lhcbIdLink,
                                           LinkerWithKey<LHCb::MCParticle, LHCb::Track>& trackLink ) 
{
  const bool isVerbose = msgLevel( MSG::VERBOSE );
  double nTotT    = 0.;
  double nTotTT   = 0.;
  double nTotVelo = 0.;

  std::vector<const LHCb::MCParticle*> parts;
  std::vector<const LHCb::MCParticle*>::iterator found;

  std::vector<double> nVelo;
  std::vector<double> nTT;
  std::vector<double> nT;

  double ratio;
  LHCb::MCParticle* part = NULL;

  const std::vector<LHCb::LHCbID> & ids = tr->lhcbIDs();

  debug() << "++++ MC truth for hits on track " << tr->key()
          << " nHit = " << ids.size() << endmsg;

  if ( tr->checkFlag( LHCb::Track::Invalid ) ) 
  {
    debug() << " -> Track is Invalid -> Abort MC association" << endmsg;
    return;
  }

  std::vector<LHCb::LHCbID>::const_iterator itId;
  for ( itId = ids.begin(); ids.end() != itId  ; ++itId  ) 
  {
    const LHCb::LHCbID & myID = (*itId);

    if ( isVerbose ) {
      verbose() << " -> " << myID << " is associated to MCPart ";
    }
    if ( myID.isVelo() ) {
      nTotVelo += 1.;  // Count each VELO Reference match
      part = lhcbIdLink.first( myID.lhcbID() );
      while ( 0 != part ) {
        if ( isVerbose ) verbose() << " " << part->key();
        found = std::find( parts.begin(), parts.end(), part );
        if ( parts.end() == found ) {
          parts.push_back( part );
          nVelo.push_back( 1. );
          nTT  .push_back( 0. );
          nT   .push_back( 0. );
        } else {
          nVelo[ found -  parts.begin() ] += 1.;
        }
        part = lhcbIdLink.next();
      }
    } else if ( myID.isST() && myID.stID().isTT()  ) {
      nTotTT += 1.;
      part = lhcbIdLink.first( myID.lhcbID() );
      while ( 0 != part ) {
        if ( isVerbose ) verbose() << " " << part->key();
        found = std::find( parts.begin(), parts.end(), part );
        if ( parts.end() == found ) {
          parts.push_back( part );
          nVelo.push_back( 0. );
          nTT  .push_back( 1. );
          nT   .push_back( 0. );
        } else {
          nTT[ found -  parts.begin() ] += 1.;
        }
        part = lhcbIdLink.next();
      }
    } else {
      nTotT += 1.;
      part = lhcbIdLink.first( myID.lhcbID() );
      while ( 0 != part ) {
        if ( isVerbose ) verbose() << " " << part->key();
        found = std::find( parts.begin(), parts.end(), part );
        if ( parts.end() == found ) {
          parts.push_back( part );
          nVelo.push_back( 0. );
          nTT  .push_back( 0. );
          nT   .push_back( 1. );
        } else {
          nT[ found -  parts.begin() ] += 1.;
        }
        part = lhcbIdLink.next();
      }
    }
    if ( isVerbose ) verbose() << endmsg;
  }

  //== For Long match, count also parents hits if in VELO !

  if ( (2 < nTotVelo) && ( 2 < nTotT ) ) {
    unsigned int j1, j2;
    for ( j1 = 0; parts.size() > j1; j1++ ) {
      if ( nT[j1] <  m_fractionOK* nTotT ) continue;

      const LHCb::MCVertex* vOrigin = parts[j1]->originVertex();
      while ( 0 != vOrigin ) {
        const LHCb::MCParticle* mother = vOrigin->mother();
        if ( 0 == mother ) break;
        for ( j2 = 0 ; parts.size() > j2 ; j2++ ) {
          if ( nVelo[j2] < m_fractionOK* nTotVelo ) continue;
          if ( mother == parts[j2] ) {

            debug() << "  *** Particle " << parts[j1]->key()
                    << "[" << parts[j1]->particleID().pid()
                    << "] (" << nVelo[j1] << "," << nTT[j1]
                    << "," << nT[j1] << ")"
                    << " is daughter of " << parts[j2]->key()
                    << "[" << parts[j2]->particleID().pid()
                    << "] (" << nVelo[j2] << "," << nTT[j2]
                    << "," << nT[j2] << ")"
                    << ". Merge hits to tag both." << endmsg;

            //== Daughter hits are added to mother.
            nVelo[j2] = nVelo[j1] + nVelo[j2];
            nTT  [j2] = nTT  [j1] + nTT  [j2];
            nT   [j2] = nT   [j1] + nT   [j2];

            //== Daughter is also fully tagged
            nVelo[j1] = nVelo[j2];
            nTT  [j1] = nTT  [j2];
            nT   [j1] = nT   [j2];
          }
        }
        vOrigin = mother->originVertex();
      }
    }
  }

  //== We have collected the information. Now decide...

  for ( unsigned int jj = 0 ; parts.size() > jj ; ++jj  ) {

    //== Velo part : OK if less than 3 hits, else more than m_fractionOK.
    bool veloOK = true;
    if ( 2 < nTotVelo ) {
      ratio =  nVelo[jj] / nTotVelo;
      veloOK = m_fractionOK <= ratio;
    }

    //== T part : OK if less than 3 hits, else more than fractionOK.
    bool seedOK = true;
    if ( 2 < nTotT ) {
      ratio =  nT[jj] / nTotT;
      seedOK = m_fractionOK <= ratio;
    }

    //== TT : OK if 0 or 1 wrong hits. OK if both Velo and T on track
    bool TTOK = nTT[jj] > nTotTT - 2;
    if (  2 < nTotVelo && 2 < nTotT ) TTOK = true;

    if ( isVerbose ) {
      verbose() << " -> MC " << parts[jj]->key()
                << " Velo " <<  nVelo[jj] << "/" << nTotVelo
                << " TT " <<  nTT[jj] << "/" << nTotTT
                << " T " <<  nT[jj] << "/" << nTotT
                << endmsg;
    }

    //=== Decision. Use TT

    if ( veloOK && seedOK && TTOK) {
      debug() << "  => match MC " << parts[jj]->key() << endmsg;
      ratio = ( nVelo[jj] + nT[jj] + nTT[jj] ) / ( nTotVelo  + nTotT  + nTotTT  );
      trackLink.link( tr, parts[jj], ratio );
    }
  }
}
//=============================================================================
