// $Id: PrTTCounter.cpp,v 1.4 2007-07-19 08:55:54 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiAlg/IHistoTool.h"


// local
#include "PrTTCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrTTCounter
//
// 2006-06-28 : Olivier Callot
// 2015-01-17 : modified by Svende Braun, Michel de Cian to work with PrChecker2.cpp
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrTTCounter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrTTCounter::PrTTCounter( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
: GaudiTool ( type, name , parent ),
m_selector("",this)
{
  declareInterface<IPrTTCounter>(this);
  m_link      = NULL;
  m_invTable = NULL;
  m_writeHistos = -1;
  m_eta25cut = false;
  m_extrapolator= NULL;
  //m_idealStateCreator= NULL;
  m_triggerNumbers = false;
  m_nbGhost   = 0.;
  m_nbGhostHit = 0.;
  m_totTrack   = 0;
  m_totTrackTrigger    = 0;
  m_totGhostTrigger    = 0;
  declareProperty( "TitleSize", m_titleSize = 40 );
  declareProperty( "Selector", m_selector );

  std::string title(name);
  while(title.find(".") < title.size()){
   title = title.substr( title.find(".")+1);
  }
  m_title = title;
}
//=============================================================================
// Destructor
//=============================================================================
PrTTCounter::~PrTTCounter() {} 

//=========================================================================
//  
//=========================================================================
StatusCode PrTTCounter::finalize ( ) {
  if ( NULL != m_link ) delete m_link;
  return GaudiTool::finalize();
}

//=========================================================================
// Initialize
//=========================================================================

StatusCode PrTTCounter::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool
  // retrieve the selector if it is set
  if ( !m_selector.empty() ) {
    if( msgLevel(MSG::DEBUG) ) debug()<<"Get Selector "<<endmsg;
    sc = m_selector.retrieve() ;
    if(sc.isFailure())
      error() << "Failed to retrieve selector." << endmsg ;
  }
  return sc;
}

