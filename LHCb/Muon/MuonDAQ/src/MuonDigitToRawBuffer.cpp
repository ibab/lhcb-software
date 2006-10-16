// $Id: MuonDigitToRawBuffer.cpp,v 1.15 2006-10-16 08:12:10 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "MuonDAQ/SortDigitInODE.h"
#include "MuonDigitToRawBuffer.h"
#include "MuonDAQ/MuonHLTDigitFormat.h"
#include "MuonDet/MuonTSMap.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonStationCabling.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/BankWriter.h"
#include "Event/MuonDigit.h" 

//-----------------------------------------------------------------------------
// Implementation file for class : MuonDigitToRawBuffer
//
// 2004-01-16 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MuonDigitToRawBuffer );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDigitToRawBuffer::MuonDigitToRawBuffer( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  basePath[0] = "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/";
  basePath[1] = "/dd/Conditions/ReadoutConf/Muon/Cabling/M2/";
  basePath[2] = "/dd/Conditions/ReadoutConf/Muon/Cabling/M3/";
  basePath[3] = "/dd/Conditions/ReadoutConf/Muon/Cabling/M4/";
  basePath[4] = "/dd/Conditions/ReadoutConf/Muon/Cabling/M5/";

}
//=============================================================================
// Destructor
//=============================================================================
MuonDigitToRawBuffer::~MuonDigitToRawBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonDigitToRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm


  debug() << "==> Initialise" << endmsg;


 
  unsigned int ODEStart=0;
  unsigned int ODEEnd=1;  
  m_L1Name.resize(80);  
  m_TotL1Board=0;
  unsigned int L1Count=0;
  m_TotODEBoard=0;
  
  for(int station=0;station<5;station++){
    debug()<<"station number "<<station<<endmsg;
    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(),
                                              cablingPath);
    if( 0 == cabling ) {
      err() << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }
    
    m_TotL1Board=m_TotL1Board+cabling->getNumberOfL1Board();
    if(station==0)m_M1Tell1=cabling->getNumberOfL1Board();
    
    
    debug()<<"station path "<<cablingPath<<endmsg;    
    debug()<<"station number "<<station<<" "<<
      "L1number "<<  cabling->getNumberOfL1Board()<<endmsg;
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){
    
      debug()<<"L1 number "<<cabling->getL1Name(L1Board)<<endmsg;
      std::string L1path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
      m_TotODEBoard=m_TotODEBoard+l1->numberOfODE();
      m_TELL1Name.push_back(L1path);      
    }
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){      
      //info()<<"L1 number "<<L1Board<<" "<<cabling->getL1Name(L1Board)<<endmsg;
      L1Count++;
      
      std::string L1path=cablingBasePath+
        cabling->getL1Name(L1Board);
      SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);      
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
        m_ODEInL1[L1Count-1].push_back(odenum);        
        long region=ode->region();  
        debug()<<"ODE region "<<region<<endmsg;        
        for(int quadrant=0;quadrant<4;quadrant++){
          unsigned int index=station*16+region*4+quadrant; 
          debug()<<" ode in quarter " <<quadrant 
                 <<" "<<ode->isQuadrantContained(quadrant)<<endmsg;          
          if(ode->isQuadrantContained(quadrant)){            
            debug()<<"quadrant number "<<quadrant<<endmsg;
            m_L1Name[index]=L1path; 
            debug()<<"index "<<index<<" "<<L1path<<endmsg;
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
        debug()<<"number of L1 "<<l1->numberOfODE()<<endmsg;  
         debug()<<station<<" "<<region<<" "<<quadrant<<" "<<
           L1path<<endmsg;
         
         for(int ODEBoard=0;ODEBoard<l1->numberOfODE();ODEBoard++){
           std::string ODEpath=cablingBasePath+
             l1->getODEName(ODEBoard);
           debug()<<" ode path "<<ODEpath<<endmsg;          
           SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);          
           if(ode->region()==region){
             debug()<<" ode quadrant "<<
              ode->isQuadrantContained(quadrant)<<endmsg;            
             if(ode->isQuadrantContained(quadrant)){
               debug()<<station<<" "<<region<<" "<<quadrant<<" "<<
                 ODEpath<<endmsg;               
               numODE++;
               m_ODEName.push_back(ODEpath);               
             }            
             debug()<<numODE<<endmsg;             
           }           
         }         
         m_ODENameStart[station][region][quadrant]=ODEStart;      
         ODEEnd=ODEStart+numODE;
         m_ODENameEnd[station][region][quadrant]=ODEEnd;
         ODEStart=ODEEnd;
      }
      
      
    }    
  }
  for(int i=0;i<m_TotL1Board;i++){
    std::vector<unsigned int> listOfODE=m_ODEInL1[i];
    std::vector<unsigned int>::iterator iODE;    
     for(iODE=listOfODE.begin();iODE<listOfODE.end();iODE++){
      unsigned int odenumber=(*iODE)-1;
      m_digitsInODE[odenumber].resize(0);
      firedInODE[odenumber]=0;
     }
  }

  debug()<<"board number "<<m_TotL1Board<<endmsg;
  
  
  return StatusCode::SUCCESS;
};
//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDigitToRawBuffer::execute() {


  debug() << "==> Execute" << endmsg;
 
  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );

  SmartDataPtr<LHCb::MuonDigits> digit(eventSvc(),
                                 LHCb::MuonDigitLocation::MuonDigit);
  LHCb::MuonDigits::iterator idigit;
  for(idigit=digit->begin();idigit<digit->end();idigit++){    
    LHCb::MuonTileID digitTile=(*idigit)->key();
 //    debug()<<"digit tile "<<digitTile<<endmsg;	    
//     debug()<<" digit tile "<<digitTile.layout()<<" "<<digitTile.station()
//             <<" "<<digitTile.region()<<" "<<
//        digitTile.quarter()<<" "<<digitTile.nX()<<" "<<digitTile.nY()<<endmsg;
    unsigned int time=(*idigit)->TimeStamp();    
    //time=0;    
    long L1Number=0;
    long ODENumber=0;    
    long DigitOutputPosition=DAQaddress(digitTile,L1Number,ODENumber); 
    debug()<<"L1Number "<<L1Number<<" "<<ODENumber<<endmsg;
    debug()<<" digitOutputPosition "<<DigitOutputPosition<<endmsg;    
    unsigned int digitInDAQ=0;
    MuonHLTDigitFormat temp;
    temp.setAddress(DigitOutputPosition);
    temp.setTime(time);
    digitInDAQ=temp.getWord();
//     debug()<<" digit word "<<L1Number-1<<" "<<ODENumber-1<<" "<<
//       temp.getAddress()<<endmsg;    
    m_digitsInODE[ODENumber-1].push_back(digitInDAQ);  
    firedInODE[ODENumber-1]++; 
  }  
  //  debug()<<" tot ODE "<<m_TotODEBoard<<endreq;
  
  for(int i=0;i<m_TotODEBoard;i++){
    std::stable_sort(m_digitsInODE[i].begin(),
                     m_digitsInODE[i].end(),SortDigitInODE());    
  }
  std::vector<unsigned  int>::iterator itDigit;
  //now the pads can be defined....skip M1 whichis already in pads..
  for(unsigned int i=m_M1Tell1;i<(unsigned int) m_TotL1Board;i++){
    //get Tell1 information 
    std::string L1path=m_TELL1Name[i];
    SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
    int station=l1->getStation();    
    std::string cablingBasePath=getBasePath(station);
    unsigned int padsInTell1=0;
    //    debug()<<" tell1 "<<L1path<<" "<<station<<" "<<i<<endreq;    
    //get ODE info
    std::vector<unsigned int> listOfODE=m_ODEInL1[i];
    std::vector<unsigned int>::iterator iODE; 
    //unsigned int TSNumberInTell1=0;    
    unsigned int maxPads=0;
    unsigned int ODEBoard=0;
    unsigned int TSInODE=0;
    
    //loop on ODE
    for(iODE=listOfODE.begin();iODE<listOfODE.end();iODE++,ODEBoard++){
      //      debug()<<i<<" ode board "<<ODEBoard<<" "<<listOfODE.size()<<endreq;
      
      std::string ODEpath=cablingBasePath
        +l1->getODEName(ODEBoard);
      SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
      //debug()<<" odename "<<ODEpath<<endreq;
      
      unsigned int odenumber=(*iODE)-1;
      // how many digit in the TS?
      TSInODE=ode->getTSNumber();
      
      std::string  TSPath= cablingBasePath+ode->getTSName(0);
      SmartDataPtr<MuonTSMap>  TS(detSvc(),TSPath);
      long channelInTS=TS->numberOfOutputSignal();
      if(TS->numberOfLayout()==2){
        maxPads=TS->gridXLayout(0)*TS->gridYLayout(1);         
      }
      else{
        maxPads=channelInTS;         
      }
      
      std::vector<unsigned int> list_input(channelInTS,0); 
      //debug()<<" channelInTS "<<channelInTS<<endreq;      
      //build header == channels fired for each ode
      unsigned int TSnumberInODE=0;
      for(itDigit=m_digitsInODE[odenumber].begin();
          itDigit<m_digitsInODE[odenumber].end();
          itDigit++){
        MuonHLTDigitFormat temp(*itDigit);
        unsigned int address=temp.getAddress();
        if(itDigit==m_digitsInODE[odenumber].begin()){
           TSnumberInODE=address/channelInTS;
        }
        //debug()<<" address "<<address<<" "<<TSnumberInODE<<endreq;
        bool swap=true;
        
        if(channelInTS*(TSnumberInODE+1)<=address||
           itDigit==m_digitsInODE[odenumber].end()-1){
          // info()<<" entra qui "<<TSnumberInODE<<endreq;
          if(itDigit==(m_digitsInODE[odenumber].end()-1)&&
             address<channelInTS*(TSnumberInODE+1)) {            
              list_input[address-channelInTS*TSnumberInODE]=1;
              swap=false;              
          }          
          std::string TSPath=cablingBasePath+
            ode->getTSName(TSnumberInODE);     
          // debug()<<" TSPath "<<  TSPath <<endreq;          
          std::vector<unsigned int> resultsAddress=
            padsinTS(list_input, 
                     TSPath);
          //store the output
          if(resultsAddress.size()){            
            for(std::vector<unsigned int>::iterator itpad=
                  resultsAddress.begin();itpad<resultsAddress.end();
                itpad++){              
              m_padInL1[i].push_back(padsInTell1+maxPads*TSnumberInODE+
                                     (*itpad));              
            }
          }
          
          //add te offset padsInTell1;
          //padsInTell1=padsInTell1+maxPads;          
          //clear the area
          std::vector<unsigned int>::iterator itClear;          
          for(itClear= list_input.begin();itClear<list_input.end();itClear++){
            *itClear=0;            
          }          
          TSnumberInODE=address/channelInTS;
          if(swap) {  
            //  debug()<<"adding in position "<<
            //address-channelInTS*TSnumberInODE
            //     <<" "<<address<<endreq;
            
             list_input[address-channelInTS*TSnumberInODE]=1;
             //add the pads mechanism....
             if(itDigit==m_digitsInODE[odenumber].end()-1){               
                std::vector<unsigned int> resultsAddress=
               padsinTS(list_input, 
                        TSPath);
                if(resultsAddress.size()){            
                  for(std::vector<unsigned int>::iterator itpad=
                        resultsAddress.begin();
                      itpad<resultsAddress.end();itpad++){          
                    m_padInL1[i].push_back(padsInTell1+
                                           maxPads*TSnumberInODE+
                                           (*itpad));              
                  }
                }
             }
          }          
        }else{
          list_input[address-channelInTS*TSnumberInODE]=1;          
        }        
      }
      padsInTell1=padsInTell1+maxPads*TSInODE;
      
      //info()<<" eccoci "<<endreq;      
    }    
  }
  
  
  
  for(unsigned int i=0;i<(unsigned int) m_TotL1Board;i++){
    std::vector<unsigned int> listOfODE=m_ODEInL1[i];
    std::vector<unsigned int>::iterator iODE;    
    unsigned int totalChInL1=0;    
    unsigned int odeInTell1=0;    
    for(iODE=listOfODE.begin();iODE<listOfODE.end();iODE++){
      unsigned int odenumber=(*iODE);      
      //build header == channels fired for each ode
      unsigned int firedChannels=firedInODE[odenumber-1];     
      //fill in bank.....
      totalChInL1=totalChInL1+firedChannels;
      odeInTell1++;      
    }
    // now the dimension of the bank can be fixed....
    //the algo if (odeInTell1+2*totalChInL1+0.5*totalChInL1)/4
    unsigned int bankLenght=0;
    unsigned int bankLenPad=0;
    //  info()<<" pad lenght "<<i<<" "<<m_padInL1[i].size()<<endreq;    
    if(i>=m_M1Tell1)bankLenPad=(m_padInL1[i].size()+1)*16;
    
    unsigned int bankLenAdd=odeInTell1*8+8*totalChInL1;
    unsigned int bankLenTime=4*totalChInL1;


    //info()<<"ode in Tell1 "<<odeInTell1<<" tot ch "<<totalChInL1<<endmsg  ;
     if(bankLenPad%32!=0){
      bankLenght=bankLenght+bankLenPad/32+1;
    }else{
      bankLenght=bankLenght+bankLenPad/32;      
    }
    if(bankLenAdd%32!=0){
      bankLenght=bankLenght+bankLenAdd/32+1;
    }else{
      bankLenght=bankLenght+bankLenAdd/32;      
    }
    if(bankLenTime%32!=0){
      bankLenght=bankLenght+bankLenTime/32+1;
    }else{
      bankLenght=bankLenght+bankLenTime/32;      
    }

    //    info()<<"Tell 1 "<<i<<" "<<bankLenPad<<" "
    //<<bankLenAdd<<" "<<bankLenTime<<" "<<bankLenght<<endmsg;  
    LHCb::BankWriter bank(bankLenght);
    if(i>=m_M1Tell1){
      short padSize=m_padInL1[i].size();      
      bank<<padSize;
      for(std::vector<unsigned int>::iterator itPad=m_padInL1[i].begin();
          itPad<m_padInL1[i].end();itPad++){
        
        //fill the pad address
        short padADD=*itPad;
        bank<<padADD;
      }     
    }
    //padding at the end
    short nullWord=0;    
    if(bankLenPad%32!=0)bank<<nullWord; 
    //  info()<<"after creation "<<sizeof(int)<<" "<<sizeof(char)<<endmsg;
    for(iODE=listOfODE.begin();iODE<listOfODE.end();iODE++){
      unsigned int odenumber=(*iODE);
      //build header == channels fired for each ode
      unsigned int firedChannels=firedInODE[odenumber-1];     
      //info()<<" fired channels "<<firedChannels<<" "<<odenumber<<endmsg;
      // //fill in bank.....
      unsigned char fired=(unsigned char) firedChannels;
      bank<<fired;      
    }
    //info()<<"after fired "<<bank.dataBank().size()<<endmsg;
    
    for(iODE=listOfODE.begin();iODE<listOfODE.end();iODE++){
      unsigned int odenumber=(*iODE)-1;
      //      unsigned int firedChannels=firedInODE[odenumber];
      for(itDigit=m_digitsInODE[odenumber].begin();
          itDigit<m_digitsInODE[odenumber].end();
          itDigit++){            
        MuonHLTDigitFormat temp(*itDigit);
        unsigned int adress=temp.getAddress();
        unsigned char chad=(unsigned char) adress;        
        bank<< chad;        
      }
    }
    // padding if needed 
    if(bankLenAdd%32!=0){
      unsigned int padding=(bankLenAdd%32)/8;
      for(unsigned int addpad=0;addpad<4-padding;addpad++){  
        //the padding is set to zero 
        unsigned char chad=(unsigned char) 0;
        bank<< chad;  
      }      
    }
    
    //verbose()<<" after address "<<endmsg;
    bool even=false;
    unsigned char time=0;
    unsigned int timetemp=0;
    
    for(iODE=listOfODE.begin();iODE<listOfODE.end();iODE++){
      unsigned int odenumber=(*iODE)-1;
      //      unsigned int firedChannels=firedInODE[odenumber];    
      for(itDigit=m_digitsInODE[odenumber].begin();
          itDigit<m_digitsInODE[odenumber].end();
          itDigit++){
        MuonHLTDigitFormat temp1(*itDigit);        
        unsigned int time1=temp1.getTime();
        if(even){
          timetemp=timetemp+(time1<<4);          
        }else{
          timetemp=time1;          
        }
        //        info()<<" time .."<<even <<" "<<(unsigned int )time1<<" "<<
        //  (unsigned int)time<<endmsg;
        time=(unsigned char) timetemp;
      
        if(even)bank<<time;  
        even=!even;
        //  info()<<" time .."<<even <<" "<<(unsigned int )time1<<" "<<
        // (unsigned int)time<<endmsg;
      }
    }
    if(even)bank<<time;
    //fill with zeros the padding
    if(bankLenTime%32!=0){
      unsigned int padding=(bankLenTime%32)/8;
      for(unsigned int addpad=0;addpad<padding;addpad++){  
        //the padding is set to zero 
        unsigned char chad=(unsigned char) 0;
        bank<< chad;  
      }      
    }
    
    raw->addBank(i,RawBank::Muon,1,bank.dataBank());
  }
    
  for(int i=0;i<m_TotL1Board;i++){
    std::vector<unsigned int> listOfODE=m_ODEInL1[i];
    std::vector<unsigned int>::iterator iODE;    
    m_padInL1[i].clear();    
     for(iODE=listOfODE.begin();iODE<listOfODE.end();iODE++){
      unsigned int odenumber=(*iODE)-1;
      m_digitsInODE[odenumber].resize(0);
      firedInODE[odenumber]=0;
     }
  }
  debug()<<" exit "<<endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonDigitToRawBuffer::finalize() {

  
  debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions

}

