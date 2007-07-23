// $Id: AlignMuonRec.cpp,v 1.1.1.1 2007-07-23 07:14:36 asatta Exp $
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
  declareProperty("DecodingFromCoord", m_decodingFromCoord = true );
  declareProperty("BField", m_Bfield = true );
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
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
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

  // create the container for storing muon tracks
  
  //  PatDataStore* store = tool<PatDataStore> ( "PatDataStore", "" );
  m_onTES        = true;  
  PatDataStore* store = tool<PatDataStore> ( "PatDataStore" );
  
  //  m_muonTracksContainer = store->createTrackContainer(m_outputMuonTracksName,
  //              10 );
  m_states       = store->states();
  release( store );
  
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_muonBuffer)info()<<"error retrieving the muon raw buffer tool "
                         <<endreq;
  
  //retrieve the pointer to the muon detector 
  
  m_muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  
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
    info()<<" push back station "<<station<<endreq;
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
    
    //    for(unsigned int region = 0 ; region < m_nRegion ; region++ ){      
    //  AlignMuonRegion* reg = new AlignMuonRegion();
    //  //set some parameters 
    //  m_station[station].region().push_back( reg );
    // }
    
    //double deltaX,deltaY,z,deltaZ;
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
  
  //  HltAlgorithm::beginExecute();
  
  //setFilterPassed(true);
  
  int m_countMuonCandidates=0;
  
  m_countEvents += 1;
  

  for(unsigned  station = 0 ; station < m_nStation ; station++ ){
    m_station[station].clearRegions();
  }
  createCoords();
  
  
  muonSearch();   //Trova le tracce di muoni candidate
  
  if(m_cloneKiller)    detectClone();   //Definisce un clone se 2 tracce condividono pad su M2,M3,M4 
  
  if(m_cloneKiller)    strongCloneKiller(); //Definisce un clone se 2 tracce condividono pad su M2,M3
  
  
  
  m_countMuCandidates += m_muonTracks.size();
  debug()<<" size "<<m_muonTracks.size()<<endreq;
  LHCb::Tracks* muonsContainer= new LHCb::Tracks;
  debug()<<muonsContainer<<endreq;
  std::vector<AlignMuonTrack>::iterator itMuonTrack;
  for(itMuonTrack=m_muonTracks.begin();
      itMuonTrack<m_muonTracks.end();itMuonTrack++){
    if(!(itMuonTrack)->clone()){	
      m_countMuonCandidates++;
      LHCb::Track* trgTr =new Track();
      
        // m_muonTracksContainer->newEntry();
      //        trgTr->setType(Track::Muon);
      debug() << "==> Execute1" << endmsg; 

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
      double txref = ( firstpoint.x() - lastpoint.x() ) /  (firstpoint.z() - lastpoint.z() )  ;
      double tyref = ( firstpoint.y() - lastpoint.y() ) /  (firstpoint.z() - lastpoint.z() )  ;

      for(unsigned ista=0;ista<m_nStation;ista++){
        info()<<" new muon track "<<endreq;
        std::vector<AlignMuonPoint> vpoint = itMuonTrack->points(ista);
        for(std::vector<AlignMuonPoint>::const_iterator iV=vpoint.begin(); iV != vpoint.end(); iV++) {
          
          float xm=(*iV).x();       
          float ym=(*iV).y();
          float zm=(*iV).z();
          float txm,tym;


          if(ista<4) {
            txm= (*itMuonTrack).slopeX(ista,ista+1);
            tym= (*itMuonTrack).slopeY(ista,ista+1);
          }else{ 
            txm= (*itMuonTrack).slopeX(ista,ista-1);
            tym= (*itMuonTrack).slopeY(ista,ista-1);
          }
          LHCb::State temp;
          temp.setZ(zm);
          temp.setX(xm);
          temp.setY(ym);
          temp.setTx(txref);
          temp.setTy(tyref);
	  temp.setQOverP( 1 / 10000.) ;
          temp.setLocation(LHCb::State::EndVelo);
	  m_fCalcPtKick->calculate(&temp);

          trgTr->addToStates( temp );
          trgTr->addToLhcbIDs((*iV).tile());            
        }
      }
      
      trgTr->setPatRecStatus( Track::PatRecIDs ) ; 
      muonsContainer->insert(trgTr);
            
    }
    
  }

  debug()<<" mmm "<<muonsContainer->size()<<endreq;  
  StatusCode fg=eventSvc()->registerObject(m_outputMuonTracksName,
					   muonsContainer);
  info()<<fg.isSuccess()<<endreq;
  if(fg.isFailure()){
    put(muonsContainer,m_outputMuonTracksName); 
  }
  
  m_muonTracks.clear();
  info()<<" stored candidates "<<m_countMuonCandidates<<endreq; 
  debug()<<" in container "<< muonsContainer->size()<<endreq;

  Tracks* muonTracks=get<Tracks>(m_outputMuonTracksName);
  info()<<"size "<<muonTracks->size()<<endreq; 

    
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





