// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "PrCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrCounter
//
// 2005-06-02 : Olivier Callot
// Modified by Wenbin Qian for VeloPix Efficiency
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PrCounter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrCounter::PrCounter( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<PrCounter>(this);
  m_link        = NULL;
  m_totTrack    = 0;
  m_totGhost    = 0;
  m_selectId    = 0;
  m_fracGhost   = 0.;
  m_nEvent      = 0.;
  declareProperty( "TitleSize", m_titleSize = 30 );
}
//=============================================================================
// Destructor
//=============================================================================
PrCounter::~PrCounter() {}

//=========================================================================
//  Finalize, delete the linker tool
//=========================================================================
StatusCode PrCounter::finalize ( ) {
  delete m_link;
  return GaudiTool::finalize();
}

//=========================================================================
//  Add a selection for the container monitoring
//=========================================================================
void PrCounter::addSelection ( std::string name ) {
  if ( name.size() > m_titleSize ) {
    name = name.substr( m_titleSize );
  } else {
    std::string blank ( m_titleSize-name.size(), ' ');
    name = blank + name;
  }
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
void PrCounter::initEvent ( ) {
  m_validData = false;
  if ( !exist<LHCb::Tracks>( m_container ) ) {
    debug() << "Track container '" << m_container << "' does not exist" <<endmsg;
    return;
  }
  if ( NULL == m_link ) m_link = new MyAsct( evtSvc(), m_container );
  m_nbGhost = 0;

  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_container );
  m_nbTrack = tracks->size();
  const Table* table = m_link->direct();
  if ( NULL == table ) { 
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
    }
    m_totTrack++;
    nbTracks++;
  }
  m_totGhost += m_nbGhost;
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
int PrCounter::count( const LHCb::MCParticle* part, std::vector<bool> flags,
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
    for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin(); ids.end() != itId; ++itId ) {
      if ( (*itId).isVeloPix() || (*itId).isVL() || (*itId).isVelo() ) {
        if ( 0 != (m_selectId & 3) ) nTrue += 1.;
      } else if ( (*itId).isTT() ) {
        if ( 0 != (m_selectId & 4) ) nTrue += 1.;
      } else if ( (*itId).isOT() || (*itId).isIT() || (*itId).isFT() ) {
        if ( 0 != (m_selectId & 8) ) nTrue += 1.;
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
            if ( std::find( ids.begin(), ids.end(), *itId ) == ids.end() ) continue;
            if ( (*itId).isVeloPix() || (*itId).isVL() || (*itId).isVelo() ) {
              if ( 0 != (m_selectId & 3) ) nbMeas += 1;
            } else if ( (*itId).isOT() || (*itId).isIT() || (*itId).isFT() ) {
              if ( 0 != (m_selectId & 8) ) nbMeas += 1;
            } else if ( (*itId).isTT() ) {
              if ( 0 != (m_selectId & 4) ) nbMeas += 1;
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
void PrCounter::printStatistics ( ) {
  if ( 0 == m_nEvent ) return;

  double totT = m_totTrack + 0.00000000001;
  double frac = 100. * double( m_totGhost ) / totT;
  std::string title( m_container);
  while( title.find("/") < title.size() ) {
    title = title.substr( title.find("/")+1 );
  }
  title.resize( m_titleSize, ' ' );
  info() << "**** " << title
         << format( "%7d tracks including       %7d ghosts [%4.1f %%], Event average %5.1f %% ****",
                    m_totTrack, m_totGhost, frac, 100. * m_fracGhost / m_nEvent ) << endmsg;

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
    info() << format( ", purity:%6.2f %%, hitEff:%6.2f %%", purity, hitEff ) << endmsg;
  }
  info() << endmsg;
}

//=============================================================================
