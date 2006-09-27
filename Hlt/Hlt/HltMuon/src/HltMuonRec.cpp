// $Id: HltMuonRec.cpp,v 1.2 2006-09-27 13:55:23 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RawEvent.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"


// local
#include "HltMuonRec.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltMuonRec
//
// 2004-10-06 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HltMuonRec );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMuonRec::HltMuonRec( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MeasureTime"   , m_measureTime   = true );
  declareProperty( "CloneKiller"   , m_cloneKiller   = true );
  //declareProperty( "StoreTracks"   , m_storeTracks   = true );
  declareProperty( "OutputMuonTracksName" ,
                   m_outputMuonTracksName="Hlt/Tracks/Muon");
  declareProperty("DecodingFromCoord", m_decodingFromCoord = true );
  
}
//=============================================================================
// Destructor
//=============================================================================
HltMuonRec::~HltMuonRec() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMuonRec::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
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
  m_muonTracksContainer = m_patDataStore->createTrackContainer(m_outputMuonTracksName,
                                                        10 );
  m_states       = m_patDataStore->states();
  //release( store );

  //retrieve the pointer to the muon detector 

  m_muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");

  //retrieve the pointer to the tool to convert tileID to x,y,z in a fast way (using a LUT)

  m_iPosTool  = tool<IMuonPosTool>( "MuonPosTool" );
  if(!m_iPosTool)info()<<"error retrieving the pos tool "<<endreq;

  //all geometry stuff
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_nStation= basegeometry.getStations();
  m_nRegion = basegeometry.getRegions();
  unsigned int i=0;
  while( i < m_nStation ){  
    m_stationNames.push_back( basegeometry.getStationName(i) );
    i++;
  }

  basePath[0]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/";
  basePath[1]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M2/"; 
  basePath[2]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M3/";
  basePath[3]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M4/";
  basePath[4]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M5/";


 //how many tell1
  m_totL1Board = 0;  
  m_stationL1Start[0]=0;
  
  for(int station=0;station<5;station++){
    debug()<<"station number "<<station<<endreq;    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);
    if( 0 == cabling ) {
      error() << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }   
    m_stationL1Start[station]= m_totL1Board;    
    m_totL1Board =m_totL1Board+cabling->getNumberOfL1Board();
    //alessia add the start and stop per station
    m_stationL1Stop[station]= m_totL1Board ;    
  }
  //skip M1 in the decoding and reconstruction
  debug()<<"boards "<<endreq;
  
  m_L1BoardStart=4;  
  m_L1BoardStop= m_totL1Board; 

  //create container for pad position

  for(unsigned int station=0;station<m_nStation;station++){
    m_station.push_back( HltMuonStationRec(msgSvc()));

    for(unsigned int region = 0 ; region < m_nRegion ; region++ ){      
      HltMuonRegion* reg = new HltMuonRegion();
      //set some parameters 
      debug()<<station<<" "<<region<<endreq;
      
      m_station[station].region().push_back( reg );
    }
  }
  
  //set some usefull info about the layout of the muon system

  debug()<<"boards "<<endreq;  
  for(unsigned int station=0;station<m_nStation;station++){
    for(unsigned int region = 0 ; region < m_nRegion ; region++ ){
      int xMap,yMap; 
      unsigned int numLayout=m_muonDetector->getLogMapInRegion(station,region);
      m_station[station].region(region)->setLayoutNumber(numLayout);
       debug()<<"numLayout "<<numLayout<<endreq;  
      for(unsigned int which_layout=0;which_layout<numLayout;which_layout++){
        xMap=m_muonDetector->
          getLayoutX (which_layout, station,region);          
        yMap=m_muonDetector->  getLayoutY (which_layout, station,region); 
         
        m_station[station].region(region)->
           setLayoutGridX(which_layout,(unsigned int) xMap);
        m_station[station].region(region)->
          setLayoutGridY(which_layout,(unsigned int) yMap); 
      }
      debug()<<"numLayout "<<numLayout<<endreq;  
      if(numLayout>1){
        int gridx1=m_station[station].region(region)->
          layoutGridX(0);
        int gridx2=m_station[station].region(region)->
          layoutGridX(1);
        if(gridx1> gridx2){
          //reverse the order....
          int gridy1=m_station[station].region(region)->
            layoutGridY(0);
          int gridy2=m_station[station].region(region)->
            layoutGridY(1);
          m_station[station].region(region)->
            setLayoutGridX(0,(unsigned int) gridx2);
          m_station[station].region(region)->
            setLayoutGridY(0,(unsigned int) gridy2);
          m_station[station].region(region)->
            setLayoutGridX(1,(unsigned int) gridx1);
          m_station[station].region(region)->
            setLayoutGridY(1,(unsigned int) gridy1);    
        }        
      }
    }    
  }
  

  
  //initialze the decoding of the raw buffer

  if(m_decodingFromCoord ){
    initializePadDecoding();    
  }else{    
    initializeLogChanDecoding();
    for(int index=0;index<12;index++){
      twelfthX[index].reserve(50);
      twelfthY[index].reserve(50);
    }
  }


  //set muon station position
  
  for(unsigned int station=1;station<m_nStation;station++){
 
    //    for(unsigned int region = 0 ; region < m_nRegion ; region++ ){      
    //  HltMuonRegion* reg = new HltMuonRegion();
    //  //set some parameters 
    //  m_station[station].region().push_back( reg );
    // }
 
    //double deltaX,deltaY,z,deltaZ;
    double zSta= m_muonDetector->getStationZ(station);
    

    m_station[station].setZ( zSta );
    //set cut for muon reconstruction
    debug()<<" station "<<endreq;
    
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
StatusCode HltMuonRec::execute() {

  debug() << "==> Execute" << endmsg;

  HltAlgorithm::beginExecute();
    
  setFilterPassed(true);


  int m_countMuonCandidates=0;
  
  m_countEvents += 1;
  m_padM5=false;
  m_padM4=false;
  m_padM3=false;
  m_padM2=false;


  for(unsigned  station = 1 ; station < m_nStation ; station++ ){
    m_station[station].clearRegions();
  }



  if(!m_decodingFromCoord){
    if ( m_measureTime ) m_timer->start( m_timeLoad );    
    decodeBuffer();
    if ( m_measureTime ) {
      m_timer->stop( m_timeLoad );   
    }
  }
  if ( m_measureTime ) m_timer->start( m_timePad );
  // decode only station M5
  if(!m_padM5){
    createCoords(4);
    m_padM5=true;
  }
  if(!m_padM4){
    createCoords(3);
    m_padM4=true;
  }
  
  if ( m_measureTime ) {

    m_timer->stop ( m_timePad );
    m_timer->start( m_timeMuon );
  }  

  muonSearch();
  if(m_cloneKiller)    detectClone();
  if(m_cloneKiller)    strongCloneKiller();

  if ( m_measureTime ) {
    m_timer->stop ( m_timeMuon );
    m_timer->start( m_timeMuonStore );
  }  

  
  m_countMuCandidates += m_muonTracks.size();
  
  std::vector<HltMuonTrack>::iterator itMuonTrack;
  for(itMuonTrack=m_muonTracks.begin();
      itMuonTrack<m_muonTracks.end();itMuonTrack++){
    if(!(itMuonTrack)->clone()){	
      m_countMuonCandidates++;
      LHCb::Track* trgTr = m_muonTracksContainer->newEntry();
      //        trgTr->setType(Track::Muon);
      
     
//	info()<<" new muon track "<<endreq;
      float xm=itMuonTrack->point(1).x();       
      float ym=itMuonTrack->point(1).y();
      float txm= itMuonTrack->
        slopeX(1,2,m_station[1].z(),m_station[2].z());
      float tym= itMuonTrack->
        slopeY(1,2,m_station[1].z(),m_station[2].z());
      if ( !m_onTES ) {
        LHCb::State* temp = m_states->newEntry();
        //        trgTr->states().push_back( temp );
        temp->setZ(m_station[1].z());
        temp->setX(xm);
        temp->setY(ym);
        temp->setTx(txm);
        temp->setTy(tym);
        temp->setLocation(LHCb::State::Muon);
      } else {
        LHCb::State temp;
        temp.setZ(m_station[1].z());
        temp.setX(xm);
        temp.setY(ym);
        temp.setTx(txm);
        temp.setTy(tym);
        trgTr->addToStates( temp );
        temp.setLocation(LHCb::State::Muon);
      }
      
      xm=itMuonTrack->point(2).x();       
      ym=itMuonTrack->point(2).y();
      txm= itMuonTrack->
        slopeX(2,3,m_station[2].z(),m_station[3].z());
      tym= itMuonTrack->
        slopeY(2,3,m_station[2].z(),m_station[3].z());
      if ( !m_onTES ) {
        LHCb::State* temp = m_states->newEntry();
        //        trgTr->states().push_back( temp );
        temp->setZ(m_station[2].z());
        temp->setX(xm);
        temp->setY(ym);
        temp->setTx(txm);
        temp->setTy(tym);
        temp->setLocation(LHCb::State::Muon);
      } else {
        LHCb::State temp;
        temp.setZ(m_station[2].z());
        temp.setX(xm);   
        temp.setY(ym);   
        temp.setTx(txm);
        temp.setTy(tym);
        trgTr->addToStates( temp );
        temp.setLocation(LHCb::State::Muon);
      }
      
      
      
      xm=itMuonTrack->point(3).x();       
      ym=itMuonTrack->point(3).y();
      txm= itMuonTrack->
        slopeX(3,4,m_station[3].z(),m_station[4].z());
      tym= itMuonTrack->
        slopeY(3,4,m_station[3].z(),m_station[4].z());
      if ( !m_onTES ) {
        LHCb::State* temp = m_states->newEntry();
        //        trgTr->states().push_back( temp );
        temp->setZ(m_station[3].z());
        temp->setX(xm);
        temp->setY(ym);
        temp->setTx(txm);
        temp->setTy(tym);
        temp->setLocation(LHCb::State::Muon);
        
      } else {
        LHCb::State temp;
        temp.setZ(m_station[3].z());
        temp.setX(xm);   
        temp.setY(ym);   
        temp.setTx(txm);
        temp.setTy(tym);
        trgTr->addToStates( temp );
        temp.setLocation(LHCb::State::Muon);
      }
      
      
      xm=itMuonTrack->point(4).x();       
      ym=itMuonTrack->point(4).y();
      txm= itMuonTrack->
        slopeX(3,4,m_station[3].z(),m_station[4].z());
      tym= itMuonTrack->
        slopeY(3,4,m_station[3].z(),m_station[4].z());
      
      if ( !m_onTES ) {
        LHCb::State* temp = m_states->newEntry();
        //        trgTr->states().push_back( temp );
        temp->setZ(m_station[4].z());
        temp->setX(xm);
        temp->setY(ym);
        temp->setTx(txm);
        temp->setTy(tym);
        temp->setLocation(LHCb::State::Muon);
      } else {
        LHCb::State temp;
        temp.setZ(m_station[4].z());
        temp.setX(xm);   
        temp.setY(ym);   
        temp.setTx(txm);
        temp.setTy(tym);
        trgTr->addToStates( temp );
        temp.setLocation(LHCb::State::Muon);
      }    
      trgTr->addToLhcbIDs(itMuonTrack->point(1).tile());    
      trgTr->addToLhcbIDs(itMuonTrack->point(2).tile());     
      trgTr->addToLhcbIDs(itMuonTrack->point(3).tile());     
      trgTr->addToLhcbIDs(itMuonTrack->point(4).tile());
      
    }
  }  
    
  
  
  
  if ( m_measureTime ) { 
    m_timer->stop( m_timeMuonStore );
  }
  
  m_muonTracks.clear();
  debug()<<" stored candidates "<<m_countMuonCandidates<<endreq; 
  debug()<<" in container "<< m_muonTracksContainer->size()<<endreq;

 HltAlgorithm::endExecute();


  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMuonRec::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  info () << "Number of events processed: " << double(m_countEvents) << endreq;
  info () << "Average number of muon tracks: " 
          << double(m_countMuCandidates)/double(m_countEvents) << endreq;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
};


//=============================================================================
StatusCode HltMuonRec::decodeBuffer() {

  std::vector<MuonTileID> storage;
  
  LHCb::RawEvent* raw = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);  
  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;  
  for( itB = b.begin(); itB != b.end(); itB++ ) {    
    const RawBank* r = *itB;
    //info()<<"start of the bank "<<(*itB)->sourceID()<<endmsg;
    //char* it;
    //skip the tell1 corresponding to M1
    const unsigned char* it=r->begin<unsigned char>();   
    short skip=0;
    
    if((unsigned int)(*itB)->sourceID()>=m_M1Tell1){
      //how many pads ?
      const short * itPad=r->begin<short>();
      short nPads=*itPad;
      if((nPads+1)%2==0){
        skip=(nPads+1)/2;
      }else {
        skip=(nPads+1)/2+1;
      }
    }
    
    it=it+4*skip;
    verbose()<<" skipping "<< skip <<" bytes words "<<endreq;
    
    // how many ODE in this tell1?
    unsigned int nODE=m_ODENumberInTell1[(*itB)->sourceID()];
    //info()<<" number of ODE "<<nODE<<endmsg;    
    std::vector<unsigned int> firedInODE;
    firedInODE.resize(nODE) ;    
    unsigned int itODE=0;
    //const unsigned char* it=r->begin<unsigned char>();    
    for(itODE=0; itODE < nODE ; itODE++) {
      //first decode the header
        firedInODE[itODE]=(unsigned int)(*it);
        //   info()<<"channels in ODE "<<(unsigned int)(*it)<<endmsg;        
        it++;
    }
    for(itODE=0;itODE<nODE;itODE++){
      unsigned int channels=0;
      
      unsigned int odenumber=(m_ODEInTell1[(*itB)->sourceID()])[itODE];      
      for(channels=0;channels<firedInODE[itODE];channels++){
        unsigned int address=(unsigned int)(*it);
        //info()<<"address "<<address<<" "<<odenumber<<endmsg;        
        MuonTileID tile=(m_mapTileInODE[odenumber-1])[address];
        //info()<<"tile in tiol "<<tile.layout()<<" "<<tile.station()<<" "<<
        // tile.region()<<" "<<tile.nX()<<" "<<tile.nY()<<endmsg;
        int station = tile.station();      
        int region  = tile.region();
        m_station[station].region(region)->addTile( tile );
        
        //        info()<<" qui "<<(unsigned int) tile<<endmsg;       
        it++;
        //channels++;        
      }
    }  
  }
  return StatusCode::SUCCESS;
  
};



