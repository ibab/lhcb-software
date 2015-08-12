// Include files

#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/Track.h"
#include "Event/MCTrackInfo.h"

// local
#include "PatKShortChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatKShortChecker
//
// 2002-11-23 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatKShortChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatKShortChecker::PatKShortChecker( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_inputLocation    ( LHCb::TrackLocation::Default )
  , m_trToMCP(NULL)
  , m_seedToMCP(NULL)
  , m_downToMCP(NULL)
{
  declareProperty( "InputLocation"  , m_inputLocation );
}

//=============================================================================
// Destructor
//=============================================================================
PatKShortChecker::~PatKShortChecker() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PatKShortChecker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_trToMCP   = new TrAsct( evtSvc(), m_inputLocation);
  m_seedToMCP = new TrAsct( evtSvc(), LHCb::TrackLocation::Seed );
  m_downToMCP = new TrAsct( evtSvc(), LHCb::TrackLocation::Downstream );

  unsigned int kk;
  for ( kk=0 ; 20 > kk ; kk++ ) m_counter.push_back(0);

  for ( kk=0 ; 10>kk ; kk++ ) {
    m_cntSeed.push_back( 0. );
    m_cntDown.push_back( 0. );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatKShortChecker::execute() {

  debug() << "==> Execute" << endmsg;

  if ( !exist<LHCb::Tracks>( m_inputLocation ) ) return StatusCode::SUCCESS;
  
  LHCb::Tracks* tracks  = get<LHCb::Tracks>( m_inputLocation );
  LHCb::Tracks* seeds   = get<LHCb::Tracks>( LHCb::TrackLocation::Seed    );
  LHCb::Tracks* downs   = get<LHCb::Tracks>( LHCb::TrackLocation::Downstream );

  LHCb::MCParticles* parts  = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  TrAsct::DirectType* trTable   =  m_trToMCP->direct();
  TrAsct::DirectType* seedTable =  m_seedToMCP->direct();
  TrAsct::DirectType* downTable =  m_downToMCP->direct();

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );

  LHCb::MCParticles::const_iterator itP;
  LHCb::Tracks::const_iterator itT;
  SmartRefVector<LHCb::MCVertex>::iterator itV;
  SmartRefVector<LHCb::MCParticle>::iterator itPr;
  std::vector<LHCb::MCParticle*>::iterator itPi;

  if ( 0 != seedTable ) {
    debug() << "Start counting seeds" << endmsg;

    for ( itT = seeds->begin(); seeds->end() != itT; itT++  ) {
      TrAsct::DirectType::Range range = seedTable->relations( *itT );
      TrAsct::DirectType::iterator it;
      if ( range.empty() ) {
        m_cntSeed[0] += 1.;
        verbose() << "No truth for track " << (*itT)->key() << endmsg;
      } else {
        m_cntSeed[1] += 1.;
        verbose() << "Truth for track " << (*itT)->key() << " : ";
        for ( it = range.begin(); range.end() != it; it++ ) {
          verbose() << it->to()->key() << " ";
          
          bool has_TT = trackInfo.hasTT( it->to() );
          if ( has_TT ) {
            m_cntSeed[2] += 1.;
            bool KChild = isKChild( it->to() );
            if ( KChild ) {
              m_cntSeed[4] += 1.;
            }
            if ( 5.*Gaudi::Units::GeV < it->to()->p() ) {
              m_cntSeed[3] += 1.;
              if ( KChild ) m_cntSeed[5] += 1.;
            }
          }
        }
        verbose() << endmsg;
      }
    }
  }

  if ( 0 != downTable ) {
    debug() << "Start counting Downstream " << endmsg;
    
    for ( itT = downs->begin(); downs->end() != itT; itT++  ) {
      TrAsct::DirectType::Range range = downTable->relations( *itT );
      TrAsct::DirectType::iterator it;
      if ( range.empty() ) {
        m_cntDown[0] += 1.;
      } else {
        m_cntDown[1] += 1.;
        for ( it = range.begin(); range.end() != it; it++ ) {
          bool has_TT = trackInfo.hasTT( it->to() );
          if ( has_TT ) {
            m_cntDown[2] += 1.;
            bool KChild = isKChild( it->to() );
            if ( KChild ) {
              m_cntDown[4] += 1.;
            }
            if ( 5.*Gaudi::Units::GeV < it->to()->p() ) {
              m_cntDown[3] += 1.;
              if ( KChild ) m_cntDown[5] += 1.;
            }
          }
        }
      }
    }
  }
  
  //== Count the topology of true KShorts

  debug() << "Start counting true KShorts " << endmsg;

  m_counter[0]++;

  for ( itP = parts->begin(); parts->end() != itP; itP++  ) {
    LHCb::MCParticle* part = *itP;

    if ( part->particleID().pid() != 310 ) continue;
    const LHCb::MCVertex* vert   = part->originVertex();
    if ( 0 == vert ) continue;
    const LHCb::MCParticle* mother = vert->mother();
    if ( 0 == mother ) continue;
    if ( !(mother->particleID().hasBottom() && ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() ))) continue;

    m_counter[1]++;

    std::vector<LHCb::MCParticle*> children;

    SmartRefVector<LHCb::MCVertex> endVert = part->endVertices();

    unsigned int nWithVelo = 0;

    for ( itV = endVert.begin(); endVert.end() != itV; itV++ ) {
      SmartRefVector<LHCb::MCParticle> products = (*itV)->products();
      for ( itPr = products.begin(); products.end() != itPr; itPr++ ) {
        LHCb::MCParticle* decay = *itPr;
        if ( !trackInfo.hasT( decay ) ) continue;
        if ( !trackInfo.hasTT( decay ) ) continue;
        if ( trackInfo.hasVelo( decay ) ) nWithVelo++;
        children.push_back( decay );
      }
    }

    if ( 2 == children.size() ) {
      m_counter[2]++;
      m_counter[12+nWithVelo]++;

      debug() << "== KShort from B == N with velo : " << nWithVelo << endmsg;
      unsigned int nbReco = 0;
      unsigned int nbLong = 0;
      unsigned int nbDown = 0;
      unsigned int nbSeed = 0;
      for ( itPi = children.begin(); children.end() != itPi; itPi++ ) {
       
        bool found = false;
        bool longTrack = false;
        bool downstream = false;
        bool seed = false;
        if ( 0 != trTable ) {
          for ( itT = tracks->begin(); tracks->end() != itT; itT++ ) {
            TrAsct::DirectType::Range range = trTable->relations( *itT );
            TrAsct::DirectType::iterator it;
            for ( it = range.begin(); range.end() != it; it++ ) {
              if ( *itPi == it->to() ) {
                found = true;
                if ( LHCb::Track::Long       == (*itT)->type() ) longTrack  = true;
                if ( LHCb::Track::Downstream == (*itT)->type() ) downstream = true;
                if ( LHCb::Track::Ttrack     == (*itT)->type() ) seed       = true;
                
                debug() << "  -- child found as " << (*itT)->key()
                        << " type " << trackType( (*itT) )
                        << endmsg;
              }
            }
          }
        }
        if ( found ) {
          nbReco++;
          if ( longTrack ) {
            nbLong++;
          } else if ( downstream ) {
            nbDown++;
          } else if ( seed ) {
            nbSeed++;
          }
        }
      }
      if ( children.size() == nbReco ) {
        m_counter[3]++;
        if ( 2 == nbReco ) {
          m_counter[4]++;
          if ( 2 == nbLong  )                          { m_counter[5]++;
          } else if ( 1 == nbLong    && 1 == nbDown )  { m_counter[6]++;
          } else if ( 2 == nbDown )                    { m_counter[7]++;
          } else if ( 1 == nbSeed    && 1 == nbLong )  { m_counter[8]++;
          } else if ( 1 == nbSeed    && 1 == nbDown )  { m_counter[9]++;
          } else if ( 2 == nbSeed     )                { m_counter[10]++;
          } else                                       { m_counter[11]++;
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatKShortChecker::finalize() {

  if ( 0 != m_counter[0] ) {
    info() << format( "Nb events          %6d", m_counter[0] ) << endmsg;
    double frac = m_counter[1] / (double)m_counter[0];
    info() << format( "Nb KShort from B   %6d %6.2f per event",
                      m_counter[1], frac ) << endmsg;
    frac = m_counter[2] / (double)m_counter[0];
    info() << format( "Nb reconstructible %6d %6.2f per event",
                      m_counter[2], frac ) << endmsg;
    if ( 0 != m_counter[2] ) {
      double den = 100. / m_counter[2];

      frac = m_counter[12] * den;
      info() << format( "   with 0 Velo    %6d %6.2f %%",  m_counter[12], frac ) << endmsg;
      frac = m_counter[13] * den;
      info() << format( "   with 1 Velo    %6d %6.2f %%",  m_counter[13], frac ) << endmsg;
      frac = m_counter[14] * den;
      info() << format( "   with 2 Velo    %6d %6.2f %%",  m_counter[14], frac ) << endmsg;

      frac = m_counter[3] / (double)m_counter[2];
      info() << format( "Nb with tracks    %6d %6.2f per reconstructible", m_counter[3], frac ) << endmsg;
      frac = m_counter[4] / (double)m_counter[2];
      info() << format( "Nb with 2 tracks  %6d %6.2f per reconstructible", m_counter[4], frac ) << endmsg;
    }
    if ( 0 != m_counter[4] ) {
      double den = 100. / m_counter[4];

      info() << format( "  2 Long          %6d %6.2f %% of 2-tracks",
                        m_counter[5], m_counter[5] * den ) << endmsg;
      info() << format( "  1 Long  1 Dwnst %6d %6.2f %% of 2-tracks",
                        m_counter[6], m_counter[6] * den ) << endmsg;
      info() << format( "  2 downstream    %6d %6.2f %% of 2-tracks",
                        m_counter[7], m_counter[7] * den ) << endmsg;
      info() << format( "  1 Long 1 Seed   %6d %6.2f %% of 2-tracks",
                        m_counter[8], m_counter[8] * den ) << endmsg;
      info() << format( "  1 Dwnst 1 Seed  %6d %6.2f %% of 2-tracks",
                        m_counter[9], m_counter[9] * den ) << endmsg;
      info() << format( "  2 Seed          %6d %6.2f %% of 2-tracks",
                        m_counter[10], m_counter[10] * den ) << endmsg;
      info() << format( "  Other...        %6d %6.2f %% of 2-tracks",
                        m_counter[11], m_counter[11] * den ) << endmsg;
    }
  }

  if ( 0 < m_cntSeed[1] ) {
    info() << "                     "
           << "Ghosts      Found          WithTT        >5GeV "
           << "   TT+KChild    TT+K+>5GeV"
           << endmsg;
    double frac = 100. * m_cntSeed[0] / (m_cntSeed[0] + m_cntSeed[1]);
    info() << format( "Seed           %7.0f %5.1f%7.0f      ",
                      m_cntSeed[0], frac, m_cntSeed[1] );

    double mult = 100. /  m_cntSeed[1];
    info() << format( "%7.0f %5.1f", m_cntSeed[2], mult * m_cntSeed[2]);
    info() << format( "%7.0f %5.1f", m_cntSeed[3], mult * m_cntSeed[3]);
    info() << format( "%7.0f %5.1f", m_cntSeed[4], mult * m_cntSeed[4]);
    info() << format( "%7.0f %5.1f", m_cntSeed[5], mult * m_cntSeed[5]);
    info() << endmsg;

    if ( 0 < m_cntDown[1] ) {
      // frac = 100. * m_cntDown[0] / (m_cntDown[0] + m_cntDown[1]);
      //info() << format( "Downstream   %7.0f %5.1f %7.0f",
      //               m_cntDown[0], frac, m_cntDown[1] );

      //     double mult = 100. /  m_cntDown[1];
      //info() << format( "%7.0f %5.1f", m_cntDown[2], mult * m_cntDown[2]);
      //info() << format( "%7.0f %5.1f", m_cntDown[3], mult * m_cntDown[3]);
      //info() << format( "%7.0f %5.1f", m_cntDown[4], mult * m_cntDown[4]);
      //info() << format( "%7.0f %5.1f", m_cntDown[5], mult * m_cntDown[5]);
      //info() << endmsg;

      //=== Ratios, Downstream efficiency...

      info() << format ( "Downstream/Seed%7.0f      ", m_cntDown[0] );
      for ( unsigned int kk = 1; 6>kk; kk++ ){
        if ( 0 < m_cntSeed[kk] ) {
          frac = 100. * m_cntDown[kk] / m_cntSeed[kk];
        } else {
          frac = 0.;
        }
        info() << format( "%7.0f %5.1f", m_cntDown[kk], frac );
      }
      info() << endmsg;
    }
  }

  delete m_trToMCP;
  delete m_seedToMCP;
  delete m_downToMCP;

  return GaudiAlgorithm::finalize();
}

//=========================================================================
// returns a string with the type of the track
//=========================================================================
std::string PatKShortChecker::trackType ( const LHCb::Track* tr ) {
  std::string result = "";

  if ( !tr->checkFlag( LHCb::Track::Clone) ) {
    result = "unique ";
  }

  if ( LHCb::Track::Velo       == tr->type() ) result += "Velo ";
  if ( LHCb::Track::Long       == tr->type() ) result += "Long ";
  if ( LHCb::Track::Ttrack     == tr->type() ) result += "TTrack ";
  if ( LHCb::Track::Upstream   == tr->type() ) result += "Upstream ";
  if ( LHCb::Track::Downstream == tr->type() ) result += "Downstream ";

  return result;
}

//=========================================================================
//  Return if there is a K0S in the motherhood
//=========================================================================
bool PatKShortChecker::isKChild ( const LHCb::MCParticle* part ) {
  const LHCb::MCParticle* mother = part;
  const LHCb::MCVertex*   vert   = mother->originVertex();
  if ( 0 != vert ) {
    mother = vert->mother();
    if ( 0 != mother ) {
      if ( mother->particleID().pid() == 310 ) return true;
    }
  }
  return false;
}
//=============================================================================
