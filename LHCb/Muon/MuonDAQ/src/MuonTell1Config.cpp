// Include files 

// local
#include "MuonTell1Config.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTell1Config
//
// 2008-02-29 : 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonTell1Config )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTell1Config::MuonTell1Config( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_muonDet(0)
{
  declareInterface<IMuonTell1Config>(this);

}
//=============================================================================
// Destructor
//=============================================================================
MuonTell1Config::~MuonTell1Config() {}
//=============================================================================
StatusCode MuonTell1Config::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  m_muonDet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDet) return Error("Could not locate Muon detector element");
  
  
  TUName.push_back(std::pair<std::string,long>("TSM2R1Map1",0));
  TUName.push_back(std::pair<std::string,long>("TSM2R1Map2",1));
  TUName.push_back(std::pair<std::string,long>("TSM2R2Map1",2));
  TUName.push_back(std::pair<std::string,long>("TSM2R3Map1",3));
  TUName.push_back(std::pair<std::string,long>("TSM2R4Map1",3));
  TUName.push_back(std::pair<std::string,long>("TSM3R1Map1",0));
  TUName.push_back(std::pair<std::string,long>("TSM3R1Map2",1));
  TUName.push_back(std::pair<std::string,long>("TSM3R2Map1",2));
  TUName.push_back(std::pair<std::string,long>("TSM3R3Map1",3));
  TUName.push_back(std::pair<std::string,long>("TSM3R4Map1",3));
  TUName.push_back(std::pair<std::string,long>("TSM4R1Map1",4));
  TUName.push_back(std::pair<std::string,long>("TSM4R1Map2",5));
  TUName.push_back(std::pair<std::string,long>("TSM4R2Map1",6));
  TUName.push_back(std::pair<std::string,long>("TSM4R3Map1",7));
  TUName.push_back(std::pair<std::string,long>("TSM4R4Map1",7));
  TUName.push_back(std::pair<std::string,long>("TSM5R1Map1",4));
  TUName.push_back(std::pair<std::string,long>("TSM5R1Map2",5));
  TUName.push_back(std::pair<std::string,long>("TSM5R2Map1",6));
  TUName.push_back(std::pair<std::string,long>("TSM5R3Map1",7));
  TUName.push_back(std::pair<std::string,long>("TSM5R4Map1",7));
  

  return StatusCode::SUCCESS ;
}

StatusCode  MuonTell1Config::configTell1(int Tell1){
  StatusCode sc=StatusCode::SUCCESS;
  
  MuonTell1ConfigData data;
  MuonL1Board*   l1=m_muonDet->getDAQInfo()->getL1Board(Tell1);
  long HitInODE=0;  
  long TUType=0;
  long pad_off=0;
  //  info()<<l1<<" l1 "<<endmsg;
  
  for(int link=0;link<24;link++){   
    TUType=0;   
    data.setPadOffset(link,0); 
    data.setHitOffset(link,0);
    data.setPadTU(link,0);
    data.setPadOffset(link,pad_off); 
    data.setHitOffset(link,HitInODE);
    
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug()<<" link connected ?"<<l1->isLinkConnected(link)<<" "<<
        l1->getLinkConnection(link)<<endmsg;
    
    if(l1->isLinkConnected(link)){ 
      unsigned int ode_num=m_muonDet->getDAQInfo()->
        getODENumberInLink(Tell1,link);
      MuonODEBoard*   ode=m_muonDet->getDAQInfo()->getODEBoard(l1,ode_num);
      //unsigned int tsNum=ode->getTSNumber();
      MuonTSMap* ts=m_muonDet->getDAQInfo()->getTSMap(l1, ode,0);
      long padInTS=ts->numberOfPad();      
      long padInODE=padInTS*ode->getTSNumber();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug()<<padInTS<<" "<<padInODE<<endmsg;
      
      pad_off=pad_off+padInODE;
      HitInODE=192+HitInODE;
      std::string TSName=ode->getTSName(0);
      char tsName[20];
      //name=tsName;
      strcpy(tsName, TSName.c_str());
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug()<<" ts name "<<tsName<<" ciao"<<endmsg;
      std::vector<std::pair<std::string,long> >::iterator itName;
      for(itName=TUName.begin();itName<TUName.end();itName++){
        if(TSName==(*itName).first)TUType=(*itName).second;
        
      }

      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" TUTYPE "<<TUType<<endmsg;
      
    }
    data.setPadTU(link,TUType);
  }
  

  info()<<" Printing Tell1 # "<<Tell1<<" configuration \n";
  info()<<data<<endmsg;
  return sc;
  

}

