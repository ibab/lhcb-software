// Include files 

// local
#include "EventTimeMonitor.h"
#include "Event/ODIN.h" // event & run number
#include <boost/assign/list_of.hpp>
#include "AIDA/IHistogram1D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventTimeMonitor
//
// 2012-04-19 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventTimeMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventTimeMonitor::EventTimeMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_histoS ( "GPS Seconds" ,  0 , 3600., 3600 ) 
  , m_histoH ( "GPS Hours" , -0.5 , 23.5,   24 ) 
  , m_histoD ( "GPS Days" ,  0.5 , 365.5, 366 ) 
  , m_histoY ( "GPS Year" , 2008.5 , 2028.5, 20 ) 
  , m_plotS()
  , m_plotH()
  , m_plotD()
  , m_plotY()
{
  declareProperty 
    ( "SecondsPlot" , 
      m_histoS , 
      "The parameters of 'delta memory' histogram" ) ;

}
//=============================================================================
// Destructor
//=============================================================================
EventTimeMonitor::~EventTimeMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EventTimeMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if ( produceHistos() ) { 
    m_plotS  = book    ( m_histoS      ) ; 
    m_plotH  = book    ( m_histoH      ) ; 
    m_plotD  = book    ( m_histoD      ) ; 
    m_plotY  = book    ( m_histoY      ) ; 
  }
  std::vector<unsigned int> tmp = boost::assign::list_of(0)(21)(60)(91)(121)(152)(182)(213)(244)(274)(305)(335);
  m_moffsets = tmp;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventTimeMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  // Load the ODIN
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>( LHCb::ODINLocation::Default );
  if( NULL == odin )
  {
    odin = getIfExists<LHCb::ODIN>( LHCb::ODINLocation::Default, false );
  }

  if( NULL == odin )
  {
    // should always be available ...
    return Error( "Cannot load the ODIN data object", StatusCode::SUCCESS );
  }

  const Gaudi::Time gtime = odin->eventTime();
  m_plotY->fill(gtime.year(false));
  m_plotD->fill(m_moffsets[gtime.month(false)]+gtime.day(false));
  m_plotH->fill(gtime.hour(false));
  m_plotS->fill(60*gtime.minute(false)+gtime.second(false)+gtime.nsecond()/1000000000.);

  return StatusCode::SUCCESS;
}

//=============================================================================
