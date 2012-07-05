// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/MCHit.h"

// FTDet
#include "FTDet/DeFTDetector.h"

// from FTEvent
#include "Event/MCFTDeposit.h"
#include "FTDet/DeFTLayer.h"

// local
#include "MCFTDepositMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCFTDepositMonitor
//
// 2012-07-04 : Eric Cogneras
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCFTDepositMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFTDepositMonitor::MCFTDepositMonitor( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("HitLocation" , m_hitLocation = LHCb::MCHitLocation::FT, "Path to  MCHits");
  declareProperty("DepositLocation" , m_depositLocation =  LHCb::MCFTDepositLocation::Default, "Path to MCFTDeposits");
}
//=============================================================================
// Destructor
//=============================================================================
MCFTDepositMonitor::~MCFTDepositMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MCFTDepositMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// Retrieve and initialize DeFT
  m_deFT = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
  if (m_deFT) { debug() << "Successfully retrieved DeFT" << endmsg; }
  else { error() << "Error getting DeFT" << endmsg; }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCFTDepositMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // retrieve Hits
  const MCHits* mcHitsCont = get<MCHits>(m_hitLocation);
  debug() <<"mcHitsCont->size() : " << mcHitsCont->size()<< endmsg;
  

  MCHits::const_iterator iterHit = mcHitsCont->begin();
  for (; iterHit!=mcHitsCont->end();++iterHit){
    //pointer to the Hit
    MCHit* ftHit = *iterHit;

    plot(ftHit->energy(),"DepEnergyPerHit", "Energy deposited [Hit level]; Energy [MeV];Number of hits" , 0 , 10 );  

    // ( call of calculateHits method) 
    const DeFTLayer* pL = m_deFT->findLayer(ftHit->midPoint());
    FTDoublePairs vectCF;
    if (pL) {
      pL->calculateHits(ftHit->entry(), ftHit->exit(), vectCF);
    }
    plot(vectCF.size(),"DepNbChannel", 
         "Number of fired channels per Hit; Number of fired channels; Number of hits" , 
         0 , 100);
  }
  
  // retrieve FTDeposits
  const MCFTDeposits* mcDepositsCont = get<MCFTDeposits>(m_depositLocation);
  debug() <<"mcDepositsCont->size() : " << mcDepositsCont->size()<< endmsg;
  MCFTDeposits::const_iterator iterDeposit = mcDepositsCont->begin();
  for (; iterDeposit!=mcDepositsCont->end();++iterDeposit){
    plot((double)(*iterDeposit)->channelID(), "DepFiredChannel","Fired Channel; ChannelID" , 0. , 1000000., 10000);
    plot((double)(*iterDeposit)->mcHitVec().size(), "DepHitPerChannel",
         "Number of Hits per Channel;Number of Hits per Channel;" , 0. , 50.);

    std::vector<std::pair<LHCb::MCHit*,double> >::const_iterator energyPerHit = (*iterDeposit)->mcHitVec().begin();
    
    for(;energyPerHit != (*iterDeposit)->mcHitVec().end(); ++energyPerHit)
      plot(energyPerHit->second,
           "DepEnergyPerChannel",
           "Energy deposited [Channel level];Energy [MeV];Number of SiPM channels", 
           0, 10);

  }


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCFTDepositMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
