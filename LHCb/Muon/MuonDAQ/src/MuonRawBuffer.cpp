// $Id: MuonRawBuffer.cpp,v 1.4 2006-03-24 11:03:22 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "Event/RawBank.h"
#include "MuonRawBuffer.h"
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
  sc=initializeLUTCrossing();
  
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
    debug()<<" skipping "<< skip <<" bytes words "<<endreq;
    
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
    debug()<<"start of the bank "<<(*itB)->sourceID()<<endmsg;
   //char* it;
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
    for(int k=0;k<4*skip;k++){      
      it++;
    }
    
    debug()<<" skipping a pad number  of "<<*((*itB)->begin<short>())<<" then skip "
          <<skip<<" 4 byte words "<<endreq;
    

    // how many ODE in this tell1?
    unsigned int nODE=m_ODENumberInTell1[(*itB)->sourceID()];
    debug()<<"tell1 "<<(*itB)->sourceID()<<" number of ODE "<<nODE<<endmsg;    
    std::vector<unsigned int> firedInODE;
    firedInODE.resize(nODE) ;    
    unsigned int itODE=0;
    unsigned int channelsInTell1=0;    
    for(itODE=0; itODE < nODE ; itODE++) {
      //first decode the header
        firedInODE[itODE]=(unsigned int)(*it);
        debug()<<"channels in ODE "<<itODE<<" "<<
          (unsigned int)(*it)<<endmsg;        
        
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

    //first skip the byte required for padding
    //how many? 
    int padding=0;
    if((channelsInTell1+nODE)%4){
      padding=4-(channelsInTell1+nODE)%4;      
    }
    for(int ipad=0;ipad<padding;ipad++){
      it++;     
    }    


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


StatusCode MuonRawBuffer::initializeLUTCrossing()
{
  //skip M1
  int countTell1=m_M1Tell1; 
 
  for(int station=1;station<5;station++){
    debug()<<"station number "<<station<<endreq;    
    std::string cablingBasePath=getBasePath(station);    
    std::string cablingPath=cablingBasePath+"Cabling";
    SmartDataPtr<MuonStationCabling>  cabling(detSvc(), cablingPath);   
    for(int L1Board=0;L1Board<cabling->getNumberOfL1Board();L1Board++){    
      debug()<<"L1 number "<<cabling->getL1Name(0)<<endreq;
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
          debug()<<"trigger sector "<<TSPath<<endreq;
          
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
            MuonTileID muontile(station,lay,region,
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

std::vector<LHCb::MuonTileID> MuonRawBuffer::DoPad(std::vector<
                                                   LHCb::MuonTileID> digit,
                                                   MuonTSMap* TS)
{
  
  std::vector<LHCb::MuonTileID> list_of_pads;
  if(TS->numberOfLayout()==2){
    //how many subsector????
    int NX=TS->gridXLayout(1);    
    int NY=TS->gridYLayout(0);
    int Nsub=NX*NY;

    debug()<<"number NX NY "<<NX<<" "<<NY<<endreq;   
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
      debug()<<" horizontal ? "<<     horizontal<<endreq;      
    }
    // now the address of fired pads..
    for(int i=0;i<Nsub;i++){
      // cross only local to each sub matrix
      std::vector<unsigned int>::iterator itx;
      std::vector<unsigned int>::iterator ity;
      //debug 
      debug()<<" sub matrix "<<i<<endreq;
      debug()<<" horizontal sequence ";
      for(itx=horiz[i].begin();
          itx<horiz[i].end();itx++){
        debug()<<*itx<<" ";
      }
      debug()<<endreq;
      debug()<<" vertical sequence ";     
      for(ity=hvert[i].begin();ity<hvert[i].end();ity++){
        debug()<<*ity<<" ";        
      }
      debug()<<endreq;      
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
            debug()<<" result of the address "<<address<<endreq;   
            LHCb::MuonTileID padTile=tileY.intercept(*itx);            
            list_of_pads[address]=padTile; 
            debug()<<" TS dec "<<address<<" "<<
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
      debug()<<" result of the address "<<address<<endreq; 
    }    
  }
  return list_of_pads;  


}


std::vector<LHCb::MuonTileID> MuonRawBuffer::getPads()
{
  std::vector<MuonTileID> storage;
  
  LHCb::RawEvent* raw = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);  
  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;  
  unsigned int chIterator=0;
  
  for( itB = b.begin(); itB != b.end(); itB++ ) {    
    const RawBank* r = *itB;
    debug()<<"start of the bank "<<(*itB)->sourceID()<<endmsg;   
    const short * it=r->begin<short>();    
    if((unsigned int)(*itB)->sourceID()>=m_M1Tell1){
      unsigned int tell=(unsigned int)(*itB)->sourceID();      
      unsigned int pads=*it;
      it++;      
      for(unsigned int loop=0;loop<pads;loop++){
        unsigned int address=*it;
        storage.push_back(m_mapPad[tell][address]);        
        it++;        
      }      
    }
  }
  return storage;  
}
