// Include files

#include "GaudiKernel/SmartDataPtr.h"

// local
#include "MuonDet/MuonDAQHelper.h"
#include "SortTileInTU.h"
#include "SortPairTileInTU.h"
#include "sstream"
//-----------------------------------------------------------------------------
// Implementation file for class : MuonDAQHelper
//
// 2007-12-05 : Alessia Satta
//-----------------------------------------------------------------------------
using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDAQHelper::MuonDAQHelper(  ) : m_msgStream(NULL),
    m_detSvc(0),
    m_msgSvc(0),
    m_TotTell1(0),
    m_M1Tell1(0),
    m_basegeometry(NULL)
{

}
//=============================================================================
// Destructor
//=============================================================================
MuonDAQHelper::~MuonDAQHelper() { 
  delete m_msgStream; 
  m_msgStream = NULL;
  delete m_basegeometry;
}

//=============================================================================

void MuonDAQHelper::initSvc(IDataProviderSvc* detSvc , IMessageSvc * msgSvc )
{
  delete m_msgStream;
  m_msgStream = NULL;

  m_detSvc=detSvc ;
  m_msgSvc =msgSvc;

  m_basegeometry = new MuonBasicGeometry(detSvc,msgSvc);

  m_nStations = m_basegeometry->getStations();
  int isM1defined = m_basegeometry->retrieveM1status();

  basePath["M1"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/";
  basePath["M2"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M2/";
  basePath["M3"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M3/";
  basePath["M4"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M4/";
  basePath["M5"]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M5/";

  if(isM1defined) {
    
    m_layoutX[0][0]=24;
    m_layoutX[0][1]=24;
    m_layoutX[0][2]=24;
    m_layoutX[0][3]=24;
    m_layoutX[0][4]=48;
    m_layoutX[0][5]=48;
    m_layoutX[0][6]=48;
    m_layoutX[0][7]=48;
    m_layoutX[1][4]=8;
    m_layoutX[1][5]=4;
    m_layoutX[1][6]=2;
    m_layoutX[1][7]=2;
    m_layoutX[0][8]=48;
    m_layoutX[0][9]=48;
    m_layoutX[0][10]=48;
    m_layoutX[0][11]=48;
    m_layoutX[1][8]=8;
    m_layoutX[1][9]=4;
    m_layoutX[1][10]=2;
    m_layoutX[1][11]=2;
    m_layoutX[0][12]=12;
    m_layoutX[0][13]=12;
    m_layoutX[0][14]=12;
    m_layoutX[0][15]=12;
    m_layoutX[1][13]=4;
    m_layoutX[1][14]=2;
    m_layoutX[1][15]=2;
    m_layoutX[0][16]=12;
    m_layoutX[0][17]=12;
    m_layoutX[0][18]=12;
    m_layoutX[0][19]=12;
    m_layoutX[1][17]=4;
    m_layoutX[1][18]=2;
    m_layoutX[1][19]=2;
    
    m_layoutY[0][0]=8;
    m_layoutY[0][1]=8;
    m_layoutY[0][2]=8;
    m_layoutY[0][3]=8;
    m_layoutY[0][4]=1;
    m_layoutY[0][5]=2;
    m_layoutY[0][6]=2;
    m_layoutY[0][7]=2;
    m_layoutY[1][4]=8;
    m_layoutY[1][5]=8;
    m_layoutY[1][6]=8;
    m_layoutY[1][7]=8;
    m_layoutY[0][8]=1;
    m_layoutY[0][9]=2;
    m_layoutY[0][10]=2;
    m_layoutY[0][11]=2;
    m_layoutY[1][8]=8;
    m_layoutY[1][9]=8;
    m_layoutY[1][10]=8;
    m_layoutY[1][11]=8;
    m_layoutY[0][12]=8;
    m_layoutY[0][13]=2;
    m_layoutY[0][14]=2;
    m_layoutY[0][15]=2;
    m_layoutY[1][13]=8;
    m_layoutY[1][14]=8;
    m_layoutY[1][15]=8;
    m_layoutY[0][16]=8;
    m_layoutY[0][17]=2;
    m_layoutY[0][18]=2;
    m_layoutY[0][19]=2;
    m_layoutY[1][17]=8;
    m_layoutY[1][18]=8;
    m_layoutY[1][19]=8;
    
    // this is the number of layouts (i.e. H and V)    
    layout[0]=1; // only pads
    layout[1]=1;
    layout[2]=1;
    layout[3]=1;
    layout[4]=2; // H and V strips
    layout[5]=2;
    layout[6]=2;
    layout[7]=2;
    layout[8]=2;
    layout[9]=2;
    layout[10]=2;
    layout[11]=2;
    layout[12]=1; // only pads
    layout[13]=2; // H and V strips
    layout[14]=2;
    layout[15]=2;
    layout[16]=1; // only pads
    layout[17]=2; // H and V strips
    layout[18]=2;
    layout[19]=2;
  } else {
    //    m_layoutX[0][0]=24;  //M1
    //    m_layoutX[0][1]=24;  //M1
    //    m_layoutX[0][2]=24;  //M1
    //    m_layoutX[0][3]=24;  //M1
    m_layoutX[0][ 0]=48; //M2 vstrips
    m_layoutX[0][ 1]=48; //M2 vstrips
    m_layoutX[0][ 2]=48; //M2 vstrips
    m_layoutX[0][ 3]=48; //M2 vstrips
    m_layoutX[1][ 0]=8;  //M2 hstrips
    m_layoutX[1][ 1]=4;  //M2 hstrips
    m_layoutX[1][ 2]=2;  //M2 hstrips
    m_layoutX[1][ 3]=2;  //M2 hstrips
    m_layoutX[0][ 4]=48; //M3 vstrips
    m_layoutX[0][ 5]=48; //M3 vstrips
    m_layoutX[0][ 6]=48; //M3 vstrips
    m_layoutX[0][ 7]=48; //M3 vstrips
    m_layoutX[1][ 4]=8;  //M3 hstrips
    m_layoutX[1][ 5]=4;  //M3 hstrips
    m_layoutX[1][ 6]=2;  //M3 hstrips
    m_layoutX[1][ 7]=2;  //M3 hstrips
    m_layoutX[0][ 8]=12; //M4 vstrips
    m_layoutX[0][ 9]=12; //M4 vstrips
    m_layoutX[0][10]=12; //M4 vstrips
    m_layoutX[0][11]=12; //M4 vstrips
    m_layoutX[1][ 9]=4;  //M4 hstrips
    m_layoutX[1][10]=2;  //M4 hstrips
    m_layoutX[1][11]=2;  //M4 hstrips
    m_layoutX[0][12]=12; //M5 vstrips
    m_layoutX[0][13]=12; //M5 vstrips
    m_layoutX[0][14]=12; //M5 vstrips
    m_layoutX[0][15]=12; //M5 vstrips
    m_layoutX[1][13]=4;  //M5 hstrips
    m_layoutX[1][14]=2;  //M5 hstrips
    m_layoutX[1][15]=2;  //M5 hstrips
 
    //    m_layoutY[0][0]=8; //M1
    //    m_layoutY[0][1]=8; //M1
    //    m_layoutY[0][2]=8; //M1
    //    m_layoutY[0][3]=8; //M1
    m_layoutY[0][ 0]=1;  //M2 vstrips
    m_layoutY[0][ 1]=2;	//M2 vstrips
    m_layoutY[0][ 2]=2;	//M2 vstrips
    m_layoutY[0][ 3]=2;	//M2 vstrips
    m_layoutY[1][ 0]=8;	//M2 hstrips
    m_layoutY[1][ 1]=8;	//M2 hstrips
    m_layoutY[1][ 2]=8;	//M2 hstrips
    m_layoutY[1][ 3]=8;	//M2 hstrips
    m_layoutY[0][ 4]=1;	//M3 vstrips
    m_layoutY[0][ 5]=2;	//M3 vstrips
    m_layoutY[0][ 6]=2;	//M3 vstrips
    m_layoutY[0][ 7]=2;	//M3 vstrips
    m_layoutY[1][ 4]=8;	//M3 hstrips
    m_layoutY[1][ 5]=8;	//M3 hstrips
    m_layoutY[1][ 6]=8;	//M3 hstrips
    m_layoutY[1][ 7]=8;	//M3 hstrips
    m_layoutY[0][ 8]=8;	//M4 vstrips
    m_layoutY[0][ 9]=2;	//M4 vstrips
    m_layoutY[0][10]=2;	//M4 vstrips
    m_layoutY[0][11]=2;	//M4 vstrips
    m_layoutY[1][ 9]=8;	//M4 hstrips
    m_layoutY[1][10]=8;	//M4 hstrips
    m_layoutY[1][11]=8;	//M4 hstrips
    m_layoutY[0][12]=8;	//M5 vstrips
    m_layoutY[0][13]=2;	//M5 vstrips
    m_layoutY[0][14]=2;	//M5 vstrips
    m_layoutY[0][15]=2;	//M5 vstrips
    m_layoutY[1][13]=8;	//M5 hstrips
    m_layoutY[1][14]=8;	//M5 hstrips
    m_layoutY[1][15]=8;	//M5 hstrips
    
    
    //    layout[0]=1; //M1
    //    layout[1]=1;
    //    layout[2]=1;
     //   layout[3]=1;
    layout[ 0]=2; //M2
    layout[ 1]=2;
    layout[ 2]=2;
    layout[ 3]=2;
    layout[ 4]=2;
    layout[ 5]=2;
    layout[ 6]=2;
    layout[ 7]=2;
    layout[ 8]=1;
    layout[ 9]=2;
    layout[10]=2;
    layout[11]=2;
    layout[12]=1;
    layout[13]=2;
    layout[14]=2;
    layout[15]=2;
  }
}

//------------------------------------------------------------------------
//
// Steering routine to retrieve the information 
// about cabling in the condition database
//
//------------------------------------------------------------------------

StatusCode MuonDAQHelper::initDAQMaps()
{
  // Perform sanity checks about number of TELL1s and ODEs
  StatusCode sc;
  //
  // TELL1 sanity checks
  //
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"Initialize TELL1 "<<endmsg;

  sc=initTELL1();
  if(sc.isFailure()){
    msgStream()<<MSG::ERROR<<"TELL1 sanity checks failed"<<endmsg;
    return sc;
  }

  //
  // ODE sanity checks
  //
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" Initialize ODE "<<endmsg;

  sc=initODE();
  if(sc.isFailure()){
    msgStream()<<MSG::ERROR<<"ODE sanity checks failed"<<endmsg;
    return sc;
  }

  //
  // Ititialize TELL1/ODE maps
  //
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" Initialize TELL1/ODE maps "<<endmsg;

  sc=initMaps();
  if(sc.isFailure()){
    msgStream()<<MSG::ERROR<<"TELL1/ODE map initialization failed"<<endmsg;
    return sc;
  }

  //
  // TELL1 pad LUT
  //
  sc=initLUTCrossing();
  if(sc.isFailure()){
    msgStream()<<MSG::ERROR<<"TELL1 pad LUT initialization failed"<<endmsg;
    return sc;
  }

  //
  // reverse mapping
  //
  sc=initReverseMaps();
  if(sc.isFailure()){
    msgStream()<<MSG::ERROR<<"Reverse mapping initialization failed"<<endmsg;
    return sc;
  }

  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"Map initialization sucessful"<<endmsg;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------
//
// Perform sanity cheks on number of configured TELL1
// in CondDB
//
//------------------------------------------------------------------------

StatusCode MuonDAQHelper::initTELL1()
{
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"Retrieve the number of TELL1s and perform a sanity check"<<endmsg;

  m_TotTell1=0;  // TELL1 counter

  for(int station=0; station<m_nStations; station++){ // loop over stations

    // parse the xml file and load the parameters
    std::string cablingBasePath=getBasePath(m_basegeometry->getStationName(station)); // hardwired at the moment
    std::string cablingPath=cablingBasePath+"Cabling"; // path to the Cabling Condition for station
    SmartDataPtr<MuonStationCabling> cabling(m_detSvc, cablingPath); 

    if( UNLIKELY( 0 == cabling) ) {
      msgStream()<<MSG::ERROR << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }

    m_TotTell1 = m_TotTell1 + cabling->getNumberOfL1Board(); // update the TELL1 counter

    // special counter for M1 TELL1s
    if(m_basegeometry->getStationName(station) == "M1") m_M1Tell1=cabling->getNumberOfL1Board();
    //

  } // end of loop over stations

  if( UNLIKELY( m_TotTell1 > MuonDAQHelper_maxTell1Number ) ){
    msgStream()<<MSG::ERROR<<"Number of retrieved TELL1 in CondDB exceeds max allowed in software "
	       <<endmsg;
    return StatusCode::FAILURE;
  }

  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"Number of retrieved TELL1 in CondDB: "<<m_TotTell1<<endmsg;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------
//
// Perform sanity cheks on number of configured ODE
// in CondDB
//
//------------------------------------------------------------------------

StatusCode MuonDAQHelper::initODE()
{
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" Retrieve the ODEs connected to each TELL1"<<endmsg;

  unsigned int ODECounter=0;

  for(int station=0; station < m_nStations; station++){ // loop over stations

    // load the parameters
    std::string cablingBasePath=getBasePath(m_basegeometry->getStationName(station));
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling> cabling(m_detSvc, cablingPath);

    if( UNLIKELY( 0 == cabling) ) {
      msgStream()<<MSG::ERROR << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }

    for(int L1Board=0; L1Board < cabling->getNumberOfL1Board(); L1Board++){ // loop over TELL1s

      // path to the MxL1BoardNumbery Condition
      std::string L1Path = cablingBasePath+cabling->getL1Name(L1Board);
      // parse the xml file and load the parameters
      SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);

      if( UNLIKELY( 0 == l1) ) {
	msgStream()<<MSG::ERROR << L1Path << " not found in XmlDDDB" << endmsg;
	return StatusCode::FAILURE;
      }

      ODECounter=ODECounter+l1->numberOfODE();

    } // end of loop over TELL1s
  } // end of loop over stations

  if( UNLIKELY( ODECounter > MuonDAQHelper_maxODENumber ) ){
    msgStream()<<MSG::ERROR<<" Number of retrieved ODEs in CondDB exceeds max allowed in software: "
               <<ODECounter<<endmsg;
    return StatusCode::FAILURE;
  }

  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"Number of retrieved ODEs in CondDB: "<<ODECounter<<endmsg;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------
//
//  load TELL1/ODE pad mapping parameters
// 
//
//------------------------------------------------------------------------

StatusCode MuonDAQHelper::initMaps()
{
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"Load TELL1/ODE mapping parameters "<<endmsg;

  int Tell1Number=0;
  for(int station=0; station < m_nStations; station++){

    std::string cablingBasePath = getBasePath(m_basegeometry->getStationName(station));
    std::string cablingPath     = cablingBasePath + "Cabling";
    SmartDataPtr<MuonStationCabling> cabling(m_detSvc, cablingPath);

    if( UNLIKELY( 0 == cabling ) ) {
      msgStream()<<MSG::ERROR << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }

    for(int L1Board=0; L1Board < cabling->getNumberOfL1Board(); L1Board++){ // loop over TELL1s

      std::string L1Path = cablingBasePath+cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board> l1(m_detSvc,L1Path);

      if( UNLIKELY( 0 == l1) ) {
	msgStream()<<MSG::ERROR << L1Path << " not found in XmlDDDB" << endmsg;
	return StatusCode::FAILURE;
      }

      m_TELL1Name.push_back(L1Path); // array of TELL1 names
      // load list of TELL1 serial numbers for each station
      m_tellPerStation[station].push_back(l1->L1Number()); 

      for(unsigned int i=0; i < MuonDAQHelper_linkNumber; i++){ // loop over TELL1 inputs
	//
        if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
          msgStream()<<MSG::DEBUG<<" TELL1 Optical Link: "
		     <<i<<" is connected to ODE: "<<l1->getLinkConnection(i)<<endmsg;
	//

	// load optical link map per TELL1
	//        m_linkInTell1[Tell1Number].push_back(l1->getLinkConnection(i)); // <-- modify it !!!
        m_linkInTell1[l1->L1Number()].push_back(l1->getLinkConnection(i)); // <-- modify it !!!

      } // end of loop over TELL1 inputs

      //
      // get mapping for the ODE connected to each TELL1
      //
      unsigned totODE = 0; // counter of ODEs per TELL1
      for(int ODEBoard=0; ODEBoard < l1->numberOfODE(); ODEBoard++){ // loop over ODES
        unsigned int ODE_channels = 0;

        std::string ODEPath = cablingBasePath+l1->getODEName(ODEBoard);
        SmartDataPtr<MuonODEBoard> ode(m_detSvc,ODEPath);

        if( UNLIKELY( 0 == ode ) ) {
          msgStream()<<MSG::ERROR << ODEPath << " not found in XmlDDDB" << endmsg;
          return StatusCode::FAILURE;
        }
        totODE++;
        long odenum = ode->getODESerialNumber();
        m_ODEInTell1[Tell1Number].push_back(odenum); // ODE numbers in this TELL1
        m_ODENameInECS[odenum]=ode->ECSName(); // ONLINE ODE names

	//
	// get the trigger sector (TS) maps
	//
        unsigned int region    = ode->region();
	unsigned int TSLayoutX = ode->getTSLayoutX();
	unsigned int TSLayoutY = ode->getTSLayoutY();
        for(int TS=0; TS < ode->getTSNumber(); TS++){ // loop over TS

          std::string TSPath = cablingBasePath+ode->getTSName(TS);
          SmartDataPtr<MuonTSMap>  TSMap(m_detSvc,TSPath);

	  if( UNLIKELY( 0 == TSMap) ) {
	    msgStream()<<MSG::ERROR << TSPath << " not found in XmlDDDB" << endmsg;
	    return StatusCode::FAILURE;
	  }

          unsigned int quadrant  = ode->getTSQuadrant(TS);
          unsigned int TSGridX   = ode->getTSGridX(TS);
          unsigned int TSGridY   = ode->getTSGridY(TS);

          m_TUSize[station][region]=TSMap->synchChSize(); // #of sync channels in this TS

          int iOdeChan = 0;
          for(int isynch=0; isynch<TSMap->synchChSize(); isynch++){
	    
            if( UNLIKELY( msgStream().level() <= MSG::VERBOSE ) )
              msgStream()<<MSG::VERBOSE<<" SYNC channel "<<isynch<<" is used/unused (1/0): "
			 <<TSMap->synchChUsed(isynch)<<endmsg;
	    
            ODE_channels++;
	    
            if(TSMap->synchChUsed(isynch)){
              unsigned int layout  = TSMap->layoutOutputChannel(iOdeChan);
              unsigned int layoutX = TSMap->gridXLayout(layout);
              unsigned int layoutY = TSMap->gridYLayout(layout);
              unsigned int digitOffSetX = layoutX*TSGridX;
              unsigned int digitOffSetY = layoutY*TSGridY;
              unsigned int digitX = digitOffSetX + TSMap->gridXOutputChannel(iOdeChan);
              unsigned int digitY = digitOffSetY + TSMap->gridYOutputChannel(iOdeChan);
              MuonLayout lay(TSLayoutX*layoutX, TSLayoutY*layoutY); // layout in terms of H- or V- strips
              MuonTileID muontile(station, lay, region, quadrant, digitX, digitY);

	      // load maps
              m_mapTileInTell1[Tell1Number].push_back(muontile); //tile map by TELL1
              m_mapTileInODE[odenum-1].push_back(muontile); // tile map by ODE
              m_mapTileInODEDC06[odenum-1].push_back(muontile); // special for DC06...

              iOdeChan++;

            } else { // empty tiles for unused channels
              MuonTileID muontile;
              m_mapTileInTell1[Tell1Number].push_back(muontile);
              m_mapTileInODE[odenum-1].push_back(muontile);
            }

          }
        }

        //check if 192 channels/ODE have been filled , if not fill with zero
        if(ODE_channels != 192){
          for(unsigned int ODE_ch_missing=ODE_channels;
	      ODE_ch_missing<192; ODE_ch_missing++){
	    MuonTileID muontile;
	    m_mapTileInTell1[Tell1Number].push_back(muontile);
	  }
        }
	//

      } // end loop over ODEs
      
      m_ODENumberInTell1[Tell1Number]=totODE; // #ODEs in this TELL1
      Tell1Number++;

    } // end loop over TELL1s
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------
//
// Initialize TELL1 pad LUT (to emulate pad mapping in TELL1s)
// 
//
//------------------------------------------------------------------------
/*
StatusCode MuonDAQHelper::initLUTCrossing()
{
  int Tell1Number=m_M1Tell1;

  for(int station=0; station<m_nStations; station++){

    std::string cablingBasePath = getBasePath(m_basegeometry->getStationName(station));
    std::string cablingPath     = cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling> cabling(m_detSvc, cablingPath);

    for(int L1Board=0; L1Board < cabling->getNumberOfL1Board(); L1Board++){

      std::string L1Path = cablingBasePath + cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board> l1(m_detSvc,L1Path);

      if( UNLIKELY( 0 == l1) ) {
	msgStream()<<MSG::ERROR << L1Path << " not found in XmlDDDB" << endmsg;
	return StatusCode::FAILURE;
      }

      for(int ODEBoard=0; ODEBoard < l1->numberOfODE();ODEBoard++){

        std::string ODEPath=cablingBasePath + l1->getODEName(ODEBoard);
        SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);

	if( UNLIKELY( 0 == ode) ) {
	  msgStream()<<MSG::ERROR << ODEPath << " not found in XmlDDDB" << endmsg;
	  return StatusCode::FAILURE;
	}

        unsigned int region=ode->region();
	unsigned int TSLayoutX = ode->getTSLayoutX();
	unsigned int TSLayoutY = ode->getTSLayoutY();

        MuonTSMap* FirstTSMap = NULL;

        for(int TS=0;TS<ode->getTSNumber();TS++){

          unsigned int quadrant  = ode->getTSQuadrant(TS);
          unsigned int TSGridX   = ode->getTSGridX(TS);
          unsigned int TSGridY   = ode->getTSGridY(TS);

          std::string  TSPath= cablingBasePath + ode->getTSName(TS);
          SmartDataPtr<MuonTSMap>  TSMap(m_detSvc,TSPath);

          if(TS==0){
            FirstTSMap=static_cast<MuonTSMap*>(TSMap);
          }

          std::vector<LHCb::MuonTileID> digitInTS;
          std::vector<LHCb::MuonTileID> wrongdigitInTS; // this is to patch a Firmware bug

          for(int i=0 ; i < TSMap->numberOfOutputSignal(); i++){

            unsigned int layout  = TSMap->layoutOutputChannel(i);
            unsigned int layoutX = TSMap->gridXLayout(layout);
            unsigned int layoutY = TSMap->gridYLayout(layout);
            unsigned int digitOffSetX = layoutX*TSGridX;
            unsigned int digitOffSetY = layoutY*TSGridY;
            unsigned int digitX = digitOffSetX+TSMap->gridXOutputChannel(i);
            unsigned int digitY = digitOffSetY+TSMap->gridYOutputChannel(i);

            MuonLayout lay(TSLayoutX*layoutX, TSLayoutY*layoutY);
            MuonTileID muontile(station, lay, region, quadrant, digitX, digitY);
            digitInTS.push_back(muontile);

	    // this is to patch a Firmware bug
            unsigned int wrongDigitX=digitOffSetX+FirstTSMap->gridXOutputChannel(i);
            unsigned int wrongDigitY=digitOffSetY+FirstTSMap->gridYOutputChannel(i);
            MuonTileID wrongmuontile(station,lay,region,
                                     quadrant,wrongDigitX,wrongDigitY);
            wrongdigitInTS.push_back(wrongmuontile);
	    //

          }

          std::vector<LHCb::MuonTileID> crossAddressDC06 = DoPadDC06(digitInTS,TSMap);
          std::vector<LHCb::MuonTileID> crossAddressV1   = DoPadV1(digitInTS,wrongdigitInTS,TSMap);
          std::vector<LHCb::MuonTileID>::iterator itPad;
          for(itPad=crossAddressDC06.begin();itPad<crossAddressDC06.end();
              itPad++){
            m_mapPadDC06[Tell1Number].push_back(*itPad);
          }
          for(itPad=crossAddressV1.begin();itPad<crossAddressV1.end(); itPad++){
            m_mapPadV1[Tell1Number].push_back(*itPad);
          }
        }
      }
      Tell1Number++;
    }
  }
  return StatusCode::SUCCESS;
}
*/
//------------------------------------------------------------------------
//
// Initialize TELL1/ODE mapping by quadrant. Useful to retrieve
// information knowing for example the TS or other geographical parameters
//
//------------------------------------------------------------------------

StatusCode MuonDAQHelper::initReverseMaps()
{
  unsigned int ODEStart=0;
  unsigned int ODEEnd=1;

  m_L1Name.resize(80); // internal array of TELL1 names

  for(int station=0; station < m_nStations; station++){ // loop over stations

    std::string cablingBasePath = getBasePath(m_basegeometry->getStationName(station));
    std::string cablingPath     = cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling> cabling(m_detSvc, cablingPath);

    if( UNLIKELY( 0 == cabling ) ) {
      msgStream()<<MSG::ERROR<< cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }

    for(int L1Board=0; L1Board <cabling->getNumberOfL1Board(); L1Board++){//loop over TELL1

      std::string L1Path=cablingBasePath + cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board> l1(m_detSvc,L1Path);

      if( UNLIKELY( 0 == l1) ) {
	msgStream()<<MSG::ERROR << L1Path << " not found in XmlDDDB" << endmsg;
	return StatusCode::FAILURE;
      }

      for(int ODEBoard=0; ODEBoard < l1->numberOfODE(); ODEBoard++){ // loop over ODEs
        std::string ODEPath=cablingBasePath + l1->getODEName(ODEBoard);
        SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);

        if( UNLIKELY( 0 == ode ) ) {
          msgStream()<<MSG::ERROR << ODEPath << " not found in XmlDDDB" << endmsg;
          return StatusCode::FAILURE;
        }

        long region = ode->region();
        for(int quadrant=0; quadrant < 4; quadrant++){ // loop over quadrants
          unsigned int index=station*16+region*4+quadrant;

          if( ode->isQuadrantContained(quadrant) ){
            m_L1Name[index]=L1Path; // fill all the TELL1 names by quadrant
          }
        } // end loop over quadrants
      } // end loop over ODEs
    } // end loop over TELL1s
  } // end loop over stations

  for(int station=0; station < m_nStations; station++){
    std::string cablingBasePath = getBasePath(m_basegeometry->getStationName(station));

    for(int region=0; region < 4; region++){
      for(int quadrant=0;quadrant < 4; quadrant++){
        int numODE=0;
        unsigned int index=station*16+region*4+quadrant;

        std::string L1Path=m_L1Name[index];
        SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);

	if( UNLIKELY( 0 == l1) ) {
	  msgStream()<<MSG::ERROR << L1Path << " not found in XmlDDDB" << endmsg;
	  return StatusCode::FAILURE;
	}

        for(int ODEBoard=0; ODEBoard < l1->numberOfODE(); ODEBoard++){

          std::string ODEPath=cablingBasePath + l1->getODEName(ODEBoard);
          SmartDataPtr<MuonODEBoard> ode(m_detSvc,ODEPath);

	  if( UNLIKELY( 0 == ode ) ) {
	    msgStream()<<MSG::ERROR << ODEPath << " not found in XmlDDDB" << endmsg;
	    return StatusCode::FAILURE;
	  }

          if( ode->region() == region){
            if(ode->isQuadrantContained(quadrant)){
              numODE++;
              m_ODEName.push_back(ODEPath); // ode names by quadrant
            }
          }
        }

	// questa roba mi sembra un orrore, revise deeply !
        m_ODENameStart[station][region][quadrant]=ODEStart;
        ODEEnd=ODEStart+numODE;
        m_ODENameEnd[station][region][quadrant]=ODEEnd;
        ODEStart=ODEEnd;
      }
    }
  }

  return StatusCode::SUCCESS;

}

//------------------------------------------------------------------------
//
// Class methods
// 
//
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// get the CondDB base path for the Cabling condition
//------------------------------------------------------------------------

std::string MuonDAQHelper::getBasePath(std::string statname)
{
  return basePath[statname];
}

//------------------------------------------------------------------------
// Create pads. Backward compatibility with DC06
//------------------------------------------------------------------------

std::vector<LHCb::MuonTileID> MuonDAQHelper::DoPadDC06(std::vector<
                                                       LHCb::MuonTileID> digit,
                                                       MuonTSMap* TS)
{
  std::vector<LHCb::MuonTileID> list_of_pads;

  if(TS->numberOfLayout()==2){
    int NX=TS->gridXLayout(1);
    int NY=TS->gridYLayout(0);
    int Nsub=NX*NY;

    int maxPads=TS->gridXLayout(0)*TS->gridYLayout(1);
    list_of_pads.reserve(maxPads);

    LHCb::MuonTileID t;
    for(int i=0;i<maxPads;i++){
      list_of_pads.push_back(t);
    }
    // work on sub sector
    if(Nsub>8){
      return list_of_pads;
    }

    std::vector<unsigned int> horiz[8];
    std::vector<unsigned int> hvert[8];

    // clear the memory;
    // start fill the sub sector matrices
    std::vector<LHCb::MuonTileID>::iterator it;
    int index=0;
    for(it=digit.begin();it<digit.end();it++,index++){
      int mx=TS->gridXOutputChannel(index)/
        (TS->gridXLayout(TS->layoutOutputChannel(index))/NX);
      int my=TS->gridYOutputChannel(index)/
        (TS->gridYLayout(TS->layoutOutputChannel(index))/NY);
      int Msub=mx+my*NX;

      // horizntal o vertical?
      bool horizontal=false;
      if(TS->layoutOutputChannel(index)==0) horizontal=true;
      if(horizontal)horiz[Msub].push_back(*it);
      else hvert[Msub].push_back(*it);
    }

    // now the address of fired pads..
    for(int i=0;i<Nsub;i++){
      // cross only local to each sub matrix
      std::vector<unsigned int>::iterator itx;
      std::vector<unsigned int>::iterator ity;
      //debug
      //debug()<<" sub matrix "<<i<<endmsg;
      //debug()<<" horizontal sequence ";
      for(itx=horiz[i].begin();
          itx<horiz[i].end();itx++){
        //debug()<<*itx<<" ";
      }
      //debug()<<endmsg;
      //debug()<<" vertical sequence ";
      for(ity=hvert[i].begin();ity<hvert[i].end();ity++){
        //debug()<<*ity<<" ";
      }
      //debug()<<endmsg;
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
          //debug()<<" result of the address "<<address<<endmsg;
          LHCb::MuonTileID padTile=tileY.intercept(*itx);
          list_of_pads[address]=padTile;
          //debug()<<" TS dec "<<address<<" "<<
          //      padTile.layout()<<" "<<padTile.station()<<" "<<
          //  padTile.region()<<" "<<padTile.quarter()<<" "<<
          //  padTile.nX()<<" "<<padTile.nY()<<" "<<endmsg;

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
      // debug()<<" result of the address "<<address<<endmsg;
    }
  }
  return list_of_pads;
}


std::vector<LHCb::MuonTileID> MuonDAQHelper::DoPadV1(std::vector<LHCb::
                                                     MuonTileID> digit,
                                                     std::vector<LHCb::
                                                     MuonTileID> wrongdigit,
                                                     MuonTSMap* TS)
{
  std::vector<LHCb::MuonTileID> list_of_pads;
  std::vector<LHCb::MuonTileID> wrong_list_of_pads;
  std::vector<LHCb::MuonTileID>::iterator it1;
  std::vector<LHCb::MuonTileID>::iterator it2;

  int index=0;
  bool wrongFirmware = false;

  int station = digit.begin()->station();
  int region  = digit.begin()->region();

  if(m_basegeometry->getStationName(station) == "M4" && region==0)wrongFirmware=true;
  if(m_basegeometry->getStationName(station) == "M5" && region==0)wrongFirmware=true;
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"Station/Region with wrong firmware: "
	       <<station<<" "<<region<<" "<<wrongFirmware<<endmsg;

  // regions with logical strips
  if(TS->numberOfLayout()==2){
    for(it1=digit.begin(); it1<digit.end(); it1++){
      for(it2=it1+1; it2<digit.end(); it2++){
	
        LHCb::MuonTileID cross=it1->intercept(*it2);
        if(cross.isValid()){
          list_of_pads.push_back(cross);
        }
      }
    }
  } else {
    if(wrongFirmware)it2=wrongdigit.begin();
    for(it1=digit.begin();it1<digit.end();it1++,index++){
      list_of_pads.push_back(*it1);
      if(wrongFirmware){
	
        wrong_list_of_pads.push_back(*it2);
        it2++;
      }
    }
  }

  if(!wrongFirmware){
    std::stable_sort(list_of_pads.begin(),list_of_pads.end(),SortTileInTU());
  }else if(wrongFirmware){
    std::vector<std::pair<LHCb::MuonTileID,LHCb::MuonTileID> > doubleList;
    it2= wrong_list_of_pads.begin();

    for(it1=list_of_pads.begin();it1<list_of_pads.end();it1++){
      std::pair<LHCb::MuonTileID,LHCb::MuonTileID> tilePair;
      tilePair.first=*it2;
      tilePair.second=*it1;
      doubleList.push_back(tilePair);

      it2++;

    }
    std::stable_sort(doubleList.begin(),doubleList.end(),SortPairTileInTU());
    list_of_pads.clear();
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
      msgStream()<<MSG::DEBUG<<" after reordering "<<endmsg;

    std::vector<std::pair<LHCb::MuonTileID,LHCb::MuonTileID> >::iterator iPair;
    for(iPair=doubleList.begin();iPair!=doubleList.end();iPair++){
      std::pair<LHCb::MuonTileID,LHCb::MuonTileID> tilePair=*iPair;
      if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
        msgStream()<<MSG::DEBUG<<tilePair.second<<endmsg;

      list_of_pads.push_back(tilePair.second);

    }
  }
  return list_of_pads;
}





LHCb::MuonTileID MuonDAQHelper::getADDInLink(unsigned int Tell1_num,
                                             long link_num,long ch){
  //check if ODE is connect to link
  MuonTileID emptyTile;
  if(Tell1_num<=m_TotTell1){
    if(link_num<24){
      if((m_linkInTell1[Tell1_num])[link_num]>0&&
         (m_linkInTell1[Tell1_num])[link_num]<
         static_cast<int>(MuonDAQHelper_maxODENumber)){
        if(static_cast<unsigned int> (ch)<
           (m_mapTileInODE[(m_linkInTell1[Tell1_num])[link_num]]).size())
          return (m_mapTileInODE[(m_linkInTell1[Tell1_num])[link_num]])[ch];

      }
    }
  }
  return emptyTile;
}

LHCb::MuonTileID MuonDAQHelper::getADDInODE(long ODE_num, long ch){
  MuonTileID emptyTile;
  if(ODE_num<static_cast<int>(MuonDAQHelper_maxODENumber)){
    if(static_cast<unsigned int> (ch)<
       (m_mapTileInODE[ODE_num]).size())return (m_mapTileInODE[ODE_num])[ch];
  }
  return emptyTile;
}

LHCb::MuonTileID MuonDAQHelper::getADDInODENoHole(long ODE_num, long ch){
  MuonTileID emptyTile;
  if(ODE_num<static_cast<int>(MuonDAQHelper_maxODENumber)){
    if(static_cast<unsigned int> (ch)<(m_mapTileInODEDC06[ODE_num]).size())
      return (m_mapTileInODEDC06[ODE_num])[ch];
  }
  return emptyTile;
}

LHCb::MuonTileID MuonDAQHelper::getADDInTell1(unsigned int Tell1_num, long ch){
  MuonTileID emptyTile;
  if(Tell1_num<=m_TotTell1){
    if(static_cast<unsigned int> (ch)<(m_mapTileInTell1[Tell1_num]).size()){
      return (m_mapTileInTell1[Tell1_num])[ch];
    }
  }
  return emptyTile;
}


LHCb::MuonTileID MuonDAQHelper::getPadInTell1DC06(unsigned int Tell1_num,
                                                  long pad){
  MuonTileID emptyTile;
  if(Tell1_num<=m_TotTell1){
    if(static_cast<unsigned int> (pad)<(m_mapPadDC06[Tell1_num]).size()){
      return (m_mapPadDC06[Tell1_num])[pad];
    }
  }
  return emptyTile;

}

LHCb::MuonTileID MuonDAQHelper::getPadInTell1V1(unsigned int Tell1_num,
                                                long pad){
  MuonTileID emptyTile;
  if(Tell1_num<=m_TotTell1){
    if(static_cast<unsigned int> (pad)<(m_mapPadV1[Tell1_num]).size()){
      return (m_mapPadV1[Tell1_num])[pad];
    }
  }
  return emptyTile;

}


LHCb::MuonTileID  MuonDAQHelper::findTS(LHCb::MuonTileID digit)
{
  //msgStream()<<MSG::INFO<<" enter "<<endmsg;
  //msgStream()<<MSG::INFO<<digit<<endmsg;
  unsigned int station=digit.station();

  unsigned int region=digit.region();

  // get TS layout in region from L1
  unsigned int index=station*16+region*4;
  //msgStream()<<MSG::INFO<<m_L1Name.size()<<endmsg;

  std::string L1Path=m_L1Name[index];

  SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);
  unsigned int TSLayoutX=(unsigned int)l1->getTSLayoutX(region);
  unsigned int TSLayoutY=(unsigned int)l1->getTSLayoutY(region);
  MuonLayout TSLayout(TSLayoutX,TSLayoutY);
  //msgStream()<<MSG::INFO<<"TS Layout "<<TSLayoutX<<" "<<TSLayoutY<<endmsg;
  LHCb::MuonTileID TSTile=TSLayout.contains(digit);
  return TSTile;
}


unsigned int MuonDAQHelper::findDigitInTS(std::string TSPath,
                                          LHCb::MuonTileID TSTile,
                                          LHCb::MuonTileID digit,
                                          bool hole){
  SmartDataPtr<MuonTSMap>  TS(m_detSvc,TSPath);
  unsigned int TSLayoutX=TSTile.layout().xGrid();
  unsigned int TSLayoutY=TSTile.layout().yGrid();

  unsigned int layoutX=digit.layout().xGrid();
  unsigned int layoutY=digit.layout().yGrid();

  // calculate the relative position of the digit in the TS
  unsigned int xScaleFactor=layoutX/TSLayoutX;
  unsigned int yScaleFactor=layoutY/TSLayoutY;

  unsigned int gridx=digit.nX()- xScaleFactor*TSTile.nX();
  unsigned int gridy=digit.nY()- yScaleFactor*TSTile.nY();

  int layout=-1;

  // calculate the relative position of the digit in the TS


  for(int i=0;i<TS->numberOfLayout();i++){
    if((unsigned int)TS->gridXLayout(i)==xScaleFactor&&
       (unsigned int)TS->gridYLayout(i)==yScaleFactor){
      layout=i;
    }
  }
  //  msgStream()<<MSG::INFO<<"layout "<<layout<<" "<<gridx<<" "<<gridy<<endmsg;
  //msgStream()<<MSG::INFO<<TSTile<<endmsg;

  //for(int i=0;i<TS->numberOfOutputSignal();i++){
  unsigned int ch_no_hole=0;

  for(int i=0;i<TS->synchChSize();i++){
    //  msgStream()<<MSG::INFO<<TS->synchChSize()<<" "<<ch_no_hole<<" "<<
    //TS->synchChUsed(i)<<endmsg;

    if(TS->synchChUsed(i))
    {
      //     msgStream()<<MSG::INFO<<" TS loop "<<
      //TS->layoutOutputChannel(ch_no_hole)<<" "<<
      //TS->gridXOutputChannel(ch_no_hole)<<" "<<
      // TS->gridYOutputChannel(ch_no_hole)<<endmsg;

      if(TS->layoutOutputChannel(ch_no_hole)==layout){
        if((unsigned int)TS->gridXOutputChannel(ch_no_hole)==gridx){
          if((unsigned int)TS->gridYOutputChannel(ch_no_hole)==gridy){
            //                 info()<<"digit in ts "<<i<<endmsg;
            if(hole){
              return i;
            }else{
              return ch_no_hole;

            }

          }

        }

      }

      ch_no_hole++ ;


    }

  }

  msgStream()<<MSG::ERROR<<" error in findging digit in TS "<<endmsg;
  msgStream()<<MSG::ERROR<<" position in  TS should be "<<gridx<<" "<<
    gridy<<" "<<layout<<endmsg;
  msgStream()<<MSG::ERROR<<" layout should be  "<<xScaleFactor<<" "<<
    yScaleFactor<<endmsg;


  return 100000;

}





unsigned int MuonDAQHelper::findODENumber(std::string odePath)
{
  SmartDataPtr<MuonODEBoard>  ode(m_detSvc,odePath);
  unsigned int ODENumber=ode->getODESerialNumber();
  return ODENumber;


}


unsigned int MuonDAQHelper::findODEPosition(std::string L1Path,
                                            long odeNumber,bool hole)
{
  SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);
  //  std::cout<<" find ode pos "<<l1->getODEPosition(odeNumber,hole)<<
  //std::endl;

  return l1->getODEPosition(odeNumber,hole);
}

std::string MuonDAQHelper::findODEPath(LHCb::MuonTileID TS)
{

  unsigned int station=TS.station();
  unsigned int region=TS.region();
  unsigned int quadrant=TS.quarter();
  int odeStart=m_ODENameStart[station][region][quadrant];
  int odeEnd=m_ODENameEnd[station][region][quadrant];
  //verbose()<<station<<" "<<region<<" "<<quadrant<<endmsg;
  //verbose()<<odeStart<<" "<<odeEnd<<" "<<m_ODEName[odeStart]<<endmsg;

  for(int ode=odeStart;ode<odeEnd;ode++){
    std::string odePath=m_ODEName[ode];
    //verbose()<<odePath<<endmsg;
    //std::cout<<odePath<<std::endl;
    SmartDataPtr<MuonODEBoard>  odeBoard(m_detSvc,odePath);
    //verbose()<<odeBoard->isTSContained(TS)<<endmsg;
    if(odeBoard->isTSContained(TS))return odePath;
  }
  return NULL;

}


std::string MuonDAQHelper::findL1(LHCb::MuonTileID TS)
{
  unsigned int station=TS.station();
  unsigned int region=TS.region();
  unsigned int quadrant=TS.quarter();
  unsigned int index=station*16+region*4+quadrant;
  std::string L1Path=m_L1Name[index];
  return L1Path;
}

unsigned int MuonDAQHelper::findTSPosition(std::string ODEPath,
                                           LHCb::MuonTileID TSTile)
{
  unsigned int quadrant=TSTile.quarter();
  unsigned int gridx=TSTile.nX();
  unsigned int gridy=TSTile.nY();

  SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);
  //info()<<gridx<<" "<<gridy<<endmsg;

  for(int TS=0;TS<ode->getTSNumber();TS++){
    //  info()<<TS<<" "<<ode->getTSGridX(TS)<<endmsg;
    if((unsigned int)ode->getTSQuadrant(TS)==quadrant){
      if((unsigned int)ode->getTSGridX(TS)==gridx){
        if((unsigned int)ode->getTSGridY(TS)==gridy){

          return TS;

        }
      }
    }
  }

  msgStream()<<MSG::ERROR<<"error in finding TS postion "<<endmsg;

  return 100000;
}


std::string MuonDAQHelper::findTSPath(std::string ODEPath,
                                      long TSPosition,int station){


  SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);
  std::string base=getBasePath(m_basegeometry->getStationName(station));
  std::string  TSPath=ode->getTSName(TSPosition);
  //info()<<TSPosition<<ode->name()<<endmsg;

  //std::string base="/dd/Cabling/Muon/M4/";

  //info()<<base<<" "<<TSPosition<<TSPath<<endmsg;
  std::string out=base+TSPath;
  return out;
}

long MuonDAQHelper::channelsInL1BeforeODE(std::string L1Path,
                                          long ODENumber,bool hole)
{

  SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);
  //  long station=l1->getStation();

  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<L1Path<<" "<<ODENumber<<" "<<hole<<endmsg;

  return  192*(findODEPosition(L1Path, ODENumber,hole));


}

unsigned int MuonDAQHelper::DAQaddressInL1(LHCb::MuonTileID digitTile,
                                           long& L1Number,
                                           long& ODENumber,
                                           unsigned int & ODEAdd,
                                           bool hole){
  ODEAdd=DAQaddressInODE(digitTile,L1Number,ODENumber,hole);
  std::string L1Path=findL1(digitTile);
  //  unsigned int ODEPos=findODEPosition(L1Path,ODENumber,hole);
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" check "<<hole<<" "<<L1Path<<" "<<ODENumber<<endmsg;

  unsigned int chToAdd=channelsInL1BeforeODE(L1Path,ODENumber,false);
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" ch add "<<chToAdd+ODEAdd<<endmsg;
  return chToAdd+ODEAdd;


}

unsigned int MuonDAQHelper::DAQaddressInODE(LHCb::MuonTileID digitTile,
                                            long& L1Number,
                                            long& ODENumber,bool hole){

  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<"************** start coding a digit "<<endmsg;
  bool print=false;

  long station=digitTile.station();
  LHCb::MuonTileID TS=findTS(digitTile);
  if(print){
    msgStream()<<MSG::DEBUG<<"digit is contained in TS "<<endmsg;
  }

  std::string L1Path=findL1(TS);
  if(print)msgStream()<<MSG::DEBUG<<"the TS is contained in L1 "<<L1Path<<endmsg;
  SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);
  if(print)msgStream()<<MSG::DEBUG<<"l1 "<<endmsg;

  if(print)msgStream()<<MSG::DEBUG<<" station "<<l1->getStation()<<endmsg;
  //long
  L1Number=l1->L1Number();
  std::string ODEPath= findODEPath(TS);

  if(print)
    msgStream()<<MSG::DEBUG<<"the TS is contained in ODE "<<ODEPath<<endmsg;
  SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);
  ODENumber=findODENumber(ODEPath);
  unsigned int ODESerialNumber=findODEPosition(L1Path, ODENumber,hole);
  if(print)msgStream()<<MSG::DEBUG<<"ODE= "<<ODENumber<<" "<<ODESerialNumber<<endmsg;
  unsigned int TSSerialNumber=findTSPosition(ODEPath,TS);
  if(print)
    msgStream()<<MSG::DEBUG<<"the TS position is ODE is = "<<TSSerialNumber<<endmsg;
  std::string TSPath= findTSPath(ODEPath,TSSerialNumber,station);
  //  debug()<<"the TS map is located in  "<<TSPath<<endmsg;
  // debug()<<" station "<<l1->getStation()<<endmsg;

  unsigned int DigitPosition=findDigitInTS(TSPath,TS,digitTile,hole);
  SmartDataPtr<MuonTSMap>  TSMap(m_detSvc,TSPath);
  unsigned int digitInODE=10000;

  if(hole){
    digitInODE=TSSerialNumber*TSMap->synchChSize();

  }else{
    digitInODE=TSSerialNumber*TSMap->numberOfOutputSignal();
  }
  unsigned int DigitOutputPosition=digitInODE+DigitPosition;
  if(print)
    msgStream()<<MSG::DEBUG<<" the output position in L1 of the digit is "<<
      DigitOutputPosition<<endmsg;
  return DigitOutputPosition;
}





