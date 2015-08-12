// Include files 

// local
#include "PatTTCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatTTCounter
//
// 2006-06-28 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatTTCounter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatTTCounter::PatTTCounter( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPatTTCounter>(this);
  m_link      = NULL;
  m_invTable  = NULL;
  m_nbGhost   = 0.;
  m_nbGhostHit = 0.;
  m_totTrack   = 0;
}
//=============================================================================
// Destructor
//=============================================================================
PatTTCounter::~PatTTCounter() {} 

//=========================================================================
//  
//=========================================================================
StatusCode PatTTCounter::finalize ( ) {
  if ( NULL != m_link ) delete m_link;
  return GaudiTool::finalize();
}

//=========================================================================
//  Add a selection for the container monitoring
//=========================================================================
void PatTTCounter::addSelection ( std::string name ) {
  m_name.push_back( name );
  m_nbTrack.push_back( 0. );
  m_mcHits.push_back( 0. );
  m_foundOK.push_back( 0. );
  m_wrong.push_back( 0. );

  m_nbTrack3.push_back( 0. );
  m_mcHits3.push_back( 0. );
  m_foundOK3.push_back( 0. );
  m_wrong3.push_back( 0. );  
}
//=========================================================================
//  Event initialisation, get the linker table.
//=========================================================================
void PatTTCounter::initEvent ( ) {
  m_validData = false;
  if ( !exist<LHCb::Tracks>( m_container ) ) 
  {
    Warning( "Track container '" + m_container + "' does not exist" );
    return;
  }
  if ( NULL == m_link ) m_link = new TrackAsct( evtSvc(), m_container );
  m_invTable  = m_link->inverse();
  if ( NULL == m_invTable || NULL == m_link->direct() ) 
  { 
    Warning( "Problem with MC associations for " + m_container );
    return; 
  }
  m_validData = true;

  // Process the ghost tracks
  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_container );
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) 
  {
    const LHCb::Track* tr = (*itT);
    if ( m_link->direct()->relations( tr ).empty() ) {
      double nbInTT = 0;
      for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
            tr->lhcbIDs().end() != itId; ++itId ) {
        if ( (*itId).isST() ) {
          if (  (*itId).stID().isTT() ) nbInTT += 1.;
        }
      }
      m_nbGhost += 1.;
      m_nbGhostHit += nbInTT;
    }
    m_totTrack++;
  }
}

//=========================================================================
//  Increment the counter for a given MC track, according to flags.
//=========================================================================
void PatTTCounter::count( const LHCb::MCParticle* part, std::vector<bool> flags, 
                          std::vector<LHCb::LHCbID>& ids ) {
  if ( !m_validData ) return;

  if ( flags.size() > m_name.size() ) {
    Warning("Flag size mismatch").ignore();
    debug() << "... Flag size " << flags.size() << " >  "
              << m_name.size() << " declared selections" << endmsg;
    return;
  }

  InvRange trackList = m_invTable->relations( part );

  std::vector<LHCb::LHCbID> ttIds;
  
  for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin();
          ids.end() != itId; ++itId ) {
    if ( (*itId).isST() ) {
      if ( (*itId).stID().isTT() ) ttIds.push_back( *itId );
    }
  }

  verbose() << "MCPart " << part->key() << " has " << ttIds.size() << " TT measurements "
            <<endmsg;

  for ( unsigned int kk = 0; flags.size() > kk; ++kk ) {
    if ( flags[kk] ) {
      for ( InvIterator it = trackList.begin(); trackList.end() != it; ++it ) {
        const LHCb::Track* tr = it->to();
        unsigned int nbOK = 0;
        unsigned int nbWrong = 0;
        
        for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
              tr->lhcbIDs().end() != itId; ++itId ) {
          if ( (*itId).isST() ) {
            if (  (*itId).stID().isTT() ) {
              LHCb::LHCbID t = (*itId);
              bool found = false;
              for ( std::vector<LHCb::LHCbID>::const_iterator itMc = ttIds.begin();
                    ttIds.end() != itMc; ++itMc ) {
                if ( t == (*itMc) ) found = true;
              }
              if ( found ) {
                nbOK++;
              } else {
                nbWrong++;
              } 
            }
          }
        }
        m_nbTrack[kk] += 1.;
        m_mcHits[kk]  += ttIds.size();
        m_foundOK[kk] += nbOK;
        m_wrong[kk]   += nbWrong;
        if ( 2 < ttIds.size() ) {
          m_nbTrack3[kk] += 1.;
          m_mcHits3[kk]  += ttIds.size();
          m_foundOK3[kk] += nbOK;
          m_wrong3[kk]   += nbWrong;
        }
      }
    }
  }
}

//=========================================================================
//  Print at the end the statistics
//=========================================================================
void PatTTCounter::printStatistics ( ) {
  if ( m_totTrack == 0 ) return;

  info() << "++++ TT Efficiency for " << m_container << " ++++  ";
  if ( 0 != m_nbGhost ) {
    double bad = m_nbGhostHit / m_nbGhost;
    info() << format( "%6.0f ghost, %5.2f TT per track", m_nbGhost, bad );
  }
  info() << endmsg;
  
  for ( unsigned int kk = 0; m_name.size() > kk; ++kk ) {
    if ( 0.5 > m_nbTrack[kk] ) continue;
    double eff = 0.;
    double bad = 0.;
    double meanHits = 0.;
    if ( 0.5 < m_nbTrack[kk] ) {
      meanHits = m_mcHits[kk]  / m_nbTrack[kk];
      eff      = m_foundOK[kk] / m_nbTrack[kk];
      bad      = m_wrong[kk]   / m_nbTrack[kk];
    }

    double eff3 = 0.;
    double bad3 = 0.;
    double meanHits3 = 0.;
    if ( 0.5 < m_nbTrack3[kk] ) {
      meanHits3 = m_mcHits3[kk]  / m_nbTrack3[kk];
      eff3      = m_foundOK3[kk] / m_nbTrack3[kk];
      bad3      = m_wrong3[kk]   / m_nbTrack3[kk];
    }

    info() << "  " << m_name[kk]
           << format( " :%6.0f tr %5.2f mcTT, OK%5.2f wrong%5.2f",
                      m_nbTrack[kk], meanHits, eff, bad);
    info() << format( " %6.0f tr>3TT %5.2f mcTT, OK%5.2f wrong %5.2f",
                      m_nbTrack3[kk], meanHits3, eff3, bad3) << endmsg;
  }
}

//=============================================================================