StatusCode HltMuonRec::createCoords(int station) {



  if(m_decodingFromCoord){    
    StatusCode sc=getPads(station);
    return sc;    
  }else{

    StatusCode sc=createCoordsFromLC(station);
    return sc;
  }
  

}



StatusCode HltMuonRec::crossStrips(unsigned int station, unsigned int region) {

  
  HltMuonRegion* thisRegion = m_station[station].region(region);
  thisRegion->clearPoints();
  
  std::vector<LHCb::MuonTileID> padTiles = thisRegion->tiles();

  //only same 12 subregion can cross...
  //  std::vector<std::pair<MuonTileID,bool> > twelfthX[12];
  //std::vector<std::pair<MuonTileID,bool> > twelfthY[12];

  /*  for(int index=0;index<12;index++){
    twelfthX[index].reserve(50);
    twelfthY[index].reserve(50);
    }*/
  
  unsigned int which_third=0;  
  unsigned int which_layout=0;
  
  std::vector<LHCb::MuonTileID>::const_iterator itTile;
  unsigned int layoutX[2];
  unsigned int layoutY[2];
  layoutX[0]=thisRegion->layoutGridX(0);
  layoutX[1]=thisRegion->layoutGridX(1);
  layoutY[0]=thisRegion->layoutGridY(0);
  layoutY[1]=thisRegion->layoutGridY(1);
  

  
  for( itTile = padTiles.begin() ; itTile != padTiles.end() ; 
       ++itTile ){          
    unsigned int quadrant=(*itTile).quarter();
    
    if((itTile->nX())>=(itTile->layout()).xGrid()){
      if((itTile->nY())>=(itTile->layout()).yGrid()){
         which_third=2;
      }
      
      else{
         which_third=1;
      }
 
    }else{
      which_third=3;
      
    }
    
    
    // insert the pad in the correct container
    if((itTile->layout()).xGrid()== layoutX[0]){
      which_layout=0;  
      twelfthX[quadrant*3+which_third-1].
        push_back(std::pair<LHCb::MuonTileID,bool>((*itTile),false));      
    }else{
      which_layout=1;     
      twelfthY[quadrant*3+which_third-1].
        push_back(std::pair<LHCb::MuonTileID,bool>((*itTile),false)); 
      
    }
    
  }
  //release storage for pad storing...
  padTiles.clear();
  
  
  std::vector<std::pair<LHCb::MuonTileID,bool> >::iterator itTileX,itTileY;
    
  for(int itTwelfth=0; itTwelfth<12;  itTwelfth++){
    for(itTileX=twelfthX[itTwelfth].begin();
        itTileX<twelfthX[itTwelfth].end();
        itTileX++){
      for(itTileY=twelfthY[itTwelfth].begin();
          itTileY<twelfthY[itTwelfth].end();
          itTileY++){
        //        MuonTileID pad = itTileX->first.intercept( itTileY->first );
        LHCb::MuonTileID pad = intercept( itTileX->first,itTileY->first );

        if(pad.isValid()){
            // have a pad to write out
            // zero the layer and readout bits
          //pad.setLayer(0);
          //pad.setReadout(0);
          //          padTiles.push_back( pad );
          double x,y,z,dx,dy,dz;
          
          m_iPosTool-> calcTilePos(pad,x, dx,y, dy,z, dz);
          thisRegion->addPoint( x, y,pad );          
          // set flags to used on iOne and iTwo
                //    info()<<"crossed "<<pad.station()<<" "<<
        //   pad.nX()<<" "<<pad.nY()<<" "<<
         //  x<<" "<<y<<endreq;
          itTileX->second = true;
          itTileY->second = true;                                    
        }
        
      }
      
      
    }
    
    
    
    //decide what to do with unused strips

    for(itTileX=twelfthX[itTwelfth].begin();
        itTileX<twelfthX[itTwelfth].end();
        itTileX++){
      
      if(!(itTileX->second)){
          // no crossing map here
        
        
        unsigned int offSetX=(itTileX->first.nX())*layoutX[1]/layoutX[0];
        
        for(unsigned int addTile=0;addTile<layoutX[1]/layoutX[0];addTile++){
          
          LHCb::MuonTileID pad = itTileX->first;
          // zero the layer and readout bits
          //correct layoutX  and gridX
          pad.setLayout(MuonLayout(layoutX[1],layoutY[0]));            
          pad.setX(offSetX+addTile);            
          //pad.setLayer(0);
          //pad.setReadout(0);
          double x,y,z,dx,dy,dz;
          
          m_iPosTool-> calcTilePos(pad,x, dx,y, dy,z, dz);
          thisRegion->addPoint( x, y,pad );          
//               info()<<"pad "<<pad.station()<<" "<<x<<" "<<y<<endreq;
          
          //          padTiles.push_back( pad );
        }
        
      
      }
    }
    
    for(itTileY=twelfthY[itTwelfth].begin();
        itTileY<twelfthY[itTwelfth].end();
        itTileY++){
      
      if(!(itTileY->second)){  
        
        // no crossing map here
        unsigned int offSetY=(itTileY->first.nY())*layoutY[0]/layoutY[1];
        
        for(unsigned int addTile=0;addTile<layoutY[0]/layoutY[1];addTile++){
            
          LHCb::MuonTileID pad = itTileY->first;
          // zero the layer and readout bits
          //correct layoutX  and gridX
          pad.setLayout(MuonLayout(layoutX[1],layoutY[0]));            
          pad.setY(offSetY+addTile);            
          //pad.setLayer(0);
          //pad.setReadout(0);
          double x,y,z,dx,dy,dz;
          
          m_iPosTool-> calcTilePos(pad,x, dx,y, dy,z, dz);
          thisRegion->addPoint( x, y,pad );          
//                   info()<<"pad "<<pad.station()<<" "<<x<<" "<<y<<endreq;
          //          padTiles.push_back( pad );
            
        }
      }     
    }
  }
  for(int index=0;index<12;index++){
    twelfthX[index].clear();
    twelfthY[index].clear();
    }
  return StatusCode::SUCCESS;  
};