std::vector<unsigned int> MuonDAQHelper::padsinTS(
                                                  std::vector<unsigned
                                                  int>& TSDigit,
                                                  std::string TSPath){

  //input the sequence of 0/1 for fired and not fired channels..
  std::vector<unsigned int> list_of_pads;
  //maxPads=0;
  SmartDataPtr<MuonTSMap>  TS(m_detSvc,TSPath);
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) ) {
    msgStream()<<MSG::DEBUG<<"----- start a trigger sector cross ---------"<<endmsg;
    msgStream()<<MSG::DEBUG<<TSPath<<" "<<TS->numberOfOutputSignal()<<endmsg;
    msgStream()<<MSG::DEBUG<<" seq ";
    std::vector<unsigned int>::iterator idebug;
    for(idebug=TSDigit.begin();idebug<TSDigit.end();idebug++){
      msgStream()<<MSG::DEBUG<<" "<<*idebug;
    }
    msgStream()<<MSG::DEBUG<<endmsg;
  }

  if(TS->numberOfLayout()==2){
    //how many subsector????
    int NX=TS->gridXLayout(1);
    int NY=TS->gridYLayout(0);
    int Nsub=NX*NY;
    //debug()<<"number NX NY "<<NX<<" "<<NY<<endmsg;
    //maxPads=TS->gridXLayout(0)*TS->gridYLayout(1);
    // work on sub sector
    if(Nsub>8){
      msgStream()<<MSG::ERROR<<
        "error the dimensioning of the TS subsector is wrong "<<endmsg;
      return list_of_pads;

    }
    //msgStream()<<MSG::INFO<<" # pads "<<
    std::vector<unsigned int> horiz[8];
    std::vector<unsigned int> hvert[8];
    // clear the memory;

    // start fill the sub sector matrices
    std::vector<unsigned int>::iterator it;
    int index=0;

    for(it=TSDigit.begin();it<TSDigit.end();it++,index++){
      //also zero must be set
      // which subsector?
      int mx=TS->gridXOutputChannel(index)/
        (TS->gridXLayout(TS->layoutOutputChannel(index))/NX);
      int my=TS->gridYOutputChannel(index)/
        (TS->gridYLayout(TS->layoutOutputChannel(index))/NY);
      //debug()<<" digit "<<index<<" "<<mx<<" "<<my<<" "<<*it<<endmsg;
      int Msub=mx+my*NX;
      // horizntal o vertical?
      bool horizontal=false;
      if(TS->layoutOutputChannel(index)==0)horizontal=true;
      if(horizontal)horiz[Msub].push_back(*it);
      else hvert[Msub].push_back(*it);
      // debug()<<" horizontal ? "<<     horizontal<<endmsg;
    }
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
      msgStream()<<MSG::DEBUG<<" after h/v "<<endmsg;


    // now the address of fired pads..
    for(int i=0;i<Nsub;i++){
      // cross only local to each sub matrix
      std::vector<unsigned int>::iterator itx;
      std::vector<unsigned int>::iterator ity;
      //debug
      if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
        msgStream()<<MSG::DEBUG<<" sub matrix "<<i<<endmsg;
      //debug()<<" horizontal sequence ";

      // for(itx=horiz[i].begin();
      //    itx<horiz[i].end();itx++)
      // {
      //  debug()<<*itx<<" ";
      //}
      //debug()<<endmsg;
      //debug()<<" vertical sequence ";

      //for(ity=hvert[i].begin();ity<hvert[i].end();ity++){
      //  debug()<<*ity<<" ";
      //}
      //debug()<<endmsg;
      //end debug
      unsigned int y_index=0;
      unsigned int subY=i/NX;
      unsigned int subX=i-subY*NX;

      unsigned int offsetY=subY*(TS->gridYLayout(1)/NY);
      unsigned int offsetX=subX*(TS->gridXLayout(0)/NX);
      for(ity=hvert[i].begin();
          ity<hvert[i].end();ity++,y_index++){
        if(*ity!=0){
          unsigned int x_index=0;
          for(itx=horiz[i].begin();
              itx<horiz[i].end();itx++,x_index++){
            if(*itx!=0){
              unsigned int address=offsetX+x_index+
                (offsetY+y_index)*(TS->gridXLayout(0));
              //debug()<<" result of the address "<<address<<endmsg;

              list_of_pads.push_back(address);
            }
          }
        }
      }
    }
  }else{
    //easy only zero suppression
    std::vector<unsigned int>::iterator it;
    unsigned int index=0;
    for(it=TSDigit.begin();it<TSDigit.end();it++,index++){
      unsigned int address=index;
      if(*it!=0){list_of_pads.push_back(address);
        //      debug()<<" result of the address "<<address<<endmsg;
      }
    }
  }
  return list_of_pads;
}


