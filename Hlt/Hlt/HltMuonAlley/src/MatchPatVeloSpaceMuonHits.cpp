// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/Track.h"


// local
#include "MatchPatVeloSpaceMuonHits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MatchPatVeloSpaceMuonHits
//
// 2005-09-21 : Sandra Amato
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MatchPatVeloSpaceMuonHits>          s_factory ;
const        IAlgFactory& MatchPatVeloSpaceMuonHitsFactory = s_factory ; 

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MatchPatVeloSpaceMuonHits::MatchPatVeloSpaceMuonHits( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator ) 
  , m_outputMuonTracks()
  , m_myState()
  , m_iPosTool()
{
  declareProperty( "ptkickConstant",  m_ptkickConstant  =  1300 );
  declareProperty("OutputMuonTracksName"   , 
                  m_outputMuonTracksName = "Hlt/Tracks/VeloSpaceMuon");
}
//=============================================================================
// Destructor
//=============================================================================
MatchPatVeloSpaceMuonHits::~MatchPatVeloSpaceMuonHits() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MatchPatVeloSpaceMuonHits::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endmsg;
  
  //m_store     = tool<PatDataStore>( "PatDataStore" );
  /*  m_trackSelector = NULL;
  if (m_trackSelectorName != "None")
    m_trackSelector = tool<ITrackSelector>( m_trackSelectorName, this);
  
  m_inputTracks  = m_store->tracks( m_inputTracksName );  
  m_Muon      = m_store->tracks( m_outputTracksName);*/
//  PatContainerMap<LHCb::Track>::iterator pippo;
//for(pippo=m_patDataStore->beginTracks();pippo!=m_patDataStore->endTracks();pippo++){
  //debug()<<" container path "<<pippo->first<<endreq;
//}

  m_outputMuonTracks  = m_patDataStore->tracks( m_outputMuonTracksName);
  m_states            = m_patDataStore->states();
