//
//Muon_OK
//
// $Id: AlignMuonRec.cpp,v 1.4 2008-02-19 15:53:46 spozzi Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RawEvent.h"
#include "Event/Track.h" 
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
// local
#include "AlignMuonRec.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignMuonRec
//
// 2007-07-20 : Alessia Satta
//            : + Simone Brusa, Silvia Pozzi, Stefania Vecchi 
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( AlignMuonRec );

using namespace LHCb; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignMuonRec::AlignMuonRec( const std::string& name,
                  ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MeasureTime"   , m_measureTime   = true );
  declareProperty( "CloneKiller"   , m_cloneKiller   = true );
  //declareProperty( "StoreTracks"   , m_storeTracks   = true );
  declareProperty( "OutputMuonTracksName" ,
		   m_outputMuonTracksName="/Event/Rec/Muon/MuonsForAlignment");

  declareProperty( "OutputMuonTracksForAlignmentName" ,
		   m_outputMuonTracksForAlignmentName="/Event/Rec/Muon/MuonsTrackForAlignment");

  declareProperty( "DecodingFromCoord", m_decodingFromCoord = true );
  declareProperty( "BField", m_Bfield = true );
}
//=============================================================================
// Destructor
//=============================================================================
AlignMuonRec::~AlignMuonRec() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlignMuonRec::initialize() { 
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;   // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endmsg;

  // Counters
  m_countEvents = 0;
  m_countMuCandidates = 0;
  
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

  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_muonBuffer) info()<<"error retrieving the muon raw buffer tool "
			  <<endreq;
  
  //retrieve the pointer to the muon detector 
  m_muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");

  //calculate the transverse momentum  
  m_fCalcPtKick = tool<ITrackPtKick>("TrackPtKick");
  info() << "In init, PTKick from geometry " << endreq;
  


  //all geometry stuff
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_nStation= basegeometry.getStations();
  m_nRegion = basegeometry.getRegions();
  
  //create container for pad position
  for(unsigned int station=0;station<m_nStation;station++){
    m_station.push_back( AlignMuonStationRec(msgSvc(),randSvc()));
    m_station[station].initialize();
    for(unsigned int region = 0 ; region < m_nRegion ; region++ ){      
      AlignMuonRegion* reg = new AlignMuonRegion();
      //set some parameters 
      m_station[station].region().push_back( reg );
    }
  }
  
  //set some useful info about the layout of the muon system

  //initialze the decoding of the raw buffer

  //set muon station position
  
  for(unsigned int station=0;station<m_nStation;station++){
    
    double zSta= m_muonDetector->getStationZ(station);
    
    m_station[station].setZ( zSta );

    //set cut for muon reconstruction
    debug()<<" station "<<endreq;
    if(station==0){
      m_station[station].setMaxY(30.0,0);
      m_station[station].setMaxX(100.0,0);
      m_station[station].setMaxY(60.0,1);
      m_station[station].setMaxX(200.0,1); 
      m_station[station].setMaxY(100.0,2);
      m_station[station].setMaxX(300.0,2);
      m_station[station].setMaxY(150.0,3);
      m_station[station].setMaxX(400.0,3);
    }
    
    if(station==1){
      m_station[station].setMaxY(60.0,0);
      m_station[station].setMaxX(100.0,0);
      m_station[station].setMaxY(120.0,1);
      m_station[station].setMaxX(200.0,1);
      m_station[station].setMaxY(180.0,2);
      m_station[station].setMaxX(300.0,2);
      m_station[station].setMaxY(240.0,3);
      m_station[station].setMaxX(400.0,3);
    }
    if(station==2){
      m_station[station].setMaxY(60.0,0);
      m_station[station].setMaxX(100.0,0);
      m_station[station].setMaxY(120.0,1);
      m_station[station].setMaxX(200.0,1);
      m_station[station].setMaxY(240.0,2);
      m_station[station].setMaxX(300.0,2);
      m_station[station].setMaxY(480.0,3);
      m_station[station].setMaxX(400.0,3);
    }
    if(station==3){
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
};

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

  for(unsigned  station = 0 ; station < m_nStation ; station++ ){
    m_station[station].clearRegions();
  }
  createCoords();
  
  muonSearch();   //find the tracks of muon candidates
  
  if(m_cloneKiller)    detectClone();   //defines a clone if two traks share pads on M2,M3,M4 
  
  if(m_cloneKiller)    strongCloneKiller(); //defines a clone if two traks share pads on M2,M3

  double xM1=0.;
  double yM1=0.;
  double zM1=0.;
  double zM5=0.;
  double err2xM1 = 0.;
  double err2xM5 = 0.;
  double err2yM1 = 0.;
  double err2yM5 = 0.;  
  float xm=0;
  float ym=0;
  float zm=0;

  LHCb::Tracks* bestCont = get<Tracks>(TrackLocation::Default ); 
  
  m_countMuCandidates += m_muonTracks.size();
  debug()<<"m_muonTracks.size() "<<m_muonTracks.size()<<endreq;

  // create the container for storing muon tracks
  LHCb::Tracks* muonsContainer= new LHCb::Tracks;
  muonsContainer->reserve(10);
  put(muonsContainer, m_outputMuonTracksName);

  std::vector<AlignMuonTrack>::iterator itMuonTrack;
  for(itMuonTrack=m_muonTracks.begin();
      itMuonTrack<m_muonTracks.end();itMuonTrack++){
    if(!(itMuonTrack)->clone()){	
      m_countMuonCandidates++;
      LHCb::Track* trgTr =new Track();
      
      trgTr->setType(Track::Muon);

      AlignMuonPoint firstpoint, lastpoint ;
      bool first =true ;
      for(unsigned ista=0;ista<m_nStation;ista++){
	const std::vector<AlignMuonPoint>& vpoint = itMuonTrack->points(ista);
        for(std::vector<AlignMuonPoint>::const_iterator iV=vpoint.begin(); iV != vpoint.end(); iV++) {
	  if(first) firstpoint =*iV ;
	  else      lastpoint = *iV ;
	  first = false ;
	}
      }

      // slope of muon tracks calculated between the first and the last station
      double txref = ( firstpoint.x() - lastpoint.x() ) /  (firstpoint.z() - lastpoint.z() )  ;
      double tyref = ( firstpoint.y() - lastpoint.y() ) /  (firstpoint.z() - lastpoint.z() )  ;

      LHCb::MuonTileID padTileM1= firstpoint.tile();
      LHCb::MuonTileID padTileM5= lastpoint.tile();
      double Tilex, Tiley, Tilez, Tiledx, Tiledy, Tiledz;
      m_muonDetector->Tile2XYZ(padTileM1, Tilex, Tiledx, Tiley, Tiledy, Tilez, Tiledz);
      err2xM1 = pow(Tiledx,2.);
      err2yM1 = pow(Tiledy,2.);
      xM1 = firstpoint.x();
      yM1 = firstpoint.y();
      zM1 = firstpoint.z();
      m_muonDetector->Tile2XYZ(padTileM5, Tilex, Tiledx, Tiley, Tiledy, Tilez, Tiledz);
      err2xM5 = pow(Tiledx,2.);
      err2yM5 = pow(Tiledy,2.);
      zM5 = lastpoint.z();


      // Add the long track
      LHCb::Tracks::const_iterator iterT;
      LHCb::State bestState;
      LHCb::Track* bestTrack;
      LHCb::Track* mytr = new Track();
  
      bool flag = false;
      double bestChisq = 10000.;
      double Chisq = 0;

      //Processing LongTrack for matching with muon

      for( iterT = bestCont->begin(); iterT != bestCont->end(); iterT++){
	Track* testTrack = *iterT;

	if (testTrack->type() != LHCb::Track::Long){
	  continue;
	}
	
	Gaudi::XYZVector slopeLong = testTrack->slopes();

	double slopeLong_x = slopeLong.X();
	double slopeLong_y = slopeLong.Y(); 
	double slopeMuon_x = txref;
	double slopeMuon_y = tyref; 
	double err2slopexMuon,err2slopeyMuon;

	err2slopexMuon = (err2xM5 + err2xM1)/pow(zM5-zM1,2.);
	err2slopeyMuon = (err2yM5 + err2yM1)/pow(zM5-zM1,2.);

	Chisq=pow(slopeMuon_x - slopeLong_x,2.)/err2slopexMuon 
	  + pow(slopeMuon_y - slopeLong_y,2.)/err2slopeyMuon
	  + pow(xM1-testTrack->closestState(zM1).x(),2.)/err2xM1 
	  + pow(yM1-testTrack->closestState(zM1).y(),2.)/err2yM1;

	if (Chisq<40&&Chisq<bestChisq) {
	  debug() << "Match found: Chisq = " << Chisq << endreq;
	  flag = true;

	  bestState = testTrack->closestState(10000);
	  bestTrack = testTrack;
 	  bestChisq =  Chisq;

	  mytr = bestTrack->clone();
	}else{
	  debug() << "Chisquare too big " << Chisq << endreq;
	}

      }
	
      if (flag) {
	trgTr->addToStates( bestState);
      }
      for(unsigned ista=0;ista<m_nStation;ista++){
	debug() <<" new muon track passing by station "<<ista+1<<endreq;
	std::vector<AlignMuonPoint> vpoint = itMuonTrack->points(ista);
	for(std::vector<AlignMuonPoint>::const_iterator iV=vpoint.begin(); iV != vpoint.end(); iV++) {
	  
	  xm=(*iV).x();       
	  ym=(*iV).y();
	  zm=(*iV).z();
	  
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
	    m_fCalcPtKick->calculate(&temp);
	  }
	  
	  temp.setLocation(LHCb::State::EndVelo);

	  trgTr->addToStates( temp );
	  trgTr->addToLhcbIDs((*iV).tile());            	  

	  mytr->addToStates(temp);
	  if(flag) mytr->addToLhcbIDs((*iV).tile());
	  if(!flag) mytr->setType(Track::Muon);
	  
	  std::vector<LHCb::LHCbID> IDs;
	  std::vector<LHCb::LHCbID>::const_iterator itID;
	  
	}
      }
       
      trgTr->setType( Track::Muon );      
      trgTr->setPatRecStatus( Track::PatRecIDs ) ; 
      mytr->setPatRecStatus( Track::PatRecIDs ) ;
      muonsContainer->insert(trgTr);
      if(flag) outputTracks->insert(mytr);
    }
    
  }
  
  m_muonTracks.clear();
  info()<<" stored candidates "<<m_countMuonCandidates<<endreq; 
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AlignMuonRec::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  
  info () << "Number of events processed: " << double(m_countEvents) << endreq;
  info () << "Average number of muon tracks: " 
          << double(m_countMuCandidates)/double(m_countEvents) << endreq;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
};


