// $Id: HltMassMonitor.h,v 1.6 2010/06/02 08:14:47 jost Exp $
#ifndef COMPONENT_HltMassMonitor_H
#define COMPONENT_HltMassMonitor_H 1

// Include files
// from system
#include "math.h"
#include <map>
// from Gaudi

#include "GaudiAlg/GaudiHistoAlg.h"
//for hltselreports
#include "GaudiKernel/DataObject.h"
#include "Event/HltSelReports.h"
#include "Event/HltDecReports.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/ObjectContainerBase.h"

// ============================================================================
// ROOT
// ============================================================================
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include "TF1.h"
#include "TF2.h"
#include "TDirectory.h"

class HistoWrapper;
class IHltMonitorSvc;

class HltMassMonitor : public GaudiHistoAlg {
public:
   /// Standard constructor
   HltMassMonitor( const std::string& name, ISvcLocator* pSvcLocator );

   ~HltMassMonitor( ); ///< Destructor

   virtual StatusCode initialize();    ///< Algorithm initialization
   virtual StatusCode execute   ();    ///< Algorithm execution
   virtual StatusCode finalize  ();    ///< Algorithm finalization

   // update the condition
   StatusCode updateCondition();

   IHltMonitorSvc* hltMonSvc() {
      return m_hltMonSvc;
   }

private:

   IHltMonitorSvc* m_hltMonSvc = nullptr;
   std::string m_monSvc;

   // Histogram definitions
   typedef std::map< std::string, std::vector< double > > histos_t;
   typedef std::map< std::string, std::string > decisions_t;
   typedef std::map< std::string, std::vector< double > > decisionstructure_t;
   decisionstructure_t m_decisionstructure;
   decisions_t m_decisions;
   histos_t m_histos;
   std::vector< HistoWrapper* > m_wrappers;

   std::string m_decReportsLocation;
   std::string m_selReportsLocation;

};
#endif // COMPONENT_HltMassMonitor_H
