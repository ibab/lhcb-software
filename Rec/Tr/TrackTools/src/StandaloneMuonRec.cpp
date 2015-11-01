// Include files

#include <algorithm>

#include "Event/RawEvent.h"

#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"


// local
#include "StandaloneMuonRec.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StandaloneMuonRec
//
// 2004-10-06 : Alessia Satta
//
//  Removed from Hlt/HltMuon and ported to Tr/TrackTools
//
// 2011-03-03 : Paul Seyfert
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( StandaloneMuonRec )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StandaloneMuonRec::StandaloneMuonRec( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MeasureTime"   , m_measureTime   = true );
  declareProperty( "CloneKiller"   , m_cloneKiller   = true );
  declareProperty( "OutputMuonTracksName" ,
                   m_outputMuonTracksName="Hlt1/Track/MuonSeg");
  declareProperty("DecodingFromCoord", m_decodingFromCoord = true );
}
//=============================================================================
// Destructor
//=============================================================================
StandaloneMuonRec::~StandaloneMuonRec() {
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode StandaloneMuonRec::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

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

  //retrieve the pointer to the muon detector

  m_muonDetector=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");

  //retrieve the pointer to the tool to convert tileID to x,y,z in a fast way (using a LUT)

  m_iPosTool  = tool<IMuonFastPosTool>( "MuonFastPosTool" );
  if(!m_iPosTool)info()<<"error retrieving the pos tool "<<endmsg;

  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_muonBuffer)info()<<"error retrieving the decoding tool "<<endmsg;


  //all geometry stuff
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_nStation= basegeometry.getStations();
  m_nRegion = basegeometry.getRegions();
  unsigned int i=0;

  while( i < m_nStation ){
    m_stationNames.push_back( basegeometry.getStationName(i) );
    i++;
  }

  //GP
  m_basePath["M1"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/";
  m_basePath["M2"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M2/";
  m_basePath["M3"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M3/";
  m_basePath["M4"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M4/";
  m_basePath["M5"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M5/";

  //GP as is this won't work without M1
  /*
  m_basePath[0]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/";
  m_basePath[1]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M2/";
  m_basePath[2]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M3/";
  m_basePath[3]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M4/";
  m_basePath[4]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M5/";
  */

 //how many tell1
  m_totL1Board = 0;
  m_stationL1Start[0]=0;

  for(unsigned int station=0;station<m_nStation;station++){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<"station number "<<station<<endmsg;
    // GP changed argument in getBasePath
    std::string cablingBasePath=getBasePath(basegeometry.getStationName(station));


    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);
    if( !cabling ) {
      error() << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }
    m_stationL1Start[station]= m_totL1Board;
    m_totL1Board =m_totL1Board+cabling->getNumberOfL1Board();
    //alessia add the start and stop per station
    m_stationL1Stop[station]= m_totL1Board ;
  }
  //skip M1 in the decoding and reconstruction
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<"boards "<<endmsg;

  m_L1BoardStart=4;
  m_L1BoardStop= m_totL1Board;

  //create container for pad position

  for(unsigned int station=0;station<m_nStation;station++){
    m_station.push_back( StandaloneMuonStationRec(msgSvc()));
    for(unsigned int region = 0 ; region < m_nRegion ; region++ ){
      //set some parameters
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug()<<station<<" "<<region<<endmsg;
      m_station[station].region().push_back( new StandaloneMuonRegion() );
    }
  }

  //set some usefull info about the layout of the muon system

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<"boards "<<endmsg;
  for(unsigned int station=0;station<m_nStation;station++){
    for(unsigned int region = 0 ; region < m_nRegion ; region++ ){
      unsigned int numLayout=m_muonDetector->getLogMapInRegion(station,region);
      StandaloneMuonRegion *r = m_station[station].region(region);
      r->setLayoutNumber(numLayout);
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug()<<"numLayout "<<numLayout<<endmsg;
      for(unsigned int which_layout=0;which_layout<numLayout;which_layout++){
        int xMap=m_muonDetector->getLayoutX (which_layout, station,region);
        int yMap=m_muonDetector->getLayoutY (which_layout, station,region);
        r-> setLayoutGridX(which_layout,(unsigned int) xMap);
        r-> setLayoutGridY(which_layout,(unsigned int) yMap);
      }
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug()<<"numLayout "<<numLayout<<endmsg;
      if(numLayout>1){
        int gridx1=r-> layoutGridX(0);
        int gridx2=r-> layoutGridX(1);
        if(gridx1> gridx2){
          //reverse the order....
          int gridy1=r-> layoutGridY(0);
          int gridy2=r-> layoutGridY(1);
          r-> setLayoutGridX(0,(unsigned int) gridx2);
          r-> setLayoutGridY(0,(unsigned int) gridy2);
          r-> setLayoutGridX(1,(unsigned int) gridx1);
          r-> setLayoutGridY(1,(unsigned int) gridy1);
        }
      }
    }
  }



  //initialze the decoding of the raw buffer

  if(m_decodingFromCoord ){

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
    //  StandaloneMuonRegion* reg = new StandaloneMuonRegion();
    //  //set some parameters
    //  m_station[station].region().push_back( reg );
    // }

    //double deltaX,deltaY,z,deltaZ;
    double zSta= m_muonDetector->getStationZ(station);


    m_station[station].setZ( zSta );
    //set cut for muon reconstruction
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" station "<<endmsg;

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
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode StandaloneMuonRec::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  setFilterPassed(true);


  LHCb::Tracks* outputTracks  = new LHCb::Tracks();
  put(outputTracks, m_outputMuonTracksName);


  int m_countMuonCandidates=0;

  m_countEvents += 1;
  m_padM5=false;
  m_padM4=false;
  m_padM3=false;
  m_padM2=false;


  //FIXME: what about m_station[0]
  for(unsigned  station = 1 ; station < m_nStation ; station++ ){
    m_station[station].clearRegions();
  }



  if(!m_decodingFromCoord){
    if ( m_measureTime ) m_timer->start( m_timeLoad );
    StatusCode scb=decodeBuffer();
    if(scb.isFailure())return scb;
    if ( m_measureTime ) m_timer->stop( m_timeLoad );
  }
  if ( m_measureTime ) m_timer->start( m_timePad );
  // decode only station M5
  if(!m_padM5){
    createCoords(4).ignore();
    m_padM5=true;
  }
  if(!m_padM4){
    createCoords(3).ignore();
    m_padM4=true;
  }

  if ( m_measureTime ) {
    m_timer->stop ( m_timePad );
    m_timer->start( m_timeMuon );
  }

  StatusCode sc = muonSearch();
  if (sc.isFailure()) return sc;
  if(m_cloneKiller)  {
      StatusCode sck=detectClone();
      if(sck.isFailure())return sck;
      sck=strongCloneKiller();
      if(sck.isFailure())return sck;	
  }

  if ( m_measureTime ) {
    m_timer->stop ( m_timeMuon );
    m_timer->start( m_timeMuonStore );
  }


  m_countMuCandidates += m_muonTracks.size();

  for(const auto& muTrack : m_muonTracks ) {
    if(muTrack.clone())continue;

      m_countMuonCandidates++;
      LHCb::Track* trgTr = new LHCb::Track();
      //        trgTr->setType(Track::Muon);

      for (int i=1;i<5;++i) {
        LHCb::State temp;
        temp.setX( muTrack.point(i).x() );
        temp.setY( muTrack.point(i).y() );
        temp.setZ( m_station[i].z()          );
        int ii = std::min(i,3);
        temp.setTx(muTrack.slopeX(ii,ii+1,m_station[ii].z(),m_station[ii+1].z()) );
        temp.setTy(muTrack.slopeY(ii,ii+1,m_station[ii].z(),m_station[ii+1].z()) );
        temp.setLocation(LHCb::State::Muon);
        trgTr->addToStates( temp );
        trgTr->addToLhcbIDs(muTrack.point(i).tile());
      }
      outputTracks->insert(trgTr);
  }

  if ( m_measureTime ) {
    m_timer->stop( m_timeMuonStore );
  }

  m_muonTracks.clear();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug()<<" stored candidates "<<m_countMuonCandidates<<endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode StandaloneMuonRec::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  //to avoid menmory leak delete the regions created in initialize
  std::vector<StandaloneMuonStationRec>::iterator itSt;
  for (itSt=m_station.begin();itSt<m_station.end();itSt++){
    std  ::vector<StandaloneMuonRegion*> regVector=itSt->region();
    std::vector<StandaloneMuonRegion*>::iterator itReg;
    for(itReg=regVector.begin();itReg!=regVector.end();itReg++){
        delete *itReg;
    }
  }

  info () << "Number of events processed: " << double(m_countEvents) << endmsg;
  if ( m_countEvents > 0 )
    info () << "Average number of muon tracks: "
            << double(m_countMuCandidates)/double(m_countEvents) << endmsg;
  else
    info () << "Average number of muon tracks: 0  (obviously)" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=============================================================================
StatusCode StandaloneMuonRec::decodeBuffer() {

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
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<" skipping "<< skip <<" bytes words "<<endmsg;

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
        m_station[tile.station()].region(tile.region())->addTile( tile );

        //        info()<<" qui "<<(unsigned int) tile<<endmsg;
        it++;
        //channels++;
      }
    }
  }
  return StatusCode::SUCCESS;

}



StatusCode StandaloneMuonRec::createCoords(int station) {

  return m_decodingFromCoord ?  getPads(station)
	  		     :  createCoordsFromLC(station);

}



StatusCode StandaloneMuonRec::crossStrips(unsigned int station, unsigned int region) {


  StandaloneMuonRegion* thisRegion = m_station[station].region(region);
  thisRegion->clearPoints();

  std::vector<LHCb::MuonTileID> padTiles = thisRegion->tiles();

  //only same 12 subregion can cross...
  //  std::vector<std::pair<MuonTileID,bool> > twelfthX[12];
  //std::vector<std::pair<MuonTileID,bool> > twelfthY[12];



  unsigned int layoutX[2];
  unsigned int layoutY[2];
  layoutX[0]=thisRegion->layoutGridX(0);
  layoutY[0]=thisRegion->layoutGridY(0);
  layoutX[1]=thisRegion->layoutGridX(1);
  layoutY[1]=thisRegion->layoutGridY(1);



  unsigned int which_third=0;
  std::vector<LHCb::MuonTileID>::const_iterator itTile;
  for( itTile = padTiles.begin() ; itTile != padTiles.end() ;
       ++itTile ){
    unsigned int quadrant=(*itTile).quarter();

    if((itTile->nX())>=(itTile->layout()).xGrid()){
      if((itTile->nY())>=(itTile->layout()).yGrid()) {
         which_third=2;
      } else{
         which_third=1;
      }
    }else{
      which_third=3;
    }


    // insert the pad in the correct container
    if((itTile->layout()).xGrid()== layoutX[0]){
      twelfthX[quadrant*3+which_third-1].
        push_back(std::pair<LHCb::MuonTileID,bool>((*itTile),false));
    }else{
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
         //  x<<" "<<y<<endmsg;
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
//                   info()<<"pad "<<pad.station()<<" "<<x<<" "<<y<<endmsg;
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
}




LHCb::MuonTileID StandaloneMuonRec::intercept(const LHCb::MuonTileID& stripX,
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
}



StatusCode StandaloneMuonRec::muonSearch()
{
  m_muonTracks.clear();
  //  return StatusCode::SUCCESS;

  std::vector<StandaloneMuonPoint>::iterator itM5;
  for (unsigned int region=0;region<m_nRegion;region++){
    StandaloneMuonRegion* thisRegion = m_station[4].region(region);
    for(itM5=thisRegion->points().begin();itM5<thisRegion->points().end();
        itM5++){
      //serach coincidence in M4
      //info()<<"seed "<<(*itM5).x()<<" "<<(*itM5).y()<<endmsg;
      StandaloneMuonPoint* bestCandidateM4=nullptr;
      double x=(*itM5).x()*m_station[3].z()/m_station[4].z();
      double y=(*itM5).y()*m_station[3].z()/m_station[4].z();
      unsigned int regionM5=(*itM5).tile().region();
      m_station[3].findCoincidence(x,y,regionM5,bestCandidateM4);
      // info()<<"M4 concidence "<<x<<" "<<y<<" "<<bestCandidateM4<<endmsg;

      if(!bestCandidateM4)continue;

      if(!m_padM3){
        if ( m_measureTime ) {

          m_timer->stop( m_timeMuon );
          m_timer->start( m_timePad );
        }
        createCoords(2).ignore();
        m_padM3=true;
        if ( m_measureTime ) {
          m_timer->stop( m_timePad );
          m_timer->start( m_timeMuon );
        }
      }

      x=-((*itM5).x()-bestCandidateM4->x())+bestCandidateM4->x();
      y=(bestCandidateM4)->y()*m_station[2].z()/m_station[3].z();
      StandaloneMuonPoint* bestCandidateM3=nullptr;
      unsigned int regionM4=(bestCandidateM4)->tile().region();
      m_station[2].findCoincidence(x,y,regionM4,bestCandidateM3);
      //            info()<<"M3 concidence "<<bestCandidateM3<<endmsg;

      if(!bestCandidateM3)continue;

      if(!m_padM2){
        if ( m_measureTime ) {
          m_timer->stop( m_timeMuon );
          m_timer->start( m_timePad );
        }
        createCoords(1).ignore();
        m_padM2=true;
        if ( m_measureTime ) {
          m_timer->stop( m_timePad );
          m_timer->start( m_timeMuon );
        }
      }

      x=-(bestCandidateM4->x()-bestCandidateM3->x())+bestCandidateM3->x();
      y=(bestCandidateM3)->y()*m_station[1].z()/m_station[2].z();
      StandaloneMuonPoint* bestCandidateM2=nullptr;
      unsigned int regionM3=(bestCandidateM3)->tile().region();
      m_station[1].findCoincidence(x,y,regionM3,bestCandidateM2);
      if(!bestCandidateM2)continue;

      //            info()<<"M2 concidence "<<bestCandidateM2<<endmsg;

      /*      info()<<" muon  M5 "<<(*itM5).x()<<" "<<(*itM5).y()<<endmsg;
      //info()<<" muon  M4 "<<bestCandidateM4->x()<<" "<<
      //  bestCandidateM4->y()<<endmsg;
      //info()<<" muon  M3 "<<bestCandidateM3->x()<<" "<<
       // bestCandidateM3->y()<<endmsg;
      //info()<<" muon  M2 "<<bestCandidateM2->x()<<" "<<
      //bestCandidateM2->y()<<endmsg;*/

      //create the muon track
      //info()<<"ciao "<<m_muonTracks.size()<<endmsg;
      StandaloneMuonTrack muon;
      muon.setPoint(1,*bestCandidateM2);
      muon.setPoint(2,*bestCandidateM3);
      muon.setPoint(3,*bestCandidateM4);
      muon.setPoint(4,*itM5);
      //info()<<"ciao prima "<<m_muonTracks.size()<<endmsg;
      m_muonTracks.push_back(muon);
      //info()<<"ciao dopo "<<m_muonTracks.size()<<endmsg;
    }
  }
  //info()<<"fine "<<m_muonTracks.size()<<endmsg;
  return StatusCode::SUCCESS;

}



StatusCode StandaloneMuonRec::detectClone()
{
 bool sameM2;
 bool sameM3;
 bool sameM4;

 for(auto itMuonTrackFirst=m_muonTracks.begin();
     itMuonTrackFirst<m_muonTracks.end();itMuonTrackFirst++) {
   for(auto itMuonTrackSecond=itMuonTrackFirst+1;
       itMuonTrackSecond<m_muonTracks.end();itMuonTrackSecond++){
     sameM2=false;
     sameM3=false;
     sameM4=false;
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
       double distuno=pow((itMuonTrackFirst->point(4).x()-x_extra5),2)+
         pow((itMuonTrackFirst->point(4).y()-y_extra5),2);
       double distdue=pow((itMuonTrackSecond->point(4).x()-x_extra5),2)+
         pow((itMuonTrackSecond->point(4).y()-y_extra5),2);
       if(distuno>distdue){
         itMuonTrackFirst->setClone();
       }else
         itMuonTrackSecond->setClone();
     }
   }
 }
 return StatusCode::SUCCESS;

}


StatusCode StandaloneMuonRec::strongCloneKiller()
{
 bool sameM2;
 bool sameM3;


 for(auto itMuonTrackFirst=m_muonTracks.begin();
     itMuonTrackFirst<m_muonTracks.end();itMuonTrackFirst++){
   for(auto itMuonTrackSecond=itMuonTrackFirst+1;
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
       double distuno=pow((itMuonTrackFirst->point(4).x()-x_extra5),2)+
         pow((itMuonTrackFirst->point(4).y()-y_extra5),2);
       double distdue=pow((itMuonTrackSecond->point(4).x()-x_extra5),2)+
         pow((itMuonTrackSecond->point(4).y()-y_extra5),2);
       if(distuno>distdue){
         itMuonTrackFirst->setClone();
       }else
         itMuonTrackSecond->setClone();
     }
   }
 }
 return StatusCode::SUCCESS;

}




StatusCode StandaloneMuonRec::printOut()
{

  for(auto itMuonTrack=m_muonTracks.begin();
      itMuonTrack<m_muonTracks.end();itMuonTrack++){
    info()<<itMuonTrack->clone()<<" points along the tracks "
          <<itMuonTrack->point(1).x()<<" "<<
      itMuonTrack->point(1).y()<<" "<<
      itMuonTrack->point(2).x()<< " "<<itMuonTrack->point(2).y()<<" "<<
      itMuonTrack->point(3).x()<< " "<<itMuonTrack->point(3).y()<<" "<<
      itMuonTrack->point(4).x()<< " "<<itMuonTrack->point(4).y()<<endmsg;
  }
  return StatusCode::SUCCESS;

}


StatusCode StandaloneMuonRec::getPads(int station)
{

for (int test=0;test<1;test++){
  for(unsigned int region=0;region< m_nRegion;region++){
     StandaloneMuonRegion* thisRegion = m_station[station].region(region);
     thisRegion->clearPoints();
  }
  std::vector<std::vector<LHCb::MuonTileID>* > pads;

  StatusCode sc=m_muonBuffer->getPadsInStation(station,pads);
  if(sc.isFailure())return sc;

       double x,y,z,dx,dy,dz;

  for(auto iList=pads.begin();iList!=pads.end();iList++){
     for(auto iPad=(*iList)->begin();iPad!=(*iList)->end();iPad++){

       StatusCode sc=m_iPosTool-> calcTilePos(*iPad,x, dx,y, dy,z, dz);
       if(sc.isFailure())break;
       int region=iPad->region();
       StandaloneMuonRegion* thisRegion = m_station[station].region(region);
       thisRegion->addPoint( x, y,*iPad );
    }
  }

}
  return StatusCode::SUCCESS;

}



StatusCode StandaloneMuonRec::initializeLogChanDecoding()
{

 //how many tell1

  unsigned int countL1=0;
  // GP needed to call correctly getBasePath when no M1
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());

  for(int station=0;station<5;station++){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<"station number "<<station<<endmsg;

    //GP
    //  changed argument in getBasePath
    std::string cablingBasePath=getBasePath(basegeometry.getStationName(station));


    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);
    if( !cabling ) {
      error() << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }
    if(station==0)m_M1Tell1=cabling->getNumberOfL1Board();

    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug()<<"L1 number "<<cabling->getL1Name(0)<<endmsg;
      std::string L1path=cablingBasePath+ cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
      unsigned totODE=0;

      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
        std::string ODEpath=cablingBasePath +l1->getODEName(ODEBoard);
        //info()<<"ODE number "<<L1Board<<" "<<l1->getODEName(ODEBoard)<<endmsg;
        SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
        if( !ode ) {
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
            //msg<<MSG::INFO<<"cabling base 2 "<<cablingBasePath<<endmsg;
            unsigned int layout=TSMap->layoutOutputChannel(i);
            unsigned int  layoutX=TSMap->gridXLayout(layout);
            unsigned int  layoutY=TSMap->gridYLayout(layout);
            digitOffSetX=layoutX*TSGridX;
            digitOffSetY=layoutY*TSGridY;
            unsigned int digitX=digitOffSetX+TSMap->gridXOutputChannel(i);
            unsigned int digitY=digitOffSetY+TSMap->gridYOutputChannel(i);
            MuonLayout lay(TSLayoutX*layoutX,TSLayoutY*layoutY);
            MuonTileID muontile(station,lay,region, quadrant,digitX,digitY);
            m_mapTileInODE[odenum-1].push_back(muontile);
          }
        }
      }
      m_ODENumberInTell1[countL1]=totODE;
      countL1++;
    }
  }

  return StatusCode::SUCCESS;


}