LHCb::MuonTileID HltMuonRec::intercept(const LHCb::MuonTileID& stripX, 
                              const LHCb::MuonTileID& stripY) {

  // check first that the two strips are really intercepting



  int thisGridX = stripX.layout().xGrid();
  int thisGridY = stripX.layout().yGrid();

  int otherGridX = stripY.layout().xGrid();
  int otherGridY = stripY.layout().yGrid();
  unsigned int calcX = stripY.nX()*thisGridX/otherGridX;
  if (calcX != stripX.nX() ) return LHCb::MuonTileID();
  unsigned int calcY = stripX.nY()*otherGridY/thisGridY;
  if (calcY != stripY.nY() ) return LHCb::MuonTileID();
  // Now the strips are intercepting - get it !

  int indX = stripY.nX();
  int indY = stripX.nY();

  LHCb::MuonTileID resultID(stripX);
  resultID.setX(indX);
  resultID.setY(indY);

  resultID.setLayout(MuonLayout(otherGridX,thisGridY));

  return resultID;
};



StatusCode HltMuonRec::muonSearch()
{
  m_muonTracks.clear();
  //  return StatusCode::SUCCESS;
  
  std::vector<HltMuonPoint>::iterator itM5;
  for (unsigned int region=0;region<m_nRegion;region++){    
    HltMuonRegion* thisRegion = m_station[4].region(region);
    for(itM5=thisRegion->points().begin();itM5<thisRegion->points().end();
        itM5++){
      //serach coincidence in M4
      //info()<<"seed "<<(*itM5).x()<<" "<<(*itM5).y()<<endreq;          
      HltMuonPoint* bestCandidateM4=NULL;
      double x=(*itM5).x()*m_station[3].z()/m_station[4].z();
      double y=(*itM5).y()*m_station[3].z()/m_station[4].z();        
      unsigned int regionM5=(*itM5).tile().region();
      m_station[3].findCoincidence(x,y,regionM5,bestCandidateM4);
      // info()<<"M4 concidence "<<x<<" "<<y<<" "<<bestCandidateM4<<endreq;
      
      if(bestCandidateM4==NULL)continue;

      if(!m_padM3){
        if ( m_measureTime ) {        

          m_timer->stop( m_timeMuon );
          m_timer->start( m_timePad );
        }
        createCoords(2);
        m_padM3=true;
        if ( m_measureTime ) {
          m_timer->stop( m_timePad );
          m_timer->start( m_timeMuon );
        }
      }

      x=-((*itM5).x()-bestCandidateM4->x())+bestCandidateM4->x();
      y=(bestCandidateM4)->y()*m_station[2].z()/m_station[3].z();
      HltMuonPoint* bestCandidateM3=NULL;
      unsigned int regionM4=(bestCandidateM4)->tile().region();
      m_station[2].findCoincidence(x,y,regionM4,bestCandidateM3);
      //            info()<<"M3 concidence "<<bestCandidateM3<<endreq;
      
      if(bestCandidateM3==NULL)continue;      

      if(!m_padM2){
        if ( m_measureTime ) {    
          m_timer->stop( m_timeMuon );     
          m_timer->start( m_timePad );
        }
        createCoords(1);
        m_padM2=true;
        if ( m_measureTime ) {
          m_timer->stop( m_timePad );
          m_timer->start( m_timeMuon );
        }  
      }

      x=-(bestCandidateM4->x()-bestCandidateM3->x())+bestCandidateM3->x();
      y=(bestCandidateM3)->y()*m_station[1].z()/m_station[2].z();
      HltMuonPoint* bestCandidateM2=NULL;
      unsigned int regionM3=(bestCandidateM3)->tile().region();
      m_station[1].findCoincidence(x,y,regionM3,bestCandidateM2);
      if(bestCandidateM2==NULL)continue;

      //            info()<<"M2 concidence "<<bestCandidateM2<<endreq;

      /*      info()<<" muon  M5 "<<(*itM5).x()<<" "<<(*itM5).y()<<endreq;
      //info()<<" muon  M4 "<<bestCandidateM4->x()<<" "<<
      //  bestCandidateM4->y()<<endreq;
      //info()<<" muon  M3 "<<bestCandidateM3->x()<<" "<<
       // bestCandidateM3->y()<<endreq;
      //info()<<" muon  M2 "<<bestCandidateM2->x()<<" "<<
      //bestCandidateM2->y()<<endreq;*/

      //create the muon track
      //info()<<"ciao "<<m_muonTracks.size()<<endreq;
      HltMuonTrack muon;
      muon.setPoint(1,*bestCandidateM2);
      muon.setPoint(2,*bestCandidateM3);
      muon.setPoint(3,*bestCandidateM4);
      muon.setPoint(4,*itM5);
      //info()<<"ciao prima "<<m_muonTracks.size()<<endreq;
      m_muonTracks.push_back(muon);      
      //info()<<"ciao dopo "<<m_muonTracks.size()<<endreq;
    }    
  }  
  //info()<<"fine "<<m_muonTracks.size()<<endreq;
  return StatusCode::SUCCESS;
  
};