//debug()<<" the container is "<<m_

  //release( m_store );
  
  
  
  // TrgL0Muons

  
  //TrgDataProvider* trgData = tool<TrgDataProvider>( "TrgDataProvider" );
  
  m_myState = new State();
  
  m_iPosTool=tool<IMuonPosTool>( "MuonPosTool" );
  
  m_y_cut_track.push_back(25.);
  m_y_cut_track.push_back(60.);
  m_y_cut_track.push_back(100.);
  m_y_cut_track.push_back(180.);
  m_x_cut_track.push_back(100.);
  m_x_cut_track.push_back(200.);
  m_x_cut_track.push_back(300.);
  m_x_cut_track.push_back(800.);
  
  z_magnet_center=5200.;
  initializeHisto(m_histoXdist,"X dist",0.,1000.,100);
  initializeHisto(m_histoYdist,"Y dist",0.,1000.,100);
  initializeHisto(h_patOutTrack,"Pat Output Tracks",0.,100,100);

  return StatusCode::SUCCESS;
};
//=============================================================================
bool MatchPatVeloSpaceMuonHits::acceptTrack(const Track& track) 
{
  bool ok = !(track.checkFlag( Track::Invalid) );
  ok = ok && (!(track.checkFlag( Track::Backward) ));
  /*  if (m_trackSelector) 
    ok = ok && (m_trackSelector->accept(track));
  */
  //  verbose() << " accept track " << ok << endreq;
  return ok;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode MatchPatVeloSpaceMuonHits::execute() {
  
  debug() << "==> Execute" << endreq;

  bool ok  = HltAlgorithm::beginExecute();

  m_inputL0Muon=get<L0MuonCandidates>(L0MuonCandidateLocation::Default);
 debug()<<" L0 Muon "<<endreq;

  /*  
  if( 0 == ((int) m_inputL0Muons->size()) ) { 
    // nothing to do, no muons
    return StatusCode::SUCCESS;
  } else {
    debug() << " Nr of L0 muons: " << m_inputL0Muons->size() 
            << endreq;
  }

  L0DUReport* rprtL0 = get<L0DUReport>(L0DUReportLocation::Default);
  if (!(rprtL0->isTrig( L0Trig::Mu1) || rprtL0->isTrig( L0Trig::SumMu)))
    return StatusCode::SUCCESS;
  */
  if(m_outputMuonTracks)debug()<<"existing output container "<<endreq;
  //debug()<<" tried "<<endreq;  
  //debug() << "Muon Matched size " << m_outputMuonTracks->size() << endreq;
  //if ( ((int)m_outputMuonTracks->size()) < 1) return StatusCode::SUCCESS;
  
//info()<<" already matched in 3D "<<m_Muon->size()<<endreq;	  
  muonMatch();  
  //debug()<<" tot matched in 3D "<<m_outputMuonTracks->size()<<endreq;	  
  fillHisto(h_patOutTrack,m_outputMuonTracks->size(),1.0);
  
  HltAlgorithm::endExecute();  
  
  return StatusCode::SUCCESS;
};



//=============================================================================
//  Finalize
//=============================================================================
StatusCode MatchPatVeloSpaceMuonHits::finalize() {
  
  if ( 0 != m_myState ) delete m_myState;
  
  return HltAlgorithm::finalize();
}
//=============================================================================
StatusCode MatchPatVeloSpaceMuonHits::muonMatch()
{
  
  double  ymin ;
  MuonTileID tileIDMatched;
  MuonTileID tileIDMatchedM2;
  MuonTileID idtile;
  double qp=0.;      
  std::vector<Track*> m_matchedTracks;
  std::vector<bool> m_velo_repeated;
  
  std::vector<float> m_ydistance;
  std::vector<float> m_qp;
  std::vector<MuonTileID> m_muon;
  std::vector<MuonTileID> m_muonM2;
  m_matchedTracks.reserve(100);
  m_muon.reserve(100);
  m_muonM2.reserve(100);
  m_ydistance.reserve(100);
  m_qp.reserve(100);
  /*  for(int i=0;i<100;i++){
    m_matchedTracks.push_back(NULL);
    
    m_muon.push_back(0);
    
    m_muonM2.push_back(0);
    m_ydistance.push_back(0);
    m_qp.push_back(0);
    
    }*/
  
  m_matchedTracks.clear();
  m_muon.clear();
  m_muonM2.clear();
  m_ydistance.clear();
  m_qp.clear();
  
  
  //  debug() << " " << endreq;
  //debug() << "New Event " << endreq;
  //std::vector<MuonTrack>::iterator itMuonTrack;
  //  PatTrackContainer::const_iterator pItr;
  //for ( pItr = m_patInputTracks->begin(); m_patInputTracks->end() != pItr; pItr++ ) {
  Hlt::TrackContainer::const_iterator pItr;
  for ( pItr = m_inputTracks->begin(); m_inputTracks->end() != pItr; pItr++ ) {
    Track* veloTr = (*pItr);
    if(veloTr->checkFlag( Track::Backward ))continue;
    if( veloTr->checkFlag(Track::PIDSelected) ) continue;
    /*
    if (m_checkMC){
      
      LinkedTo<MCParticle> trLink(evtSvc(),msgSvc(),TrackLocation::HltSpaceVelo);
      MCParticle* part = trLink.first( veloTr->key() );
      while ( 0 != part ) {
	int pidmc = part->particleID().pid();
	debug()<< "pidmc " << pidmc <<endreq;
	if(part->mother()) debug()<< "mother "
				  <<part->mother()->particleID().pid()<<endreq;
	part = trLink.next();
      }
      
    } 
    */

    //only for those which have not been matched
    Track* pTrCheck = NULL;
    std::vector<Track*>::const_iterator itT1;
    //info()<<m_Muon->size()<<endreq;
    for ( itT1 = m_outputMuonTracks->begin();
          m_outputMuonTracks->end() != itT1; itT1++ ) {
      Track* muonMatched = (*itT1);	  
      const Track* ancestor = *(muonMatched->ancestors().end()-1);	  
      //debug() << "veloTr " << veloTr << "ancestor " << ancestor << endreq;
      
      if( veloTr == ancestor ) {
        pTrCheck = (*itT1);
        //        debug() << "already match " << endreq;
        break;
      }
    } 
    if (!pTrCheck){ 	 
//           debug() << " not already match " << endreq;      
      double slX = veloTr->firstState().tx();
      double slY = veloTr->firstState().ty();   
      double Xstate=veloTr->firstState().x();  
      double Ystate=veloTr->firstState().y();  
      double Zstate=veloTr->firstState().z();  
      //== Ignore uninteresting tracks
      if ( .350 < fabs( slX ) || .300 < fabs( slY ) ) continue;
      
      ymin = 99999.;
      
      float y_cut;
      float x_cut;
      unsigned int region = 9;
      std::vector<Track*>::const_iterator pMuon;
      for(pMuon=m_patInputTracks->begin();
          pMuon<m_patInputTracks->end();pMuon++){
  //      debug()<<  "muon new segment "<<endreq;
        
        Track* muon=(*pMuon);   	
        std::vector<LHCbID> muonTiles=
          muon->lhcbIDs();
        MuonTileID tileM2=muonTiles[0].muonID();
        MuonTileID tileM3=muonTiles[1].muonID();
       
        //std::vector<MuonTileID > muonTiles=
        //  muon->muIDs();
        //MuonTileID tileM2=muonTiles[0];
        //MuonTileID tileM3=muonTiles[1];
        bool sameM3=false;
        std::vector<Track*>::const_iterator itT1;
        Track* pTrCheck = NULL;
        for ( itT1 = m_outputMuonTracks->begin(); 
              m_outputMuonTracks->end() != itT1; itT1++ ) {
          pTrCheck = (*itT1);
          const Track& track = *(*itT1);
          MuonTileID idtileL0;
          // std::vector<LHCbID>::const_iterator imuid;
          const std::vector<LHCbID> lhcbID=track.lhcbIDs();
          unsigned int nIDs=track.nLHCbIDs () ;
          idtileL0=(lhcbID[nIDs-1]).muonID();
          
          //        debug() << "L0 tileid " << idtileL0 << "tile from hits " 
          //      <<tileM3 << endreq;
          sameM3=false;
          //Loop over muon segments reconstructed here
          if (tileM3 == idtileL0 )sameM3=true;
          
          if (sameM3) break;
        }//itT1 = m_Muon->begin(); m_Muon->end() != itT1; itT1++ 
        if (sameM3) continue;
        unsigned int muon_region=3;
        muon_region=tileM2.region();
        y_cut=m_y_cut_track[muon_region];
        x_cut=m_x_cut_track[muon_region];
        
	
        double xM2,xM3,yM2,yM3,zM2,zM3,dx,dy,dz;
        
        m_iPosTool->calcTilePos(tileM2,xM2,dx,yM2,dy,zM2,dz);
        m_iPosTool->calcTilePos(tileM3,xM3,dx,yM3,dy,zM3,dz);
        
        double x_muon_magnet=xM2-(xM3-xM2)/(zM3-zM2)*(zM2-z_magnet_center) ;
        
        double y_muon_point=yM2;
        
	
	
        float x_distance=
          ((slX*(z_magnet_center-Zstate)+Xstate)-x_muon_magnet);
        float y_distance=
          ((slY*(zM2-Zstate)+Ystate)-y_muon_point);          
        
        if(fabs(x_distance)>x_cut)continue;
        if(fabs(y_distance)>y_cut)continue;
	
        double aux;
        if (fabs(y_distance) < fabs(ymin)){
          
          aux = ymin;
          ymin = y_distance; 
          tileIDMatched = tileM3;
          tileIDMatchedM2 = tileM2;

          double xprojM3 =  Xstate+ (slX *(zM3 - Zstate));
          double xkick = xM3 - xprojM3;
          double pRec = m_ptkickConstant * 
            (zM3 - z_magnet_center) /xkick;
          
          if (0. != pRec) qp = 1./ (pRec);
          region = muon_region;
          
        }
        
	
	
      }//for(itMuonTrack=m_muonTracks.begin()
      
      if (fabs(ymin) < 99999.){
        if(fabs(ymin) <  m_y_cut_track[region]  ) {
	  
	  
          //          debug() << " Matched track " << veloTr << endreq;
          //debug() << "tileIDMatched " << tileIDMatched <<
          //  "ymin " << ymin <<
          //  " qp " << 1/qp << endreq;
	  
          //Fill 3 vectors: veloTr muon ydistance
	  
          m_matchedTracks.push_back(veloTr);
          m_muon.push_back(tileIDMatched);
          m_muonM2.push_back(tileIDMatchedM2);
          m_ydistance.push_back(ymin);
          m_qp.push_back(qp);
          
          
          //	   fillHisto(m_histoXdist, xmin,1.);
          //fillHisto(m_histoYdist, ymin,1.);
	  
        }//if(fabs(ymin) <  m_y_cut_track[region]    
      } //if (fabs(ymin) < 99999.)
    }//if (!pTrCheck)
    
  }//for ( pItr = m_inputTracks->begin() 
  





  
  //sort by tileIDMatched
//  debug() << "m_matchedTracks size "<< m_matchedTracks.size()<< endreq; 
  if( m_matchedTracks.size()==0) return StatusCode::SUCCESS;
  if( m_matchedTracks.size()>1){
    for (unsigned int i=0;i<m_matchedTracks.size()-1;i++){
      bool change = false;
      int min = m_muon[i];
      int indexmin=0;
      for (unsigned int j=i+1;j<m_matchedTracks.size();j++){
        if((int)m_muon[j]< min){
          min=m_muon[j];
          indexmin = j;
          change = true;
        } 
      }
      if (change){
        m_muon[indexmin] = m_muon[i];
        m_muon[i] = min; 
        int  mtemp = m_muonM2[i];
        m_muonM2[i] = m_muonM2[indexmin];
        m_muonM2[indexmin] = mtemp;
	
        Track* temp = m_matchedTracks[i];
        m_matchedTracks[i] = m_matchedTracks[indexmin];
        m_matchedTracks[indexmin] = temp;
	
        double ytemp = m_ydistance[i];
        m_ydistance[i] = m_ydistance[indexmin];
        m_ydistance[indexmin] = ytemp;
	
        double qptemp = m_qp[i];
        m_qp[i] = m_qp[indexmin];
        m_qp[indexmin] =  qptemp;
      } //if (change)
    } //for (int i=0;i<m_matchedTracks.size()-1
  }//if( m_matchedTracks.size()>1) 
  
  //debug() << "ordered " << endreq;
  for (unsigned int i=0;i<m_matchedTracks.size();i++){
    
    //debug() << m_muon[i] <<
    // "  ymin " << m_ydistance[i] <<
    // " qp " << 1/m_qp[i] << endreq;     
    
    
  }
  
  double  ymin1 ;
  double  ymin2 ;
  
  
  //Chose the two smallest ydist
  std::vector<bool> keep; 
  keep.reserve(50);
  // keep.reserve[m_matchedTracks.size()];
  keep.clear();
  





  for (unsigned int i=0;i<m_matchedTracks.size();i++){
    keep.push_back(false);
    
    //keep[i]=false;
  } 
  double aux;  
  int indaux;

  for (unsigned int i=0;i<m_matchedTracks.size();i++){
    MuonTileID iPreviousId = m_muon[i];
    if( i >= m_matchedTracks.size()) continue; 
    if( m_matchedTracks.size()== i+1){
      keep[i]=true;
      continue;
    }
    if( m_muon[i+1] != iPreviousId){
      keep[i]=true; 
      continue;
    } 
    ymin1 = fabs(m_ydistance[i]);
    ymin2 = 99999.;
    int ind1=i;
    int ind2=0;
    while (i != m_matchedTracks.size()-1 &&  
           m_muon[i+1] == iPreviousId){
      if(fabs(m_ydistance[i+1]) < fabs(ymin1)){
        aux = ymin1;
        indaux = ind1;
        ymin1 = m_ydistance[i+1];   
        ind1 = i+1;
        if(fabs(aux) < fabs(ymin2)){
          ymin2 = aux;
          ind2 = indaux;
        }
      } 
      else{
        if (fabs(m_ydistance[i+1]) < fabs(ymin2)){
          ymin2 = m_ydistance[i+1];
          ind2 = i+1;
        }  
      }  
      i++;   
    }//while (i .ne. m_matchedTracks.size()-1 	    
    keep[ind1]=true;
    keep[ind2]=true;
    
  }//for (i=0;i<m_matchedTracks.size();i++)
  //debug()<< "Saved " << endreq; 
  


  for (unsigned int i=0;i<m_matchedTracks.size();i++){
    
    if(keep[i]){
      for ( pItr = m_inputTracks->begin(); m_inputTracks->end() != pItr; 
            pItr++ ){
        Track* veloTr = (*pItr);
        if ( veloTr == m_matchedTracks[i]){
          //m_myState->reset();
          double xM2,xM3,yM2,yM3,zM2,zM3,dx,dy,dz;
          
          m_iPosTool->calcTilePos(m_muonM2[i],xM2,dx,yM2,dy,zM2,dz);
          m_iPosTool->calcTilePos(m_muon[i],xM3,dx,yM3,dy,zM3,dz);
          debug()<<" state parameters "<<m_qp[i]<<endreq;

          m_myState->setState( xM2, 
                               yM2,
                               zM2,
                               (xM3-xM2)/(zM3-zM2), 
                               (yM3-yM2)/(zM3-zM2),
                               m_qp[i] );
          //(xM3-xM2)/(zM3-zM2);
          
          m_myState->setLocation( State::Muon );
          Track* outTr = m_outputMuonTracks->newEntry();
          outTr->copy(*veloTr);
          outTr->addToLhcbIDs(m_muonM2[i]); 
          outTr->addToLhcbIDs(m_muon[i]); 
          // add track state
          //debug()<<" qui "<<endreq;
//          debug()<<" match muonseg "<<m_qp[i]<<m_myState->p()<<endreq;
          
          outTr->addToStates( *m_myState);
          outTr->firstState().setQOverP(m_myState->qOverP());
          outTr->setFlag(Track::PIDSelected,true);
          outTr->setHistory( Track::TrackMatching );
          outTr->addToAncestors( veloTr );
          //float xmatch=
          fillHisto(m_histoYdist, m_ydistance[i],1.);
          //fillHisto(m_histoXdist, m_ydistance[i],1.);
          //outTr->firstState().setLocation( State::Muon );
          //debug() << m_muon[i] << "ydist " << m_ydistance[i] 
          //       << "ptr " << veloTr << " qp " << 1/m_qp[i] << endreq;
	  
	  
          continue;
        } 
	
      } //for ( pItr = m_inputTracks->begin()
      
    } //if(keep[i])
  } //for (int i=0;i<m_matchedTracks.size();



  return StatusCode::SUCCESS;
  
};



