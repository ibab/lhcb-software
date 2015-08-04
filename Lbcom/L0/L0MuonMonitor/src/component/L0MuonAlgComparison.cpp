// Include files 

#include "boost/format.hpp"

#include "Kernel/MuonLayout.h"

#include "Event/RawEvent.h"
#include "Event/ODIN.h"

// local
#include "L0MuonAlgComparison.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonAlgComparison
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonAlgComparison )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonAlgComparison::L0MuonAlgComparison( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
  , m_candHistosPU0(0)
  , m_candHistosPU1(0)
{
  declareProperty("Extension0"     , m_extension_0 = ""  );
  declareProperty("Extension1"     , m_extension_1 = ""  );
  m_diff = false;
  for (int i=0; i<NCounters; ++i) m_counters[i]=0;
  
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

  m_h_online = book1D("Hard_Vs_Soft",-0.5,2.5,3);
  
  if (!m_online) {
    m_candHistosPU0    = tool<L0MuonCandHistos>( "L0MuonCandHistos", "PUCand_only0", this);
    m_candHistosPU1    = tool<L0MuonCandHistos>( "L0MuonCandHistos", "PUCand_only1", this);

    if (!m_shortnames) m_candHistosPU0->setHistoDir("L0MuonComparison");
    m_candHistosPU0->bookHistos(384,m_shortnames);
    if (!m_shortnames) m_candHistosPU1->setHistoDir("L0MuonComparison");
    m_candHistosPU1->bookHistos(384,m_shortnames);
 
    for (int i=0; i<NCounters; ++i) m_counters[i]=0;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonAlgComparison::execute() {
  
  if (m_rnd()>m_prescale) return StatusCode::SUCCESS;

  debug() << "==> Execute" << endmsg;
  StatusCode sc;
  m_diff=false;
  
  ++m_counters[TOTAL];
  if (excludedBx()) return StatusCode::SUCCESS;
  if (!selectedTrigger()) return StatusCode::SUCCESS;
  ++m_counters[PROCESSED];

  debug()<<" Event # : "<<m_counters[TOTAL] << " Processed event #: "<<m_counters[PROCESSED] << endmsg;
  
  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){
    setProperty("RootInTes",L0Muon::MonUtilities::timeSlot(*it_ts));
    bool histo= (!m_online);
    
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    
    ++m_counters[BUNCHES];
    
    debug()<<"Verifying -- Ctrl :"<<endmsg;
    sc = compare(LHCb::L0MuonCandidateLocation::Default);
    if(sc==StatusCode::FAILURE) {
      return Error("Failed to compare candidates at "+LHCb::L0MuonCandidateLocation::Default+" ... abort"
                   ,StatusCode::SUCCESS,100);
    }
    
    //   debug()<<"Verifying -- BCSU :"<<endmsg;
    //   sc = compare(LHCb::L0MuonCandidateLocation::BCSU+"fromPB");
    //   if(sc==StatusCode::FAILURE) {
    //     return Error("Failed to compare candidates at "+LHCb::L0MuonCandidateLocation::BCSU+"fromPB"+" ... abort"
    //                  ,StatusCode::SUCCESS,100);
    //   }
    
    debug()<<"Verifying -- PU :"<<endmsg;
    sc = compare(LHCb::L0MuonCandidateLocation::PU,histo);
    if(sc==StatusCode::FAILURE) {
      return Error("Failed to compare candidates at "+LHCb::L0MuonCandidateLocation::PU+" ... abort"
                   ,StatusCode::SUCCESS,100);
    }

  } // End of loop over time slots
  
  fill(m_h_online,0.,1.);
  if (m_diff) fill(m_h_online,2.,1.);
  else fill(m_h_online,1.,1.);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonAlgComparison::finalize() {

  debug() << "==> Finalize" << endmsg;

  info()<<"Number of events processed  :  "<<m_counters[PROCESSED]    << endmsg;
  //   info()<<"Number of bunches processed :  "<<m_counters[BUNCHES]    << endmsg;
  info()<<"Number of candidates      found by the emulator "<<m_counters[FOUND]    << endmsg;
  info()<<"Number of candidates NOT  found by the emulator "<<m_counters[NOT_FOUND]<< endmsg;
  info()<<"Number of candidates ONLY found by the emulator "<<m_counters[ADDITIONAL]<< endmsg;

  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================

StatusCode L0MuonAlgComparison::compare(std::string location, bool histo, int ts)
{
  StatusCode sc = StatusCode::SUCCESS;
  std::string location0 = location + m_extension_0;
  if (!exist<LHCb::L0MuonCandidates>( location0 )) 
    return Error("L0MuonCandidates not found at "+location0,StatusCode::FAILURE,100);  
  LHCb::L0MuonCandidates* original0 = get<LHCb::L0MuonCandidates>( location0 );
  LHCb::L0MuonCandidates* cands0 = new LHCb::L0MuonCandidates();
  filterCand(original0,cands0);

  std::string location1 = location + m_extension_1;
  if (!exist<LHCb::L0MuonCandidates>( location1 ))  
    return Error("L0MuonCandidates not found at "+location1,StatusCode::FAILURE,100);  
  LHCb::L0MuonCandidates* original1 = get<LHCb::L0MuonCandidates>( location1 );
  LHCb::L0MuonCandidates* cands1 = new LHCb::L0MuonCandidates();
  filterCand(original1,cands1);

  LHCb::L0MuonCandidates::const_iterator it0;
  LHCb::L0MuonCandidates::const_iterator it1;

  debug()<<"Nb of candidates : 0= "<<cands0->size()<<"  - 1= "<<cands1->size()<<endmsg;
  
  LHCb::L0MuonCandidates* cands_only0 = new LHCb::L0MuonCandidates();
  for ( it0= cands0->begin(); it0<cands0->end();++it0) {
    bool found=false;
    for ( it1= cands1->begin(); it1<cands1->end();++it1) {
      if (**it1==**it0) {
        found=true;
        break;
      } 
    }
    if (!found) {
      m_diff=true;
      cands_only0->add((*it0));
      ++m_counters[NOT_FOUND];
      sc = StatusCode::FAILURE;
      LHCb::MuonTileID mid= ((*it0)->muonTileIDs(2))[0];
      if (mid.isValid()) {
        LHCb::MuonTileID pu = MuonLayout(2,2).contains(mid);
        int qua = pu.quarter();
        int reg = pu.region();
        int iboard =  ( ((pu.nY()/2)*2) + (pu.nX()/2) ) -1 + (reg*3);
        int ipu = ((pu.nY()%2)*2) +  (pu.nX()%2);
        info()<<"Candidate not found in 2nd processing ... evt # "<<m_counters[TOTAL]
              <<" Q"<<(qua+1)<< " R"<<(reg+1)<<" PB"<<iboard<<" PU"<<ipu<<endmsg;
      } else {
        info()<<"Candidate not found in 2nd processing ... evt # "<<m_counters[TOTAL]
              <<" mid M3 not valid => "<<(*it0)<<endmsg;
      }
    } else {
      ++m_counters[FOUND];
      debug()<<"Candidate found !!! "<<endmsg;
    }
  }
  if (histo) m_candHistosPU0->fillHistos(cands_only0,ts);
  if (histo) m_candHistosPU0->fillHistos(cands_only0->size(),ts);
  delete cands_only0;
  
  LHCb::L0MuonCandidates* cands_only1 = new LHCb::L0MuonCandidates();
  for ( it1= cands1->begin(); it1<cands1->end();++it1) {
    bool found=false;
    for ( it0= cands0->begin(); it0<cands0->end();++it0) {
      if (**it1==**it0) {
        found=true;
        break;
      } 
    }
    if (!found) {
      m_diff=true;
      cands_only1->add((*it1));
      ++m_counters[ADDITIONAL];
      sc = StatusCode::FAILURE;
      LHCb::MuonTileID mid= ((*it1)->muonTileIDs(2))[0];
      if (mid.isValid()) {
        LHCb::MuonTileID pu = MuonLayout(2,2).contains(mid);
        int qua = pu.quarter();
        int reg = pu.region();
        int iboard =  ( ((pu.nY()/2)*2) + (pu.nX()/2) ) -1 + (reg*3);
        int ipu = ((pu.nY()%2)*2) +  (pu.nX()%2);
        info()<<"Candidate not found in 1st processing ... evt # "<<m_counters[TOTAL]
              <<" Q"<<(qua+1)<< " R"<<(reg+1)<<" PB"<<iboard<<" PU"<<ipu<<endmsg;
      }
      else {
        info()<<"Candidate not found in 2nd processing ... evt # "<<m_counters[TOTAL]
              <<" mid M3 not valid => "<<(*it1)<<endmsg;
        
      }
    } else {
      debug()<<"Candidate found !!! "<<endmsg;
    }
  }
  if (histo) {
    m_candHistosPU1->fillHistos(cands_only1,ts);
    m_candHistosPU1->fillHistos(cands_only1->size(),ts);
  }
  
  delete cands_only1;
  
  delete cands0;
  delete cands1;

  if( (msgLevel(MSG::INFO) && sc==StatusCode::FAILURE) || msgLevel(MSG::DEBUG) ) {
    printCand(location0);
    printCand(location1);
  }
  return StatusCode::SUCCESS;
}

void L0MuonAlgComparison::filterCand(LHCb::L0MuonCandidates* originals, LHCb::L0MuonCandidates* filtered)
{
  
  filtered->clear();
  for (LHCb::L0MuonCandidates::const_iterator it=originals->begin(); it<originals->end();++it) {  
    std::vector<LHCb::MuonTileID> mids=(*it)->muonTileIDs(2);
    int qua=mids[0].quarter();
    if (!quarterInUse(qua)) continue;
    int reg=mids[0].region();
    if (!regionInUse(reg)) continue;
    filtered->insert(*it);
    
  }
    
}

void L0MuonAlgComparison::printCand(std::string location){

  if (!exist<LHCb::L0MuonCandidates>( location )) return;
  LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>( location );

  LHCb::L0MuonCandidates* filtered = new LHCb::L0MuonCandidates();
  filterCand(cands,filtered);

  std::cout<<boost::format("%-30s %3d candidates found  ") % location % filtered->size();
  std::cout<<std::endl;
  if (filtered->size()>8) {
    for (std::vector<int>::iterator iq=m_quarters.begin(); iq<m_quarters.end(); ++iq) {
      LHCb::L0MuonCandidates* candsQ = new LHCb::L0MuonCandidates();
      for (LHCb::L0MuonCandidates::const_iterator it=filtered->begin(); it<filtered->end();++it) {
        int qua=(((*it)->muonTileIDs(2))[0].quarter());
        if ( (*iq)==qua) candsQ->add(*it);
      }
      if (candsQ->size()>0) printCand(candsQ,location);
      delete candsQ;
    }
  } else {
    printCand(filtered,location);
  }
  

  delete filtered;
}

void L0MuonAlgComparison::printCand(LHCb::L0MuonCandidates* filtered, std::string tab)
{
  
  if (filtered->size()>8) {
    LHCb::L0MuonCandidates* cands8 = new LHCb::L0MuonCandidates();
    int ncands=0;
    for (LHCb::L0MuonCandidates::const_iterator it=filtered->begin(); it<filtered->end();++it) {
      ++ncands;
      cands8->add(*it);
      if (0==((ncands)%8)) 
      {
        printCand(cands8,tab);
        cands8->clear();
        ncands=0;
      }
    }
    if (0!=((ncands)%8)) printCand(cands8,tab);
    cands8->clear();
    delete cands8;
    return;
  }

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