//=============================================================================

StatusCode AlignMuonRec::muonSearch() {
  m_muonTracks.clear();

  std::vector<AlignMuonPoint>::iterator itM5;
  for (unsigned int region=0;region<m_nRegion;region++){    
    AlignMuonRegion* thisRegion = m_station[4].region(region);
    for(itM5=thisRegion->points().begin();itM5<thisRegion->points().end();
        itM5++){
      //search coincidence in M4
      std::vector<AlignMuonPoint> CandidateM4;
      
      double x=(*itM5).x()*m_station[3].z()/m_station[4].z();
      double y=(*itM5).y()*m_station[3].z()/m_station[4].z();        
      unsigned int regionM5=(*itM5).tile().region();
      debug() <<" +++++++++++++++++++ new seed "<<endreq;          
      debug() <<"seed "<<(*itM5).x()<<" "<<(*itM5).y()<<" REGION "<< regionM5<<endreq;          
      m_station[3].findCoincidence(x,y,regionM5,CandidateM4);
      if(CandidateM4.size()==0)continue;
      if(CandidateM4.size()>1)continue;
      AlignMuonPoint* bestCandidateM4=&CandidateM4[0];
      debug() << "M4 concidence  x = " << x << " y = "<< y
	      << " BCx = "<< bestCandidateM4->x()
	      << " BCy = "<< bestCandidateM4->y() <<endreq;
      
      x=-((*itM5).x()-bestCandidateM4->x())+bestCandidateM4->x(); 
      y=(bestCandidateM4)->y()*m_station[2].z()/m_station[3].z();

      std::vector<AlignMuonPoint> CandidateM3;
      unsigned int regionM4=(bestCandidateM4)->tile().region();
      m_station[2].findCoincidence(x,y,regionM4,CandidateM3);
      if(CandidateM3.size()==0)continue;      
      if(CandidateM3.size()>1)continue;      
      AlignMuonPoint* bestCandidateM3=&CandidateM3[0];
      debug() << "M3 concidence  x = " << x << " y = " << y
	      << " BCx = " << bestCandidateM3->x()
	      << " BCy = " << bestCandidateM3->y() <<endreq;

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
      if(CandidateM2.size()==0)continue;
      if(CandidateM2.size()>1)continue;
      AlignMuonPoint* bestCandidateM2=&CandidateM2[0];
      debug() << "M2 concidence  x = " << x << " y = " << y
	      << " BCx = " << bestCandidateM2->x()
	      << " BCy = " << bestCandidateM2->y() <<endreq;
      
      x=-(2.55 * (bestCandidateM3->x()-bestCandidateM2->x()))+bestCandidateM2->x();
      y=(bestCandidateM2)->y()*m_station[0].z()/m_station[1].z();

      std::vector<AlignMuonPoint> CandidateM1;
      unsigned int regionM2=(bestCandidateM2)->tile().region();
      m_station[0].findCoincidence(x,y,regionM2,CandidateM1);
      if(CandidateM1.size()==0)continue;
      if(CandidateM1.size()>1)continue;
      AlignMuonPoint* bestCandidateM1=&CandidateM1[0];
      debug() << "M1 concidence  x = " << x << " y = " << y
	      << " BCx = " << bestCandidateM1->x()
	      << " BCy = " << bestCandidateM1->y() <<endreq;
      std::vector<AlignMuonPoint> CandidateM5;
      CandidateM5.push_back((*itM5));

      AlignMuonTrack muon;
      muon.setPoint(0,CandidateM1);
      muon.setPoint(1,CandidateM2);
      muon.setPoint(2,CandidateM3);
      muon.setPoint(3,CandidateM4);
      muon.setPoint(4,CandidateM5);
      m_muonTracks.push_back(muon);      
    }    
  }
  info()<<"number of muon Tracks "<<m_muonTracks.size()<<endreq;
  return StatusCode::SUCCESS;
  
};



