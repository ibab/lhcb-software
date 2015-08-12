// stdlib
#include <iostream>
#include <string>

// STL
#include <vector>
#include <algorithm>

// boost
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/numeric/conversion/cast.hpp>
// from Gaudi
#include <GaudiKernel/SystemOfUnits.h>
#include "GaudiKernel/IUpdateManagerSvc.h"

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
DECLARE_ALGORITHM_FACTORY( HltRateMonitor )

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
   : HltMonitorBase ( name , pSvcLocator ), m_runParameters( 0 ),
     m_startOfRun( 0 ), m_runNumber( 0 ), m_disabled( false )
{
   declareProperty( "ODINLocation", m_ODINLocation =
                    ODINLocation::Default );
   declareProperty( "Regexes", m_regexes = std::vector<std::string>(1, "Hlt2Express.*Decision" ) );
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
      IUpdateManagerSvc* updMgrSvc = svc< IUpdateManagerSvc >( "UpdateManagerSvc" );
      updMgrSvc->registerCondition( this, m_runParameterLocation,
                                    &HltRateMonitor::i_updateConditions, m_runParameters );
      sc = updMgrSvc->update( this );
      if( !sc.isSuccess() ) {
         error() << "Could not update UpdateManagerSvc" << endmsg;
         return sc;
      }
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

   auto decReports = hltDecReports();
   if (decReports.empty())  return StatusCode::SUCCESS;

   if ( !m_filledDecisions ) { 
       std::for_each( std::begin(decReports), std::end(decReports),
                      [&](const HltDecReports* r) { fillDecisions(r); } );
   }

   const ODIN* odin = get< ODIN >( m_ODINLocation );
   if ( m_runNumber != odin->runNumber() ) {
    
     if ( msgLevel(MSG::DEBUG) ){
        Gaudi::Time gtime = odin->eventTime();
        debug() << "Run number from database and ODIN do not match: " 
                 << m_runNumber << " " << odin->runNumber() << ", skipping event. GPS time: " 
                 << odin->gpsTime() << " (" 
                 << gtime.year(false) << "/"
                 << gtime.month(false)+1 << "/"
                 << gtime.day(false) << ", "
                 << gtime.hour(false) << ":"
                 << gtime.minute(false) << ":"
                 << gtime.second(false)+gtime.nsecond()/1000000000. << ")"
                << endmsg;
     }
     return Warning("Incorrect run number", StatusCode::SUCCESS, 0); // for statistics
   }

   double time = double( odin->gpsTime() - m_startOfRun ) / 1e6;
   // Loop over the configured regexes and check if any match has a positive decision.
   const decByRegex_t& dbr = m_decisions.get< regexTag >();
   index_iterator< decMap_t, regexTag >::type it, end;
   for( it = dbr.begin(); it != dbr.end() ; ) {
      const string& expression = it->first;
      end = dbr.upper_bound( expression );

      // Calculate the weight which ensures the y axis makes sense.
      double weight = double( 1 ) / ( m_secondsPerBin );

      unsigned int total = 0;
      for ( ; it != end; ++it ) {
         const string& decision = it->second;
         if (std::any_of( std::begin(decReports), std::end(decReports), 
                    [&decision](const HltDecReports* r) { 
                                const auto* decReport = r->decReport( decision );
                                return ( decReport && decReport->decision() );
                    } ) ) {
            ++total;
            string title = decision.substr( 0, decision.size() - 8 ) + "Rate";
            plot( time, title, 0., 9000, 9000 / m_secondsPerBin, weight );
         }
      }

      // Fill the total rate histogram with the total number of matches for this
      // regex.
      string title = "Total" + expression + "Rate";
      for ( unsigned int i = 0; i < total; ++i )
         plot( time, title, 0., 9000, 9000 / m_secondsPerBin, weight );
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

//=============================================================================
StatusCode HltRateMonitor::i_updateConditions()
{
   if ( m_runParameters == 0 ) { 
      error() << "Could not obtain Condition for run parameters from conditions DB" << endmsg;
      return StatusCode::FAILURE;
   }
   if ( !m_runParameters->exists( "RunStartTime" ) ) {
      error() << "Condition does not contain RunStartTime " << endmsg;
      return StatusCode::FAILURE;
   }
   if ( !m_runParameters->exists( "RunNumber" ) ) {
      error() << "Condition does not contain RunNumber " << endmsg;
      return StatusCode::FAILURE;
   }

   //multiply by 1e6
   m_startOfRun = 1000000 * numeric_cast< long long unsigned int >
      ( m_runParameters->param< int >( "RunStartTime" ) );
   m_runNumber = numeric_cast< unsigned int >
      ( m_runParameters->param< int >( "RunNumber" ) );

   if (msgLevel(MSG::DEBUG)) debug()<< "Updated to run " <<  m_runNumber << " starting at " << m_startOfRun << endmsg ;

   return StatusCode::SUCCESS;
}
