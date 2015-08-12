// Include files 

// boost
#include <boost/regex.hpp>
#include <boost/foreach.hpp>

// Event
#include <Event/ODIN.h>
#include <Event/HltDecReports.h>

// local
#include "HltMonitorBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltMonitorBase
//
// 2010-08-24 : 
//-----------------------------------------------------------------------------

namespace {
   using namespace LHCb;
   using std::string;
   using std::vector;
   using namespace boost;
}

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMonitorBase )

//=============================================================================
HltMonitorBase::HltMonitorBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
   : GaudiHistoAlg ( name , pSvcLocator ), m_filledDecisions( false )
{
   declareProperty( "HltDecReportsLocations", m_decReportsLocations = {{ HltDecReportsLocation::Default}} );
}

//=============================================================================
HltMonitorBase::~HltMonitorBase()
{

} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMonitorBase::initialize() {

   StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by HltMonitorBase
   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

   setHistoTopDir("HltMonitors/") ;

   return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMonitorBase::finalize() {

   if ( msgLevel( MSG::DEBUG ) ) debug() << "Finalize base class HltMonitorBase."
                                         << endmsg;
   return GaudiHistoAlg::finalize();  // must be called after all other actions
}
//=============================================================================
std::vector<const HltDecReports*> 
HltMonitorBase::hltDecReports() const 
{
   std::vector<const HltDecReports*> v; v.reserve( m_decReportsLocations.size());
   std::for_each( std::begin(m_decReportsLocations), std::end(m_decReportsLocations),
                  [&](const std::string& loc) { 
                        const auto* r = getIfExists< HltDecReports >( loc );
                        if (r) v.push_back(r);
                  } );
   return v;
}



//=============================================================================
void HltMonitorBase::fillDecisions( const HltDecReports* decReports ) {

   match_flag_type flags = match_default;
   match_results< string::const_iterator > match;

   // Loop over the configured regexex and fill the decision container with
   // decisions which match a regex.
   for( const std::string& expression: m_regexes ) {
      if ( m_decisions.get< regexTag >().count( expression ) ) continue;

      regex re( expression );
      const auto& decisions = decReports->decisionNames();

      for( const string& decision: decisions ) {
         if ( regex_match( decision.begin(), decision.end(), match, re, flags ) ) {
            m_decisions.insert( { expression, decision } );
            if ( msgLevel( MSG::DEBUG ) )
               debug() << "Added decision " << decision << " for " << expression << endmsg;
         }
      }
   }
   m_filledDecisions = true;
}