StatusCode AlignMuonRec::detectClone()
{
 std::vector<AlignMuonTrack>::iterator itMuonTrackFirst;
 std::vector<AlignMuonTrack>::iterator itMuonTrackSecond;
 bool sameM2;
 bool sameM3;
 bool sameM4;
 bool sameM5;
 
 for(itMuonTrackFirst=m_muonTracks.begin();
     itMuonTrackFirst<m_muonTracks.end();itMuonTrackFirst++){
   for(itMuonTrackSecond=itMuonTrackFirst+1;
       itMuonTrackSecond<m_muonTracks.end();itMuonTrackSecond++){
     sameM2=false;
     sameM3=false;
     sameM4=false;
     sameM5=false;
     if(itMuonTrackFirst->point(1).tile()==
        itMuonTrackSecond->point(1).tile())sameM2=true;
     if(itMuonTrackFirst->point(2).tile()==
        itMuonTrackSecond->point(2).tile())sameM3=true;
     if(itMuonTrackFirst->point(3).tile()==
        itMuonTrackSecond->point(3).tile())sameM4=true;
     if(sameM2&&sameM3&&sameM4){
       double x_extra5=-(itMuonTrackFirst->point(2).x()-
                         itMuonTrackFirst->point(3).x())
         +itMuonTrackFirst->point(3).x();
       double y_extra5=itMuonTrackFirst->point(3).y()*18800.0/17600.0;
       double 
	 distuno=sqrt(pow((itMuonTrackFirst->point(4).x()-x_extra5),2)+
         pow((itMuonTrackFirst->point(4).y()-y_extra5),2));
       double 
	 distdue=sqrt(pow((itMuonTrackSecond->point(4).x()-x_extra5),2)+
         pow((itMuonTrackSecond->point(4).y()-y_extra5),2));
       if(distuno>distdue){
         itMuonTrackFirst->setClone();
       }else
         itMuonTrackSecond->setClone();         
     }     
   }
 }
 return StatusCode::SUCCESS;
 
};


