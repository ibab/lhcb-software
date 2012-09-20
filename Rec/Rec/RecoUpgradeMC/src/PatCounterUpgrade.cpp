// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "PatCounterUpgrade.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatCounterUpgrade
//
// 2005-06-02 : Olivier Callot
// Modified by Wenbin Qian for VP Efficiency
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PatCounterUpgrade )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatCounterUpgrade::PatCounterUpgrade( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<PatCounterUpgrade>(this);
  m_link     = NULL;
  m_totTrackVlp = 0;
  m_totTrack    = 0;
  m_totGhost    = 0;
  m_totGhostVlp = 0;
  m_selectId = 0;
  m_fracGhost = 0.;
  m_nEvent    = 0.;
}
//=============================================================================
// Destructor
//=============================================================================
PatCounterUpgrade::~PatCounterUpgrade() {}

//=========================================================================
//  Finalize, delete the linker tool
//=========================================================================
StatusCode PatCounterUpgrade::finalize ( ) {
  delete m_link;
  return GaudiTool::finalize();
}

//=========================================================================
//  Add a selection for the container monitoring
//=========================================================================
void PatCounterUpgrade::addSelection ( std::string name ) {
  m_name.push_back( name );
  m_wanted.push_back( 0 );
  m_counted.push_back( 0 );
  m_clone.push_back( 0 );
  m_purity.push_back( 0. );
  m_hitEff.push_back( 0. );
}
//=========================================================================
//  Event initialisation, get the linker table.
//=========================================================================
void PatCounterUpgrade::initEvent ( ) {
  m_validData = false;
  if ( !exist<LHCb::Tracks>( m_container ) ) 
  {
    debug() << "Track container '" << m_container << "' does not exist" <<endmsg;
    return;
  }
  if ( NULL == m_link ) m_link = new MyAsct( evtSvc(), m_container );
  m_nbGhostVlp = 0;
  m_nbGhost = 0;

  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_container );
  m_nbTrack = tracks->size();
  const Table* table = m_link->direct();
  if ( NULL == table ) 
  { 
    Warning( "Problem with MC associations for " + m_container );
    return; 
  }

  double nbTracks = 0;
  for ( LHCb::Tracks::const_iterator itT = tracks->begin();
        tracks->end() != itT; ++itT ) {
    if ( (*itT)->checkFlag( LHCb::Track::Invalid ) ) continue;
    Range range = table->relations( *itT );
    if ( range.empty() ){
      m_nbGhost++;
      if(3==m_selectId){
        if(22==(*itT)->history())m_nbGhostVlp++;
      }
    }
    m_totTrack++;
    nbTracks++;
    if(3==m_selectId){
      if(22==(*itT)->history())m_totTrackVlp++;
    }
  }
  m_totGhost += m_nbGhost;
  if(3==m_selectId){
    m_totGhostVlp += m_nbGhostVlp;
  }
  double fracGhost = 0.;
  if ( 0 < nbTracks ) fracGhost = double(m_nbGhost) / nbTracks;
  m_fracGhost += fracGhost;
  m_nEvent    += 1.;

  m_invTable = m_link->inverse();
  m_validData = true;
}

