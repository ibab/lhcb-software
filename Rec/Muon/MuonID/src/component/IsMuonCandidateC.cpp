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
  return IsMuonCandidate(muTrack.lhcbIDs(),muTrack.p());
}

bool IsMuonCandidateC::IsMuonCandidate(const std::vector<LHCb::LHCbID>& LHCbIDs,const double& mom)
{
  std::vector<int> mysts;
  for (const auto& id : LHCbIDs ) {
    if (!id.isMuon()) continue;
    bool ok=stInStations(id.muonID().station(),mysts);
    if (!ok) mysts.push_back(id.muonID().station());
  }

  if (m_ismopt==1) return IsMuonSimple(mysts);
  else if (m_ismopt==2) return IsMuonLoose(mysts,mom);
  else return IsMuon(mysts,mom);

}


// find out if st myst is in input array of stations
bool IsMuonCandidateC::stInStations(const int myst,const std::vector<int>& stations)
{
  return std::find( stations.begin(), stations.end(), myst) != stations.end();
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
bool IsMuonCandidateC::IsMuon(const std::vector<int>& stations,double mom)
{
  if (mom<m_MomRangeIsMuon[0]) return false;
  auto i12 = stInStations(1,stations) && stInStations(2,stations);
  if (mom<m_MomRangeIsMuon[1]) {
    return i12;
  }
  if (mom<m_MomRangeIsMuon[2]) {
    return i12 && (stInStations(3,stations) || stInStations(4,stations));
  }
  return i12 && stInStations(3,stations) && stInStations(4, stations);
}


//common IsMuonLoose requirements from set of stations with hits in FOI
bool IsMuonCandidateC::IsMuonLoose(const std::vector<int>& stations,double mom)
{

  if (mom<m_MomRangeIsMuon[0]) return false;
  if (mom<m_MomRangeIsMuon[1]) {
    return std::count_if( stations.begin(), stations.end(),
                         [&](int i) { return stInStations(i,{1,2,3}); }
                        ) > 1;
  }
  return std::count_if( stations.begin(), stations.end(),
                       [&](int i) { return stInStations(i,{1,2,3,4}); }
                      ) > 2;
}

//=============================================================================
// Destructor
//=============================================================================
IsMuonCandidateC::~IsMuonCandidateC() = default;

//=============================================================================