StatusCode AlignMuonRec::strongCloneKiller()
{
 std::vector<AlignMuonTrack>::iterator itMuonTrackFirst;
 std::vector<AlignMuonTrack>::iterator itMuonTrackSecond;
 bool sameM2;
 bool sameM3;

 
 for(itMuonTrackFirst=m_muonTracks.begin();
     itMuonTrackFirst<m_muonTracks.end();itMuonTrackFirst++){
   for(itMuonTrackSecond=itMuonTrackFirst+1;
       itMuonTrackSecond<m_muonTracks.end();itMuonTrackSecond++){
     sameM2=false;
     sameM3=false;
 
     if(itMuonTrackFirst->point(1).tile()==
        itMuonTrackSecond->point(1).tile())sameM2=true;
     if(itMuonTrackFirst->point(2).tile()==
        itMuonTrackSecond->point(2).tile())sameM3=true;

     debug()<<sameM2<<" "<<sameM3<<" "<<endreq;
     debug()<<itMuonTrackFirst->point(1).tile()<<" "<<itMuonTrackSecond->point(1).tile()<<" "<<
       itMuonTrackFirst->point(2).tile()<<" "<<itMuonTrackSecond->point(2).tile()<<endreq;	
     if(sameM2&&sameM3){
       double x_extra5=-(itMuonTrackFirst->point(1).x()-
                         itMuonTrackFirst->point(2).x())*2
         +itMuonTrackFirst->point(2).x();
       double y_extra5=itMuonTrackFirst->point(2).y()*18800.0/16400.0;
       debug()<<" extra on M5 "<<x_extra5<<" "<<y_extra5<<endreq;

       double 
	 distuno=sqrt(pow((itMuonTrackFirst->point(4).x()-x_extra5),2)+
		      pow((itMuonTrackFirst->point(4).y()-y_extra5),2));
       double 
	 distdue=sqrt(pow((itMuonTrackSecond->point(4).x()-x_extra5),2)+
		      pow((itMuonTrackSecond->point(4).y()-y_extra5),2));
       debug()<<distuno<<" "<<distdue<<endreq;
       if(distuno>distdue){
         itMuonTrackFirst->setClone();
	 debug()<<" kill first "<<itMuonTrackFirst->point(4).x()<<endreq;
       }else{
         itMuonTrackSecond->setClone();  
	 debug()<<" kill second "<<itMuonTrackSecond->point(4).x()<<endreq;
       }       
     }     
   }
 }
 return StatusCode::SUCCESS;
 
};




