// $Id: MuonDigitToRawBuffer.cpp,v 1.1.1.1 2004-02-05 16:26:03 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "SortDigitInL1.h"
#include "MuonDigitToRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonDigitToRawBuffer
//
// 2004-01-16 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonDigitToRawBuffer>          s_factory ;
const        IAlgFactory& MuonDigitToRawBufferFactory = s_factory ; 




//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDigitToRawBuffer::MuonDigitToRawBuffer( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
  basePath[0] = "/dd/ReadOut/MuonCabling/M1/";
  basePath[1] = "/dd/ReadOut/MuonCabling/M2/";
  basePath[2] = "/dd/ReadOut/MuonCabling/M3/";
  basePath[3] = "/dd/ReadOut/MuonCabling/M4/";
  basePath[4] = "/dd/ReadOut/MuonCabling/M5/";

}
//=============================================================================
// Destructor
//=============================================================================
MuonDigitToRawBuffer::~MuonDigitToRawBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonDigitToRawBuffer::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  //  std::string cablingBase="/dd/Cabling/Muon/";
 
  unsigned int ODEStart=0;
  unsigned int ODEEnd=1;  
  m_L1Name.resize(80);  
  m_TotL1Board=0;
  
  for(int station=0;station<5;station++){
    msg<<MSG::DEBUG<<"station number "<<station<<endreq;
    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(),
                                              cablingPath);
    if( 0 == cabling ) {
      msg << MSG::ERROR << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }
    
    m_TotL1Board=m_TotL1Board+cabling->getNumberOfL1Board();
    
    msg<<MSG::DEBUG<<"station path "<<cablingPath<<endreq;
    
    msg<<MSG::DEBUG<<"station number "<<station<<" "<<
      "L1number "<<  cabling->getNumberOfL1Board()<<endreq;
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){
    
      msg<<MSG::DEBUG<<"L1 number "<<cabling->getL1Name(L1Board)<<endreq;
      std::string L1path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
      for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
        std::string ODEpath=cablingBasePath
          +l1->getODEName(ODEBoard);
        msg<<MSG::DEBUG<<"ODE number "<<l1->getODEName(ODEBoard)<<endreq;
        SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
        long region=ode->region();  
        msg<<MSG::DEBUG<<"ODE region "<<region<<endreq;
        
        for(int quadrant=0;quadrant<4;quadrant++){
          unsigned int index=station*16+region*4+quadrant; 
          msg<<MSG::DEBUG<<" ode in quarter " <<quadrant 
            <<" "<<ode->isQuadrantContained(quadrant)<<endreq;
          
          if(ode->isQuadrantContained(quadrant)){
            
            msg<<MSG::DEBUG<<"quadrant number "<<quadrant<<endreq;
          m_L1Name[index]=L1path; 
          msg<<MSG::DEBUG<<"index "<<index<<" "<<L1path<<endreq;
          }
          
        }
        
      } 
    }    
  }
  
  for(int station=0;station<5;station++){
    std::string cablingBasePath=getBasePath(station);  
    for(int region=0;region<4;region++){
      for(int quadrant=0;quadrant<4;quadrant++){
        int numODE=0;        
        unsigned int index=station*16+region*4+quadrant; 
        std::string L1path=m_L1Name[index];
        SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
        msg<<MSG::DEBUG<<"number of L1 "<<l1->numberOfODE()<<endreq;  
         msg<<MSG::DEBUG<<station<<" "<<region<<" "<<quadrant<<" "<<
           L1path<<endreq;
         
        for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
          std::string ODEpath=cablingBasePath+
            l1->getODEName(ODEBoard);
          msg<<MSG::DEBUG<<" ode path "<<ODEpath<<endreq;          
          SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);          
          if(ode->region()==region){
            msg<<MSG::DEBUG<<" ode quadrant "<<
              ode->isQuadrantContained(quadrant)<<endreq;            
            if(ode->isQuadrantContained(quadrant)){
              msg<<MSG::DEBUG<<station<<" "<<region<<" "<<quadrant<<" "<<
                ODEpath<<endreq;
              
              numODE++;
              m_ODEName.push_back(ODEpath);               
            }
            
            msg<<MSG::DEBUG<<numODE<<endreq;
            
          }           
        }
        
        m_ODENameStart[station][region][quadrant]=ODEStart;      
        ODEEnd=ODEStart+numODE;
        m_ODENameEnd[station][region][quadrant]=ODEEnd;
        ODEStart=ODEEnd;
      }
      
      
    }    
  }
  msg<<MSG::DEBUG<<"board number "<<m_TotL1Board<<endreq;
  
  
  return StatusCode::SUCCESS;
};
//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDigitToRawBuffer::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  //sia digitTile l'imput
  SmartDataPtr<MuonDigits> digit(eventSvc(),
                                 MuonDigitLocation::MuonDigit);
  MuonDigits::iterator idigit;
  for(idigit=digit->begin();idigit<digit->end();idigit++){    
    MuonTileID digitTile=(*idigit)->key();
    
    unsigned int time=(*idigit)->TimeStamp();    
    time=0;
    
    
    long L1Number=0;
    long DigitOutputPosition=DAQaddress(digitTile,L1Number); 
    msg<<MSG::VERBOSE<<"L1Number "<<L1Number<<endreq;
    msg<<MSG::VERBOSE<<" digitOutputPosition "<<DigitOutputPosition<<endreq;
    
    unsigned int digitInDAQ=0;
    MuonHLTDigitFormat temp;
    temp.setAddress(DigitOutputPosition);
    temp.setTime(time);
    digitInDAQ=temp.getWord();
    msg<<MSG::DEBUG<<" digit word "<<L1Number-1<<" "<<
      temp.getAddress()<<endreq;
    
    m_digitsInL1[L1Number-1].push_back(digitInDAQ);      
    //}    
  }  
  for(int i=0;i<m_TotL1Board;i++){
    // std::stable_sort(m_digitsInL1[i].begin(),
    //                 m_digitsInL1[i].end(),SortDigitInL1());    
  }
  // Retrieve the RawBuffer
  SmartDataPtr<RawBuffer> rawBuffer( eventSvc(), RawBufferLocation::Default );
  if ( 0 == rawBuffer ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::ERROR
        << "Unable to retrieve Raw buffer from " <<RawBufferLocation::Default
        << endreq;
    return StatusCode::FAILURE;
   }
  std::vector<int>::iterator itDigit;
  bool half=false;
  
  for(unsigned int i=0;i<(unsigned int) m_TotL1Board;i++){

    std::vector<raw_int> L1Bank;

    half=false;
    
 MuonHLTData data;
    for(itDigit=m_digitsInL1[i].begin();itDigit<m_digitsInL1[i].end();
        itDigit++){
      
      if(!half){        
        data.setFirstHalf(*itDigit); 
        data.setSecondHalf(0);
        msg<<MSG::VERBOSE<<"first part "<<data.getFirstHalf()<<" "<<i<<endreq;
        half=true;
      }else{
        data.setSecondHalf(*itDigit);
        msg<<MSG::VERBOSE<<"second part "<<data.getSecondHalf()<<" "<<i<<endreq;
        half=false;        
      }
      //msg<<MSG::DEBUG<<"half "<<half<<endreq;
      
      if(!half){L1Bank.push_back((raw_int)data.getWord());
      }else if(itDigit==m_digitsInL1[i].end()-1){
        L1Bank.push_back((raw_int)data.getWord());
      }
      
      
      // msg<<"all word "<<data.getWord()<<endreq;
            
    }
    //long buffSize =L1Bank.size();
    //raw_int * prawBuffer = new raw_int[buffSize];
    //for (long index=0;index<buffSize;index++)
    //{prawBuffer[index]=L1Bank[index];}
    raw_int header=i;
    if(m_digitsInL1[i].size()%2)header=header+100;
    rawBuffer->addBank(header,RawBuffer::Muon,L1Bank);    
    //    rawBuffer->addBank(header,RawBuffer::Muon,prawBuffer,buffSize);
  }
    
  for(int i=0;i<m_TotL1Board;i++){
     m_digitsInL1[i].resize(0);
  }
  

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonDigitToRawBuffer::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================