unsigned int MuonDAQHelper::getPPNumber(unsigned int Tell1_num,
                                        unsigned int ODE_num){

  unsigned int pp_num=100;
  for (int j=0;j<4;j++)
  {
    for (int i=0;i<6;i++)
    {
      if(getODENumberInLink(Tell1_num,j*6+i)==ODE_num)return j;
    }
  }
  return pp_num;
}

StatusCode MuonDAQHelper::checkMapConsistency()
{


  //check if all log ch tile are present
  for (int station=0;station<m_nStations;station++){
    for (int region=0;region<4;region++){
      int num_lay=layout[station*4+region];
      for(int ilay=0;ilay<num_lay;ilay++){
        MuonLayout lay(m_layoutX[ilay][station*4+region],
                       m_layoutY[ilay][station*4+region]);
        if( UNLIKELY( msgStream().level() <= MSG::VERBOSE ) )
          msgStream()<<MSG::VERBOSE<<num_lay<<" "<<lay<<" "<<station<<" "<<region<<endmsg;

        for(int quadrant=0;quadrant<4;quadrant++){
          for(int x=0;x<m_layoutX[ilay][station*4+region];x++){
            for(int y=0;y<m_layoutY[ilay][station*4+region];y++){
              MuonTileID muontile1(station,lay,region,
                                   quadrant,x+m_layoutX[ilay][station*4+region],
                                   y);
              MuonTileID muontile2(station,lay,region,
                                   quadrant,x,y+
                                   m_layoutY[ilay][station*4+region]);
              MuonTileID muontile3(station,lay,region,
                                   quadrant,x+m_layoutX[ilay][station*4+region],
                                   y+m_layoutY[ilay][station*4+region]);
              if( UNLIKELY( msgStream().level() <= MSG::VERBOSE ) )
                msgStream()<<MSG::VERBOSE<<
                  muontile1<<" "<<muontile2<<" "<<muontile3<<endmsg;

              long L1Number=0;
              long ODENumber=0;
              unsigned int ODEAdd=0;

              long DigitOutputPosition=DAQaddressInL1(muontile1,L1Number,
                                                      ODENumber,ODEAdd);
              long DigitOutputPositionInODE=DAQaddressInODE(muontile1,
                                                            L1Number,
                                                            ODENumber,false);
              if( !((m_mapTileInTell1[L1Number])[DigitOutputPosition]==
                    muontile1)||
                  !((m_mapTileInODEDC06[ODENumber-1])
                    [DigitOutputPositionInODE]==muontile1)){
                msgStream()<<MSG::ERROR<<" failed test "<<muontile1<<endmsg;

                return StatusCode::FAILURE;
              }

              DigitOutputPosition=DAQaddressInL1(muontile2,L1Number,
                                                 ODENumber,ODEAdd);
              DigitOutputPositionInODE=DAQaddressInODE(muontile2,L1Number,
                                                       ODENumber,false);
              if( !((m_mapTileInTell1[L1Number])[DigitOutputPosition]==
                    muontile2)||
                  !((m_mapTileInODEDC06[ODENumber-1])
                    [DigitOutputPositionInODE]==muontile2)){
                msgStream()<<MSG::ERROR<<" failed test "<<muontile2<<endmsg;

                return StatusCode::FAILURE;
              }

              DigitOutputPosition=DAQaddressInL1(muontile3,L1Number,
                                                 ODENumber,ODEAdd);
              DigitOutputPositionInODE=DAQaddressInODE(muontile3,L1Number,
                                                       ODENumber,false);
              if( !((m_mapTileInTell1[L1Number])[DigitOutputPosition]==
                    muontile3)||
                  !((m_mapTileInODEDC06[ODENumber-1])
                    [DigitOutputPositionInODE]==muontile3)){
                msgStream()<<MSG::ERROR<<" failed test "<<muontile2<<endmsg;

                return StatusCode::FAILURE;
              }


            }
          }
        }
      }
    }
  }



  return StatusCode::SUCCESS;
}

