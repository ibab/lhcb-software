// $Id: HltMassMonitor.cpp,v 1.14 2010/06/02 08:14:47 jost Exp $
// AIDA
#include <AIDA/IHistogram1D.h>
#include <AIDA/IHistogram2D.h>
#include <AIDA/IHistogram3D.h>
#include <AIDA/IProfile1D.h>
#include <AIDA/IProfile2D.h>
#include <AIDA/IAxis.h>

// GaudiKernel
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/Timing.h>

// GaudiUtils
#include <GaudiUtils/HistoStats.h>
#include <GaudiUtils/HistoLabels.h>

// HltInterfaces
#include <Kernel/ReadRoutingBits.h>
#include <Kernel/IHltMonitorSvc.h>

// Event
#include <Event/RawBank.h>
#include <Event/RecVertex.h>
#include <Event/RawEvent.h>
#include <Event/HltVertexReports.h>
#include <Event/HltSelReports.h>
#include <Event/ODIN.h>

// STL
#include <string>
#include <vector>
#include <utility>
#include <math.h>

// boost
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/regex.hpp>

// local
#include "HltMassMonitor.h"
#include "HistoWrapper.h"

using LHCb::RawBank;
using LHCb::RawEvent;

using boost::assign::list_of;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMassMonitor
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(HltMassMonitor)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMassMonitor::HltMassMonitor( const std::string& name, ISvcLocator* pSvcLocator)
 : GaudiHistoAlg ( name , pSvcLocator ),
   m_hltMonSvc{nullptr}
{
   declareProperty( "Decisions",  m_decisions );
   declareProperty( "Histograms", m_histos );
   declareProperty( "DecisionStructure", m_decisionstructure );
   declareProperty( "DecReportsLocation", m_decReportsLocation = "Hlt1/DecReports");
   declareProperty( "SelReportsLocation", m_selReportsLocation = "Hlt1/SelReports");
   declareProperty( "MonitorSvc", m_monSvc = "HltMonitorSvc/Hlt2MonSvc");
}

//=============================================================================
// Destructor
//=============================================================================
HltMassMonitor::~HltMassMonitor()
{

}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMassMonitor::initialize()
{
   StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first

   if ( m_decisions.size() != m_histos.size() ) {
      Exception( "The number of decisions must match the number of histogram definitions",
                 StatusCode::FAILURE );
   }

   // Hlt Monitoring Service
   try {
      m_hltMonSvc = svc<IHltMonitorSvc>(m_monSvc, false);
   } catch (const GaudiException&) {
      info() << "Could not retrieve " << m_monSvc << endmsg;
   }

   for (const auto& entry : m_decisions) {
      const std::string& name = entry.first;
      const std::string& decision = entry.second;
      const auto it = m_histos.find( name );
      const auto itt = m_decisionstructure.find(name);
      if (it == m_histos.end()) {
         Exception( "No histogram definition for name " + name, StatusCode::FAILURE );
      } else if (itt == m_decisionstructure.end()) {
         m_wrappers.push_back(new HistoWrapper( this, name, decision, it->second ));
      } else {
         m_wrappers.push_back(new HistoWrapper( this, name, decision,
                                                it->second, itt->second ));
      }
   }
   return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMassMonitor::execute()
{
   const LHCb::HltSelReports* selReports
      = getIfExists< LHCb::HltSelReports >( m_selReportsLocation );
   if (selReports) {
      for (HistoWrapper* wrapper : m_wrappers) {
         wrapper->fill(selReports);
      }
   } else {
      warning() << "No HltSelReports found at " << m_selReportsLocation << endmsg;
   }
   setFilterPassed(true);
   return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMassMonitor::finalize()
{

   for (HistoWrapper* wrapper : m_wrappers ) {
      delete wrapper;
   }
   m_wrappers.clear();

   return GaudiHistoAlg::finalize();  // must be called after all other actions
}