//=========================================================================
//  Increment the counter for a given MC track, according to flags.
//=========================================================================
int PatCounterUpgrade::count( const LHCb::MCParticle* part, std::vector<bool> flags,
                       std::vector<LHCb::LHCbID>& ids) {


  if ( !m_validData ) return -1;

  if ( flags.size() > m_name.size() ) {
    Warning("Flag size mismatch").ignore();
    debug() << "... Flag size " << flags.size() << " >  "
              << m_name.size() << " declared selections" << endmsg;
    return -1;
  }

  InvRange trackList = m_invTable->relations( part );
  bool found = false;
  int clone = 0;

  if ( trackList.size() != 0 ) {
    found = true;
    clone = trackList.size() - 1;
  }
  
  //== Count how many of the proper type...
  double nTrue = 0.;

  if ( 0 == m_selectId ) {
    nTrue = double( ids.size() );
  } else {
    for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin();
          ids.end() != itId; ++itId ) {
      if ( (*itId).isVP() ) {
        if ( 0 == (m_selectId & 3 ) ) continue;
        nTrue += 1.;
      } else if ( (*itId).isOT() ) {
        if ( 0 != (m_selectId & 8 ) ) nTrue += 1.;
      } else if ( (*itId).isST() ) {
        if ( (*itId).stID().isTT() ) {
          if ( 0 !=  (m_selectId & 4)  ) nTrue += 1.;
        } else {
          if ( 0 !=  (m_selectId & 8)  ) nTrue += 1.;
        }
      }
    }
  }

  for ( unsigned int kk = 0; flags.size() > kk; ++kk ) {
    if ( flags[kk] ) {
      m_wanted[kk] ++;
      if ( found ) {
        m_counted[kk]++;
        m_clone[kk] += clone;
        for ( InvIterator it = trackList.begin(); trackList.end() != it; ++it ) {
          m_purity[kk] += it->weight();
          const LHCb::Track* tr = it->to();
          unsigned int nbMeas = 0;
          for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
                tr->lhcbIDs().end() != itId; ++itId ) {
            if(clone > 0 && (*itId).isVP()){
//              info()<<part->key()<<" "<<tr->key()<<" "<<(*itId)<<endmsg;
            }
            bool isFromMC = false;
            for ( std::vector<LHCb::LHCbID>::const_iterator itIm = ids.begin();
                  ids.end() != itIm; ++itIm ) {
              if ( (*itIm) == (*itId) ) {
                isFromMC = true;
                break;
              }
            }
            if ( !isFromMC ) continue;
            if ( (*itId).isVP() ) {
              if ( 0 == (m_selectId & 3 ) ) continue;
              nbMeas += 1;
            } else if ( (*itId).isOT() ) {
              if ( 0 != ( (m_selectId & 8) )  ) nbMeas += 1;
            } else if ( (*itId).isST() ) {
              if (  (*itId).stID().isTT() ) {
                if ( 0 != ( (m_selectId & 4) ) ) nbMeas += 1;
              } else {
                if ( 0 != ( (m_selectId & 8) ) ) nbMeas += 1;
              }
            }
          }
          if ( 0 < nTrue ) {
            double eff = double( nbMeas ) / nTrue;
            m_hitEff[kk] += eff;
          }
        }
      }
    }
  }

  if ( 0 == trackList.size() ) return -1;
  
  return trackList.begin()->to()->key();
}

//=========================================================================
//  Print at the end the statistics
//=========================================================================
void PatCounterUpgrade::printStatistics ( ) {
  if ( 0 == m_totTrack ) return;

  double totT = m_totTrack + 0.00000000001;
  double frac = 100. * double( m_totGhost ) / totT;
  info() << "**** " << m_container
         << format( " : %8d tracks including %8d ghosts [%5.1f %%] Event average %5.1f %% ****",
                    m_totTrack, m_totGhost, frac, 100. * m_fracGhost / m_nEvent ) << endreq;

  if(m_totTrackVlp>0) info() << format( "                      %8d by \'VP\' PR %8d ghosts [%5.1f %%]",
                      m_totTrackVlp, m_totGhostVlp, 100.*double(m_totGhostVlp)/double(m_totTrackVlp) ) << endreq;

  for ( unsigned int kk = 0; m_name.size() > kk; ++kk ) {
    if ( 0 == m_wanted[kk] ) continue;
    frac = 100. * double( m_counted[kk] ) / double( m_wanted[kk] );
    double nTot =  double( m_counted[kk] + m_clone[kk] ) + 0.00000001;
    double clo = 100. * double( m_clone[kk] ) / nTot;
    double purity = 100. * m_purity[kk] / nTot;
    double hitEff = 100. * m_hitEff[kk] / nTot;
    info() << "  " << m_name[kk]
           << format( " :%8d from %8d [%5.1f %%] %6d clones [%4.1f %%]",
                      m_counted[kk], m_wanted[kk], frac, m_clone[kk], clo);
    info() << format( ", purity %6.2f %% hitEff %6.2f %%", purity, hitEff ) << endreq;
  }
}

//=============================================================================