void MuonDAQHelper::resetLUT(){
  m_TotTell1=0;
  m_M1Tell1=0;
  m_L1Name.clear();
  m_TELL1Name.clear();
  m_ODEName.clear();

  for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
    m_ODENumberInTell1[i]=0;
    m_ODEInTell1[i].clear();
    m_mapPadDC06[i].clear();
    m_mapPadV1[i].clear();
    m_mapTileInTell1[i].clear();
    m_linkInTell1[i].clear();
  }

  for (unsigned int i=0;i<MuonDAQHelper_maxODENumber;i++){
    m_ODENameInECS[i].clear();
    m_mapTileInODE[i].clear();
    m_mapTileInODEDC06[i].clear();
  }
  for(int i=0;i<5;i++){
    m_tellPerStation[i].clear();
  }


}

StatusCode MuonDAQHelper::updateLUT(){
  resetLUT();

  StatusCode sc= initDAQMaps();
  if(sc.isFailure())return sc;

  return StatusCode::SUCCESS;
}

unsigned int MuonDAQHelper::findODEChNumber(LHCb::MuonTileID digit)
{
  long  L1Number=-1;
  long ODENumber=-1;

  unsigned int ODE_Ch_number=DAQaddressInODE(digit,L1Number,ODENumber);


  if(ODE_Ch_number>200)return 999;
  return ODE_Ch_number;


}

