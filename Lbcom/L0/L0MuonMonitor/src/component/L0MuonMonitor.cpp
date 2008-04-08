// $Id: L0MuonMonitor.cpp,v 1.1 2008-04-08 11:31:03 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/L0MuonData.h"

// local
#include "L0MuonMonUtils.h"
#include "L0MuonMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonMonitor
//
// 2008-04-08 : 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonMonitor::L0MuonMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : L0MuonMonitorBase ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonMonitor::~L0MuonMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonMonitor::initialize() {
  StatusCode sc = L0MuonMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Tools
  m_channelHist_l0muon = tool<PhysicalChannelsHist>( "PhysicalChannelsHist", "Channels_l0muon", this);
  m_channelHist_muon = tool<PhysicalChannelsHist>( "PhysicalChannelsHist", "Channels_muon", this);
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRawTool", this);

  // Physical channels
  m_channelHist_l0muon->setHistoDir("L0Muon/Analysis");
  m_channelHist_muon->setHistoDir("L0Muon/Analysis");
  for (std::vector<int>::iterator itq=m_quarters.begin(); itq<m_quarters.end(); ++itq){
    int iq = (*itq);
    for (std::vector<int>::iterator itr=m_regions.begin(); itr<m_regions.end(); ++itr){
      int reg = (*itr);
      for (std::vector<int>::iterator its=m_stations.begin(); its<m_stations.end(); ++its){
        int sta = (*its);
//         info()<<"booking histo for Q"<<(iq+1)<<" R"<<(reg+1)<<" M"<<(sta+1)<<endmsg;
        m_channelHist_l0muon->bookHistos(iq,reg,sta);
        m_channelHist_muon->bookHistos(iq,reg,sta);
      }
    }
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    setProperty("RootInTes",L0MuonMonUtils::timeSlot(*it_ts));
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;
    
    std::string location;
    std::string name;

    // Get L0Muon Hits
    std::vector<LHCb::MuonTileID> l0muontiles;
    location = LHCb::L0MuonDataLocation::Default ;
    if (  exist<LHCb::L0MuonDatas>(location ) ) {
      LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( location );
      LHCb::L0MuonDatas::const_iterator itdata;
      for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
        LHCb::MuonTileID mkey = (*itdata)->key();    
        std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
        if (ols.size()>0) {
          for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
            if (!itol->isValid()){
              info()<<"PU "<<mkey.toString()<<" tile is not valid"<<endreq;
            }

            if (!quarterInUse(itol->quarter())) continue;
            if (!stationInUse(itol->station())) continue;
            if (!regionInUse(itol->region())  ) continue;
            
            l0muontiles.push_back(*itol);
          }
        }
      }
    } // End if container found in TES

    // Get Muon Hits
    std::vector<LHCb::MuonTileID> muontiles;
    if (m_muonBuffer) { // If muon raw buffer tool
      m_muonBuffer->_setProperty("RootInTes", rootInTES());
      IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
      if( prop ) {
        StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
        if( sc.isFailure() )
          return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
      } else return Error( "Unable to locate IProperty interface of MuonRawBuffer" );

      m_muonBuffer->getTile(muontiles);
      m_muonBuffer->forceReset();
      //       std::vector<LHCb::MuonTileID> tiles;
      //       for (std::vector<LHCb::MuonTileID>::iterator it=muontiles.begin(); it!=muontiles.end(); ++itol){
      //         if (!it->isValid()){
      //           info()<<"Muon tile "<<it->toString()<<" tile is not valid"<<endreq;
      //         }
      //         if (!quarterInUse(it->quarter())) continue;
      //         if (!stationInUse(it->station())) continue;
      //         if (!regionInUse(it->region())  ) continue;
      //         tiles.push_back(*itol);
      //       }
      //       muontiles = tiles;
    }// End if muon raw buffer tool

    // Physical channels
    m_channelHist_l0muon->fillHistos(l0muontiles);
    m_channelHist_muon->fillHistos(muontiles);

  } // End of loop over time slots

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return L0MuonMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