//=============================================================================


LHCb::MuonTileID  MuonDigitToRawBuffer::findTS(LHCb::MuonTileID digit)
{
  unsigned int station=digit.station();
  
  unsigned int region=digit.region();
  
 // get TS layout in region from L1
  unsigned int index=station*16+region*4;  
  std::string L1path=m_L1Name[index];
  //debug()<<L1path<<" "<<station<<" "<<region<<endmsg;
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
  unsigned int TSLayoutX=(unsigned int)l1->getTSLayoutX(region);
  unsigned int TSLayoutY=(unsigned int)l1->getTSLayoutY(region);
  MuonLayout TSLayout(TSLayoutX,TSLayoutY);
  //  debug()<<"TS Layout "<<TSLayoutX<<" "<<TSLayoutY<<endmsg;  
  LHCb::MuonTileID TSTile=TSLayout.contains(digit);
  //TilePrintOut(TSTile);  
  return TSTile;  
};


std::string MuonDigitToRawBuffer::findODEPath(LHCb::MuonTileID TS)
{
 
  unsigned int station=TS.station();  
  unsigned int region=TS.region();
  unsigned int quadrant=TS.quarter();
  int odeStart=m_ODENameStart[station][region][quadrant];
  int odeEnd=m_ODENameEnd[station][region][quadrant];
  //verbose()<<station<<" "<<region<<" "<<quadrant<<endmsg;
  //verbose()<<odeStart<<" "<<odeEnd<<" "<<m_ODEName[odeStart]<<endmsg;
  //TilePrintOut(TS);
  
  for(int ode=odeStart;ode<odeEnd;ode++){
    std::string odePath=m_ODEName[ode];
    //verbose()<<odePath<<endmsg;    
    //std::cout<<odePath<<std::endl;    
    SmartDataPtr<MuonODEBoard>  odeBoard(detSvc(),odePath);
    //verbose()<<odeBoard->isTSContained(TS)<<endmsg;  
    if(odeBoard->isTSContained(TS))return odePath;
  }
  return NULL;
  
};