MuonTileID  MuonDigitToRawBuffer::findTS(MuonTileID digit)
{
  unsigned int station=digit.station();
  
  unsigned int region=digit.region();
  
 // get TS layout in region from L1
  unsigned int index=station*16+region*4;  
  std::string L1path=m_L1Name[index];
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
  unsigned int TSLayoutX=(unsigned int)l1->getTSLayoutX(region);
  unsigned int TSLayoutY=(unsigned int)l1->getTSLayoutY(region);
  MuonLayout TSLayout(TSLayoutX,TSLayoutY);
  MuonTileID TSTile=TSLayout.contains(digit);
  return TSTile;  
};


std::string MuonDigitToRawBuffer::findODEPath(MuonTileID TS)
{
 MsgStream  msg( msgSvc(), name() );
  unsigned int station=TS.station();  
  unsigned int region=TS.region();
  unsigned int quadrant=TS.quarter();
  int odeStart=m_ODENameStart[station][region][quadrant];
  int odeEnd=m_ODENameEnd[station][region][quadrant];
  msg<<MSG::VERBOSE<<station<<" "<<region<<" "<<quadrant<<endreq;
  msg<<MSG::VERBOSE<<odeStart<<" "<<odeEnd<<" "<<m_ODEName[odeStart]<<endreq;
  //TilePrintOut(TS);
  
  for(int ode=odeStart;ode<odeEnd;ode++){
    std::string odePath=m_ODEName[ode];
    msg<<MSG::VERBOSE<<odePath<<endreq;    
    //std::cout<<odePath<<std::endl;    
    SmartDataPtr<MuonODEBoard>  ode(detSvc(),odePath);
    msg<<MSG::VERBOSE<<ode->isTSContained(TS)<<endreq;  
    if(ode->isTSContained(TS))return odePath;
  }
  return NULL;
  
};