StatusCode HltMuonRec::detectClone()
{
 std::vector<HltMuonTrack>::iterator itMuonTrackFirst;
 std::vector<HltMuonTrack>::iterator itMuonTrackSecond;
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
       double distuno=pow((itMuonTrackFirst->point(4).x()*-x_extra5),2)+
         pow((itMuonTrackFirst->point(4).y()*-y_extra5),2);
       double distdue=pow((itMuonTrackSecond->point(4).x()*-x_extra5),2)+
         pow((itMuonTrackSecond->point(4).y()*-y_extra5),2);
       if(distuno>distdue){
         itMuonTrackFirst->setClone();
       }else
         itMuonTrackSecond->setClone();         
     }     
   }
 }
 return StatusCode::SUCCESS;
 
};


StatusCode HltMuonRec::strongCloneKiller()
{
 std::vector<HltMuonTrack>::iterator itMuonTrackFirst;
 std::vector<HltMuonTrack>::iterator itMuonTrackSecond;
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
     if(sameM2&&sameM3){
       double x_extra5=-(itMuonTrackFirst->point(2).x()-
                         itMuonTrackFirst->point(3).x())
         +itMuonTrackFirst->point(3).x();
       double y_extra5=itMuonTrackFirst->point(3).y()*18800.0/17600.0;
       double distuno=pow((itMuonTrackFirst->point(4).x()*-x_extra5),2)+
         pow((itMuonTrackFirst->point(4).y()*-y_extra5),2);
       double distdue=pow((itMuonTrackSecond->point(4).x()*-x_extra5),2)+
         pow((itMuonTrackSecond->point(4).y()*-y_extra5),2);
       if(distuno>distdue){
         itMuonTrackFirst->setClone();
       }else
         itMuonTrackSecond->setClone();         
     }     
   }
 }
 return StatusCode::SUCCESS;
 
};