unsigned int MuonDigitToRawBuffer::findODENumber(std::string odePath)
{
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),odePath);
  unsigned int ODENumber=ode->getODESerialNumber();
  return ODENumber;
  

}


unsigned int MuonDigitToRawBuffer::findODEPosition(std::string L1Path, long odeNumber)
{
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1Path);
  return l1->getODEPosition(odeNumber);
}

std::string MuonDigitToRawBuffer::findL1(LHCb::MuonTileID TS)
{
  unsigned int station=TS.station();
  unsigned int region=TS.region();
  unsigned int quadrant=TS.quarter();
  unsigned int index=station*16+region*4+quadrant;
  std::string L1Path=m_L1Name[index];
  return L1Path;  
};



unsigned int MuonDigitToRawBuffer::findTSPosition(std::string ODEPath, 
                                          LHCb::MuonTileID TSTile)
{
  unsigned int quadrant=TSTile.quarter();
  unsigned int gridx=TSTile.nX();
  unsigned int gridy=TSTile.nY();
  
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEPath);
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
  
  err()<<"error in finding TS postion "<<endmsg;
  
  return 100000;  
};
std::string MuonDigitToRawBuffer::findTSPath(std::string ODEPath, 
                                              long TSPosition,int station){
  
  
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEPath);
  std::string base=getBasePath(station); 
   std::string  TSPath=ode->getTSName(TSPosition);
  //info()<<TSPosition<<ode->name()<<endmsg;
  
  //std::string base="/dd/Cabling/Muon/M4/";
  
  //info()<<base<<" "<<TSPosition<<TSPath<<endmsg;
  std::string out=base+TSPath;    
  return out;
}; 


