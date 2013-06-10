// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "GaudiAlg/IHistoTool.h"


// local
#include "PrCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrCounter
//
// 2005-06-02 : Olivier Callot
// Modified by Wenbin Qian for VP Efficiency
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
  m_writeHistos = -1;
  m_eta25cut = false;
  m_totTrack    = 0;
  m_totGhost    = 0;
  m_selectId    = 0;
  m_fracGhost   = 0.;
  m_nEvent      = 0.;
  m_trackType   = LHCb::Track::TypeUnknown;
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
void PrCounter::addSelection ( std::string name, bool writeHisto ) {
  if ( name.size() > m_titleSize ) {
    name = name.substr( m_titleSize );
  }
  m_name.push_back( name );
  m_writeHisto.push_back(writeHisto);
  m_wanted.push_back( 0 );
  m_counted.push_back( 0 );
  m_clone.push_back( 0 );
  m_purity.push_back( 0. );
  m_hitEff.push_back( 0. );
}
//=========================================================================
//  Event initialisation, get the linker table.
//=========================================================================
void PrCounter::initEvent(const IHistoTool* htool = NULL, const int nPV = 0){
  m_validData = false;
  LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>( m_container );
  if ( NULL == tracks ) {
    if( msgLevel(MSG::DEBUG) ) debug() << "Track container '" << m_container << "' does not exist" <<endmsg;
    return;
  }
  if ( NULL == m_link ) m_link = new MyAsct( evtSvc(), m_container );
  m_nbGhost = 0;

  m_nbTrack = tracks->size();
  const Table* table = m_link->direct();
  if ( NULL == table ) { 
    Warning( "Problem with MC associations for " + m_container ).ignore();
    return; 
  }

  std::string title( m_container);
  while( title.find("/") < title.size() ) {
    title = title.substr( title.find("/")+1 );
  }

  double nbTracks = 0;
  if(htool && m_writeHistos>0 )htool->plot1D(nPV,title+"/nPV_Events","nPV_Events",-0.5,20.5,21);

  for ( LHCb::Tracks::const_iterator itT = tracks->begin();
        tracks->end() != itT; ++itT ) {
    if ( (*itT)->checkFlag( LHCb::Track::Invalid ) ) continue;
    if ( (m_trackType!=LHCb::Track::TypeUnknown) && ((*itT)->type()!=m_trackType) ) continue;
    bool eta25 = !m_eta25cut || ((*itT)->pseudoRapidity() > 2. && (*itT)->pseudoRapidity() < 5.);
    Range range = table->relations( *itT );
    if ( range.empty() ){
      m_nbGhost++;
      if(htool && m_writeHistos>1 && eta25){
        htool->plot1D(nPV,title+"/nPV_Ghosts","nPV_Ghosts",-0.5,20.5,21);
        htool->plot1D((*itT)->pseudoRapidity(),title+"/Eta_Ghosts","Eta_Ghosts",0.,7.,50);
        if((*itT)->type() != LHCb::Track::Velo ){
          htool->plot1D((*itT)->momentum().Phi(),title+"/Phi_Ghosts","Phi_Ghosts",-3.142,3.142,25);
          htool->plot1D((*itT)->pt(),title+"/Pt_Ghosts","Pt_Ghosts",0.,10000.,50);
          htool->plot1D((*itT)->p(),title+"/P_Ghosts","P_Ghosts",0.,100000.,50);
       }
      }
    }
    if(htool && m_writeHistos>1 && eta25){
      htool->plot1D(nPV,title+"/nPV_Total","nPV_Total",-0.5,20.5,21);
      htool->plot1D((*itT)->pseudoRapidity(),title+"/Eta_Total","Eta_Total",0.,7.,50);
      if((*itT)->type() != LHCb::Track::Velo ){
        htool->plot1D((*itT)->momentum().Phi(),title+"/Phi_Total","Phi_Total",-3.142,3.142,25);
        htool->plot1D((*itT)->pt(),title+"/Pt_Total","Pt_Total",0.,10000.,50);
        htool->plot1D((*itT)->p(),title+"/P_Total","P_Total",0.,100000.,50);
      }
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
//  PLot into HistoTool
//=========================================================================
int PrCounter::countAndPlot(const IHistoTool* htool,const LHCb::MCParticle* part, std::vector<bool> flags,
                       std::vector<LHCb::LHCbID>& ids, const int nPV) {

  if ( !m_validData ) return -1;

  if ( flags.size() > m_name.size() ) {
    Warning("Flag size mismatch").ignore();
    if( msgLevel(MSG::DEBUG) )
      debug() << "... Flag size " << flags.size() << " >  "
              << m_name.size() << " declared selections" << endmsg;
    return -1;
  }

  std::string title( m_container);
  while( title.find("/") < title.size() ) {
    title = title.substr( title.find("/")+1 );
  } 

  InvRange trackList = m_invTable->relations( part );
  bool found = false;
  int clone = 0;
  int key = -1;

  if ( LHCb::Track::TypeUnknown == m_trackType ){
    if ( trackList.size() != 0 ) {
      found = true;
      clone = trackList.size() - 1;
      key = trackList.begin()->to()->key();
    }
  }else{
    for ( InvIterator it = trackList.begin(); trackList.end() != it; ++it ) {
      const LHCb::Track* tr = it->to();
      if (tr->type()==m_trackType) {
        found = true;
        clone = trackList.size() - 1;
        key = it->to()->key();
        break;
      }
    }
  }

  //== Count how many of the proper type...
  double nTrue = 0.;

  if ( 0 == m_selectId ) {
    nTrue = double( ids.size() );
  } else {
    for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin(); ids.end() != itId; ++itId ) {
      if ( (*itId).isVP() || (*itId).isVL() || (*itId).isVelo() ) {
        if ( 0 != (m_selectId & 3) ) nTrue += 1.;
      } else if ( (*itId).isTT() || (*itId).isUT() ) {
        if ( 0 != (m_selectId & 4) ) nTrue += 1.;
      } else if ( (*itId).isOT() || (*itId).isIT() || (*itId).isFT() ) {
        if ( 0 != (m_selectId & 8) ) nTrue += 1.;
      }
    }
  }

  unsigned int maxRecHits = 0.;
  for ( unsigned int kk = 0; flags.size() > kk; ++kk ) {
    if ( flags[kk] ) {
      m_wanted[kk] ++;
      if ( found ) {
        m_counted[kk]++;
        m_clone[kk] += clone;
        for ( InvIterator it = trackList.begin(); trackList.end() != it; ++it ) {
          const LHCb::Track* tr = it->to();
          if ( (m_trackType!=LHCb::Track::TypeUnknown) && (tr->type()!=m_trackType) ) continue;
          m_purity[kk] += it->weight();
          unsigned int nbMeas = 0;
          for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
                tr->lhcbIDs().end() != itId; ++itId ) {
            if ( std::find( ids.begin(), ids.end(), *itId ) == ids.end() ) continue;
            if ( (*itId).isVP() || (*itId).isVL() || (*itId).isVelo() ) {
              if ( 0 != (m_selectId & 3) ) nbMeas += 1;
            } else if ( (*itId).isOT() || (*itId).isIT() || (*itId).isFT() ) {
              if ( 0 != (m_selectId & 8) ) nbMeas += 1;
            } else if ( (*itId).isTT() || (*itId).isUT() ) {
              if ( 0 != (m_selectId & 4) ) nbMeas += 1;
            }
          }
          if ( 0 < nTrue ) {
            maxRecHits = std::max(maxRecHits,nbMeas);
            double eff = double( nbMeas ) / nTrue;
            m_hitEff[kk] += eff;
          }
        }
      }
    }
  }

  double prodx = part->originVertex()->position().X();
  double prody = part->originVertex()->position().Y();
  double docaz = std::abs(1./part->momentum().Pt()*(prodx*part->momentum().Py() - prody*part->momentum().Px()));

  const LHCb::MCParticle* mother = part;
  while(mother->mother() != NULL)mother = mother->mother();
  double PVz = mother->originVertex()->position().Z();
  double mcq = part->particleID().threeCharge()  > 0 ? 1. : -1.;

  for( unsigned int k = 0; flags.size() > k; ++k ) {
    if(!htool)break;
    if(m_writeHistos<(m_writeHisto[k]?1:2))continue;
    if ( !flags[k] ) continue;
    htool->plot1D(nPV,title+"/"+m_name[k]+"_nPV_reconstructible",m_name[k]+"_nPV_reconstructible",-0.5,20.5,21);
    htool->plot1D(part->momentum().Eta(),title+"/"+m_name[k]+"_Eta_reconstructible",m_name[k]+"_Eta_reconstructible",0.,7.,50);
    htool->plot1D(part->momentum().Phi(),title+"/"+m_name[k]+"_Phi_reconstructible",m_name[k]+"_Phi_reconstructible",-3.142,3.142,25);
    htool->plot1D(part->momentum().Pt(),title+"/"+m_name[k]+"_Pt_reconstructible",m_name[k]+"_Pt_reconstructible",0.,10000.,50);
    htool->plot1D(part->momentum().P(),title+"/"+m_name[k]+"_P_reconstructible",m_name[k]+"_P_reconstructible",0.,100000.,50);
    if(m_writeHistos>1){
      htool->plot1D(nTrue,title+"/"+m_name[k]+"_expectedHits_reconstructible",m_name[k]+"_expectedHits_reconstructible",-0.5,20.5,21);
      htool->plot1D(docaz,title+"/"+m_name[k]+"_docaz_reconstructible",m_name[k]+"_docaz_reconstructible",0.,10.,50);
      htool->plot1D(PVz,title+"/"+m_name[k]+"_PVz_reconstructible",m_name[k]+"_PVz_reconstructible",-200.,200.,50);
      htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),title+"/"+m_name[k]+"_EtaPhi_reconstructible",m_name[k]+"_EtaPhi_reconstructible",0.,7.,-3.241,3.142,20,20);
      if(mcq>0){
        htool->plot1D(part->momentum().Eta(),title+"/"+m_name[k]+"_Eta_pos_reconstructible",m_name[k]+"_Eta_pos_reconstructible",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),title+"/"+m_name[k]+"_Phi_pos_reconstructible",m_name[k]+"_Phi_pos_reconstructible",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),title+"/"+m_name[k]+"_EtaPhi_pos_reconstructible",m_name[k]+"_EtaPhi_pos_reconstructible",0.,7.,-3.241,3.142,20,20);
      }else{
        htool->plot1D(part->momentum().Eta(),title+"/"+m_name[k]+"_Eta_neg_reconstructible",m_name[k]+"_Eta_neg_reconstructible",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),title+"/"+m_name[k]+"_Phi_neg_reconstructible",m_name[k]+"_Phi_neg_reconstructible",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),title+"/"+m_name[k]+"_EtaPhi_neg_reconstructible",m_name[k]+"_EtaPhi_neg_reconstructible",0.,7.,-3.241,3.142,20,20);
      }
    }
    if ( !found ) continue; 
    htool->plot1D(nPV,title+"/"+m_name[k]+"_nPV_reconstructed",m_name[k]+"_nPV_reconstructed",-0.5,20.5,21);
    htool->plot1D(part->momentum().Eta(),title+"/"+m_name[k]+"_Eta_reconstructed",m_name[k]+"_Eta_reconstructed",0.,7.,50);
    htool->plot1D(part->momentum().Phi(),title+"/"+m_name[k]+"_Phi_reconstructed",m_name[k]+"_Phi_reconstructed",-3.142,3.142,25);
    htool->plot1D(part->momentum().Pt(),title+"/"+m_name[k]+"_Pt_reconstructed",m_name[k]+"_Pt_reconstructed",0.,10000.,50);
    htool->plot1D(part->momentum().P(),title+"/"+m_name[k]+"_P_reconstructed",m_name[k]+"_P_reconstructed",0.,100000.,50);
    if(m_writeHistos>1){
      htool->plot1D(nTrue,title+"/"+m_name[k]+"_expectedHits_reconstructed",m_name[k]+"_expectedHits_reconstructed",-0.5,20.5,21);
      htool->plot1D(maxRecHits,title+"/"+m_name[k]+"_reconstructedHits",m_name[k]+"_reconstructedHits",-0.5,20.5,21);
      htool->plot1D(maxRecHits/nTrue,title+"/"+m_name[k]+"_HitEff",m_name[k]+"_HitEff",0.0,1.1,50);
      htool->plot1D(docaz,title+"/"+m_name[k]+"_docaz_reconstructed",m_name[k]+"_docaz_reconstructed",0.,10.,50);
      htool->plot1D(PVz,title+"/"+m_name[k]+"_PVz_reconstructed",m_name[k]+"_PVz_reconstructed",-200.,200.,50);
      htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),title+"/"+m_name[k]+"_EtaPhi_reconstructed",m_name[k]+"_EtaPhi_reconstructed",0.,7.,-3.241,3.142,20,20);

      if(mcq>0){
        htool->plot1D(part->momentum().Eta(),title+"/"+m_name[k]+"_Eta_pos_reconstructed",m_name[k]+"_Eta_pos_reconstructed",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),title+"/"+m_name[k]+"_Phi_pos_reconstructed",m_name[k]+"_Phi_pos_reconstructed",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),title+"/"+m_name[k]+"_EtaPhi_pos_reconstructed",m_name[k]+"_EtaPhi_pos_reconstructed",0.,7.,-3.241,3.142,20,20);
      }else{
        htool->plot1D(part->momentum().Eta(),title+"/"+m_name[k]+"_Eta_neg_reconstructed",m_name[k]+"_Eta_neg_reconstructed",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),title+"/"+m_name[k]+"_Phi_neg_reconstructed",m_name[k]+"_Phi_neg_reconstructed",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),title+"/"+m_name[k]+"_EtaPhi_neg_reconstructed",m_name[k]+"_EtaPhi_neg_reconstructed",0.,7.,-3.241,3.142,20,20);
      }
    }
  }

  return key;
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
    std::string nameformat = m_name[kk];
    std::string blank ( m_titleSize-nameformat.size(), ' ');
    nameformat = blank + nameformat;
    info() << "  " << nameformat
           << format( " :%8d from %8d [%5.1f %%] %6d clones [%4.1f %%]",
                      m_counted[kk], m_wanted[kk], frac, m_clone[kk], clo);
    info() << format( ", purity:%6.2f %%, hitEff:%6.2f %%", purity, hitEff ) << endmsg;
  }
  info() << endmsg;
}

//=============================================================================