StatusCode HltMuonRec::printOut()
{

  std::vector<HltMuonTrack>::iterator itMuonTrack;
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




StatusCode HltMuonRec::initializePadDecoding()
{

  std::string cablingBasePath=getBasePath(0);    
  std::string cablingPath=cablingBasePath+"Cabling";
  SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);
  m_M1Tell1=cabling->getNumberOfL1Board();
  int countTell1=m_M1Tell1;

  for(int station=1;station<5;station++){
    verbose()<<"station number "<<station<<endreq;    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);   
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){    
      verbose()<<"L1 number "<<cabling->getL1Name(0)<<endreq;
      std::string L1path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
      //      unsigned totODE=0;      
      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
        std::string ODEpath=cablingBasePath
          +l1->getODEName(ODEBoard);
        SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
        //build LUT with ode ID --> MuonTileID
        unsigned int region=ode->region();        
        for(int TS=0;TS<ode->getTSNumber();TS++){        
          std::string  TSPath= cablingBasePath+
            ode->getTSName(TS);          
          unsigned int quadrant= ode->getTSQuadrant(TS);
          unsigned int TSLayoutX=ode->getTSLayoutX();
          unsigned int TSLayoutY=ode->getTSLayoutY();
          unsigned int TSGridX=ode->getTSGridX(TS);
          unsigned int TSGridY=ode->getTSGridY(TS);
          unsigned int digitOffSetX=0;
          unsigned int digitOffSetY=0;          
          SmartDataPtr<MuonTSMap>  TSMap(detSvc(),TSPath);
          verbose()<<"trigger sector "<<TSPath<<endreq;
          
          std::vector<LHCb::MuonTileID> digitInTS;          
          for(int i=0;i<TSMap->numberOfOutputSignal();i++){
            //msg<<MSG::INFO<<"cabling base 2 "<<cablingBasePath<<endreq;
            unsigned int layout=TSMap->layoutOutputChannel(i);
            unsigned int  layoutX=TSMap->gridXLayout(layout);
            unsigned int  layoutY=TSMap->gridYLayout(layout);            
            digitOffSetX=layoutX*TSGridX;
            digitOffSetY=layoutY*TSGridY;
            unsigned int digitX=digitOffSetX+TSMap->gridXOutputChannel(i);
            unsigned int digitY=digitOffSetY+TSMap->gridYOutputChannel(i);
            MuonLayout lay(TSLayoutX*layoutX,TSLayoutY*layoutY);
            LHCb::MuonTileID muontile(station,lay,region,
                                quadrant,digitX,digitY);
            digitInTS.push_back(muontile);            
          }
          std::vector<LHCb::MuonTileID> crossAddress=DoPad(digitInTS,TSMap);  
          std::vector<LHCb::MuonTileID>::iterator itPad;
          for(itPad=crossAddress.begin();itPad<crossAddress.end();itPad++){
            m_mapPad[countTell1].push_back(*itPad);
          }        
        }
      }
      countTell1++;      
    }    
  }  
  return StatusCode::SUCCESS;  



  
}