//=========================================================================
//  Add a selection for the container monitoring
//=========================================================================
void PrTTCounter::addSelection ( std::string name, bool writeHisto ) {
   if ( name.size() > m_titleSize ) {
    name = name.substr( m_titleSize );
  }
  m_name.push_back( name );
  m_writeHisto.push_back(writeHisto);
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
void PrTTCounter::initEvent (const IHistoTool* htool = NULL) {
  m_validData = false;

  if ( !exist<LHCb::Tracks>( m_container ) ) 
  {
     if( msgLevel(MSG::DEBUG) ) debug() << "Track container '" << m_container << "' does not exist" <<endmsg;
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

  m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator",this);
  //m_idealStateCreator = tool<IIdealStateCreator>("IdealStateCreator", "IdealStateCreator", this);

  // Process the ghost tracks
  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_container );
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) 
  {
    const LHCb::Track* tr = (*itT);
    StatusCode sc;
    sc=StatusCode::FAILURE;
    LHCb::State state;
    if(m_trackextrapolation){
      sc = m_extrapolator->propagate( *tr, 9000, state );
      // -- Protect against nonphysical states
      if( isnan( state.x() ) || isnan( state.y() ) ) continue;
    }
    if ( !m_selector.empty() && !m_selector->accept( *tr ) )continue;
    if ( m_link->direct()->relations( tr ).empty() ) {
      double nbInTT = 0;
      for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
            tr->lhcbIDs().end() != itId; ++itId ) {
        if ( (*itId).isST() ) {
          if (  (*itId).isTT() || (*itId).isUT() ) nbInTT += 1.;
        }
      }
      m_nbGhost += 1.;
      if(htool && m_writeHistos>0 ){
         htool->plot1D((*itT)->pseudoRapidity(),m_title+"/Eta_Ghosts","Eta_Ghosts",0.,7.,50);
        if((*itT)->type() != LHCb::Track::Velo ){
          htool->plot1D((*itT)->momentum().Phi(),m_title+"/Phi_Ghosts","Phi_Ghosts",-3.142,3.142,25);
          htool->plot1D((*itT)->pt(),m_title+"/Pt_Ghosts","Pt_Ghosts",0.,10000.,50);
          htool->plot1D((*itT)->p(),m_title+"/P_Ghosts","P_Ghosts",0.,100000.,50);
	  htool->plot2D((*itT)->pseudoRapidity(),(*itT)->p(),m_title+"/EtaP_Ghosts","EtaP_Ghosts",0.,7.,0.,100000.,20, 20);
	  htool->plot2D((*itT)->pseudoRapidity(),(*itT)->momentum().Phi(),m_title+"/EtaPhi_Ghosts","EtaPhi_Ghosts",0.,7.,-3.142,3.142,20,20);
        }
	if(sc){
	  htool->plot2D( state.x(),state.y() ,m_title+"/XYZ9000_Ghosts", "XYZ9000_Ghosts",-3000,3000.,-3000.,3000.0,100,100);
	}
       
      }
      m_nbGhostHit += nbInTT;
    }
    if(htool && m_writeHistos>0 ){
       htool->plot1D((*itT)->pseudoRapidity(),m_title+"/Eta_Total","Eta_Total",0.,7.,50);
      if((*itT)->type() != LHCb::Track::Velo ){
        htool->plot1D((*itT)->momentum().Phi(),m_title+"/Phi_Total","Phi_Total",-3.142,3.142,25);
        htool->plot1D((*itT)->pt(),m_title+"/Pt_Total","Pt_Total",0.,10000.,50);
        htool->plot1D((*itT)->p(),m_title+"/P_Total","P_Total",0.,100000.,50);
	htool->plot2D((*itT)->pseudoRapidity(),(*itT)->p(),m_title+"/EtaP_Total","EtaP_Total",0.,7.,0.,100000.,20, 20);
	htool->plot2D((*itT)->pseudoRapidity(),(*itT)->momentum().Phi(),m_title+"/EtaPhi_Total","EtaPhi_Total",0.,7.,-3.142,3.142,20,20);
      }
      if(sc){
	htool->plot2D( state.x(),state.y() ,m_title+"/XYZ9000_Total", "XYZ9000_Total",-3000,3000.,-3000.,3000.0,100,100);
      }
    }
    m_totTrack++;
    if(m_triggerNumbers && ((*itT)->type() != LHCb::Track::Velo) && (*itT)->p() > 3000. && (*itT)->pt() > 500.)    {
      if ( m_link->direct()->relations( tr ).empty() ){
        m_totGhostTrigger++;
        if(htool && m_writeHistos>0 ){
          htool->plot1D((*itT)->pseudoRapidity(),m_title+"/Eta_Ghosts_P>3GeV_Pt>0.5GeV","Eta_Ghosts_P>3GeV_Pt>0.5GeV",0.,7.,50);
          htool->plot1D((*itT)->momentum().Phi(),m_title+"/Phi_Ghosts_P>3GeV_Pt>0.5GeV","Phi_Ghosts_P>3GeV_Pt>0.5GeV",-3.142,3.142,25);
          htool->plot1D((*itT)->pt(),m_title+"/Pt_Ghosts_P>3GeV_Pt>0.5GeV","Pt_Ghosts_P>3GeV_Pt>0.5GeV",0.,10000.,50);
          htool->plot1D((*itT)->p(),m_title+"/P_Ghosts_P>3GeV_Pt>0.5GeV","P_Ghosts_P>3GeV_Pt>0.5GeV",0.,100000.,50);
	  htool->plot2D((*itT)->pseudoRapidity(),(*itT)->p(),m_title+"/EtaP_Ghosts_P>3GeV_Pt>0.5GeV","EtaP_Ghosts_P>3GeV_Pt>0.5GeV",0.,7.,0.,100000.,20, 20);
	  htool->plot2D((*itT)->pseudoRapidity(),(*itT)->momentum().Phi(),m_title+"/EtaPhi_Ghosts_P>3GeV_Pt>0.5GeV","EtaPhi_Ghosts_P>3GeV_Pt>0.5GeV",0.,7.,-3.142,3.142,20,20);
	  if(sc){
	    htool->plot2D( state.x(),state.y() ,m_title+"/XYZ9000_Ghosts_P>3GeV_Pt>0.5GeV", "XYEff_Ghosts_P>3GeV_Pt>0.5GeV",-3000,3000.,-3000.,3000.0,100,100);
	  }
	}
      }
      if(htool && m_writeHistos>0 ){
        htool->plot1D((*itT)->pseudoRapidity(),m_title+"/Eta_Total_P>3GeV_Pt>0.5GeV","Eta_Total_P>3GeV_Pt>0.5GeV",0.,7.,50);
        htool->plot1D((*itT)->momentum().Phi(),m_title+"/Phi_Total_P>3GeV_Pt>0.5GeV","Phi_Total_P>3GeV_Pt>0.5GeV",-3.142,3.142,25);
        htool->plot1D((*itT)->pt(),m_title+"/Pt_Total_P>3GeV_Pt>0.5GeV","Pt_Total_P>3GeV_Pt>0.5GeV",0.,10000.,50);
        htool->plot1D((*itT)->p(),m_title+"/P_Total_P>3GeV_Pt>0.5GeV","P_Total_P>3GeV_Pt>0.5GeV",0.,100000.,50);
	htool->plot2D((*itT)->pseudoRapidity(),(*itT)->p(),m_title+"/EtaP_Total_P>3GeV_Pt>0.5GeV","EtaP_Ghosts_P>3GeV_Pt>0.5GeV",0.,7.,0.,100000.,20, 20);
	htool->plot2D((*itT)->pseudoRapidity(),(*itT)->momentum().Phi(),m_title+"/EtaPhi_Total_P>3GeV_Pt>0.5GeV","EtaPhi_Total_P>3GeV_Pt>0.5GeV",0.,7.,-3.142,3.142,20,20);
	if(sc){
	  htool->plot2D( state.x(),state.y() ,m_title+"/XYZ9000_Total_P>3GeV_Pt>0.5GeV", "XYZ9000_Total_P>3GeV_Pt>0.5GeV",-3000,3000.,-3000.,3000.0,100,100);
	}
      }
      m_totTrackTrigger++;
    }
  }//end track loop
}//end initialize event