long MuonDigitToRawBuffer::findODENumber(std::string odePath)
{
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),odePath);
  long ODENumber=ode->getODESerialNumber();
  return ODENumber;
  

}


long MuonDigitToRawBuffer::findODEPosition(std::string L1Path, long odeNumber)
{
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1Path);
  return l1->getODEPosition(odeNumber);
}

std::string MuonDigitToRawBuffer::findL1(MuonTileID TS)
{
  unsigned int station=TS.station();
  unsigned int region=TS.region();
  unsigned int quadrant=TS.quarter();
  unsigned int index=station*16+region*4+quadrant;
  std::string L1Path=m_L1Name[index];
  return L1Path;  
};



long MuonDigitToRawBuffer::findTSPosition(std::string ODEPath, 
                                          MuonTileID TSTile)
{
  unsigned int quadrant=TSTile.quarter();
  unsigned int gridx=TSTile.nX();
  unsigned int gridy=TSTile.nY();
  //   MsgStream msg(msgSvc(), name());
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEPath);
  //msg<<MSG::INFO<<gridx<<" "<<gridy<<endreq;
  
  for(int TS=0;TS<ode->getTSNumber();TS++){
    //  msg<<MSG::INFO<<TS<<" "<<ode->getTSGridX(TS)<<endreq;
    if((unsigned int)ode->getTSQuadrant(TS)==quadrant){
      if((unsigned int)ode->getTSGridX(TS)==gridx){
        if((unsigned int)ode->getTSGridY(TS)==gridy){

          return TS;
          
        }          
      }      
    }    
  } 
  MsgStream msg(msgSvc(), name());
  msg<<MSG::FATAL<<"error in finding TS postion "<<endreq;
  
  return -1;  
};
std::string MuonDigitToRawBuffer::findTSPath(std::string ODEPath, 
                                              long TSPosition,int station){
  //  MsgStream msg(msgSvc(), name());
  
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEPath);
  std::string base=getBasePath(station); 
   std::string  TSPath=ode->getTSName(TSPosition);
  //msg<<MSG::INFO<<TSPosition<<ode->name()<<endreq;
  
  //std::string base="/dd/Cabling/Muon/M4/";
  
  //msg<<MSG::INFO<<base<<" "<<TSPosition<<TSPath<<endreq;
  std::string out=base+TSPath;    
  return out;
}; 


