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

namespace {
 static const  std::map<std::string,long> TUName {
       { { "TSM2R1Map1",0 },
         { "TSM2R1Map2",1 },
         { "TSM2R2Map1",2 },
         { "TSM2R3Map1",3 },
         { "TSM2R4Map1",3 },
         { "TSM3R1Map1",0 },
         { "TSM3R1Map2",1 },
         { "TSM3R2Map1",2 },
         { "TSM3R3Map1",3 },
         { "TSM3R4Map1",3 },
         { "TSM4R1Map1",4 },
         { "TSM4R1Map2",5 },
         { "TSM4R2Map1",6 },
         { "TSM4R3Map1",7 },
         { "TSM4R4Map1",7 },
         { "TSM5R1Map1",4 },
         { "TSM5R1Map2",5 },
         { "TSM5R2Map1",6 },
         { "TSM5R3Map1",7 },
         { "TSM5R4Map1",7 } } };
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTell1Config::MuonTell1Config( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareInterface<IMuonTell1Config>(this);
}
//=============================================================================
StatusCode MuonTell1Config::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  m_muonDet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDet) return Error("Could not locate Muon detector element");




  return StatusCode::SUCCESS ;
}

StatusCode  MuonTell1Config::configTell1(int Tell1) const{
  StatusCode sc=StatusCode::SUCCESS;

  MuonTell1ConfigData data;
  MuonL1Board* l1=m_muonDet->getDAQInfo()->getL1Board(Tell1);
  long HitInODE=0;
  long pad_off=0;
  //  info()<<l1<<" l1 "<<endmsg;

  for(int link=0;link<24;link++){
    long TUType=0;
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
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug()<<" ts name "<<TSName<<" ciao"<<endmsg;
      auto itName = TUName.find(TSName);
      if (itName!=TUName.end()) TUType=itName->second;

      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" TUTYPE "<<TUType<<endmsg;

    }
    data.setPadTU(link,TUType);
  }


  info()<<" Printing Tell1 # "<<Tell1<<" configuration \n";
  info()<<data<<endmsg;
  return sc;
}