//=========================================================================
//  Increment the counter for a given MC track, according to flags.
//=========================================================================
int PrTTCounter::countAndPlot(const IHistoTool* htool, const LHCb::MCParticle* part, std::vector<bool> flags, 
                          std::vector<LHCb::LHCbID>& ids ) {
  if ( !m_validData ) return  -1;

  if ( flags.size() > m_name.size() ) {
     Warning("Flag size mismatch").ignore();
    if( msgLevel(MSG::DEBUG) )
      debug() << "... Flag size " << flags.size() << " >  "
              << m_name.size() << " declared selections" << endmsg;
       return  -1 ;
  }

  InvRange trackList = m_invTable->relations( part );
  int key = -1;
  unsigned int nbTrack =0;
  unsigned int maxRecHits = 0.;

  std::vector<LHCb::LHCbID> ttIds;
  
  for ( std::vector<LHCb::LHCbID>::const_iterator itId = ids.begin();
          ids.end() != itId; ++itId ) {
    if ( (*itId).isST() ) {
      if ( (*itId).stID().isTT() || (*itId).stID().isUT() ) ttIds.push_back( *itId );
    }
  }

  if( msgLevel(MSG::VERBOSE) )
    verbose() << "MCPart " << part->key() << " has " << ttIds.size() << " TT measurements "
            <<endmsg;

  std::vector<bool> shallIPlotTheHistograms( flags.size(), false);

  for ( unsigned int kk = 0; flags.size() > kk; ++kk ) {
    if ( flags[kk] ) {
      for ( InvIterator it = trackList.begin(); trackList.end() != it; ++it ) {
        const LHCb::Track* tr = it->to();
	if ( !m_selector.empty() && !m_selector->accept( *tr ) ) {
	  //always()<<"Did not accept track"<<endmsg;
	  continue;
	}
	key = it->to()->key();
        unsigned int nbOK = 0;
        unsigned int nbWrong = 0;
	
        
        for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
              tr->lhcbIDs().end() != itId; ++itId ) {
          if ( (*itId).isST() ) {
            if (  (*itId).stID().isTT() || (*itId).stID().isUT() ) {
              LHCb::LHCbID t = (*itId);
	      bool found = false;
              for ( std::vector<LHCb::LHCbID>::const_iterator itMc = ttIds.begin();
                    ttIds.end() != itMc; ++itMc ) {
                if ( t == (*itMc) ) found = true;
              }
              if ( found ) {
		shallIPlotTheHistograms[kk] = true;
                nbOK++;
              } else {
                nbWrong++;
              } 
            }
          }
        }
	nbTrack ++;
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
	 if ( 0 < nbTrack ) {
            maxRecHits = std::max(maxRecHits,nbOK);
                     }
      }
    }
  }

  const LHCb::MCParticle* mother = part;
  while(mother->mother() != NULL)mother = mother->mother();
  double PVz = mother->originVertex()->position().Z();
  double mcq = part->particleID().threeCharge() > 0 ? 1. : -1.;
  
  double PVx = part->originVertex()->position().X();
  double PVy = part->originVertex()->position().Y();
  double slopex = (part->momentum().Px())/(part->momentum().Pz());
  double slopey = (part->momentum().Py())/(part->momentum().Pz());
  double q_over_p = (mcq/3)/(part->momentum().P());
  LHCb::State state;
  state.setState(PVx,PVy, PVz, slopex, slopey, q_over_p);
  StatusCode sc;
  sc=StatusCode::FAILURE;
  if(m_trackextrapolation){
     sc = m_extrapolator->propagate(state,9000);
  //m_idealStateCreator->createState( part, 9000, state );
  }

  for( unsigned int k = 0; flags.size() > k; ++k ) {
     // -- Protect against nonphysical states
    if( isnan( state.x() ) || isnan( state.y() ) ) continue;
    if(!htool)break;
    if(m_writeHistos<(m_writeHisto[k]?1:2))continue;
    if ( !flags[k] ) continue;
    htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_reconstructible",m_name[k]+"_Eta_reconstructible",0.,7.,50);
    htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_reconstructible",m_name[k]+"_Phi_reconstructible",-3.142,3.142,25);
    htool->plot1D(part->momentum().Pt(),m_title+"/"+m_name[k]+"_Pt_reconstructible",m_name[k]+"_Pt_reconstructible",0.,10000.,50);
    htool->plot1D(part->momentum().P(),m_title+"/"+m_name[k]+"_P_reconstructible",m_name[k]+"_P_reconstructible",0.,100000.,50);
    if(m_writeHistos>1){
      htool->plot1D(nbTrack,m_title+"/"+m_name[k]+"_expectedHits_reconstructible",m_name[k]+"_expectedHits_reconstructible",-0.5,20.5,21);
      htool->plot2D(part->momentum().Eta(),part->momentum().P(),m_title+"/"+m_name[k]+"_EtaP_reconstructible",m_name[k]+"_EtaP_reconstructible",0.,7.,0.,100000.,20, 20);
      htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_reconstructible",m_name[k]+"_EtaPhi_reconstructible",0.,7.,-3.241,3.142,20,20);
      if(sc){
	htool->plot2D( state.x(),state.y() ,m_title+"/"+m_name[k]+"XYZ9000_reconstructible", "XYZ9000_reconstructible",-3000,3000.,-3000.,3000.0,100,100);
      }
      if(mcq>0){
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_pos_reconstructible",m_name[k]+"_Eta_pos_reconstructible",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_pos_reconstructible",m_name[k]+"_Phi_pos_reconstructible",-3.142,3.142,25);
	htool->plot2D(part->momentum().Eta(),part->momentum().P(),m_title+"/"+m_name[k]+"_EtaP_pos_reconstructible",m_name[k]+"_EtaP_pos_reconstructible",0.,7.,0.,100000.,20, 20);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_pos_reconstructible",m_name[k]+"_EtaPhi_pos_reconstructible",0.,7.,-3.241,3.142,20,20);
      if(sc){
	  htool->plot2D( state.x(),state.y() ,m_title+"/"+m_name[k]+"XYZ9000_pos_reconstructible", "XYZ9000_pos_reconstructible",-3000,3000.,-3000.,3000.0,100,100);
      }
      }else{
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_neg_reconstructible",m_name[k]+"_Eta_neg_reconstructible",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_neg_reconstructible",m_name[k]+"_Phi_neg_reconstructible",-3.142,3.142,25);
	htool->plot2D(part->momentum().Eta(),part->momentum().P(),m_title+"/"+m_name[k]+"_EtaP_neg_reconstructible",m_name[k]+"_EtaP_neg_reconstructible",0.,7.,0.,100000.,20, 20);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_neg_reconstructible",m_name[k]+"_EtaPhi_neg_reconstructible",0.,7.,-3.241,3.142,20,20);
	if(sc){
	  htool->plot2D( state.x(),state.y() ,m_title+"/"+m_name[k]+"XYZ9000_neg_reconstructible", "XYZ9000_neg_reconstructible",-3000,3000.,-3000.,3000.0,100,100);
	}
      }
    }
    if( !shallIPlotTheHistograms[k] ) continue;
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
      htool->plot1D(nbTrack,m_title+"/"+m_name[k]+"_expectedHits_reconstructed",m_name[k]+"_expectedHits_reconstructed",-0.5,20.5,21);
      htool->plot1D(maxRecHits,m_title+"/"+m_name[k]+"_reconstructedHits",m_name[k]+"_reconstructedHits",-0.5,20.5,21);
      htool->plot1D(maxRecHits/nbTrack,m_title+"/"+m_name[k]+"_HitEff",m_name[k]+"_HitEff",0.0,1.1,50);
      htool->plot2D(part->momentum().Eta(),part->momentum().P(),m_title+"/"+m_name[k]+"_EtaP_reconstructed",m_name[k]+"_EtaP_reconstructed",0.,7.,0.,100000.,20, 20);
      htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_reconstructed",m_name[k]+"_EtaPhi_reconstructed",0.,7.,-3.241,3.142,20,20);
      if(sc){
	htool->plot2D( state.x(),state.y() ,m_title+"/"+m_name[k]+"XYZ9000_reconstructed", "XYZ9000_reconstructed",-3000,3000.,-3000.,3000.0,100,100);
      }

      if(mcq>0){
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_pos_reconstructed",m_name[k]+"_Eta_pos_reconstructed",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_pos_reconstructed",m_name[k]+"_Phi_pos_reconstructed",-3.142,3.142,25);
	htool->plot2D(part->momentum().Eta(),part->momentum().P(),m_title+"/"+m_name[k]+"_EtaP_pos_reconstructed",m_name[k]+"_EtaP_pos_reconstructed",0.,7.,0.,100000.,20, 20);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_pos_reconstructed",m_name[k]+"_EtaPhi_pos_reconstructed",0.,7.,-3.241,3.142,20,20);
	if(sc){
	  htool->plot2D( state.x(),state.y() ,m_title+"/"+m_name[k]+"XYZ9000_pos_reconstructed", "XYZ9000_pos_reconstructed",-3000,3000.,-3000.,3000.0,100,100);
	}
      }else{
        htool->plot1D(part->momentum().Eta(),m_title+"/"+m_name[k]+"_Eta_neg_reconstructed",m_name[k]+"_Eta_neg_reconstructed",0.,7.,50);
        htool->plot1D(part->momentum().Phi(),m_title+"/"+m_name[k]+"_Phi_neg_reconstructed",m_name[k]+"_Phi_neg_reconstructed",-3.142,3.142,25);
	htool->plot2D(part->momentum().Eta(),part->momentum().P(),m_title+"/"+m_name[k]+"_EtaP_neg_reconstructed",m_name[k]+"_EtaP_neg_reconstructed",0.,7.,0.,100000.,20, 20);
        htool->plot2D(part->momentum().Eta(),part->momentum().Phi(),m_title+"/"+m_name[k]+"_EtaPhi_neg_reconstructed",m_name[k]+"_EtaPhi_neg_reconstructed",0.,7.,-3.241,3.142,20,20);
	if(sc){
	  htool->plot2D( state.x(),state.y() ,m_title+"/"+m_name[k]+"XYZ9000_neg_reconstructed", "XYZ9000_neg_reconstructed",-3000,3000.,-3000.,3000.0,100,100);
	}
      }
    }
  }
  return key;
}

