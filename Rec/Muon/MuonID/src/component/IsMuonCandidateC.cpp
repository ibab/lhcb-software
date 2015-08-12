// Include files 

// local
#include "IsMuonCandidateC.h"


//boost
#include <boost/assign/list_of.hpp>


//-----------------------------------------------------------------------------
// Implementation file for class : IsMuonCandidateC
//
// 2009-03-12 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( IsMuonCandidateC )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
IsMuonCandidateC::IsMuonCandidateC( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IIsMuonCandidateC>(this);
  
  
  //Mom Range for IsMuon
  std::vector<double> tmp = boost::assign::list_of(3000.)(6000.)(10000.);
  declareProperty("MomRangeIsMuon", m_MomRangeIsMuon = tmp);
  
  //IsMuon used: 
  //- 1 IsMuonCandidate
  //- 2 IsMuonLoose
  //- 3 IsMuon
  declareProperty("IsMuonOpt",m_ismopt = 1);
  
  // Min number of hits for IsMuonSimple
  declareProperty("MinHits",m_minhits = 2);
  
}

StatusCode IsMuonCandidateC::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::DEBUG) ) debug()<<"MomRangeIsMuon="<<m_MomRangeIsMuon<<endmsg;
  return StatusCode::SUCCESS;
}

bool IsMuonCandidateC::IsMuonCandidate(const LHCb::Track& muTrack)
{
  const std::vector<LHCb::LHCbID>& ids=muTrack.lhcbIDs();
  const double& mom=muTrack.p();
  return IsMuonCandidate(ids,mom);
}

bool IsMuonCandidateC::IsMuonCandidate(const std::vector<LHCb::LHCbID>& LHCbIDs,const double& mom)
{
  std::vector<int> mysts;
  for (std::vector<LHCb::LHCbID>::const_iterator it = LHCbIDs.begin();
       it != LHCbIDs.end(); ++it) 
  {
    if (!it->isMuon()) continue;
    bool ok=stInStations(it->muonID().station(),mysts);
    if (!ok) mysts.push_back(it->muonID().station());
  }
  
  if (m_ismopt==1) return IsMuonSimple(mysts);
  else if (m_ismopt==2) return IsMuonLoose(mysts,mom);
  else return IsMuon(mysts,mom);
  
}


// find out if st myst is in input array of stations
bool IsMuonCandidateC::stInStations(const int myst,const std::vector<int>& stations)
{
  
  for (std::vector<int>::const_iterator it = stations.begin();
       it != stations.end(); ++it) 
  {
    int ist = *it;
    if (ist==myst) return true;
  }
  return false;
}


// isMuonJX: are there at least minhits in FOI?
bool IsMuonCandidateC::IsMuonSimple(const std::vector<int>& stations)
{
  
  int musize=stations.size();
  if (musize < m_minhits) 
  {  
    if (msgLevel(MSG::DEBUG) ) debug()<<"size of mutrack is "<<musize<<endmsg;
    if (msgLevel(MSG::DEBUG) ) debug()<<"m_minhits is "<<m_minhits<<endmsg;
    return false;
  }
  return true;
}

//common IsMuon requirements from set of stations with hits in FOI 
bool IsMuonCandidateC::IsMuon(const std::vector<int>& stations,const double& p)
{
  const double pr1=m_MomRangeIsMuon[0];
  const double pr2=m_MomRangeIsMuon[1];
  const double pr3=m_MomRangeIsMuon[2];

  //double mom=p/Gaudi::Units::GeV;
  double mom=p;
  if (mom>pr1 && mom<pr2)
  {
    if (stInStations(1,stations) && stInStations(2,stations)) return true;
  }
  
  else if (mom>pr2 && mom<pr3)
  {
    if (stInStations(1,stations) && stInStations(2,stations) && 
        (stInStations(3,stations) || stInStations(4,stations))) return true;
  }
  else if (mom>pr3)
  {   
    if (stInStations(1,stations) && stInStations(2,stations) 
        && stInStations(3,stations) && stInStations(4, stations)) return true;
  }         
  
  return false;
}


//common IsMuonLoose requirements from set of stations with hits in FOI  
bool IsMuonCandidateC::IsMuonLoose(const std::vector<int>& stations,const double& p)
{
  //double mom=p/Gaudi::Units::GeV;
  double mom=p;
  std::vector<int> vstations_rel1=boost::assign::list_of(1)(2)(3);
  std::vector<int> vstations_rel2=boost::assign::list_of(1)(2)(3)(4);
  
  const double pr1=m_MomRangeIsMuon[0];
  const double pr2=m_MomRangeIsMuon[1];
  //  const double pr3=m_MomRangeIsMuon[2];
  
  if (mom>pr1 && mom<pr2)
  {
    int j=0;
    for (std::vector<int>::const_iterator it = stations.begin();
         it != stations.end(); ++it) 
    {
      int ist = *it;
      if (stInStations(ist,vstations_rel1)) j+=1;
    }
    if (j>1) return true;
  }
  
  else if (mom>pr2)
  {
    int j=0;
    for (std::vector<int>::const_iterator it = stations.begin();
         it != stations.end(); ++it) 
    {
      int ist = *it;
      if (stInStations(ist,vstations_rel2)) j+=1;
    }
    if (j>2) return true;
  }
  return false;
}






//=============================================================================
// Destructor
//=============================================================================
IsMuonCandidateC::~IsMuonCandidateC() {} 

//=============================================================================