unsigned int MuonDigitToRawBuffer::findDigitInTS(std::string TSPath,
                                         LHCb::MuonTileID TSTile, 
                                         LHCb::MuonTileID digit){
    
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

  //  info()<<"ale "<<TSLayoutX<<" "<<layoutX<<" "<<digit.nX()
  // <<" "<<gridx<<endmsg;
  //info()<<"ale "<<TSLayoutY<<" "<<layoutY<<" "<<digit.nY()
  // <<" "<<gridy<<endmsg;
  
  int layout=-1;

  // calculate the relative position of the digit in the TS

  
  for(int i=0;i<TS->numberOfLayout();i++){
    if((unsigned int)TS->gridXLayout(i)==xScaleFactor&&
       (unsigned int)TS->gridYLayout(i)==yScaleFactor){
      layout=i;      
    }    
  }
  // info()<<"layout "<<layout<<endmsg;
  
  for(int i=0;i<TS->numberOfOutputSignal();i++){
    if(TS->layoutOutputChannel(i)==layout){
      if((unsigned int)TS->gridXOutputChannel(i)==gridx){
        if((unsigned int)TS->gridYOutputChannel(i)==gridy){
          //                 info()<<"digit in ts "<<i<<endmsg;
          return i;
          
        }
      }      
    }    
  }
  err()<<" error in findging digit in TS "<<endmsg;
  err()<<" position in  TS should be "<<gridx<<" "<<
    gridy<<" "<<layout<<endmsg;
  err()<<" lauoyt should be  "<<xScaleFactor<<" "<<
    yScaleFactor<<endmsg;

  
  return 100000;
  
};  



