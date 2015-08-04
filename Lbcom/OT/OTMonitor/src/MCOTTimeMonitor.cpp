// Include files 

// from MCEvent
#include "Event/MCOTDeposit.h"
#include "Event/MCOTTime.h"

#include "OTDet/DeOTDetector.h"
// local
#include "MCOTTimeMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCOTTimeMonitor
//
// 2009-04-22 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCOTTimeMonitor )

MCOTTimeMonitor::MCOTTimeMonitor( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_nEvents( 0u )
{

}

MCOTTimeMonitor::~MCOTTimeMonitor() {} 

StatusCode MCOTTimeMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  /// set path
  if ("" == histoTopDir() ) setHistoTopDir("OT/");

  return StatusCode::SUCCESS;
}

StatusCode MCOTTimeMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  /// Increment event counter
  ++m_nEvents;
  
  /// Get MCOTTimes
  const LHCb::MCOTTimes* times = get<LHCb::MCOTTimes>(LHCb::MCOTTimeLocation::Default);
  
  for (LHCb::MCOTTimes::const_iterator t = times->begin(), tEnd = times->end(); t != tEnd; ++t) {
    const unsigned nDeposits = ( (*t)->deposits() ).size();
    plot( double( nDeposits ), 10, "Number of MCOTDeposits per MCOTTime", 0.0, 20.0, 20 );
    if ( nDeposits == 2u ) {
      const LHCb::MCOTDeposit* first  = ( (*t)->deposits() ).front();
      const LHCb::MCOTDeposit* second = ( (*t)->deposits() ).back();
      const double dtime = second->time() - first->time();
      plot( dtime, 20, "Delta time for double hits", 0.0, 80.0, 160 );
      if ( first->isSignal() && second->isSignal() )
	plot( dtime, 21, "Delta time for double hits of which both are signal", 0.0, 80.0, 160 );
      if ( first->isDoublePulse() || second->isDoublePulse() )
	plot( dtime, 22, "Delta time for double hits of which any is a double pulse", 0.0, 80.0, 160 );
      else 
	plot( dtime, 23, "Delta time for double hits of which none is a double pulse", 0.0, 80.0, 160 );
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode MCOTTimeMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  const double perEvent           = 1.0/double( m_nEvents );
  DeOTDetector* det               = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  unsigned nChannels              = det->nChannels();
  const double perChannel         = 1.0/double( nChannels );
  const double perChannelperEvent = perEvent*perChannel;

  scale( HistoID( 10 ), perChannelperEvent );
  scale( HistoID( 20 ), perEvent );
  scale( HistoID( 21 ), perEvent );
  scale( HistoID( 22 ), perEvent );
  scale( HistoID( 23 ), perEvent );

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