StatusCode AlignMuonRec::printOut()
{

  std::vector<AlignMuonTrack>::iterator itMuonTrack;
  for(itMuonTrack=m_muonTracks.begin();
      itMuonTrack<m_muonTracks.end();itMuonTrack++){
    debug() <<itMuonTrack->clone()<<" points along the tracks "
	    <<itMuonTrack->point(1).x()<<" "<<
      itMuonTrack->point(1).y()<<" "<<
      itMuonTrack->point(2).x()<< " "<<itMuonTrack->point(2).y()<<" "<<
      itMuonTrack->point(3).x()<< " "<<itMuonTrack->point(3).y()<<" "<<
      itMuonTrack->point(4).x()<< " "<<itMuonTrack->point(4).y()<<endreq;
  }  
  return StatusCode::SUCCESS;
  
};

StatusCode AlignMuonRec::createCoords()
{

  std::vector<LHCb::MuonTileID> decodingPads=m_muonBuffer->getPads();
  std::vector<LHCb::MuonTileID>::iterator it;
  
  for(it=decodingPads.begin();it<decodingPads.end();it++){
    LHCb::MuonTileID padTile= *it;
    double x,y,z,dx,dy,dz;
    
     m_muonDetector-> Tile2XYZ(padTile,x, dx,y, dy,z, dz);
     int region=padTile.region();
     int station=padTile.station();

     AlignMuonRegion* thisRegion = m_station[station].region(region);
     thisRegion->addPoint( x, y,z,padTile ); 

  }
  std::vector<LHCb::MuonTileID> decodingM1=m_muonBuffer->getTile();
  for(it=decodingM1.begin();it<decodingM1.end();it++){
    LHCb::MuonTileID padTile= *it;
    if(padTile.station()!=0)continue; 
    double x,y,z,dx,dy,dz;   
    m_muonDetector-> Tile2XYZ(padTile,x, dx,y, dy,z, dz);
    int region=padTile.region();
    int station=padTile.station();
    AlignMuonRegion* thisRegion = m_station[station].region(region);
    thisRegion->addPoint( x, y,z,padTile );    
  }

  return StatusCode::SUCCESS;
  
}

