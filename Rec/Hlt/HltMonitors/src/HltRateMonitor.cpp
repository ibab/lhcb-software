// $Id: $
// stdlib
#include <iostream>
#include <string>

// STL
#include <vector>

// boost
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/numeric/conversion/cast.hpp>
// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/SystemOfUnits.h>

// Condition
#include "DetDesc/Condition.h"

// Event
#include <Event/ODIN.h>
#include <Event/HltDecReports.h>

// local
#include "HltRateMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltRateMonitor
//
// 2010-08-24 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRateMonitor );

namespace {
   using namespace LHCb;
   using std::string;
   using std::vector;
   using namespace boost;
   using namespace multi_index;
   using boost::numeric_cast;
   using std::cout;
   using std::endl;
}

//=============================================================================
HltRateMonitor::HltRateMonitor( const std::string& name,
                                ISvcLocator* pSvcLocator )
   : HltMonitorBase ( name , pSvcLocator ), m_runParameters( 0 )
{
   declareProperty( "HltDecReportsLocation", m_decReportsLocation =
                    HltDecReportsLocation::Default );
   declareProperty( "ODINLocation", m_ODINLocation =
                    ODINLocation::Default );
   declareProperty( "Regexes", m_regexes = assign::list_of( "Hlt2Express.*Decision" ) );
   declareProperty( "RunParameterLocation", m_runParameterLocation 
                    = "Conditions/Online/LHCb/RunParameters" );
   declareProperty( "ForceEnable", m_forceEnable = false );
   declareProperty( "SecondsPerBin", m_secondsPerBin = 10 );
}

//=============================================================================
HltRateMonitor::~HltRateMonitor()
{

} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltRateMonitor::initialize() {

   StatusCode sc = HltMonitorBase::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by HltMonitorBase
   
   // Check for the existence of the RunParameters condition.
   // If it is not available, disable all functionality.
   if ( existDet< DataObject >( detSvc(), m_runParameterLocation ) ) {
      // Register condition and read parameters values
      registerCondition< HltRateMonitor >( m_runParameterLocation, m_runParameters );
   } else {
      m_disabled = true;
   }
   
   if ( m_disabled && !m_forceEnable ) {
      warning() << "Could not get run parameters from the CondDB, "
         "no histograms will be produced." << endmsg;
   } else if ( m_disabled && m_forceEnable ) {
      warning() << "Could not get run parameters from the CondDB, "
         "but forced to run, histograms will not make sense." << endmsg;
   }

   return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltRateMonitor::execute() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

   if ( m_disabled && !m_forceEnable ) return StatusCode::SUCCESS;

   const HltDecReports* decReports = 0;
   try { 
      decReports = get< HltDecReports >( m_decReportsLocation );
   } catch ( const GaudiException& ) {
      return StatusCode::SUCCESS;
   }

   if ( !m_filledDecisions ) fillDecisions( decReports );

   int startOfRun = m_runParameters->param< int >( "RunStartTime" );
   unsigned int runNumber = numeric_cast< unsigned int >
      ( m_runParameters->param< int >( "RunNumber" ) );

   const ODIN* odin = get< ODIN >( m_ODINLocation );
   if ( runNumber != odin->runNumber() ) {
      Error( "Run number from database and ODIN do not match", StatusCode::FAILURE );
   }

   double time = double( odin->gpsTime() - startOfRun * 1e6 ) / 1e6;

   // Loop over the configured regexes and check if any match has a positive decision.
   const decByRegex_t& dbr = m_decisions.get< regexTag >();
   index_iterator< decMap_t, regexTag >::type it, end;
   for( it = dbr.begin(); it != dbr.end() ; ) {
      const string& expression = it->first;
      end = dbr.upper_bound( expression );

      unsigned int total = 0;
      for ( ; it != end; ++it ) {
         const string& decision = it->second;
         const LHCb::HltDecReport* decReport = decReports->decReport( decision );
         if ( decReport && decReport->decision() ) {
            ++total;
            string title = decision.substr( 0, decision.size() - 8 ) + "Rate";
            plot( time, title, 0., 9000, 9000 / m_secondsPerBin );
         }
      }

      // Fill the total rate histogram with the total number of matches for this
      // regex.
      string title = "Total" + expression + "Rate";
      for ( unsigned int i = 0; i < total; ++i )
         plot( time, title, 0., 9000, 9000 / m_secondsPerBin );
   }

   setFilterPassed( true );
   return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltRateMonitor::finalize() {

   return HltMonitorBase::finalize();  // must be called after all other actions

}

