// Include files 

// local
#include "L0MuonInfoHistos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonInfoHistos
//
// 2008-07-17 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonInfoHistos )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonInfoHistos::L0MuonInfoHistos( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{ 
  declareInterface<L0MuonInfoHistos>(this);
 
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonInfoHistos::~L0MuonInfoHistos() {} 

//=============================================================================

void L0MuonInfoHistos::bookHistos(bool shortname)
{
  std::string hname;
  std::string toolname="";
  if (!shortname) toolname=name();

  hname  = L0Muon::MonUtilities::hname_info_bid(toolname);
  m_hbid = book1D(hname,hname,-0.5,-0.5+3564,3564);
  hname  = L0Muon::MonUtilities::hname_info_bid2(toolname);
  m_hbid2= book1D(hname,hname,-0.5,-0.5+3564,89);
  hname  = L0Muon::MonUtilities::hname_info_evt(toolname);
  m_hevt = book1D(hname,hname,-0.5,0.5+4095,4096);
  hname  = L0Muon::MonUtilities::hname_info_error(toolname);
  m_herror = book1D(hname,hname,0.5,0.5+4,4);
} 

void L0MuonInfoHistos::fillHistos()
{
  fill(m_hbid,m_bunchId,1);
  fill(m_hbid2,m_bunchId,1);
  fill(m_hevt,m_evtNum,1);
  int error = 0;
  if (m_error)  error|=0x1;
  error|=((m_errStatus<<1)&0x6);
  if (m_ovfStatus) error|=0x8;
  for (int i=0;i<4;++i) {
    if ( (error>>i)&1 ) fill(m_herror,(i+1),1);
  }
  
}

StatusCode L0MuonInfoHistos::getInfo(){

  if (!exist<LHCb::ODIN> (LHCb::ODINLocation::Default))
    return Error("ODIN not found",StatusCode::FAILURE,50);

  LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

  if (!exist<LHCb::L0MuonInfo> ( LHCb::L0MuonInfoLocation::Default + context() ) )
    return Error("L0MuonInfo not found",StatusCode::FAILURE,50);
  
  LHCb::L0MuonInfo * info = get<LHCb::L0MuonInfo> ( LHCb::L0MuonInfoLocation::Default + context() );

  m_bunchId=info->L0_B_Id();
  m_evtNum=info->L0EventNumber();
  m_errStatus=(info->status()>>2)&0x3;
  m_ovfStatus=(info->status()>>0)&0x3;

  int event = int(odin->eventNumber()&0xFFF) ;
  int bunch = odin->bunchId();
  
  m_error=false;
  if ( (event!=m_evtNum) || (bunch!=m_bunchId) ) m_error=true;

  if (msgLevel(MSG::DEBUG) && m_error) {
    debug() <<"Evt id inconsistency "
            <<" event : "<<event<<" -VS- "<<m_evtNum
            <<" bunch : "<<bunch<<" -VS- "<<m_bunchId
            <<endmsg;
  }
  if (msgLevel(MSG::VERBOSE) ) {
    verbose() <<"Odin -VS- L0Muon "
            <<" event : "<<event<<" -VS- "<<m_evtNum
            <<" - bunch : "<<bunch<<" -VS- "<<m_bunchId
            <<endmsg;
  }
  

  return StatusCode::SUCCESS;
  
}

