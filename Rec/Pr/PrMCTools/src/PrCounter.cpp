// Include files

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
  declareInterface<IPrCounter>(this);
  m_link        = NULL;
  m_writeHistos = -1;
  m_eta25cut = false;
  m_triggerNumbers = false;
  m_totTrack    = 0;
  m_totGhost    = 0;
  m_totTrackTrigger    = 0;
  m_totGhostTrigger    = 0;
  m_selectId    = 0;
  m_fracGhost   = 0.;
  m_nEvent      = 0.;
  m_ghostProbCut = 999;
  m_trackType   = LHCb::Track::TypeUnknown;
  declareProperty( "TitleSize", m_titleSize = 30 );

  std::string title(name);
  while(title.find(".") < title.size()){
   title = title.substr( title.find(".")+1);
  }
  m_title = title;


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
  LHCb::Track::Range tracks  = getIfExists<LHCb::Track::Range>(m_container) ;
  if ( tracks.empty() ) {
    if( msgLevel(MSG::DEBUG) ) debug() << "Track container '" << m_container << "' does not exist" <<endmsg;
    return;
  }
  if ( NULL == m_link ) m_link = new MyAsct( evtSvc(), m_container );
  m_nbGhost = 0;

  m_nbTrack = tracks.size();
  const Table* table = m_link->direct();
  if ( NULL == table ) { 
    Warning( "Problem with MC associations for " + m_container ).ignore();
    return; 
  }


  double nbTracks = 0;
  if(htool && m_writeHistos>0 )htool->plot1D(nPV,m_title+"/nPV_Events","nPV_Events",-0.5,20.5,21);
  for ( auto track  : tracks) {
    if ( track->checkFlag( LHCb::Track::Invalid ) ) continue;
    if ( (m_trackType!=LHCb::Track::TypeUnknown) && (track->type()!=m_trackType) ) continue;
    if ( track->ghostProbability() > m_ghostProbCut && track->ghostProbability() < 999) continue;
    bool eta25 = !m_eta25cut || (track->pseudoRapidity() > 2. && track->pseudoRapidity() < 5.);
    if(!eta25)continue;
    Range range = table->relations( track );
    // --
    if ( range.empty() ){
      m_nbGhost++;
      if(htool && m_writeHistos>0 ){
        htool->plot1D(nPV,m_title+"/nPV_Ghosts","nPV_Ghosts",-0.5,20.5,21);
        htool->plot1D(track->pseudoRapidity(),m_title+"/Eta_Ghosts","Eta_Ghosts",0.,7.,50);
        if(track->type() != LHCb::Track::Velo ){
          htool->plot1D(track->momentum().Phi(),m_title+"/Phi_Ghosts","Phi_Ghosts",-3.142,3.142,25);
          htool->plot1D(track->pt(),m_title+"/Pt_Ghosts","Pt_Ghosts",0.,10000.,50);
          htool->plot1D(track->p(),m_title+"/P_Ghosts","P_Ghosts",0.,100000.,50);
       }
      }
    }
    // --
    if(htool && m_writeHistos>0 ){
      htool->plot1D(nPV,m_title+"/nPV_Total","nPV_Total",-0.5,20.5,21);
      htool->plot1D(track->pseudoRapidity(),m_title+"/Eta_Total","Eta_Total",0.,7.,50);
      if(track->type() != LHCb::Track::Velo ){
        htool->plot1D(track->momentum().Phi(),m_title+"/Phi_Total","Phi_Total",-3.142,3.142,25);
        htool->plot1D(track->pt(),m_title+"/Pt_Total","Pt_Total",0.,10000.,50);
        htool->plot1D(track->p(),m_title+"/P_Total","P_Total",0.,100000.,50);
      }
    }
    // --
    m_totTrack++;
    nbTracks++;
    if(m_triggerNumbers && (track->type() != LHCb::Track::Velo) && track->p() > 3000. && track->pt() > 500.){
      if ( range.empty() ){
        m_totGhostTrigger++;
        if(htool && m_writeHistos>0 ){
          htool->plot1D(nPV,m_title+"/nPV_Ghosts_P>3GeV_Pt>0.5GeV","nPV_Ghosts_P>3GeV_Pt>0.5GeV",-0.5,20.5,21);
          htool->plot1D(track->pseudoRapidity(),m_title+"/Eta_Ghosts_P>3GeV_Pt>0.5GeV","Eta_Ghosts_P>3GeV_Pt>0.5GeV",0.,7.,50);
          htool->plot1D(track->momentum().Phi(),m_title+"/Phi_Ghosts_P>3GeV_Pt>0.5GeV","Phi_Ghosts_P>3GeV_Pt>0.5GeV",-3.142,3.142,25);
          htool->plot1D(track->pt(),m_title+"/Pt_Ghosts_P>3GeV_Pt>0.5GeV","Pt_Ghosts_P>3GeV_Pt>0.5GeV",0.,10000.,50);
          htool->plot1D(track->p(),m_title+"/P_Ghosts_P>3GeV_Pt>0.5GeV","P_Ghosts_P>3GeV_Pt>0.5GeV",0.,100000.,50);
        }
      }
      if(htool && m_writeHistos>0 ){
        htool->plot1D(nPV,m_title+"/nPV_Total_P>3GeV_Pt>0.5GeV","nPV_Total_P>3GeV_Pt>0.5GeV",-0.5,20.5,21);
        htool->plot1D(track->pseudoRapidity(),m_title+"/Eta_Total_P>3GeV_Pt>0.5GeV","Eta_Total_P>3GeV_Pt>0.5GeV",0.,7.,50);
        htool->plot1D(track->momentum().Phi(),m_title+"/Phi_Total_P>3GeV_Pt>0.5GeV","Phi_Total_P>3GeV_Pt>0.5GeV",-3.142,3.142,25);
        htool->plot1D(track->pt(),m_title+"/Pt_Total_P>3GeV_Pt>0.5GeV","Pt_Total_P>3GeV_Pt>0.5GeV",0.,10000.,50);
        htool->plot1D(track->p(),m_title+"/P_Total_P>3GeV_Pt>0.5GeV","P_Total_P>3GeV_Pt>0.5GeV",0.,100000.,50);
      }
      m_totTrackTrigger++;
    }
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

  InvRange trackList = m_invTable->relations( part );
  bool found = false;
  int clone = 0;
  int key = -1;
  //double pmeas   = -999;//
  //double ptmeas  = -999;//
  //double etameas = -999;//
  //double phimeas = -999;//
  
  //trackList link tracks related to the MCparticle passed
  if ( LHCb::Track::TypeUnknown == m_trackType ){
    //TODO loop

    if ( trackList.size() != 0){
      found = true;
      clone = trackList.size() - 1;
      key = trackList.begin()->to()->key();  
      //pmeas   = trackList.begin()->to()->p();//
     // ptmeas  = trackList.begin()->to()->pt();//
     // etameas = trackList.begin()->to()->pseudoRapidity();//
     // phimeas = trackList.begin()->to()->phi();//
    }
  }else{
    for ( InvIterator it = trackList.begin(); trackList.end() != it; ++it ) {
      const LHCb::Track* tr = it->to();
      if (tr->ghostProbability() > m_ghostProbCut && tr->ghostProbability() < 999) continue;
      if (tr->type()==m_trackType) {
        found = true;
        clone = trackList.size() - 1;
        key = it->to()->key();
       // pmeas   = it->to()->p();//
       // ptmeas  = it->to()->pt();//
       // etameas = it->to()->pseudoRapidity();//
       // phimeas = it->to()->phi();//
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
      if ( (*itId).isVP() || (*itId).isVelo() ) {
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
          if ( tr->ghostProbability() > m_ghostProbCut && tr->ghostProbability() < 999) continue;
          m_purity[kk] += it->weight();
          unsigned int nbMeas = 0;
          for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
                tr->lhcbIDs().end() != itId; ++itId ) {
            if ( std::find( ids.begin(), ids.end(), *itId ) == ids.end() ) continue;
            if ( (*itId).isVP() || (*itId).isVelo() ) {
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
  double docaz = -100.0;
  if( part->momentum().Pt() > 0.00001){
    docaz = std::abs(1./part->momentum().Pt()*(prodx*part->momentum().Py() - prody*part->momentum().Px()));
  }
  
  const LHCb::MCParticle* mother = part;
  while(mother->mother() != NULL)mother = mother->mother();
  double PVz = mother->originVertex()->position().Z();
  double mcq = part->particleID().threeCharge()  > 0 ? 1. : -1.;

  for( unsigned int k = 0; flags.size() > k; ++k ) {
    if(!htool)break;
    if(m_writeHistos<(m_writeHisto[k]?1:2))continue;
    if ( !flags[k] ) continue;
    htool->plot1D(nPV,m_title+"/"+m_name[k]+"_nPV_reconstructible",m_name[k]+"_nPV_reconstructible",-0.5,20.5,21);
    htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_reconstructible",m_name[k]+"_Eta_reconstructible",0.,7.,50);
    htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_reconstructible",m_name[k]+"_Phi_reconstructible",-3.142,3.142,25);
    htool->plot1D(part->momentum().Pt(),m_title+"/"+m_name[k]+"_Pt_reconstructible",m_name[k]+"_Pt_reconstructible",0.,10000.,50);
    htool->plot1D(part->momentum().P(),m_title+"/"+m_name[k]+"_P_reconstructible",m_name[k]+"_P_reconstructible",0.,100000.,50);
    if(m_writeHistos>1){
      htool->plot1D(nTrue,m_title+"/"+m_name[k]+"_expectedHits_reconstructible",m_name[k]+"_expectedHits_reconstructible",-0.5,20.5,21);
      htool->plot1D(docaz,m_title+"/"+m_name[k]+"_docaz_reconstructible",m_name[k]+"_docaz_reconstructible",0.,10.,50);
      htool->plot1D(PVz,m_title+"/"+m_name[k]+"_PVz_reconstructible",m_name[k]+"_PVz_reconstructible",-200.,200.,50);
      htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_reconstructible",m_name[k]+"_EtaPhi_reconstructible",0.,7.,-3.241,3.142,20,20);
      if(mcq>0){
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_pos_reconstructible",m_name[k]+"_Eta_pos_reconstructible",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_pos_reconstructible",m_name[k]+"_Phi_pos_reconstructible",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_pos_reconstructible",m_name[k]+"_EtaPhi_pos_reconstructible",0.,7.,-3.241,3.142,20,20);
      }else{
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_neg_reconstructible",m_name[k]+"_Eta_neg_reconstructible",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_neg_reconstructible",m_name[k]+"_Phi_neg_reconstructible",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_neg_reconstructible",m_name[k]+"_EtaPhi_neg_reconstructible",0.,7.,-3.241,3.142,20,20);
      }
    }
    if ( !found ) continue; 
    htool->plot1D(nPV,m_title+"/"+m_name[k]+"_nPV_reconstructed",m_name[k]+"_nPV_reconstructed",-0.5,20.5,21);
    htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_reconstructed",m_name[k]+"_Eta_reconstructed",0.,7.,50);
    htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_reconstructed",m_name[k]+"_Phi_reconstructed",-3.142,3.142,25);
    htool->plot1D(part->momentum().Pt(),m_title+"/"+m_name[k]+"_Pt_reconstructed",m_name[k]+"_Pt_reconstructed",0.,10000.,50);
    htool->plot1D(part->momentum().P(),m_title+"/"+m_name[k]+"_P_reconstructed",m_name[k]+"_P_reconstructed",0.,100000.,50);
    //
    //htool->plot1D(pmeas,m_title+"/"+m_name[k]+"_P_reconstructedmeasured",m_name[k]+"_P_reconstructedmeasured",0.,100000.,50);
    //htool->plot1D(ptmeas,m_title+"/"+m_name[k]+"_Pt_reconstructedmeasured",m_name[k]+"_Pt_reconstructedmeasured",0.,10000.,50);
    //htool->plot1D(etameas,m_title+"/"+m_name[k]+"_Eta_reconstructedmeasured",m_name[k]+"_Eta_reconstructedmeasured",0.,7.,50);
    //htool->plot1D(phimeas,m_title+"/"+m_name[k]+"_Phi_reconstructedmeasured",m_name[k]+"_Phi_reconstructedmeasured",-3.142,3.142,25);
 

    if(m_writeHistos>1){
      htool->plot1D(nTrue,m_title+"/"+m_name[k]+"_expectedHits_reconstructed",m_name[k]+"_expectedHits_reconstructed",-0.5,20.5,21);
      htool->plot1D(maxRecHits,m_title+"/"+m_name[k]+"_reconstructedHits",m_name[k]+"_reconstructedHits",-0.5,20.5,21);
      htool->plot1D(maxRecHits/nTrue,m_title+"/"+m_name[k]+"_HitEff",m_name[k]+"_HitEff",0.0,1.1,50);
      htool->plot1D(docaz,m_title+"/"+m_name[k]+"_docaz_reconstructed",m_name[k]+"_docaz_reconstructed",0.,10.,50);
      htool->plot1D(PVz,m_title+"/"+m_name[k]+"_PVz_reconstructed",m_name[k]+"_PVz_reconstructed",-200.,200.,50);
      htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_reconstructed",m_name[k]+"_EtaPhi_reconstructed",0.,7.,-3.241,3.142,20,20);

      if(mcq>0){
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_pos_reconstructed",m_name[k]+"_Eta_pos_reconstructed",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_pos_reconstructed",m_name[k]+"_Phi_pos_reconstructed",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_pos_reconstructed",m_name[k]+"_EtaPhi_pos_reconstructed",0.,7.,-3.241,3.142,20,20);
      }else{
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_neg_reconstructed",m_name[k]+"_Eta_neg_reconstructed",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_neg_reconstructed",m_name[k]+"_Phi_neg_reconstructed",-3.142,3.142,25);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_neg_reconstructed",m_name[k]+"_EtaPhi_neg_reconstructed",0.,7.,-3.241,3.142,20,20);
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
  m_title.resize( m_titleSize, ' ' );
  std::string strigger = "for P>3GeV,Pt>0.5GeV";
  strigger.resize( m_titleSize, ' ' );


  info() << "**** " << m_title
         << format( "%7d tracks including       %7d ghosts [%4.1f %%], Event average %5.1f %% ****",
                    m_totTrack, m_totGhost, frac, 100. * m_fracGhost / m_nEvent ) << endmsg;
  if(m_triggerNumbers) info() << "**** " << strigger 
         << format( "%7d tracks including       %7d ghosts [%4.1f %%]  ****",
                    m_totTrackTrigger, m_totGhostTrigger, 100.*m_totGhostTrigger/m_totTrackTrigger ) << endmsg;

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