unsigned int MuonDAQHelper::findODENumber(LHCb::MuonTileID digit)
{
  //pad or logical channel?
  unsigned int ODE_value=999;
  //  bool strip=false;
  bool pad=true;

  if(!digit.isValid())return ODE_value;

  int station=digit.station();
  int region=digit.region();
  //int quadtrant=digit.quadrant();
  int num_lay=layout[station*4+region];
  int lay_num=-1;

  for(int ilay=0;ilay<num_lay;ilay++){
    MuonLayout lay(m_layoutX[ilay][station*4+region],
                   m_layoutY[ilay][station*4+region]);
    if(digit.layout()==lay)
    {
      lay_num=ilay;
      //      strip=true;
      break;
    }
  }
  if(lay_num<0)pad=true;
  LHCb::MuonTileID  digit_to_search;

  if(pad){
    //build one strip that intercept the pad
    MuonLayout lay(m_layoutX[0][station*4+region],
                   m_layoutY[0][station*4+region]);
    digit_to_search=digit.containerID (lay);

  }else digit_to_search=digit;

  LHCb::MuonTileID TSTile=findTS(digit_to_search);
  std::string ODEPath=findODEPath(TSTile);
  //std::string L1Path=findL1(TSTile);
  ODE_value=findODENumber(ODEPath);
  return ODE_value;




}


