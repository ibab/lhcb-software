// $Id: MuonEfficiencyMonitor.cpp,v 1.2 2009-02-26 11:17:16 ggiacomo Exp $
// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/MuonPID.h"
#include "Event/Track.h"

// local
#include "MuonEfficiencyMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonEfficiencyMonitor
//
// 2009-02-25 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonEfficiencyMonitor );



MuonEfficiencyMonitor::MuonEfficiencyMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{

}

MuonEfficiencyMonitor::~MuonEfficiencyMonitor() {} 



StatusCode MuonEfficiencyMonitor::initialize() {

  const StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}


StatusCode MuonEfficiencyMonitor::execute() {

  // loop on MuonPIDs
  LHCb::MuonPIDs* pMuids=get<LHCb::MuonPIDs>(LHCb::MuonPIDLocation::Default);
  if (!pMuids){
    error() << " Can't find MuonPID objects, has the MuonID been called before me?" << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::MuonPIDs::iterator ip;
  for (ip=pMuids->begin() ; ip != pMuids->end(); ip++) {
    const LHCb::Track *mutrack = (*ip)->muonTrack () ;
    if (mutrack) {
      info() << " found MuonTrack of type " << (*ip)->idTrack()->type() << endmsg; 
      info() << "   of momentum="<< (*ip)->idTrack()->p() << "   pT="<<(*ip)->idTrack()->pt() << endmsg; 
      info() << " with "<< (*ip)->muonTrack ()->nLHCbIDs() << " muon hits:"  << endmsg; 
      int nH[5]={0};
      const std::vector< LHCb::LHCbID > & mucoords =  (*ip)->muonTrack ()->lhcbIDs();
      std::vector< LHCb::LHCbID >::const_iterator ih;
      for (ih=  mucoords.begin() ; ih != mucoords.end() ; ih++) {
        nH[ih->muonID().station()]++;
      }
      for (int station=0; station<5 ; station++) {
        info() << "    hits in M"<<(station+1)<<" = "<<nH[station]<< endmsg; 
      }
    }
  }


  setFilterPassed(true);  
  return StatusCode::SUCCESS;
}



StatusCode MuonEfficiencyMonitor::finalize() {


  return   GaudiHistoAlg::finalize();
}


