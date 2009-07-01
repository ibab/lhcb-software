// $Id: DistMuIDTool.cpp,v 1.1 2009-07-01 18:27:11 polye Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "DistMuIDTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DistMuIDTool
//
// 2009-03-12 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DistMuIDTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DistMuIDTool::DistMuIDTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  :  Chi2MuIDTool( type, name , parent )
{}

StatusCode DistMuIDTool::muonQuality(LHCb::Track& muTrack, double& Quality)
{
  
  StatusCode sc;


  if (muTrack.states().size()>1) warning()<<"MUTRACK WITH MORE THAN ONE SEED STATE ON IT"<<endreq;
  else if (muTrack.states().size()<1) 
  {
    error()<<"MUTRACK WITHOUT ANY SEED STATE ON IT"<<endreq;
    sc.setCode(301);
    Quality=0;
    return sc;
  }


  if (muTrack.lhcbIDs().size()<1) 
  {
    error()<<"NO LHCbIDs ON TRACK. IMPOSSIBLE TO CALCULATE QUALITY"<<endreq;
    sc.setCode(302);
    Quality=0;
    return sc;
  }
  
  sc=computeDistance(muTrack,Quality);
  if (sc.isFailure()) 
  {
    Quality=0;
    return sc;
  }
  
  return StatusCode::SUCCESS;
  

}





StatusCode DistMuIDTool::computeDistance(const LHCb::Track& muTrack, double& dist)
{
  
  StatusCode sc;
  debug()<<"z_stations="<<m_zstations<<endreq;
      
  sc=makeStates(muTrack);
  if (sc.isFailure()) 
  {
    sc.setCode(303);
    return sc;
  }

  debug()<<"states ready"<<endreq;
  MeasPair mym;
  std::vector<LHCb::LHCbID> lhcbIDs=muTrack.lhcbIDs();
  //initialize distance^2
  double dist2=0.0;
  int nsts=0;
  //for each lhcbid in new track, add up distance^2
  debug()<<"for each lhcbid in new track, add up distance^2"<<endreq;
  for (std::vector<LHCb::LHCbID>::const_iterator it = lhcbIDs.begin();
       it != lhcbIDs.end(); ++it) {
    const LHCb::LHCbID& id = *it;
    if (not id.isMuon()) continue;
    //find correspondent measpair given lhcbid
    mym.first  = m_measProvider->measurement(id,0);
    mym.second = m_measProvider->measurement(id,1);
    debug()<<"mym.first="<<mym.first<<endreq;
    debug()<<"mym.second="<<mym.second<<endreq;

    int ist=id.muonID().station();
    debug()<<"NUMBER OF STATION IS="<<ist<<endreq;
    //calculate geom distances and pads
    double dist_parx2=pow(m_muonProvider->distx(mym, m_states[ist]),2);
    double dist_pary2=pow(m_muonProvider->disty(mym, m_states[ist]),2);
    debug()<<"dist_parx2="<<dist_parx2<<endreq;
    debug()<<"dist_pary2="<<dist_pary2<<endreq;
    double padx2=12*pow(mym.first->errMeasure(),2);
    double pady2=12*pow(mym.second->errMeasure(),2);
    debug()<<"padx2="<<padx2<<endreq;
    debug()<<"pady2="<<pady2<<endreq;
    //add x and y contributions
    dist2+=dist_parx2/padx2;
    dist2+=dist_pary2/pady2;
    debug()<<"dist2="<<dist2<<endreq;
    nsts+=1;
  }

  delete mym.first;
  delete mym.second;
  
  //return distance divided by number of stations added
  dist= dist2/nsts;
  return StatusCode::SUCCESS;
}





//=============================================================================
// Destructor
//=============================================================================
DistMuIDTool::~DistMuIDTool() {} 

//=============================================================================