StatusCode MuonDAQHelper::findStrips(LHCb::MuonTileID pad,
                                     LHCb::MuonTileID  strips[2])
{
  MuonTileID null;

  strips[0]=null;
  strips[1]=null;
  StatusCode sc=StatusCode::FAILURE;

  if(!pad.isValid())return sc;

  int station=pad.station();
  int region=pad.region();
  //int quadtrant=digit.quadrant();
  int num_lay=layout[station*4+region];
  if(num_lay==1){

    strips[0]=pad;
    return StatusCode::SUCCESS;
  }

  for(int ilay=0;ilay<num_lay;ilay++){
    MuonLayout lay(m_layoutX[ilay][station*4+region],
                   m_layoutY[ilay][station*4+region]);
    if(lay==pad.layout()){
      strips[0]=pad.containerID(lay);
      strips[1]=null;
      return StatusCode::SUCCESS;
    }

    strips[ilay]=pad.containerID (lay);
  }
  return StatusCode::SUCCESS;

}

MuonL1Board* MuonDAQHelper::getL1Board(unsigned int board_num){

  MuonL1Board* empty=NULL;

  for(int station=0;station<m_nStations;station++){
    std::string cablingBasePath=getBasePath(m_basegeometry->getStationName(station));
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(m_detSvc, cablingPath);
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){
      std::string L1Path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);
      if(static_cast<unsigned int>(l1->L1Number())==board_num)return l1;
    }
  }
  return empty;
}