long MuonDigitToRawBuffer::findDigitInTS(std::string TSPath,
                                         MuonTileID TSTile, 
                                         MuonTileID digit){
    MsgStream msg(msgSvc(), name());
  SmartDataPtr<MuonTSMap>  TS(detSvc(),TSPath);
  unsigned int TSLayoutX=TSTile.layout().xGrid(); 
  unsigned int TSLayoutY=TSTile.layout().yGrid(); 

  unsigned int layoutX=digit.layout().xGrid();  
  unsigned int layoutY=digit.layout().yGrid();

  //unsigned int gridx=digit.nX();
  //unsigned int gridy=digit.nY();

  // calculate the relative position of the digit in the TS
  unsigned int xScaleFactor=layoutX/TSLayoutX;
  unsigned int yScaleFactor=layoutY/TSLayoutY;
  
  unsigned int gridx=digit.nX()- xScaleFactor*TSTile.nX();
  unsigned int gridy=digit.nY()- yScaleFactor*TSTile.nY();

  //   msg<<MSG::INFO<<"ale "<<TSLayoutX<<" "<<layoutX<<" "<<digit.nX()
  // <<" "<<gridx<<endreq;
  //msg<<MSG::INFO<<"ale "<<TSLayoutY<<" "<<layoutY<<" "<<digit.nY()
  //  <<" "<<gridy<<endreq;
  
  int layout=-1;

  // calculate the relative position of the digit in the TS

  
  for(int i=0;i<TS->numberOfLayout();i++){
    if((unsigned int)TS->gridXLayout(i)==xScaleFactor&&
       (unsigned int)TS->gridYLayout(i)==yScaleFactor){
      layout=i;      
    }    
  }
  //  msg<<MSG::INFO<<"layout "<<layout<<endreq;
  
  for(int i=0;i<TS->numberOfOutputSignal();i++){
    if(TS->layoutOutputChannel(i)==layout){
      if((unsigned int)TS->gridXOutputChannel(i)==gridx){
        if((unsigned int)TS->gridYOutputChannel(i)==gridy){
          //          msg<<MSG::INFO<<"digit in ts "<<i<<endreq;
          return i;
          
        }
      }      
    }    
  }
  msg<<MSG::FATAL<<" error in findging digit in TS "<<endreq;
  msg<<MSG::FATAL<<" position in  TS should be "<<gridx<<" "<<
    gridy<<" "<<layout<<endreq;
  msg<<MSG::FATAL<<" lauoyt should be  "<<xScaleFactor<<" "<<
    yScaleFactor<<endreq;

  
  return -1;
  
};  



long MuonDigitToRawBuffer::channelsInL1BeforeODE(std::string L1Path,
                                                 long ODENumber)
{
  MsgStream msg(msgSvc(), name());
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1Path);
  long station=l1->getStation();
  
  std::string base=getBasePath(station);  
  long stop=findODEPosition(L1Path, ODENumber);  
  long channels=0;
  //  msg<<MSG::INFO<<"stop "<<station<<" "<<
  //stop<<" "<<ODENumber<<l1->getODEName(0)<<endreq;  
  for(int ODEBoard=0;ODEBoard<stop;ODEBoard++){    
    std::string ODEpath=base+l1->getODEName(ODEBoard);
    msg<<MSG::DEBUG<< ODEpath <<endreq;
    
    SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
    long TSnumber=ode->getTSNumber();
    std::string TSPath=base+ode->getTSName(0);
    SmartDataPtr<MuonTSMap>  TS(detSvc(),TSPath);
    long channelInTS=TS->numberOfOutputSignal();
    long channelInODE=channelInTS*TSnumber;   
    msg<<MSG::DEBUG<<"channel in ODE "<<channelInODE<<endreq;
    
    channels=channels+channelInODE;    
  } 
  return channels;  
};




