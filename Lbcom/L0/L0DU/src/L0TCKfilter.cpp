// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "Event/L0DUReport.h"
#include "L0TCKfilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0TCKfilter
//
// 2008-03-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0TCKfilter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0TCKfilter::L0TCKfilter( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : L0AlgBase ( name , pSvcLocator )
{
  declareProperty("TCKList" , m_list);
  declareProperty( "ReportLocation"    , m_reportLocation =  LHCb::L0DUReportLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
L0TCKfilter::~L0TCKfilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0TCKfilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0TCKfilter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);

  //  std::string loc = dataLocation( m_reportLocation );

  // get tck from raw
  if( !exist<LHCb::L0DUReport>( m_reportLocation )){
    Error("L0DUReport not found at location " + m_reportLocation + " - event rejected").ignore();
    counter("Report not found L0TCKFilter reject") += 1;
    return StatusCode::SUCCESS;
  }
  LHCb::L0DUReport* report   = get<LHCb::L0DUReport>( m_reportLocation );
  unsigned int tck           = report->tck();
  std::stringstream ttck("");
  ttck << format("0x%04X", tck) ;
  // filter
  for(std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; it++){
    if( "0x" != (*it).substr( 0, 2 ) ){
      Error("SKIP the requested TCK value " + *it + " (MUST be in hexadecimal format '0x" + *it + "')").ignore();
      continue;
    }
    if( *it == ttck.str() ){
      setFilterPassed(true);
       if ( msgLevel(MSG::DEBUG) ) debug()
         << "accepted TCK = " << ttck.str() << " -> set FilterPassed to true"
         << endmsg;
      counter("L0TCKFilter accept") += 1;
      return StatusCode::SUCCESS;
    }
  }
  if ( msgLevel(MSG::DEBUG) ) debug()
    << "rejected TCK = " << ttck.str() << " -> set FilterPassed to false"
    << endmsg;
  counter("L0TCKFilter reject") += 1;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0TCKfilter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