std::vector<LHCb::MuonTileID> HltMuonRec::DoPad(std::vector<
                                                   LHCb::MuonTileID> digit,
                                                   MuonTSMap* TS)
{
  
  std::vector<LHCb::MuonTileID> list_of_pads;
  if(TS->numberOfLayout()==2){
    //how many subsector????
    int NX=TS->gridXLayout(1);    
    int NY=TS->gridYLayout(0);
    int Nsub=NX*NY;

    verbose()<<"number NX NY "<<NX<<" "<<NY<<endreq;   
    int maxPads=TS->gridXLayout(0)*TS->gridYLayout(1);   
    list_of_pads.reserve(maxPads);
    LHCb::MuonTileID t;    
    for(int i=0;i<maxPads;i++){
      list_of_pads.push_back(t);      
    }    
    // work on sub sector 
    if(Nsub>8){
      err()<<"error the dimensioning of the TS subsector is wrong "<<endreq;
      return list_of_pads;
      
    }
    std::vector<unsigned int> horiz[8];
    std::vector<unsigned int> hvert[8];
    // clear the memory;    
    // start fill the sub sector matrices
    std::vector<LHCb::MuonTileID>::iterator it;
    int index=0;
    for(it=digit.begin();it<digit.end();it++,index++){
      //also zero must be set
      // which subsector?
      int mx=TS->gridXOutputChannel(index)/
        (TS->gridXLayout(TS->layoutOutputChannel(index))/NX);
      int my=TS->gridYOutputChannel(index)/
        (TS->gridYLayout(TS->layoutOutputChannel(index))/NY);
      debug()<<" digit "<<index<<" "<<mx<<" "<<my<<" "<<*it<<endreq;     
      int Msub=mx+my*NX;
      // horizntal o vertical?
      bool horizontal=false;
      if(TS->layoutOutputChannel(index)==0)horizontal=true;
      if(horizontal)horiz[Msub].push_back(*it);
      else hvert[Msub].push_back(*it);
      verbose()<<" horizontal ? "<<     horizontal<<endreq;      
    }
    // now the address of fired pads..
    for(int i=0;i<Nsub;i++){
      // cross only local to each sub matrix
      std::vector<unsigned int>::iterator itx;
      std::vector<unsigned int>::iterator ity;
      //debug 
      verbose()<<" sub matrix "<<i<<endreq;
      verbose()<<" horizontal sequence ";
      for(itx=horiz[i].begin();
          itx<horiz[i].end();itx++){
        verbose()<<*itx<<" ";
      }
      verbose()<<endreq;
      verbose()<<" vertical sequence ";     
      for(ity=hvert[i].begin();ity<hvert[i].end();ity++){
        verbose()<<*ity<<" ";        
      }
      verbose()<<endreq;      
      //end debug
      unsigned int y_index=0;
      unsigned int subY=i/NX;
      unsigned int subX=i-subY*NX;      
      unsigned int offsetY=subY*(TS->gridYLayout(1)/NY);
      unsigned int offsetX=subX*(TS->gridXLayout(0)/NX);      
      for(ity=hvert[i].begin();
          ity<hvert[i].end();ity++,y_index++){                  
        unsigned int x_index=0;
        LHCb::MuonTileID tileY=*ity;
        
        for(itx=horiz[i].begin();
            itx<horiz[i].end();itx++,x_index++){          
            unsigned int address=offsetX+x_index+
              (offsetY+y_index)*(TS->gridXLayout(0));             
            verbose()<<" result of the address "<<address<<endreq;   
            LHCb::MuonTileID padTile=tileY.intercept(*itx);            
            list_of_pads[address]=padTile; 
            verbose()<<" TS dec "<<address<<" "<<
                  padTile.layout()<<" "<<padTile.station()<<" "<<
              padTile.region()<<" "<<padTile.quarter()<<" "<<
              padTile.nX()<<" "<<padTile.nY()<<" "<<endreq;
            
        }     
      }            
    }   
  }else{    
    //easy only zero suppression
    int maxPads=TS->gridXLayout(0)*TS->gridYLayout(0);   
    list_of_pads.reserve(maxPads);
    LHCb::MuonTileID t;    
    for(int i=0;i<maxPads;i++){
      list_of_pads.push_back(t);      
    }

    std::vector<LHCb::MuonTileID>::iterator it;
    unsigned int index=0;    
    for(it=digit.begin();it<digit.end();it++,index++){
      unsigned int address=index;
      list_of_pads[address]=*it;
      verbose()<<" result of the address "<<address<<endreq; 
    }    
  }
  return list_of_pads;  


}



