// $Id: MuonRawBuffer.cpp,v 1.2 2006-02-01 10:21:08 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MuonRawBuffer.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "Event/RawBank.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonRawBuffer
//
// 2005-10-18 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MuonRawBuffer>          s_factory ;
const        IToolFactory& MuonRawBufferFactory = s_factory ; 

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonRawBuffer::MuonRawBuffer( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMuonRawBuffer>(this);
  basePath[0]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M1/";
  basePath[1]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M2/";
  basePath[2]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M3/";
  basePath[3]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M4/";
  basePath[4]= "/dd/Conditions/ReadoutConf/Muon/Cabling/M5/";

}
//=============================================================================
// Destructor
//=============================================================================
MuonRawBuffer::~MuonRawBuffer() {}; 

//=============================================================================



StatusCode MuonRawBuffer::initialize()
{
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;

  //how many tell1
  m_TotTell1=0;  
  for(int station=0;station<5;station++){
    debug()<<"station number "<<station<<endreq;    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);
    if( 0 == cabling ) {
      error() << cablingPath << " not found in XmlDDDB" << endmsg;
      return StatusCode::FAILURE;
    }    
    m_TotTell1=m_TotTell1+cabling->getNumberOfL1Board();
  }
  //how many and which odes in each Tell1
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
  
  return StatusCode::SUCCESS ;
};

std::string MuonRawBuffer::getBasePath(int station)
{
  return basePath[station];
  
};




StatusCode MuonRawBuffer::finalize() {
  //if( m_DDS )      m_DDS->release();
  //if( m_tileTool ) toolSvc()->releaseTool( m_tileTool ); 
  return GaudiTool::finalize() ;
};


std::vector<MuonTileID>  MuonRawBuffer::getTile(){

  std::vector<MuonTileID> storage;
  
  LHCb::RawEvent* raw = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);  
  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;  
  for( itB = b.begin(); itB != b.end(); itB++ ) {    
    const RawBank* r = *itB;
    //info()<<"start of the bank "<<(*itB)->sourceID()<<endmsg;
   //char* it;
    // how many ODE in this tell1?
    unsigned int nODE=m_ODENumberInTell1[(*itB)->sourceID()];
    //info()<<" number of ODE "<<nODE<<endmsg;    
    std::vector<unsigned int> firedInODE;
    firedInODE.resize(nODE) ;    
    unsigned int itODE=0;
    const unsigned char* it=r->begin<unsigned char>();    
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
        
        storage.push_back(tile);
        //        info()<<" qui "<<(unsigned int) tile<<endmsg;       
        it++;
        //channels++;        
      }
    }  
  }      
return storage;
};

std::vector<std::pair<MuonTileID,unsigned int> > MuonRawBuffer::getTileAndTDC(){

  std::vector<std::pair<MuonTileID, unsigned int> > storage;
  
  LHCb::RawEvent* raw = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);  
  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;  
  unsigned int chIterator=0;
  
  for( itB = b.begin(); itB != b.end(); itB++ ) {    
    const RawBank* r = *itB;
    //info()<<"start of the bank "<<(*itB)->sourceID()<<endmsg;
   //char* it;
    // how many ODE in this tell1?
    unsigned int nODE=m_ODENumberInTell1[(*itB)->sourceID()];
    //  info()<<" number of ODE "<<nODE<<endmsg;    
    std::vector<unsigned int> firedInODE;
    firedInODE.resize(nODE) ;    
    unsigned int itODE=0;
    const unsigned char* it=r->begin<unsigned char>();    
    unsigned int channelsInTell1=0;    
    for(itODE=0; itODE < nODE ; itODE++) {
      //first decode the header
        firedInODE[itODE]=(unsigned int)(*it);
        //   info()<<"channels in ODE "<<(unsigned int)(*it)<<endmsg;        
        
        channelsInTell1=channelsInTell1+(unsigned int)(*it);
        it++;
        
    }
    for(itODE=0;itODE<nODE;itODE++){
      unsigned int channels=0;      
      unsigned int odenumber=(m_ODEInTell1[(*itB)->sourceID()])[itODE];      
      for(channels=0;channels<firedInODE[itODE];channels++){
        unsigned int address=(unsigned int)(*it);
        //info()<<"address "<<address<<" "<<odenumber<<endmsg;        
        MuonTileID tile=(m_mapTileInODE[odenumber-1])[address];
        // info()<<"tile in tiol "<<tile.layout()<<" "<<tile.station()<<" "<<
        // tile.region()<<" "<<tile.nX()<<" "<<tile.nY()<<endmsg;
        std::pair<MuonTileID,unsigned int> tileAndTDC;
        tileAndTDC.first=tile;        
        storage.push_back(tileAndTDC);
        //        info()<<" qui "<<(unsigned int) tile<<endmsg;       
        it++;
        //channels++;        
      }
    }
    
    //then decode the TDC info
    unsigned int TDCword=0;
    
    if(channelsInTell1%2==0) TDCword=channelsInTell1/2;
    else TDCword=channelsInTell1/2+1;
    unsigned int countChannels=0;
    
    for(unsigned int ch=0;ch<TDCword;ch++){
      //info()<<" qui "<< (unsigned int) (*it)<<" "<<channelsInTell1<<endmsg;
      //unsigned int time1  ( ( ( (unsigned int) 1 ) << bitTimeStamp  ) - 1  ) << shiftTimeStamp;
      unsigned int time1 = (*it)&((unsigned char)15 );
      //      unsigned int time2 = (((*it)&(unsigned char) 240) >> 4 );   
      unsigned int time2 = ((*it)>>4)&((unsigned char) 15);   
      MuonTileID tile=   storage[chIterator].first;
      
      //info()<<"tile in tiol "<<tile.layout()<<" "<<tile.station()<<" "<<
      //  tile.region()<<" "<<tile.nX()<<" "<<tile.nY()<<" "<<time1<<endmsg;
      MuonTileID tile1=   storage[chIterator+1].first;
//      info()<<"tile in tiol "<<tile1.layout()<<" "<<tile1.station()<<" "<<
//        tile1.region()<<" "<<tile1.nX()<<" "<<tile1.nY()<<" "<<time2<<endmsg;

      //info()<<" tempo "<< chIterator<<" "<<time1 <<" "<<time2<<endmsg;

      storage[chIterator].second=time1;
      chIterator++;
      
      if(countChannels+1<channelsInTell1) {
        storage[chIterator].second=time2;
        chIterator++;        
      }      
      countChannels=countChannels+2;      
      it++;
        //channels++;        
    
    }  
  }  
  return storage;

};


