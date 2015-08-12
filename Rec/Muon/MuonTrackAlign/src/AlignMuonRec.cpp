// Include files 

#include "Event/RawEvent.h"
#include "Event/Track.h" 
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "TrackInterfaces/ITrackExtrapolator.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiUtils/Aida2ROOT.h"
// local
#include "AlignMuonRec.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignMuonRec
//
// 2007-07-20 : Alessia Satta
//            : + Simone Brusa, Silvia Pozzi, Stefania Vecchi 
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( AlignMuonRec )

using namespace LHCb; 
using namespace Gaudi::Units;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignMuonRec::AlignMuonRec( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiHistoAlg  ( name , pSvcLocator )
  //  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MeasureTime"                     , m_measureTime                      = true );
  declareProperty( "CloneKiller"                     , m_cloneKiller                      = true );
  //declareProperty( "StoreTracks"                   , m_storeTracks                      = true );
  declareProperty( "OutputMuonTracksName"            , m_outputMuonTracksName             = "/Event/Rec/Muon/MuonsForAlignment");
  
  declareProperty( "OutputMuonTracksForAlignmentName", m_outputMuonTracksForAlignmentName = "/Event/Rec/Muon/MuonsTrackForAlignment");
  declareProperty( "M1Station"                       , m_m1Station                        = true );   
  declareProperty( "MatchChisq"                      , m_matchChisq                       = 40.);     
  declareProperty( "DecodingFromCoord"               , m_decodingFromCoord                = true );
  declareProperty( "BField"                          , m_Bfield                           = true );
  declareProperty( "Extrapolator"                    , m_extrapolatorName                 = "TrackMasterExtrapolator" );
  declareProperty( "Histos"                          , m_Histo                            = true );
  
}
//=============================================================================
// Destructor
//=============================================================================
AlignMuonRec::~AlignMuonRec() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlignMuonRec::initialize() { 
  //StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;   // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endmsg;
  
  ///M5 pad size 
  PADsizeX[0] = 31.6667;
  PADsizeX[1] = 63.0;
  PADsizeX[2] = 126.0;
  PADsizeX[3] = 252.0;

  PADsizeY[0] = 38.8585;
  PADsizeY[1] = 77.9582;
  PADsizeY[2] = 156.158;
  PADsizeY[3] = 312.557;  


  // Counters
  m_countEvents = 0;
  m_countMuCandidates = 0;
  if(m_Histo){
    m_Chi2match=book(5, "   chi2 Muon to Long track match", 0.,50.,50);
    m_tx= book(6, " SlopeTx Muon track ",-0.4,0.4,100);
    m_ty= book(7, " SlopeTy Muon track ",-0.4,0.4,100);
    m_p= book(8, "  momenta Long track ",-0.5,100.5,101);
    m_states=book(9, " number of states Muon Track",-0.5,15.5,16);
    m_nMuonTrack=book(10, " number of Muon candidates per event",-1.,10.,11);
    m_nMuonTrackNoClone=book(11, " number of NON Clone Muon candidates per event",-0.5,10.5,11);
    m_nMuonTrackMatch=book(12, " number of Muon candidates per event MATCHED to a Long Track",-0.5,10.5,11);
    
    m_Mfirst=book2D(13," 2D hit distribution on the first Muon Station", -400, 400, 400, -400, 400, 400);
    m_hitM=book2D(14," hits vs station ", 0.5, 5.5, 5 ,-0.5, 10.5, 11);

    for (int i = 0; i<5; i++){    
      for(int j = 0; j<4; j++){
        
	m_resx[i][j] = book2D(100+i*10+j, " x residuals distribution vs x", -400, 400, 400, -500, 500, 500);
	m_resy[i][j] = book2D(200+i*10+j, " y residuals distribution vs y", -400, 400, 400, -500, 500, 500);
      }
    }
    
  }
  //== Timing information
  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timer->increaseIndent();
    m_timeLoad = m_timer->addTimer( "Load from Buffer" );
    m_timePad  = m_timer->addTimer( "Make pad from strip" );   
    m_timeMuon  = m_timer->addTimer( "Reconstruct muons" );
    m_timeMuonStore  = m_timer->addTimer( "Store muons" );
    m_timer->decreaseIndent();
  }
  
  m_onTES        = true;  
  
  m_extrapolator      = tool<ITrackExtrapolator>( m_extrapolatorName, "Extrapolator",this );
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if (!m_muonBuffer) info()<<"error retrieving the muon raw buffer tool "
			  <<endmsg;
  
  //retrieve the pointer to the muon detector 
  m_muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");

  //calculate the transverse momentum  
  m_fCalcMomentum = tool<ITrackMomentumEstimate>("TrackPtKick");
  debug() << "In init, PTKick from geometry " << endmsg;
  

  
  //all geometry stuff
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_nStation= basegeometry.getStations();
  m_nRegion = basegeometry.getRegions();
  
  //create container for pad position
  for (unsigned int station=0;station<m_nStation;station++){
    m_station.push_back( AlignMuonStationRec(msgSvc(),randSvc()));
    m_station[station].initialize();
    for (unsigned int region = 0 ; region < m_nRegion ; region++ ){      
      AlignMuonRegion* reg = new AlignMuonRegion();
      //set some parameters 
      m_station[station].region().push_back( reg );
    }
  }
  
  //set some useful info about the layout of the muon system
  
  //initialze the decoding of the raw buffer
  
  //set muon station position
  
  for (unsigned int station=0;station<m_nStation;station++){
    
    double zSta= m_muonDetector->getStationZ(station);
    
    m_station[station].setZ( zSta );

    //set cut for muon reconstruction
    debug() << " station " << endmsg;
    if (station==0){
      m_station[station].setMaxY(30.0,0);
      m_station[station].setMaxX(100.0,0);
      m_station[station].setMaxY(60.0,1);
      m_station[station].setMaxX(200.0,1); 
      m_station[station].setMaxY(100.0,2);
      m_station[station].setMaxX(300.0,2);
      m_station[station].setMaxY(150.0,3);
      m_station[station].setMaxX(400.0,3);
    }
    if (station==1){
      m_station[station].setMaxY(60.0,0);
      m_station[station].setMaxX(100.0,0);
      m_station[station].setMaxY(120.0,1);
      m_station[station].setMaxX(200.0,1);
      m_station[station].setMaxY(180.0,2);
      m_station[station].setMaxX(300.0,2);
      m_station[station].setMaxY(240.0,3);
      m_station[station].setMaxX(400.0,3);
    }
    if (station==2){
      m_station[station].setMaxY(60.0,0);
      m_station[station].setMaxX(100.0,0);
      m_station[station].setMaxY(120.0,1);
      m_station[station].setMaxX(200.0,1);
      m_station[station].setMaxY(240.0,2);
      m_station[station].setMaxX(300.0,2);
      m_station[station].setMaxY(480.0,3);
      m_station[station].setMaxX(400.0,3);
    }
    if (station==3){
      m_station[station].setMaxY(60.0,0);
      m_station[station].setMaxX(400.0,0);
      m_station[station].setMaxY(120.0,1);
      m_station[station].setMaxX(400.0,1);
      m_station[station].setMaxY(240.0,2);
      m_station[station].setMaxX(400.0,2);
      m_station[station].setMaxY(480.0,3);
      m_station[station].setMaxX(400.0,3);
    }    
  }  
  m_maxMuonFound=10; 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignMuonRec::execute() {
  debug() << "==> Execute" << endmsg;
  
  //setFilterPassed(true);
  
  LHCb::Tracks* outputTracks  = new LHCb::Tracks();
  outputTracks->reserve(10);
  put(outputTracks, m_outputMuonTracksForAlignmentName);
 
  int m_countMuonCandidates=0;
  
  m_countEvents += 1;

  for (unsigned  station = 0 ; station < m_nStation ; station++ ){
    m_station[station].clearRegions();
  }
  createCoords();
  
  muonSearch();   //find the tracks of muon candidates
  
  if (m_cloneKiller)    detectClone();   //defines a clone if two traks share pads on M2,M3,M4 
  
  if (m_cloneKiller)    strongCloneKiller(); //defines a clone if two traks share pads on M2,M3


  LHCb::Tracks* bestCont = get<Tracks>(TrackLocation::Default ); 
  
  m_countMuCandidates += m_muonTracks.size();
  debug()<<"m_muonTracks.size() "<<m_muonTracks.size()<<endmsg;

  // create the container for storing muon tracks
  LHCb::Tracks* muonsContainer = new LHCb::Tracks;
  muonsContainer->reserve(10);
  put(muonsContainer, m_outputMuonTracksName);

  std::vector<AlignMuonTrack>::iterator itMuonTrack;
  for (itMuonTrack=m_muonTracks.begin();
       itMuonTrack<m_muonTracks.end();itMuonTrack++){
    if (!(itMuonTrack)->clone()){

      double x[2] = {0.,0};
      double y[2] = {0.,0};
      double z[2] = {0.,0};
      double err2x[2] = {0.,0};
      double err2y[2] = {0.,0};
      unsigned istation[2]={0,4};
      float xm = 0;
      float ym = 0;
      float zm = 0;      
      unsigned ista = 0;
  

      m_countMuonCandidates++;
      LHCb::Track* trgTr = new Track();
      
      trgTr->setType(Track::Muon);
      
      double Tilex, Tiley, Tilez, Tiledx, Tiledy, Tiledz;

      if (!m_m1Station) istation[0]= 1;
      for (int i = 0; i < 2; i++ ){
	const std::vector<AlignMuonPoint>& vpoint = itMuonTrack -> points(istation[i]);  
	for (std::vector<AlignMuonPoint>::const_iterator iV = vpoint.begin(); iV != vpoint.end(); iV++) {
	  LHCb::MuonTileID padTile = (*iV).tile();
	  m_muonDetector -> Tile2XYZ(padTile, Tilex, Tiledx, Tiley, Tiledy, Tilez, Tiledz);
	  
	  x[i] += (*iV).x()/(vpoint.size());
	  y[i] += (*iV).y()/(vpoint.size());
	  z[i] += (*iV).z()/(vpoint.size());
	  err2x[i] += pow(Tiledx,2.)/(vpoint.size());
	  err2y[i] += pow(Tiledy,2.)/(vpoint.size());
	  
	  verbose() << " vpoint.size() = " << vpoint.size() 
		    << "\n (*iV).x() = " << (*iV).x()
		    << "\n xFirst = " << x[i]
		    << "\n (*iV).y() = " << (*iV).y()
		    << "\n yFirst = " << y[i]
		    << "\n (*iV).z() = " << (*iV).z()
		    << "\n zFirst = " << z[i]
		    << "\n pow(Tiledx,2.) = " <<  pow(Tiledx,2.)
		    << "\n err2xFirst = " << err2x[i]
		    << "\n pow(Tiledy,2.) = " <<  pow(Tiledy,2.)
		    << "\n err2yFirst = " << err2y[i] << endmsg;
	  
	  debug() << " ista = " << istation[i] << endmsg;
	  
	}
      }

      double txref = ( x[0] - x[1] ) / ( z[0] - z[1] );
      double tyref = ( y[0] - y[1] ) / ( z[0] - z[1] );  
      verbose() << "slope muon x,y = " << txref << " " << tyref << endmsg;


      // Add the long track
      LHCb::Tracks::const_iterator iterT;
      LHCb::State bestState;
      LHCb::Track* bestTrack;
      LHCb::Track* mytr = new Track();
  
      bool flag = false; 
      double bestChisq = 10000.;
      double Chisq = 0;

      //Processing LongTrack for matching with muon

      for ( iterT = bestCont->begin(); iterT != bestCont->end(); iterT++){
	Track* testTrack = *iterT;

	if (testTrack->type() != LHCb::Track::Long)  continue;
	
	bestState = testTrack -> closestState(z[0]); 
	if( z[0] - bestState.z() > 10/mm ){
	  LHCb::ParticleID pid(13);
	  StatusCode sc = m_extrapolator->propagate(bestState,z[0],pid);
	  if(sc.isFailure() )   debug() << "Extrapolating to zFirst = " << z[0] << " failed "<<endmsg;
	}
	bestTrack = testTrack;
	verbose() << "bestState = " << bestState << endmsg;
		
	double slopeLong_x = bestState.tx();
	double slopeLong_y = bestState.ty();
	double slopeMuon_x = txref;
	double slopeMuon_y = tyref; 
	double err2slopex, err2slopey;
	double err2_x, err2_y;

	err2_x = err2x[0] + bestState.errX2();
	err2_y = err2y[0] + bestState.errY2();
	err2slopex = (err2x[1] + err2x[0])/pow(z[1]-z[0],2.) + bestState.errTx2() ;
	err2slopey = (err2y[1] + err2y[0])/pow(z[1]-z[0],2.) + bestState.errTy2();
	
	Chisq = pow(slopeMuon_x - slopeLong_x,2.)/err2slopex 
	  + pow(slopeMuon_y - slopeLong_y,2.)/err2slopey
	  + pow(x[0]-bestState.x(),2.)/err2_x 
	  + pow(y[0]-bestState.y(),2.)/err2_y;

	if (Chisq < m_matchChisq && Chisq < bestChisq) {
	  verbose() << " m_matchChisq = " <<  m_matchChisq << endmsg;
	  debug() << "Match found: Chisq = " << Chisq << endmsg;
	  flag = true;
	  mytr = bestTrack->clone();
	  bestChisq =  Chisq;
	}else {
	  debug() << "Chisquare too big " << Chisq << endmsg;
	}
      }

      if (flag) {
	trgTr->addToStates( bestState);
      }

      for (ista = istation[0]; ista < m_nStation; ista++){
	debug() <<" new muon track passing by station "<<ista+1<<endmsg;
	std::vector<AlignMuonPoint> vpoint = itMuonTrack->points(ista);
	m_hitM->fill(ista+1,vpoint.size());
	for(std::vector<AlignMuonPoint>::const_iterator iV=vpoint.begin(); iV != vpoint.end(); iV++) {
	  
	  xm=(float)(*iV).x();       
	  ym=(float)(*iV).y();
	  zm=(float)(*iV).z();
	  
	  LHCb::State temp;
	  temp.setZ(zm);
	  temp.setX(xm);
	  temp.setY(ym);
	  temp.setTx(txref);
	  temp.setTy(tyref);
	  if (flag) {
	    temp.setQOverP( bestState.qOverP() ) ;
	  } else {
	    temp.setQOverP( 1 / 10000.) ;
	    double qOverP, sigmaQOverP;
	    m_fCalcMomentum->calculate(&temp, qOverP, sigmaQOverP);
	    temp.setQOverP(qOverP);
	    temp.setErrQOverP2(sigmaQOverP*sigmaQOverP);
	  }
	  
	  temp.setLocation(LHCb::State::EndVelo);
	  trgTr->addToStates( temp );
	  trgTr->addToLhcbIDs((*iV).tile());            	  

	  mytr->addToStates(temp);
	  if (flag) mytr->addToLhcbIDs((*iV).tile());
	  if (!flag) mytr->setType(Track::Muon);
	  
	  std::vector<LHCb::LHCbID> IDs;
	  std::vector<LHCb::LHCbID>::const_iterator itID;
	  
	}
      }
  
      m_Chi2match->fill(Chisq);   // Chisquare of the match Long to Muon track
      m_tx ->fill(txref);
      m_ty ->fill(tyref);
      m_Mfirst->fill(x[0]/cm,y[0]/cm);
      double momenta = 0.;
      if (flag) momenta=mytr->p()/GeV;
      m_p->fill(momenta);
      m_states->fill(trgTr->nStates());

      debug() << " # states in the long track = " << mytr->nStates() << endmsg;
      debug() << " # of IDs in the long track = " << mytr->nLHCbIDs() << endmsg;
      debug() << " long track type = " <<  mytr->type() << endmsg;
      
      trgTr->setType( Track::Muon );      
      trgTr->setPatRecStatus( Track::PatRecIDs ) ; 
      mytr->setPatRecStatus( Track::PatRecIDs ) ;
      muonsContainer->insert(trgTr);
      if(flag) outputTracks->insert(mytr);

      
      for (ista = istation[0]+1; ista< istation[1]; ista++){
	
	std::vector<AlignMuonPoint> vpoint = itMuonTrack->points(ista);
	for (std::vector<AlignMuonPoint>::const_iterator iV=vpoint.begin(); iV != vpoint.end(); iV++) {

	 double resx = (*iV).x() - (txref *((*iV).z()- z[0]) + x[0]);
	 double resy = (*iV).y() - (tyref *((*iV).z()- z[0]) + y[0]);
	 unsigned region = (*iV).tile().region();
	 m_resx[ista][region]->fill((*iV).x()/cm,resx);
	 m_resy[ista][region]->fill((*iV).y()/cm,resy);
	}
      }
    }  
  }                                                 ///----------  end loop on muontracks	

  m_nMuonTrack->fill(m_muonTracks.size());          //  All Muon tracks found in muonSearch
  m_nMuonTrackNoClone->fill(m_countMuonCandidates); //  Muon tracks not clone
  m_nMuonTrackMatch->fill(outputTracks->size()); //  Muon tracks not clone

  
  m_muonTracks.clear();
  debug()<<" stored candidates "<<m_countMuonCandidates<<endmsg; 
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlignMuonRec::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  
  debug () << "Number of events processed: " << double(m_countEvents) << endmsg;
  debug () << "Average number of muon tracks: " 
          << double(m_countMuCandidates)/double(m_countEvents) << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=============================================================================
StatusCode AlignMuonRec::muonSearch() {
  m_muonTracks.clear();
  
  std::vector<AlignMuonPoint>::iterator itM5, itM5m, CT_itM5, itM5n, itCT_M5points;
  std::vector< AlignMuonPoint > 	CT_Points[5], Points[5];
  
  for (unsigned int region=0;region<m_nRegion;region++){
    AlignMuonRegion* thisRegion = m_station[4].region(region);
    for(itM5=thisRegion->points().begin(); itM5<thisRegion->points().end(); itM5++){	
      Points[4].push_back(*itM5);
    }
  }
  
  for( itM5m = Points[4].begin(); itM5m<Points[4].end(); itM5m++){
    
    std::vector<AlignMuonPoint> CandidateM5;
    CandidateM5.push_back((*itM5m));
    //search coincidence in M4
    std::vector<AlignMuonPoint> CandidateM4;
    std::vector<AlignMuonPoint> CT_M5points;
	
    bool CT_flag = true;
    
    for( CT_itM5 = CT_Points[4].begin(); CT_itM5<CT_Points[4].end(); CT_itM5++){
      debug() <<" Hit already used for another seed"<<endmsg;
      if ((*CT_itM5).tile()==(*itM5m).tile()) CT_flag = false;
    }
          
    if(CT_flag==false) continue; //remove if you want to consider also XTalk pad as new seed
    
    float M5x = (float)(*itM5m).x();
    float M5y = (float)(*itM5m).y();
    float n = 1;
    
    for( itM5n = itM5m+1; itM5n < Points[4].end(); itM5n++){
      if(fabs((*itM5m).x()-(*itM5n).x())<0.55*(PADsizeX[(*itM5m).tile().region()]+PADsizeX[(*itM5n).tile().region()])
         && fabs((*itM5m).y()-(*itM5n).y())<0.55*(PADsizeY[(*itM5m).tile().region()]+PADsizeY[(*itM5n).tile().region()])){
	M5x+=(float)(*itM5n).x();
	M5y+=(float)(*itM5n).y();
	n++;
        CT_Points[4].push_back(*itM5n);
	CT_M5points.push_back(*itM5n); 
        CandidateM5.push_back((*itM5n));
      }
    }
    
    CT_M5points.push_back(*itM5m);

    double x = M5x/n * m_station[3].z()/m_station[4].z();
    double y = M5y/n * m_station[3].z()/m_station[4].z();
    
    debug() <<(*itM5).x()<<" "<<M5x/n<<endmsg; 
    debug() <<(*itM5).y()<<" "<<M5y/n<<endmsg;
    
    double distance = 0;
    double mindistance = sqrt(pow((PADsizeX[0]+PADsizeX[1]+PADsizeX[2]+PADsizeX[3]),2)+
			      pow((PADsizeY[0]+PADsizeY[1]+PADsizeY[2]+PADsizeY[3]),2));
    unsigned int regionM5 = (*itM5m).tile().region();
    
    for(itCT_M5points = CT_M5points.begin(); itCT_M5points<CT_M5points.end(); itCT_M5points++){
      distance=sqrt(pow((M5x/n-(*itCT_M5points).x()),2)+pow((M5y/n-(*itCT_M5points).y()),2));
      if(distance<mindistance){
	mindistance=distance;
	regionM5=(*itCT_M5points).tile().region();
      }
    }
    
    debug() <<" +++++++++++++++++++ new seed "<<endmsg;          
    debug() <<"seed "<<M5x/n<<" "<<M5y/n<<" REGION "<< regionM5<<endmsg;          
    m_station[3].findCoincidence(x,y,regionM5,CandidateM4);
    if(CandidateM4.size()==0) continue;
    //if(CandidateM4.size()>1) continue;         //--- uncomment to avoid cross talk hits
    AlignMuonPoint* bestCandidateM4=&CandidateM4[0];
    debug() << "M4 concidence  x = " << x << " y = "<< y
	    << " BCx = "<< bestCandidateM4->x()
	    << " BCy = "<< bestCandidateM4->y() <<endmsg;
      
      x=-(M5x/n-bestCandidateM4->x())+bestCandidateM4->x(); 
      y=(bestCandidateM4)->y()*m_station[2].z()/m_station[3].z();

      std::vector<AlignMuonPoint> CandidateM3;
      unsigned int regionM4=(bestCandidateM4)->tile().region();
      m_station[2].findCoincidence(x,y,regionM4,CandidateM3);
      if(CandidateM3.size()==0) continue;      
      //if(CandidateM3.size()>1) continue;               //--- uncomment to avoid cross talk hits
      AlignMuonPoint* bestCandidateM3=&CandidateM3[0];
      debug() << "M3 concidence  x = " << x << " y = " << y
	      << " BCx = " << bestCandidateM3->x()
	      << " BCy = " << bestCandidateM3->y() <<endmsg;

      if(m_Bfield){
        x=-(bestCandidateM4->x()-bestCandidateM3->x())+bestCandidateM3->x();
      }
      else{
        x=(bestCandidateM3)->x()*m_station[1].z()/m_station[2].z();
      }
      
      y=(bestCandidateM3)->y()*m_station[1].z()/m_station[2].z();

      std::vector<AlignMuonPoint> CandidateM2;
      unsigned int regionM3=(bestCandidateM3)->tile().region();
      m_station[1].findCoincidence(x,y,regionM3,CandidateM2);
      if(CandidateM2.size()==0) continue;
      //if(CandidateM2.size()>1) continue;         //--- uncomment to avoid cross talk hits
      AlignMuonPoint* bestCandidateM2=&CandidateM2[0];
      debug() << "M2 concidence  x = " << x << " y = " << y
	      << " BCx = " << bestCandidateM2->x()
	      << " BCy = " << bestCandidateM2->y() <<endmsg;
      
      x=-(2.55 * (bestCandidateM3->x()-bestCandidateM2->x()))+bestCandidateM2->x();
      y=(bestCandidateM2)->y()*m_station[0].z()/m_station[1].z();

      std::vector<AlignMuonPoint> CandidateM1;
    
      if (m_m1Station){
	unsigned int regionM2=(bestCandidateM2)->tile().region();
	m_station[0].findCoincidence(x,y,regionM2,CandidateM1);
	if(CandidateM1.size()==0) continue;
	//if(CandidateM1.size()>1) continue;         //--- uncomment to avoid cross talk hits
	AlignMuonPoint* bestCandidateM1=&CandidateM1[0];
	debug() << "M1 concidence  x = " << x << " y = " << y
		<< " BCx = " << bestCandidateM1->x()
		<< " BCy = " << bestCandidateM1->y() <<endmsg;
      }
      
      AlignMuonTrack muon;
      if (m_m1Station){
	muon.setPoint(0,CandidateM1);
      }
      muon.setPoint(1,CandidateM2);
      muon.setPoint(2,CandidateM3);
      muon.setPoint(3,CandidateM4);
      muon.setPoint(4,CandidateM5);
     
      m_muonTracks.push_back(muon);      
  }
 
  debug()<<"number of muon Tracks "<<m_muonTracks.size()<<endmsg;
  return StatusCode::SUCCESS;
  
}


StatusCode AlignMuonRec::detectClone()
{
  
  std::vector<AlignMuonTrack>::iterator itMuonTrackFirst;
  std::vector<AlignMuonTrack>::iterator itMuonTrackSecond;
  bool sameM[5];
  for(itMuonTrackFirst=m_muonTracks.begin(); itMuonTrackFirst<m_muonTracks.end(); itMuonTrackFirst++){
    for(itMuonTrackSecond=itMuonTrackFirst+1; itMuonTrackSecond<m_muonTracks.end(); itMuonTrackSecond++){
      
      int Msamepoints[5];
      for (int i=1; i<4; i++) { // look only in M2,M3,M4
	sameM[i]=false;	
       
	Msamepoints[i]=0;

	std::vector<AlignMuonPoint> FirstMpoint = itMuonTrackFirst->points(i);
	for(std::vector<AlignMuonPoint>::iterator iFirstMpoint = FirstMpoint.begin();
	    iFirstMpoint < FirstMpoint.end(); iFirstMpoint++){
	  std::vector<AlignMuonPoint> SecondMpoint = itMuonTrackSecond->points(i);
	  for(std::vector<AlignMuonPoint>::iterator iSecondMpoint = SecondMpoint.begin();
	      iSecondMpoint < SecondMpoint.end(); iSecondMpoint++){
	    
	    if(iFirstMpoint->tile().key() == iSecondMpoint->tile().key() ) {
	      Msamepoints[i]++;	    
	    }
	  }
	}
	
	if(Msamepoints[i]>0) sameM[i] = true;
      }
      
      if(sameM[1]&&sameM[2]&&sameM[3]){
	
	
	double x_extra5=-(itMuonTrackFirst->point(2).x()-itMuonTrackFirst->point(3).x())+itMuonTrackFirst->point(3).x();
	double y_extra5=itMuonTrackFirst->point(3).y()*18800.0/17600.0;
	double distuno=sqrt(pow((itMuonTrackFirst->point(4).x()-x_extra5),2)+pow((itMuonTrackFirst->point(4).y()-y_extra5),2));
	double distdue=sqrt(pow((itMuonTrackSecond->point(4).x()-x_extra5),2)+pow((itMuonTrackSecond->point(4).y()-y_extra5),2));
	if(distuno>distdue){
	  itMuonTrackFirst->setClone();
	  debug()<< " Clone First track "<<endmsg;
	}else{
	  itMuonTrackSecond->setClone();
	  debug()<< " Clone Second track "<<endmsg;
	}
      }
    }
  }
  return StatusCode::SUCCESS;
  
}


StatusCode AlignMuonRec::strongCloneKiller()
{
  std::vector<AlignMuonTrack>::iterator itMuonTrackFirst;
  std::vector<AlignMuonTrack>::iterator itMuonTrackSecond;
  bool sameM[5];

  for(itMuonTrackFirst=m_muonTracks.begin();itMuonTrackFirst<m_muonTracks.end();itMuonTrackFirst++){
    for(itMuonTrackSecond=itMuonTrackFirst+1;itMuonTrackSecond<m_muonTracks.end();itMuonTrackSecond++){
      
      
      int Msamepoints[5];
      for (int i=1; i<3; i++) { // look only in M2,M3
	sameM[i]=false;

	Msamepoints[i]=0;

	std::vector<AlignMuonPoint> FirstMpoint = itMuonTrackFirst->points(i);
	for(std::vector<AlignMuonPoint>::iterator iFirstMpoint = FirstMpoint.begin();
	    iFirstMpoint < FirstMpoint.end(); iFirstMpoint++){
	  std::vector<AlignMuonPoint> SecondMpoint = itMuonTrackSecond->points(i);
	  for(std::vector<AlignMuonPoint>::iterator iSecondMpoint = SecondMpoint.begin();
	      iSecondMpoint < SecondMpoint.end(); iSecondMpoint++){
	    if(iFirstMpoint->tile().key() == iSecondMpoint->tile().key()) {
	      Msamepoints[i]++;	    
	    }
	  }
	}
	
	if(Msamepoints[i]>0) sameM[i] = true;
	
      }
      
      if(sameM[1]&&sameM[2]){
	double x_extra5 = -(itMuonTrackFirst->point(1).x()- itMuonTrackFirst->point(2).x())*2
	  + itMuonTrackFirst->point(2).x();
	double y_extra5 = itMuonTrackFirst->point(2).y()*18800.0/16400.0;
	
	double distuno=sqrt(pow((itMuonTrackFirst->point(4).x()-x_extra5),2)+pow((itMuonTrackFirst->point(4).y()-y_extra5),2));
	double distdue=sqrt(pow((itMuonTrackSecond->point(4).x()-x_extra5),2)+pow((itMuonTrackSecond->point(4).y()-y_extra5),2));
	
	if(distuno>distdue){
	  itMuonTrackFirst->setClone();
	  debug()<<" kill first "<<itMuonTrackFirst->point(4).x()<<endmsg;
	}else{
	  itMuonTrackSecond->setClone();  
	  debug()<<" kill second "<<itMuonTrackSecond->point(4).x()<<endmsg;
	}       
      }     
    }
  }
  return StatusCode::SUCCESS;
  
}

StatusCode AlignMuonRec::printOut()
{
  
  std::vector<AlignMuonTrack>::iterator itMuonTrack;
  for (itMuonTrack=m_muonTracks.begin();
       itMuonTrack<m_muonTracks.end();itMuonTrack++){
    debug() << itMuonTrack->clone() << " points along the tracks "
	    << itMuonTrack->point(1).x() << " " 
	    << itMuonTrack->point(1).y() << " " 
	    << itMuonTrack->point(2).x() << " " << itMuonTrack->point(2).y() << " " 
	    << itMuonTrack->point(3).x() << " " << itMuonTrack->point(3).y() << " " 
	    << itMuonTrack->point(4).x() << " " << itMuonTrack->point(4).y() << endmsg;
  }  
  return StatusCode::SUCCESS;
  
}

StatusCode AlignMuonRec::createCoords()
{
  
  std::vector<LHCb::MuonTileID> decodingPads;
  StatusCode sc=m_muonBuffer->getPads(decodingPads);
  if (sc.isFailure()) return sc;
  
  std::vector<LHCb::MuonTileID>::iterator it;
  
  for (it=decodingPads.begin(); it<decodingPads.end(); it++){
    LHCb::MuonTileID padTile= *it;
    double x,y,z,dx,dy,dz;
    
    m_muonDetector-> Tile2XYZ(padTile, x, dx, y, dy, z, dz);
    int region = padTile.region();
    int station = padTile.station();
    
    AlignMuonRegion* thisRegion = m_station[station].region(region);
    thisRegion->addPoint(x, y, z, padTile); 
    
  }
  std::vector<LHCb::MuonTileID> decodingM1;
  sc=m_muonBuffer->getTile(decodingM1);
  if (sc.isFailure()) return sc;
  
  for (it=decodingM1.begin(); it<decodingM1.end(); it++){
    LHCb::MuonTileID padTile= *it;
    if (padTile.station()!=0) continue; 
    double x,y,z,dx,dy,dz;   
    m_muonDetector-> Tile2XYZ(padTile, x, dx, y, dy, z, dz);
    int region=padTile.region();
    int station=padTile.station();
    AlignMuonRegion* thisRegion = m_station[station].region(region);
    thisRegion->addPoint(x, y, z, padTile);    
  }
  
  return StatusCode::SUCCESS;
  
}