StatusCode HltMuonRec::getPads(int station)
{
  

  for(unsigned int region=0;region< m_nRegion;region++){
     HltMuonRegion* thisRegion = m_station[station].region(region);
     thisRegion->clearPoints();
  }

  LHCb::RawEvent* raw = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);  
  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;  
  //unsigned int chIterator=0;
  
  for( itB = b.begin(); itB != b.end(); itB++ ) {    
    const RawBank* r = *itB;
    verbose()<<"start of the bank "<<(*itB)->sourceID()<<endmsg;   
    const short * it=r->begin<short>();
    int tell1Now=(unsigned int)(*itB)->sourceID();
    
    if(tell1Now>=m_stationL1Start[station]&&
       tell1Now<m_stationL1Stop[station]){
      
      //if((unsigned int)(*itB)->sourceID()>=m_M1Tell1){
      //  unsigned int tell=(unsigned int)(*itB)->sourceID();      
        unsigned int pads=*it;
        it++;      
        for(unsigned int loop=0;loop<pads;loop++){
          unsigned int address=*it;
          LHCb::MuonTileID padTile= (m_mapPad[tell1Now])[address]; 
          double x,y,z,dx,dy,dz;
          
          m_iPosTool-> calcTilePos(padTile,x, dx,y, dy,z, dz);
          int region=padTile.region();
          HltMuonRegion* thisRegion = m_station[station].region(region);
          thisRegion->addPoint( x, y,padTile );  

          //          storage.push_back(m_mapPad[tell][address]);        
          it++;        
        }      
        //      }      
    }
  }
  return StatusCode::SUCCESS;
  
}