MuonODEBoard* MuonDAQHelper::getODEBoard(MuonL1Board* l1,
                                         unsigned int board){
  //loop on ODE
  MuonODEBoard* empty=NULL;

  int station=l1-> getStation();

  std::string cablingBasePath=getBasePath(m_basegeometry->getStationName(station));
  for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){

    std::string ODEPath=cablingBasePath
      +l1->getODEName(ODEBoard);

    SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);
    if(static_cast<unsigned int>(ode->getODESerialNumber())
       ==board)return ode;

  }

  return empty;


}

MuonTSMap* MuonDAQHelper::getTSMap(MuonL1Board* l1, MuonODEBoard* ode,
                                   unsigned int ts){
  //loop on ODE
  MuonTSMap* empty=NULL;

  int station=l1-> getStation();

  std::string cablingBasePath=getBasePath(m_basegeometry->getStationName(station));

  std::string  TSPath= cablingBasePath+
    ode->getTSName(ts);
  SmartDataPtr<MuonTSMap>  TSMap(m_detSvc,TSPath);
  if(TSMap==0)return empty;

  return TSMap;



  //return empty;


}




StatusCode  MuonDAQHelper::findHWNumber(LHCb::MuonTileID digit,
                                        long& L1Number,long& link_number,
                                        long& ODE_number,long& ode_ch)
{

  L1Number=-1;
  link_number=-1;
  ODE_number=-1;
  ode_ch=-1;
  StatusCode sc=StatusCode::FAILURE;

  ode_ch= DAQaddressInODE(digit,L1Number, ODE_number);
  if(ode_ch>200)return sc;
  if(L1Number<0)return sc;
  if(ODE_number<0)return sc;
  if(ode_ch<0)return sc;

  for(int i=0;i<24;i++){
    if(static_cast<int>(getODENumberInLink(L1Number,i))==ODE_number){
      link_number=i;
      return StatusCode::SUCCESS;
    }
  }
  return sc;
}

StatusCode  MuonDAQHelper::findL0Number(LHCb::MuonTileID digit,
                                        std::string&  ECSName,
                                        long& link_number,
                                        long& link_ch)
{

  ECSName="";
  link_number=-1;
  link_ch=-1;
  StatusCode sc=StatusCode::FAILURE;
  int station=digit.station();
  int region=digit.region();
  long L1Number=-1;
  long ODE_number=1;

  long ode_ch= DAQaddressInODE(digit,L1Number, ODE_number);
  if(ode_ch>200)return sc;
  if(L1Number<0)return sc;
  if(ODE_number<0)return sc;
  if(ode_ch<0)return sc;
  ECSName=m_ODENameInECS[ODE_number];
  link_number=ode_ch/m_TUSize[station][region];
  link_ch=ode_ch%(m_TUSize[station][region]);


  return StatusCode::SUCCESS;
}
unsigned int MuonDAQHelper::getODENumberInQuadrant(std::string ODEName)
{
  std::string::size_type cPos=ODEName.find_first_of("_");
  if(cPos!=std::string::npos){
    std::string sCurr;
    sCurr = ODEName.substr(cPos+1); // get up to the end
    int i;
    const char *p;
    p=sCurr.c_str();
    sscanf(p, "%d", &i);
    return i;

  }
  return 0;

}


