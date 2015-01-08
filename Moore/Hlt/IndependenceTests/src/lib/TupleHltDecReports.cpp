// $Id: $
// Include files 

// boost
#include <boost/foreach.hpp>

// from Gaudi
#include <GaudiKernel/AlgFactory.h>

// Event
#include <Event/ODIN.h>
#include <Event/HltDecReports.h>

// local
#include "TupleHltDecReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleHltDecReports
//
// 2010-03-23 : Roel Aaij
//-----------------------------------------------------------------------------

using std::string;
using LHCb::ODIN;
using LHCb::HltDecReports;
using LHCb::HltDecReport;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TupleHltDecReports )

//=============================================================================
TupleHltDecReports::TupleHltDecReports(const std::string& name,
                                       ISvcLocator* pSvcLocator)
   : GaudiTupleAlg ( name , pSvcLocator )
{
   declareProperty( "ReportLocation", m_reportLocation 
                    = LHCb::HltDecReportsLocation::Default );
}

//=============================================================================
TupleHltDecReports::~TupleHltDecReports()
{

} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TupleHltDecReports::initialize() {
   StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

   return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TupleHltDecReports::execute() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

   // Get the ODIN
   ODIN* odin = get< ODIN >( LHCb::ODINLocation::Default );

   // Get the HltDecReports
   HltDecReports* reports = 0;
   if ( exist< HltDecReports >( m_reportLocation ) ) {
      reports = get< HltDecReports >( m_reportLocation );
   } else {
      return StatusCode::SUCCESS;
   }

   Tuple tuple = nTuple( "HltDecReportsTuple" );

   long unsigned int eventNumber = odin->eventNumber();
   unsigned int runNumber = odin->runNumber();
   tuple->column( "runNumber", runNumber );
   tuple->column( "eventNumber", eventNumber );

   BOOST_FOREACH( const HltDecReports::Container::value_type& entry,
                  reports->decReports() ) {
      const string& name = entry.first;
      if ( name.find( "Global" ) != string::npos ) continue;
      const HltDecReport& report = entry.second;
      tuple->column( name, report.decision() );
   }

   StatusCode sc = tuple->write();
   if (!sc) {
      error() << "Failed writing tuple" << endmsg;
   }

   return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TupleHltDecReports::finalize() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

   return GaudiTupleAlg::finalize();  // must be called after all other actions
}