std::string MuonDigitToRawBuffer::getBasePath(int station)
{
  return basePath[station];
  
};


long MuonDigitToRawBuffer::DAQaddress(MuonTileID digitTile, long& L1Number)
{
  MsgStream  msg( msgSvc(), name() );
  msg<<MSG::DEBUG<<"************** start coding a digit "<<endreq; 
  //digitTile.printOut(msg<<MSG::INFO<<"digit tile ")<<endreq;
  
  bool print=false;
  //   if(print)
  //TilePrintOut(digitTile);
  long station=digitTile.station();
  MuonTileID TS=findTS(digitTile);
  if(print)msg<<MSG::INFO<<"digit is contained in TS "<<endreq;
  //TS.printOut(msg<<MSG::DEBUG)<<endreq;
  //  if(print) 
  //TilePrintOut(TS);
  
  std::string L1Path=findL1(TS);
  if(print)msg<<MSG::INFO<<"the TS is contained in L1 "<<L1Path<<endreq;
  // return 1;
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1Path);
  if(print)msg<<MSG::INFO<<"l1 "<<endreq;
  //return 1;
  
  if(print)msg<<MSG::INFO<<" station "<<l1->getStation()<<endreq;
  //long 
  L1Number=l1->L1Number();
  std::string ODEPath= findODEPath(TS);

  if(print)
    msg<<MSG::INFO<<"the TS is contained in ODE "<<ODEPath<<endreq;      
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEPath);
  long ODENumber=findODENumber(ODEPath);
  long ODESerialNumber=findODEPosition(L1Path, ODENumber); 
  if(print)msg<<MSG::INFO<<"ODE= "<<ODENumber<<" "<<ODESerialNumber<<endreq;
  long TSSerialNumber=findTSPosition(ODEPath,TS);
  if(print)
    msg<<MSG::INFO<<"the TS position is ODE is = "<<TSSerialNumber<<endreq;
  std::string TSPath= findTSPath(ODEPath,TSSerialNumber,station);  
  msg<<MSG::DEBUG<<"the TS map is located in  "<<TSPath<<endreq;
  msg<<MSG::DEBUG<<" station "<<l1->getStation()<<endreq;
      
  long DigitPosition=findDigitInTS(TSPath,TS,digitTile); 
  long addChannels=channelsInL1BeforeODE( L1Path,ODENumber);   
  if(print)msg<<MSG::INFO
              <<" the digit position in TS is "<<DigitPosition<<endreq;
  if(print)msg<<MSG::INFO<<" the channels in L1 before the ODE are "<<
    addChannels<<endreq;
  SmartDataPtr<MuonTSMap>  TSMap(detSvc(),TSPath);    
  long digitInODE=TSSerialNumber*TSMap->numberOfOutputSignal();
  msg<<MSG::DEBUG<<" the channels in ODE before the TS are "<<
    digitInODE<<endreq;
  long DigitOutputPosition=addChannels+digitInODE+DigitPosition;
  if(print)
    msg<<MSG::INFO<<" the output position in L1 of the digit is "<<
    DigitOutputPosition<<endreq;
  return DigitOutputPosition;  
};



void MuonDigitToRawBuffer::TilePrintOut(MuonTileID digitTile)
{
  MsgStream  msg( msgSvc(), name() );
  msg<<MSG::INFO<< "["  <<  digitTile.layout() << ","
       <<  digitTile.station() << ","
       <<  digitTile.region() << ","
       <<  digitTile.quarter() << ","
       <<  digitTile.nX() << ","
     <<  digitTile.nY() << "]" <<endreq;

};