StatusCode HltMuonRec::initializeLogChanDecoding()
{

 //how many tell1

  unsigned int countL1=0;
  
  for(int station=0;station<5;station++){
    debug()<<"station number "<<station<<endreq;    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);
    if( 0 == cabling ) {
      error() << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }    
    if(station==0)m_M1Tell1=cabling->getNumberOfL1Board();
    
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){    
      debug()<<"L1 number "<<cabling->getL1Name(0)<<endreq;
      std::string L1path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
      unsigned totODE=0;
      
      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
        std::string ODEpath=cablingBasePath
          +l1->getODEName(ODEBoard);
        //info()<<"ODE number "<<L1Board<<" "<<l1->getODEName(ODEBoard)<<endmsg;
        SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
        if( 0 == ode ) {
          err() << ODEpath << " not found in XmlDDDB" << endmsg;
          return StatusCode::FAILURE;
        }
        long odenum=ode->getODESerialNumber();
        m_ODEInTell1[countL1].push_back(odenum);        
        //build LUT with ode ID --> MuonTileID
        unsigned int region=ode->region();      
        totODE++;
        
        for(int TS=0;TS<ode->getTSNumber();TS++){
        
          std::string  TSPath= cablingBasePath+
            ode->getTSName(TS);          
          unsigned int quadrant= ode->getTSQuadrant(TS);
          unsigned int TSLayoutX=ode->getTSLayoutX();
          unsigned int TSLayoutY=ode->getTSLayoutY();
          unsigned int TSGridX=ode->getTSGridX(TS);
          unsigned int TSGridY=ode->getTSGridY(TS);
          unsigned int digitOffSetX=0;
          unsigned int digitOffSetY=0;          
          SmartDataPtr<MuonTSMap>  TSMap(detSvc(),TSPath);
          for(int i=0;i<TSMap->numberOfOutputSignal();i++){
            //msg<<MSG::INFO<<"cabling base 2 "<<cablingBasePath<<endreq;
            unsigned int layout=TSMap->layoutOutputChannel(i);
            unsigned int  layoutX=TSMap->gridXLayout(layout);
            unsigned int  layoutY=TSMap->gridYLayout(layout);            
            digitOffSetX=layoutX*TSGridX;
            digitOffSetY=layoutY*TSGridY;
            unsigned int digitX=digitOffSetX+TSMap->gridXOutputChannel(i);
            unsigned int digitY=digitOffSetY+TSMap->gridYOutputChannel(i);
            MuonLayout lay(TSLayoutX*layoutX,TSLayoutY*layoutY);
            MuonTileID muontile(station,lay,region,
                                quadrant,digitX,digitY);
            m_mapTileInODE[odenum-1].push_back(muontile);
            
          }          
        }        
      }
      m_ODENumberInTell1[countL1]=totODE;      
      countL1++;      
    }
  }

  return StatusCode::SUCCESS;
  
  
};



StatusCode HltMuonRec::createCoordsFromLC(int istation)
{
  

  std::vector<LHCb::MuonTileID>::const_iterator itTile;
  double x, dx, y, dy, z, dz;

  for(unsigned int iregion=0;iregion<m_nRegion;iregion++){
    verbose()<<"readout "<< istation<<" "<<iregion<<" "<<
      m_station[istation].region(iregion)->layoutNumber()<<endreq;
    
    if(m_station[istation].region(iregion)->layoutNumber()==1){
      //already pads!!!!
      HltMuonRegion* thisRegion = m_station[istation].region(iregion);
      std::vector<LHCb::MuonTileID> padTiles = thisRegion->tiles();
      thisRegion->clearPoints();
      for( itTile = padTiles.begin() ; itTile != padTiles.end() ; 
           ++itTile ){          
        
        m_iPosTool-> calcTilePos(*itTile,x, dx,y, dy,z, dz);
        
        //          info()<<istation<<" "<<iregion<<" "<<x<<" "<<y<<endreq;
          
        thisRegion->addPoint( x, y,  *itTile );
      }        
      //info()<<"no crossing "<<istation<<" "<<iregion<<" "<<
      //  ((m_station[istation].region(iregion))->tiles()).size()<<endreq;
      
    }else if(m_station[istation].region(iregion)->layoutNumber()==2){
      //strips: need to crsso 2 layout!!!
      crossStrips(istation,iregion);
      //verbose()<<"crossing "<<istation<<" "<<iregion<<" "<<
      //  ((m_station[istation].region(iregion))->tiles()).size()<<endreq;
      
    }      
    
  }
  
  return StatusCode::SUCCESS;
}

std::string HltMuonRec::getBasePath(int station)  
{
  return basePath[station];
    
};