long MuonDigitToRawBuffer::channelsInL1BeforeODE(std::string L1Path,
                                                 long ODENumber)
{
  
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1Path);
  long station=l1->getStation();
  
  std::string base=getBasePath(station);  
  long stop=findODEPosition(L1Path, ODENumber);  
  long channels=0;
  //  info()<<"stop "<<station<<" "<<
  //stop<<" "<<ODENumber<<l1->getODEName(0)<<endmsg;  
  for(int ODEBoard=0;ODEBoard<stop;ODEBoard++){    
    std::string ODEpath=base+l1->getODEName(ODEBoard);
    //debug()<< ODEpath <<endmsg;
    
    SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
    long TSnumber=ode->getTSNumber();
    std::string TSPath=base+ode->getTSName(0);
    SmartDataPtr<MuonTSMap>  TS(detSvc(),TSPath);
    long channelInTS=TS->numberOfOutputSignal();
    long channelInODE=channelInTS*TSnumber;   
    //debug()<<"channel in ODE "<<channelInODE<<endmsg;
    
    channels=channels+channelInODE;    
  } 
  return channels;  
};




std::string MuonDigitToRawBuffer::getBasePath(int station)
{
  return basePath[station];
  
};


unsigned int MuonDigitToRawBuffer::DAQaddress(LHCb::MuonTileID digitTile, long& L1Number, 
                                      long& ODENumber)
{
  
  debug()<<"************** start coding a digit "<<endmsg; 
  //digitTile.printOut(info()<<"digit tile ")<<endmsg;  
  bool print=false;
   if(print)
  TilePrintOut(digitTile);
  long station=digitTile.station();
  LHCb::MuonTileID TS=findTS(digitTile);
  if(print){
    debug()<<"digit is contained in TS "<<endmsg;
    TilePrintOut(TS);
  }
  
  std::string L1Path=findL1(TS);
  if(print)debug()<<"the TS is contained in L1 "<<L1Path<<endmsg;
  // return 1;
  SmartDataPtr<MuonL1Board>  l1(detSvc(),L1Path);
  if(print)debug()<<"l1 "<<endmsg;
  //return 1;
  
  if(print)debug()<<" station "<<l1->getStation()<<endmsg;
  //long 
  L1Number=l1->L1Number();
  std::string ODEPath= findODEPath(TS);

  if(print)
    debug()<<"the TS is contained in ODE "<<ODEPath<<endmsg;      
  SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEPath);
  ODENumber=findODENumber(ODEPath);
  unsigned int ODESerialNumber=findODEPosition(L1Path, ODENumber); 
  if(print)debug()<<"ODE= "<<ODENumber<<" "<<ODESerialNumber<<endmsg;
  unsigned int TSSerialNumber=findTSPosition(ODEPath,TS);
  if(print)
    debug()<<"the TS position is ODE is = "<<TSSerialNumber<<endmsg;
  std::string TSPath= findTSPath(ODEPath,TSSerialNumber,station);  
  //  debug()<<"the TS map is located in  "<<TSPath<<endmsg;
  // debug()<<" station "<<l1->getStation()<<endmsg;
      
  unsigned int DigitPosition=findDigitInTS(TSPath,TS,digitTile); 
  SmartDataPtr<MuonTSMap>  TSMap(detSvc(),TSPath);    
  unsigned int digitInODE=TSSerialNumber*TSMap->numberOfOutputSignal();
  //debug()<<" the channels in ODE before the TS are "<<
  //  digitInODE<<endmsg;
  unsigned int DigitOutputPosition=digitInODE+DigitPosition;
  if(print)
    info()<<" the output position in L1 of the digit is "<<
      DigitOutputPosition<<endmsg;
  return DigitOutputPosition;  
};