std::string MuonDAQHelper::getODEECSName(int number){
return m_ODENameInECS[number];

}

LHCb::MuonTileID MuonDAQHelper::getPadTileInODE(std::string ODEName, int firstChannel, int secondChannel){

 unsigned int station;

  unsigned int region;

  unsigned int quadrant;

   std::string sCurr;
   sCurr = ODEName.substr(1,2); // get up to the end

   const char *pq;
   pq=sCurr.c_str();
   sscanf(pq, "%u", &quadrant);

   sCurr = ODEName.substr(3,4); // get up to the end

   const char *ps;

   ps=sCurr.c_str();
   sscanf(ps, "%u", &station);
   sCurr = ODEName.substr(5,6); // get up to the end

   const char *pr;

   pr=sCurr.c_str();
   sscanf(pr, "%u", &region);

   region--;
   station--;
   quadrant--;
   if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
     msgStream()<<MSG::DEBUG<<" debug "<<station<<" "<<region<<" "<<quadrant<<endmsg;

  int odeStart=m_ODENameStart[station][region][quadrant];
  int odeEnd=m_ODENameEnd[station][region][quadrant];

	  //verbose()<<station<<" "<<region<<" "<<quadrant<<endmsg;
  //verbose()<<odeStart<<" "<<odeEnd<<" "<<m_ODEName[odeStart]<<endmsg;
  int odeNumber=-1;

  for(int ode=odeStart+1;ode<odeEnd+1;ode++){
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
      msgStream()<<MSG::DEBUG<<ODEName<<" debug ode "<<m_ODENameInECS[ode]<<endmsg;
    if(m_ODENameInECS[ode]==ODEName){
      odeNumber=ode;
      break;
    }
  }
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" debug ode "<<odeNumber<<endmsg;

  MuonTileID firstTile;
  firstTile= m_mapTileInODE[odeNumber-1][firstChannel];
  MuonTileID secondTile;
  if(secondChannel>=0)secondTile= m_mapTileInODE[odeNumber-1][secondChannel];
  if(!secondTile.isValid())return firstTile;
  return firstTile.intercept(secondTile);
}


/* inutile

StatusCode MuonDAQHelper::initODEMaps()
{

  unsigned int countL1=0;
  for(int station = 0; station < m_nStations;station++){
    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
      msgStream()<<MSG::DEBUG<<"station number "<<station<<endmsg;

    std::string cablingBasePath=getBasePath(station);
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(m_detSvc, cablingPath);

    if( 0 == cabling ) {
      msgStream()<<MSG::ERROR << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }

    if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
      msgStream()<<MSG::DEBUG<<" station "<<station<<endmsg;

    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){

      if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
        msgStream()<<MSG::DEBUG<<"L1 number "<<cabling->getL1Name(0)<<endmsg;

      std::string L1Path=cablingBasePath+cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);

      unsigned totODE=0;

      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
        std::string ODEPath=cablingBasePath+l1->getODEName(ODEBoard);

        if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
          msgStream()<<MSG::DEBUG<<"ODE number "<<L1Board<<
          " "<<l1->getODEName(ODEBoard)<<endmsg;
        SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);

        if( 0 == ode ) {
          msgStream()<<MSG::ERROR << ODEPath << " not found in XmlDDDB" << endmsg;
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
          SmartDataPtr<MuonTSMap>  TSMap(m_detSvc,TSPath);
          int i=0;
          for(int isynch=0;isynch<TSMap->synchChSize();isynch++){

            if(TSMap->synchChUsed(isynch)){
              //for(int i=0;i<TSMap->numberOfOutputSignal();i++){
              //msg<<MSG::INFO<<"cabling base 2 "<<cablingBasePath<<endmsg;
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
              if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
                msgStream()<<MSG::DEBUG<<m_mapTileInODE[odenum-1].size()<<" "<<
                  muontile<<endmsg;

              m_mapTileInODEDC06[odenum-1].push_back(muontile);
              i++;
            }else{
              MuonTileID muontile;
              m_mapTileInODE[odenum-1].push_back(muontile);
            }
          }
        }
      }
      m_ODENumberInTell1[countL1]=totODE;
      if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
        msgStream()<<MSG::DEBUG<<" number "<<countL1<<" "<<totODE<<endmsg;

      countL1++;
    }
  }
  return StatusCode::SUCCESS;

}
*/

/* inutile
StatusCode  MuonDAQHelper::initODENames()
{
  //StatusCode sc=StatusCode::FAILURE;

  for(int station=0;station<m_nStations;station++){

    std::string cablingBasePath=getBasePath(station);
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(m_detSvc, cablingPath);
    if( 0 == cabling ) {
      msgStream()<<MSG::ERROR << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){
      std::string L1Path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1Path);
      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
        std::string ODEPath=cablingBasePath
          +l1->getODEName(ODEBoard);
        SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEPath);
        if( 0 == ode ) {
          msgStream()<<MSG::ERROR << ODEPath << " not found in XmlDDDB" << endmsg;
          return StatusCode::FAILURE;
        }
        long odenum=ode->getODESerialNumber();
        std::string ODEName=ode->ECSName();
        m_ODENameInECS[odenum]=ODEName;

      }
    }
  }

  return StatusCode::SUCCESS;
}
*/


  //  sc=initODEMaps(); // inutile, mergiata con la precedente
  /*
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" step 6 "<<endmsg;
  if(sc.isFailure())return sc;
  */
  /* inutile, incorporata nella precedente
  sc=initODENames();
  if( UNLIKELY( msgStream().level() <= MSG::DEBUG ) )
    msgStream()<<MSG::DEBUG<<" step odename "<<endmsg;
  if(sc.isFailure())return sc;
  */
		       //StatusCode MuonDAQHelper::initializeLUTCrossing() 
StatusCode MuonDAQHelper::initLUTCrossing() 
{ 
  int countTell1=m_M1Tell1; 
 
  for(int station=1;station<m_nStations;station++){ 
    //debug()<<"station number "<<station<<endmsg; 
    std::string cablingBasePath=getBasePath(m_basegeometry->getStationName(station)); // hardwired at the moment
    //    std::string cablingBasePath=getBasePath(station); 
    std::string cablingPath=cablingBasePath+"Cabling"; 
    SmartDataPtr<MuonStationCabling>  cabling(m_detSvc, cablingPath); 
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){ 
      //debug()<<"L1 number "<<cabling->getL1Name(0)<<endmsg; 
      std::string L1path=cablingBasePath+ 
        cabling->getL1Name(L1Board); 
      SmartDataPtr<MuonL1Board>  l1(m_detSvc,L1path); 
      //      unsigned totODE=0; 
      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){ 
        std::string ODEpath=cablingBasePath 
          +l1->getODEName(ODEBoard); 
        SmartDataPtr<MuonODEBoard>  ode(m_detSvc,ODEpath); 
        //build LUT with ode ID --> MuonTileID 
        unsigned int region=ode->region(); 
        MuonTSMap*  FirstTSMap=NULL; 
 
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
          SmartDataPtr<MuonTSMap>  TSMap(m_detSvc,TSPath); 
          //debug()<<"trigger sector "<<TSPath<<endmsg; 
          if(TS==0){ 
            FirstTSMap=static_cast<MuonTSMap*>( TSMap); 
 
          } 
 
          std::vector<LHCb::MuonTileID> digitInTS; 
          std::vector<LHCb::MuonTileID> wrongdigitInTS; 
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
            MuonTileID muontile(station,lay,region, 
                                quadrant,digitX,digitY); 
            digitInTS.push_back(muontile); 
 
            unsigned int wrongDigitX=digitOffSetX+FirstTSMap->gridXOutputChannel(i); 
            unsigned int wrongDigitY=digitOffSetY+FirstTSMap->gridYOutputChannel(i); 
 
            MuonTileID wrongmuontile(station,lay,region, 
                                     quadrant,wrongDigitX,wrongDigitY); 
            wrongdigitInTS.push_back(wrongmuontile); 
 
          } 
          std::vector<LHCb::MuonTileID> crossAddressDC06 
            =DoPadDC06(digitInTS,TSMap); 
          std::vector<LHCb::MuonTileID> crossAddressV1 
            =DoPadV1(digitInTS,wrongdigitInTS,TSMap); 
          std::vector<LHCb::MuonTileID>::iterator itPad; 
          for(itPad=crossAddressDC06.begin();itPad<crossAddressDC06.end(); 
              itPad++){ 
            m_mapPadDC06[countTell1].push_back(*itPad); 
          } 
          for(itPad=crossAddressV1.begin();itPad<crossAddressV1.end(); 
              itPad++){ 
            m_mapPadV1[countTell1].push_back(*itPad); 
          } 
        } 
      } 
      countTell1++; 
    } 
  } 
  return StatusCode::SUCCESS; 
 
} 