//=========================================================================
//  Print at the end the statistics
//=========================================================================
void PrTTCounter::printStatistics ( ) {
  if ( m_totTrack == 0 ) return;
   m_title.resize( m_titleSize, ' ' );
   std::string strigger = "for P>3GeV,Pt>0.5GeV";
   strigger.resize( m_titleSize, ' ' );

  info() << "**** TT Efficiency for " << m_container << " ****  ";
  if ( 0 != m_nbGhost ) {
    double bad = m_nbGhostHit / m_nbGhost;
    info() << format( "%6.0f ghost, %5.2f TT per track", m_nbGhost, bad )<< endmsg;
  }
  if(m_triggerNumbers) info() << "**** " << strigger 
         << format( "%7d tracks including       %7d ghosts [%4.1f %%]  ****",
                    m_totTrackTrigger, m_totGhostTrigger, 100.*m_totGhostTrigger/m_totTrackTrigger ) << endmsg;

   
  for ( unsigned int kk = 0; m_name.size() > kk; ++kk ) {
    if ( 0.5 > m_nbTrack[kk] ) continue;
    double eff = 0.;
    double fraceff = 0.;
    double bad = 0.;
    double fracbad = 0.;
    double meanHits = 0.;
    if ( 0.5 < m_nbTrack[kk] ) {
      meanHits = m_mcHits[kk]  / m_nbTrack[kk];
      eff      = m_foundOK[kk] / m_nbTrack[kk];
      fraceff  = 100. * eff / meanHits;
      bad      = m_wrong[kk]   / m_nbTrack[kk];
      fracbad  = 100. *bad / (eff+bad);
       }

    double eff3 = 0.;
    double fraceff3 = 0.;
    double bad3 = 0.;
    double fracbad3 = 0.;
    double meanHits3 = 0.;
     if ( 0.5 < m_nbTrack3[kk] ) {
      meanHits3 = m_mcHits3[kk]  / m_nbTrack3[kk];
      eff3      = m_foundOK3[kk] / m_nbTrack3[kk];
      fraceff3  = 100. *eff3 / meanHits3;
      bad3      = m_wrong3[kk]   / m_nbTrack3[kk];
      fracbad3  = 100. *bad3 / (eff3+bad3);
       }
    
    std::string nameformat = m_name[kk];
    std::string nameformat2 = m_name[kk];
    std::string blank ( m_titleSize-(nameformat.size()), ' ');
    std::string blank2 ( m_titleSize-(nameformat.size()+5.0), ' ');
    nameformat2 = blank2 + nameformat;
    nameformat = blank + nameformat;
    info() << "  " << nameformat
           << format( " :%6.0f tr %5.2f from %5.2f mcTT [%5.1f %%] %5.2f ghost hits on real tracks [%4.1f %%]",
                      m_nbTrack[kk], eff, meanHits, fraceff, bad, fracbad) << endmsg;
    
    info() << "  " << nameformat2+" >3TT" 
	   << format( " :%6.0f tr %5.2f from %5.2f mcTT [%5.1f %%] %5.2f ghost hits on real tracks [%4.1f %%]", 
		      m_nbTrack3[kk], eff3, meanHits3, fraceff3, bad3, fracbad3 ) << endmsg;
  }
  info() << endmsg;
}

//=============================================================================