StatusCode AlignMuonRec::muonSearch()   // Versione modificata per salvare tutte le hit
{
  m_muonTracks.clear();

  std::vector<AlignMuonPoint>::iterator itM5;
  for (unsigned int region=0;region<m_nRegion;region++){    
    AlignMuonRegion* thisRegion = m_station[4].region(region);
    for(itM5=thisRegion->points().begin();itM5<thisRegion->points().end();
        itM5++){
      //serach coincidence in M4
      std::vector<AlignMuonPoint> CandidateM4;
      info()<< " MuonSearch CandidateM4"<<endreq;
      
      double x=(*itM5).x()*m_station[3].z()/m_station[4].z();
      double y=(*itM5).y()*m_station[3].z()/m_station[4].z();        
      unsigned int regionM5=(*itM5).tile().region();
      info()<<" +++++++++++++++++++ new seed "<<endreq;          
      info()<<"seed "<<(*itM5).x()<<" "<<(*itM5).y()<<" REGION "<< regionM5<<endreq;          
      m_station[3].findCoincidence(x,y,regionM5,CandidateM4);
      info()<< " MuonSearch CandidateM4 out"<<endreq;
      if(CandidateM4.size()==0)continue;
      AlignMuonPoint* bestCandidateM4=&CandidateM4[0];
      info()<<"M4 concidence  x = "<<x<<" y= "<<y
	    <<" BCx= "<<bestCandidateM4->x()
	    << " BCy= "<<bestCandidateM4->y() <<endreq;
      
      x=-((*itM5).x()-bestCandidateM4->x())+bestCandidateM4->x(); 
      y=(bestCandidateM4)->y()*m_station[2].z()/m_station[3].z();

      std::vector<AlignMuonPoint> CandidateM3;
      unsigned int regionM4=(bestCandidateM4)->tile().region();
      m_station[2].findCoincidence(x,y,regionM4,CandidateM3);
      if(CandidateM3.size()==0)continue;      
      AlignMuonPoint* bestCandidateM3=&CandidateM3[0];
      info()<<"M3 concidence  x ="<<x<<" y= "<<y
	    <<" BCx= "<<bestCandidateM3->x()
	    << " BCy= "<<bestCandidateM3->y() <<endreq;

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
      AlignMuonPoint* bestCandidateM2=&CandidateM2[0];
      info()<<"M2 concidence  x = "<<x<<" y = "<<y
	    <<" BCx= "<<bestCandidateM2->x()
	    << " BCy= "<<bestCandidateM2->y() <<endreq;
      
      x=-(2.55 * (bestCandidateM3->x()-bestCandidateM2->x()))+bestCandidateM2->x();
      y=(bestCandidateM2)->y()*m_station[0].z()/m_station[1].z();

      std::vector<AlignMuonPoint> CandidateM1;
      unsigned int regionM2=(bestCandidateM2)->tile().region();
      m_station[0].findCoincidence(x,y,regionM2,CandidateM1);
      if(CandidateM1.size()==0)continue;
      AlignMuonPoint* bestCandidateM1=&CandidateM1[0];
      info()<<"M1 concidence  x ="<<x<<" y = "<<y
	    <<" BCx= "<<bestCandidateM1->x()
	    << " BCy= "<<bestCandidateM1->y() <<endreq;
      std::vector<AlignMuonPoint> CandidateM5;
      CandidateM5.push_back((*itM5));

      AlignMuonTrack muon;
      muon.setPoint(0,CandidateM1);
      muon.setPoint(1,CandidateM2);
      muon.setPoint(2,CandidateM3);
      muon.setPoint(3,CandidateM4);
      muon.setPoint(4,CandidateM5);
      info()<<" tile "<<muon.point(1).tile()<<" "<<muon.point(2).tile()<<endreq;
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

     info()<<sameM2<<" "<<sameM3<<" "<<endreq;
     info()<<itMuonTrackFirst->point(1).tile()<<" "<<itMuonTrackSecond->point(1).tile()<<" "<<
       itMuonTrackFirst->point(2).tile()<<" "<<itMuonTrackSecond->point(2).tile()<<endreq;	
     if(sameM2&&sameM3){
       double x_extra5=-(itMuonTrackFirst->point(1).x()-
                         itMuonTrackFirst->point(2).x())*2
         +itMuonTrackFirst->point(2).x();
       double y_extra5=itMuonTrackFirst->point(2).y()*18800.0/16400.0;
       info()<<" extra on M5 "<<x_extra5<<" "<<y_extra5<<endreq;

       double 
	 distuno=sqrt(pow((itMuonTrackFirst->point(4).x()-x_extra5),2)+
		      pow((itMuonTrackFirst->point(4).y()-y_extra5),2));
       double 
	 distdue=sqrt(pow((itMuonTrackSecond->point(4).x()-x_extra5),2)+
		      pow((itMuonTrackSecond->point(4).y()-y_extra5),2));
       info()<<distuno<<" "<<distdue<<endreq;
       if(distuno>distdue){
         itMuonTrackFirst->setClone();
	 info()<<" kill first "<<itMuonTrackFirst->point(4).x()<<endreq;
       }else{
         itMuonTrackSecond->setClone();  
	 info()<<" kill second "<<itMuonTrackSecond->point(4).x()<<endreq;
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
    info()<<itMuonTrack->clone()<<" points along the tracks "
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