StatusCode StandaloneMuonRec::createCoordsFromLC(int istation)
{
  double x, dx, y, dy, z, dz;

  for(unsigned int iregion=0;iregion<m_nRegion;iregion++){
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<"readout "<< istation<<" "<<iregion<<" "<<
        m_station[istation].region(iregion)->layoutNumber()<<endmsg;

    if(m_station[istation].region(iregion)->layoutNumber()==1){
      //already pads!!!!
      StandaloneMuonRegion* thisRegion = m_station[istation].region(iregion);
      std::vector<LHCb::MuonTileID> padTiles = thisRegion->tiles();
      thisRegion->clearPoints();
      for( const auto& tile : padTiles) {

        m_iPosTool-> calcTilePos(tile,x, dx,y, dy,z, dz);

        //          info()<<istation<<" "<<iregion<<" "<<x<<" "<<y<<endmsg;
        thisRegion->addPoint( x, y,  tile );
      }
      //info()<<"no crossing "<<istation<<" "<<iregion<<" "<<
      //  ((m_station[istation].region(iregion))->tiles()).size()<<endmsg;
    }else if(m_station[istation].region(iregion)->layoutNumber()==2){
      //strips: need to crsso 2 layout!!!
      crossStrips(istation,iregion);
      //verbose()<<"crossing "<<istation<<" "<<iregion<<" "<<
      //  ((m_station[istation].region(iregion))->tiles()).size()<<endmsg;
    }
  }

  return StatusCode::SUCCESS;
}
