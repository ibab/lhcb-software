// stdlib
#include <string>

// STL
#include <vector>
#include <algorithm>

// boost
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

// AIDA
#include <AIDA/IProfile1D.h>

// from Gaudi
#include <GaudiUtils/HistoLabels.h>
#include <GaudiKernel/SystemOfUnits.h>

// Event
#include <Event/ODIN.h>
#include <Event/HltDecReports.h>

// local
#include "HltCompositionMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltCompositionMonitor
//
// 2010-08-24 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltCompositionMonitor )

namespace {
   using namespace LHCb;
   using std::string;
   using std::vector;
   using std::distance;
   using namespace boost;
   using namespace multi_index;
   using Gaudi::Utils::Histos::setBinLabels;
}

//=============================================================================
HltCompositionMonitor::HltCompositionMonitor(const std::string& name,
                                             ISvcLocator* pSvcLocator)
   : HltMonitorBase ( name , pSvcLocator )
{

   declareProperty( "Regexes", m_regexes = std::vector<std::string>(1, "Hlt2.*Decision" ) );

}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltCompositionMonitor::initialize() {

   StatusCode sc = HltMonitorBase::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by HltMonitorBase

   return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltCompositionMonitor::execute() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

   auto decReports = hltDecReports();
   if (decReports.empty())  return StatusCode::SUCCESS;
   if ( !m_filledDecisions ) { 
       std::for_each( std::begin(decReports), std::end(decReports),
                      [&](const HltDecReports* r) { init(r); } );
   }

   // Loop over all decisions and increment any positive ones.
   for( const auto& entry : m_decisions.get< decisionTag >() ) {
      const auto& decision = entry.second;
      if (std::any_of( std::begin(decReports), std::end(decReports), 
                       [&decision](const HltDecReports* r) { 
                                   const auto* decReport = r->decReport( decision );
                                   return ( decReport && decReport->decision() );
                       } ) ) m_accepts[ decision ] += 1;
   }
   setFilterPassed( true );
   return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltCompositionMonitor::finalize() {

   if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Finalize" << endmsg;

   // Loop over the configured regexes and get the data needed to fill the 
   // histogram from the m_accepts container.
   const decByRegex_t& dbr = m_decisions.get< regexTag >();
   index_iterator< decMap_t, regexTag >::type it, end;
   for( it = dbr.begin(); it != dbr.end() ; ) {
      const string& expression = it->first;
      end = dbr.upper_bound( expression );

      vector< unsigned int > data;
      vector< string > labels;
      for ( ; it != end; ++it ) {
         const string& decision = it->second;
         unsigned int nAccepts = m_accepts[ decision ];
         data.push_back( nAccepts );
         labels.push_back( decision );
         if ( msgLevel( MSG::DEBUG ) )
            debug() << decision << " accepted " << nAccepts << " events." << endmsg;
      }

      // Count the total number of accepts.
      double total = 0;
      BOOST_FOREACH( unsigned int n, data ) total += n;
      if ( msgLevel( MSG::DEBUG ) )
         debug() << expression << " accepted " << total << " events." << endmsg;

      // Book a profile histogram and set its bin labels to the decision names.
      AIDA::IProfile1D* prof = bookProfile1D( expression, 0, labels.size(), labels.size() );
      setBinLabels( prof, labels );

      // Fill the histogram.
      double bin = 0.5;
      for ( unsigned int i = 0; i < data.size(); ++i ) {
         prof->fill( bin, double( data[ i ] ) / total * 100 );
         bin += 1;
      }
   }

   return HltMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
//  Finalize
//=============================================================================
void HltCompositionMonitor::init( const HltDecReports* decReports ) {

   // Fill the decision container and prepare the m_accepts container.
   fillDecisions( decReports );
   for( const auto& entry: m_decisions.get< decisionTag >() ) {
      m_accepts.insert( { entry.second, 0 } );
   }

}