void MuonDigitToRawBuffer::TilePrintOut(LHCb::MuonTileID digitTile)
{
  
  info()<< "["  <<  digitTile.layout() << ","
       <<  digitTile.station() << ","
       <<  digitTile.region() << ","
       <<  digitTile.quarter() << ","
       <<  digitTile.nX() << ","
     <<  digitTile.nY() << "]" <<endmsg;
};





std::vector<unsigned int> MuonDigitToRawBuffer::padsinTS(
                                                         std::vector<unsigned 
                                                         int>& TSDigit,
                                                         std::string TSPath){
  //input the sequence of 0/1 for fired and not fired channels..
  std::vector<unsigned int> list_of_pads;
  //maxPads=0;  
  SmartDataPtr<MuonTSMap>  TS(detSvc(),TSPath);
  //debug()<<"----- start a trigger sector cross ---------"<<endreq;
  //debug()<<TSPath<<" "<<TS->numberOfOutputSignal()<<endreq;
  //debug()<<" seq ";
  //std::vector<unsigned int>::iterator idebug;
  //for(idebug=TSDigit.begin();idebug<TSDigit.end();idebug++){
  //  debug()<<" "<<*idebug;    
  //} 
  //debug()<<endreq;
  
  if(TS->numberOfLayout()==2){
    //how many subsector????
    int NX=TS->gridXLayout(1);    
    int NY=TS->gridYLayout(0);
    int Nsub=NX*NY;
    //debug()<<"number NX NY "<<NX<<" "<<NY<<endreq;   
    //maxPads=TS->gridXLayout(0)*TS->gridYLayout(1);   
    // work on sub sector 
    if(Nsub>8){
      err()<<"error the dimensioning of the TS subsector is wrong "<<endreq;
      return list_of_pads;
    
    }

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
      //debug()<<" digit "<<index<<" "<<mx<<" "<<my<<" "<<*it<<endreq;     
      int Msub=mx+my*NX;
      // horizntal o vertical?
      bool horizontal=false;
      if(TS->layoutOutputChannel(index)==0)horizontal=true;
      if(horizontal)horiz[Msub].push_back(*it);
      else hvert[Msub].push_back(*it);
      // debug()<<" horizontal ? "<<     horizontal<<endreq;      
    }
    // now the address of fired pads..
    for(int i=0;i<Nsub;i++){
      // cross only local to each sub matrix
      std::vector<unsigned int>::iterator itx;
      std::vector<unsigned int>::iterator ity;
      //debug 
      //debug()<<" sub matrix "<<i<<endreq;
      //debug()<<" horizontal sequence ";
      
      // for(itx=horiz[i].begin();
      //    itx<horiz[i].end();itx++)
      // {
      //  debug()<<*itx<<" ";        
      //}
      //debug()<<endreq;
      //debug()<<" vertical sequence ";
      
      //for(ity=hvert[i].begin();ity<hvert[i].end();ity++){
      //  debug()<<*ity<<" ";        
      //}
      //debug()<<endreq;      
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
              //      debug()<<" result of the address "<<address<<endreq;              
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
      //      debug()<<" result of the address "<<address<<endreq;
      } 
    }    
  }
  return list_of_pads;  
}

