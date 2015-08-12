// Include files 

// local
#include "DistMuIDTool.h"
//boost
#include <boost/assign/list_of.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : DistMuIDTool
//
// 2009-03-12 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DistMuIDTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DistMuIDTool::DistMuIDTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  :  Chi2MuIDTool( type, name , parent )
{
  declareProperty( "ApplyIsmuonHits", m_applyIsmuonHits=false );
}


StatusCode DistMuIDTool::muonQuality(LHCb::Track& muTrack, double& Quality)
{
  
  StatusCode sc;
  if (muTrack.states().size()>1) warning()<<"MUTRACK WITH MORE THAN ONE SEED STATE ON IT"<<endmsg;
  else if (muTrack.states().size()<1) 
  {
    sc.setCode(301);
    Quality=0;
    return Error("MUTRACK WITHOUT ANY SEED STATE ON IT",sc);
  }
  
  if (muTrack.lhcbIDs().size()<1) 
  {
    sc.setCode(302);
    Quality=0;
    return Error("NO LHCbIDs ON TRACK. IMPOSSIBLE TO CALCULATE QUALITY",sc);
  }

  double p = muTrack.ancestors().at(0).target()->p();
  std::vector<int> sts_used  = boost::assign::list_of(1)(1)(1)(1);
  if (m_applyIsmuonHits){  
    if(p>m_PreSelMomentum && p<m_MomentumCuts[0]){
      //M2 &&M3 && M4
      sts_used[3] = 0;
    }
  }
  if (msgLevel(MSG::DEBUG) ) debug()<< "applyIsMuonHits"<< m_applyIsmuonHits 
                                    <<", sts_used="<<sts_used<<endmsg;

  
  sc=computeDistance(muTrack,Quality,sts_used);
  if (sc.isFailure()) 
  {
    Quality=0;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}


StatusCode DistMuIDTool::computeDistance(const LHCb::Track& muTrack, double& dist, const std::vector<int>& sts_used)
{
  
  StatusCode sc;
  if (msgLevel(MSG::DEBUG) ) debug()<<"z_stations="<<m_zstations<<endmsg;
 
  sc=makeStates(muTrack);
  if (sc.isFailure()) 
  {
    sc.setCode(303);
    return sc;
  }
  
  if (msgLevel(MSG::DEBUG) ) debug()<<"states ready"<<endmsg;
  MeasPair mym;
  std::vector<LHCb::LHCbID> lhcbIDs=muTrack.lhcbIDs();
  //initialize distance^2
  double dist2=0.0;
  unsigned int nsts=0;
  //for each lhcbid in new track, add up distance^2
  if (msgLevel(MSG::DEBUG) ) debug()<<"for each lhcbid in new track, add up distance^2"<<endmsg;
  for (std::vector<LHCb::LHCbID>::const_iterator it = lhcbIDs.begin();
       it != lhcbIDs.end(); ++it) {
    const LHCb::LHCbID& id = *it;
    if (!id.isMuon()) continue;
    //find correspondent measpair given lhcbid
    mym.first  = m_measProvider->measurement(id,0);
    mym.second = m_measProvider->measurement(id,1);
    if (msgLevel(MSG::DEBUG) ) {
      debug()<<"mym.first="<<mym.first<<endmsg;
      debug()<<"mym.second="<<mym.second<<endmsg;
    }
    
    int ist=id.muonID().station();
    if (sts_used[ist-1]==0) continue;
    
    if (msgLevel(MSG::DEBUG) ) debug()<<"Number of station is="<<ist<<endmsg;
    //calculate geom distances and pads
    double dist_parx2=pow(m_muonProvider->distx(mym, m_states[ist]),2);
    double dist_pary2=pow(m_muonProvider->disty(mym, m_states[ist]),2);
    if (msgLevel(MSG::DEBUG) ) {
      debug()<<"dist_parx2="<<dist_parx2<<endmsg;
      debug()<<"dist_pary2="<<dist_pary2<<endmsg;
    }
    double padx2=12*pow(mym.first->errMeasure(),2);
    double pady2=12*pow(mym.second->errMeasure(),2);
    if (msgLevel(MSG::DEBUG) ) {
      debug()<<"padx2="<<padx2<<endmsg;
      debug()<<"pady2="<<pady2<<endmsg;
    }
    //add x and y contributions
    dist2+=dist_parx2/padx2;
    dist2+=dist_pary2/pady2;
    if (msgLevel(MSG::DEBUG) ) debug()<<"dist2="<<dist2<<endmsg;
    nsts+=1;
  }

  delete mym.first;
  delete mym.second;
  
  //return distance divided by number of stations added
  if( 0 < nsts ) {
    dist= dist2/nsts;
  }
  else {
    dist = 0.;
  }
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Destructor
//=============================================================================
DistMuIDTool::~DistMuIDTool() {} 

//=============================================================================
