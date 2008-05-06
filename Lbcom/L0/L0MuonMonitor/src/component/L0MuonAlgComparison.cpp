// $Id: L0MuonAlgComparison.cpp,v 1.2 2008-05-06 12:19:27 jucogan Exp $
// Include files 

#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/RawEvent.h"
#include "Event/ODIN.h"

// local
#include "L0MuonMonUtils.h"
#include "L0MuonAlgComparison.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonAlgComparison
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonAlgComparison );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonAlgComparison::L0MuonAlgComparison( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
{
  declareProperty("Extension0"     , m_extension_0 = ""  );
  declareProperty("Extension1"     , m_extension_1 = ""  );

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonAlgComparison::~L0MuonAlgComparison() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonAlgComparison::initialize() {
  StatusCode sc = L0MuonMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;
 
  for (int i=0; i<NCounters; ++i) m_counters[i]=0;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonAlgComparison::execute() {
  
  debug() << "==> Execute" << endmsg;
  StatusCode sc;

  ++m_counters[TOTAL];
  if (excludedBx()) return StatusCode::SUCCESS;
  if (!selectedTrigger()) return StatusCode::SUCCESS;
  ++m_counters[PROCESSED];

  info()<<" Event # : "<<m_counters[TOTAL] << " Processed event #: "<<m_counters[PROCESSED] << endmsg;
  
  sc = compare(LHCb::L0MuonCandidateLocation::Default);
  
  if( msgLevel(MSG::INFO) && sc==StatusCode::FAILURE) {
    info()<<"*** Dump Candidates **** TIME SLOT=0"<<endmsg;
    info()<<"-- Ctrl :"<<endmsg;
    printCand(LHCb::L0MuonCandidateLocation::Default+ m_extension_0);
    printCand(LHCb::L0MuonCandidateLocation::Default+ m_extension_1);
    debug()<<"-- BCSU :"<<endmsg;
    printCand(LHCb::L0MuonCandidateLocation::BCSU+ m_extension_0);
    printCand(LHCb::L0MuonCandidateLocation::BCSU+ m_extension_1);
    debug()<<"-- PU :"<<endmsg;
    printCand(LHCb::L0MuonCandidateLocation::PU+ m_extension_0);
    printCand(LHCb::L0MuonCandidateLocation::PU+ m_extension_1);

//     info()<<"*** Dump Candidates **** TIME SLOT=-1"<<endmsg;
//     setProperty("RootInTes",L0MuonMonUtils::timeSlot(-1));
//     info()<<"-- Ctrl :"<<endmsg;
//     printCand(LHCb::L0MuonCandidateLocation::Default+ m_extension_0);
//     printCand(LHCb::L0MuonCandidateLocation::Default+ m_extension_1);
//     debug()<<"-- BCSU :"<<endmsg;
//     printCand(LHCb::L0MuonCandidateLocation::BCSU+ m_extension_0);
//     printCand(LHCb::L0MuonCandidateLocation::BCSU+ m_extension_1);
//     debug()<<"-- PU :"<<endmsg;
//     printCand(LHCb::L0MuonCandidateLocation::PU+ m_extension_0);
//     printCand(LHCb::L0MuonCandidateLocation::PU+ m_extension_1);
//     setProperty("RootInTes",L0MuonMonUtils::timeSlot(0));

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonAlgComparison::finalize() {

  debug() << "==> Finalize" << endmsg;

  info()<<"Number of events processed :  "<<m_counters[PROCESSED]    << endmsg;
  info()<<"Number of candidates      found by the emulator "<<m_counters[FOUND]    << endmsg;
  info()<<"Number of candidates NOT  found by the emulator "<<m_counters[NOT_FOUND]<< endmsg;
  info()<<"Number of candidates ONLY found by the emulator "<<m_counters[ADDITIONAL]<< endmsg;

  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================

StatusCode L0MuonAlgComparison::compare(std::string location)
{
  StatusCode sc = StatusCode::SUCCESS;
  std::string location0 = location + m_extension_0;
  if (!exist<LHCb::L0MuonCandidates>( location0 )) return StatusCode::FAILURE;  
  LHCb::L0MuonCandidates* original0 = get<LHCb::L0MuonCandidates>( location0 );
  LHCb::L0MuonCandidates* cands0 = filterCand(original0);

  std::string location1 = location + m_extension_1;
  if (!exist<LHCb::L0MuonCandidates>( location1 )) return StatusCode::FAILURE;
  LHCb::L0MuonCandidates* original1 = get<LHCb::L0MuonCandidates>( location1 );
  LHCb::L0MuonCandidates* cands1 = filterCand(original1);

  LHCb::L0MuonCandidates::const_iterator it0;
  LHCb::L0MuonCandidates::const_iterator it1;
  
  for ( it0= cands0->begin(); it0<cands0->end();++it0) {
    bool found=false;
    for ( it1= cands1->begin(); it1<cands1->end();++it1) {
      if (**it1==**it0) {
        found=true;
        break;
      } 
    }
    if (!found) {
      ++m_counters[NOT_FOUND];
      sc = StatusCode::FAILURE;
      info()<<"Candidate not found in 2nd processing ... evt # "<<m_counters[TOTAL]<<endmsg;
    } else {
      ++m_counters[FOUND];
      debug()<<"Candidate found !!! "<<endmsg;
    }
  }

  for ( it1= cands1->begin(); it1<cands1->end();++it1) {
    bool found=false;
    for ( it0= cands0->begin(); it0<cands0->end();++it0) {
      if (**it1==**it0) {
        found=true;
        break;
      } 
    }
    if (!found) {
      ++m_counters[ADDITIONAL];
      sc = StatusCode::FAILURE;
      info()<<"Candidate not found in 1st processing ... evt # "<<m_counters[TOTAL]<<endmsg;
    } else {
      debug()<<"Candidate found !!! "<<endmsg;
    }
  }
  
  return sc;
}

LHCb::L0MuonCandidates* L0MuonAlgComparison::filterCand(LHCb::L0MuonCandidates* originals)
{
  LHCb::L0MuonCandidates* filtered = new LHCb::L0MuonCandidates();
  for (LHCb::L0MuonCandidates::const_iterator it=originals->begin(); it<originals->end();++it) {  
    std::vector<LHCb::MuonTileID> mids=(*it)->muonTileIDs(2);
    int qua=mids[0].quarter();
    if (quarterInUse(qua)) filtered->insert(*it);
  }
    
  return filtered;
}

void L0MuonAlgComparison::printCand(std::string location){
  if (!exist<LHCb::L0MuonCandidates>( location )) return;
  LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );
  printCand(cands,location);
}

void L0MuonAlgComparison::printCand(LHCb::L0MuonCandidates* cands, std::string tab)
{
  LHCb::L0MuonCandidates* filtered = filterCand(cands);

  std::cout<<boost::format("%-30s %3d candidates found  ") % tab % filtered->size();
  std::cout<<std::endl;

  std::cout<<boost::format("%-30s PT  ") % tab;
  for (LHCb::L0MuonCandidates::const_iterator it=filtered->begin(); it<filtered->end();++it) {      
    std::cout<<boost::format("%21d - ") %  ((*it)->encodedPt());
  }
  std::cout<<std::endl;

  for (int sta=0; sta<3; ++sta){
    
    unsigned int max=1;
    std::cout<<boost::format("%-30s M%1d  ") % tab % (sta+1);
    for (LHCb::L0MuonCandidates::const_iterator it=filtered->begin(); it<filtered->end();++it) {      
      std::vector<LHCb::MuonTileID> mids=(*it)->muonTileIDs(sta);
      if (mids.size()==0) {
        std::cout<<boost::format("????????????????????? - ");
        continue;
      }
      max = mids.size()>max ? mids.size() : max;
      printMuonTile(mids[0]);
    }
    std::cout<<std::endl;
    for (unsigned int i=1;i<max;++i){
      std::cout<<boost::format("%-30s     ") % tab;
      for (LHCb::L0MuonCandidates::const_iterator it=filtered->begin(); it<filtered->end();++it) {      
        std::vector<LHCb::MuonTileID> mids=(*it)->muonTileIDs(sta);
        if (mids.size()>i){   
          printMuonTile(mids[i]);
        }
        else
          std::cout<<boost::format("%21s - ") % "";
      }
      std::cout<<std::endl;
    }
  }
  delete filtered;
}


void L0MuonAlgComparison::printMuonTile(LHCb::MuonTileID mid)
{
  int pb = 3*mid.region()+int(mid.nX()/(mid.layout().xGrid()))+((int(mid.nY()/(mid.layout().yGrid()))<<1)&2)-1;
  int pu = int(mid.nX()/(mid.layout().xGrid()/2))%2+(((int(mid.nY()/(mid.layout().yGrid()/2))%2)<<1)&2);
  std::cout<<boost::format("Q%1d R%1d PB%2d PU%1d %2d,%2d  - ") 
    % (mid.quarter()+1)
    % (mid.region() +1)
    % pb
    % pu
    % (mid.nX())
    % (mid.nY());
}

