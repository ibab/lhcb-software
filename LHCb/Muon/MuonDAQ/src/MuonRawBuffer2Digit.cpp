// $Id: MuonRawBuffer2Digit.cpp,v 1.3 2004-02-09 14:14:29 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "MuonRawBuffer2Digit.h"



//-----------------------------------------------------------------------------
// Implementation file for class : MuonRawBuffer2Digit
//
// 2004-01-26 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonRawBuffer2Digit>          s_factory ;
const        IAlgFactory& MuonRawBuffer2DigitFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonRawBuffer2Digit::MuonRawBuffer2Digit( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
  basePath[0]= "/dd/ReadOut/MuonCabling/M1/";
  basePath[1]= "/dd/ReadOut/MuonCabling/M2/";
  basePath[2]= "/dd/ReadOut/MuonCabling/M3/";
  basePath[3]= "/dd/ReadOut/MuonCabling/M4/";
  basePath[4]= "/dd/ReadOut/MuonCabling/M5/";
}
//=============================================================================
// Destructor
//=============================================================================
MuonRawBuffer2Digit::~MuonRawBuffer2Digit() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonRawBuffer2Digit::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;
  // initialize Look-up for the decoding //
  int addL1=0;
  
   for(int station=0;station<5;station++){
    msg<<MSG::DEBUG<<"station number "<<station<<endreq;
    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);
    if( 0 == cabling ) {
      msg << MSG::ERROR << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }
    
    m_TotL1Board=m_TotL1Board+cabling->getNumberOfL1Board();
    int counterInL1=0;
    
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){
      counterInL1=0;
      msg<<MSG::DEBUG<<"L1 number "<<cabling->getL1Name(0)<<endreq;
      std::string L1path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
        
        std::string ODEpath=cablingBasePath
          +l1->getODEName(ODEBoard);
        //msg<<MSG::DEBUG<<"ODE number "<<l1->getODEName(ODEBoard)<<endreq;
        SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
        unsigned int region=ode->region();      
        //msg<<MSG::INFO<<"cabling base "<<cablingBasePath<<endreq;
        for(int TS=0;TS<ode->getTSNumber();TS++){
          //msg<<MSG::INFO<<TS<<" "<<ode->getTSName(TS)<<endreq;
          std::string  TSPath= cablingBasePath+
            ode->getTSName(TS);
          //msg<<MSG::INFO<<"Ts path "<<TSPath<<cablingBasePath<<endreq;
          
          unsigned int quadrant= ode->getTSQuadrant(TS);
          unsigned int TSLayoutX=ode->getTSLayoutX();
          unsigned int TSLayoutY=ode->getTSLayoutY();
          unsigned int TSGridX=ode->getTSGridX(TS);
          unsigned int TSGridY=ode->getTSGridY(TS);
          unsigned int digitOffSetX=0;
          unsigned int digitOffSetY=0;          
          SmartDataPtr<MuonTSMap>  TSMap(detSvc(),TSPath);
          //msg<<MSG::INFO<<" map "<<TSMap<<endreq;
          
          //msg<<MSG::INFO<<" channel "<<TSMap->numberOfOutputSignal()<<endreq;
  //msg<<MSG::INFO<<"cabling base 1 "<<cablingBasePath<<endreq;
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
            MuonTileID muontile(station,0,0,lay,region,quadrant,digitX,digitY);
            //msg<<MSG::INFO<<"cabling base 2 "<<cablingBasePath<<endreq;
            m_L1Map[addL1].push_back(muontile);
            //msg<<MSG::INFO<<" map "<<counterInL1<<" "<<endreq;
            //   msg<<MSG::INFO<<"cabling base 3 "<<cablingBasePath<<endreq;
            // TilePrintOut(muontile);
            
            //define the tile of the digit
            counterInL1++;            
          }
        }      
        msg<<MSG::DEBUG<<" channels tot  "<<counterInL1<<" "<<ODEBoard<<endreq;
      }
      
      
      addL1++;
      msg<<MSG::DEBUG<<" channels "<<counterInL1<<endreq;
      
    }    
   }
   
   

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonRawBuffer2Digit::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  // Retrieve the Raw event if it already exists, or create it from RawBuffer
  RawEvent* rawEvent;
  SmartDataPtr<RawEvent> rawEvtPtr( eventSvc(), RawEventLocation::Default );
  if( 0 == rawEvtPtr ) {
    SmartDataPtr<RawBuffer> rawBuffer( eventSvc(), RawBufferLocation::Default );
    if ( 0 == rawBuffer ) {
      msg << MSG::ERROR << "Unable to retrieve RawBuffer" << endmsg;
      return StatusCode::FAILURE;
    }
    rawEvent = new RawEvent( rawBuffer );
    if ( 0 == rawEvent ) {
      msg << MSG::ERROR << "Unable to allocate memory to RawEvent" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else {
    rawEvent = rawEvtPtr;
  }

  MuonDigits* MuonDigitsCon= new MuonDigits;
  
  std::vector<RawBank>  MuonL1Buffer =rawEvent->banks(RawBuffer::Muon);
  std::vector<RawBank>::const_iterator ibank;
  MuonHLTData hltWord;
  MuonHLTDigitFormat digit;
  MuonHLTDigitFormat digit1;

  
  for(ibank=MuonL1Buffer.begin();ibank<MuonL1Buffer.end();ibank++){
    raw_int header=ibank->bankSourceID();
    raw_int L1Board;
    
    if(header>99){
      L1Board=header-100;      
    }else{
      L1Board=header;      
    }
    
    //if(header>4){    
    // raw_int L1Board=ibank->bankSourceID();
    //    msg<<MSG::INFO<<" L1 bankSource"<<L1Board<<endreq;
    raw_int *data=(*ibank).data();
    long dataSize=(*ibank).dataSize();
    long dataSizeLoop=dataSize;
    unsigned int add,time;
    
   
    if(header>99)--dataSizeLoop;
    for(int i=0;i<dataSizeLoop;i++){      
      raw_int word=data[i];     
      hltWord.setData(word);      
      digit.setData( hltWord.getFirstHalf());
      add=digit.getAddress();
      time=digit.getTime();      
      MuonTileID tile((m_L1Map[L1Board])[add]);
      //msg<<MSG::INFO<<add<<endreq;
      
      //TilePrintOut(tile);
      MuonDigit* muonDigit= new MuonDigit(tile);
      muonDigit->setTimeStamp(time);
      MuonDigitsCon->insert(muonDigit);
      
      hltWord.getSecondHalf();
      digit1.setData( hltWord.getSecondHalf());
      add=digit1.getAddress();
      time=digit.getTime();
      //msg<<MSG::INFO<<add<<endreq;
      MuonTileID tile2((m_L1Map[L1Board])[add]);
      //TilePrintOut(tile2);    
      MuonDigit* muonDigit2= new MuonDigit(tile2);
      muonDigit2->setTimeStamp(time);
      MuonDigitsCon->insert(muonDigit2);
           
    }
    if(header>99){
      raw_int word=data[dataSize-1];
      hltWord.setData(word);
      //      hltWord.getFirstHalf();
      //MuonHLTDigitFormat 
      digit.setData( hltWord.getFirstHalf());
      add=digit.getAddress();
      time=digit.getTime();   
      //msg<<MSG::INFO<<add<<endreq;
      MuonTileID tile((m_L1Map[L1Board])[add]);
      //TilePrintOut(tile);
      MuonDigit* muonDigit= new MuonDigit(tile);
      muonDigit->setTimeStamp(time);
      MuonDigitsCon->insert(muonDigit);
    }    
    //}
  }
  
  eventSvc()->registerObject( MuonDigitLocation::MuonDigit, MuonDigitsCon );
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonRawBuffer2Digit::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================


std::string MuonRawBuffer2Digit::getBasePath(int station)
{
  return basePath[station];
  
};

void MuonRawBuffer2Digit::TilePrintOut(MuonTileID digitTile)
{
  MsgStream  msg( msgSvc(), name() );
  msg<<MSG::INFO<< "["  <<  digitTile.layout() << ","
       <<  digitTile.station() << ","
       <<  digitTile.region() << ","
       <<  digitTile.quarter() << ","
       <<  digitTile.nX() << ","
     <<  digitTile.nY() << "]" <<endreq;

};
