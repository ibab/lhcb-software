// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Particle.h" 
//#include "GaudiAlg/GaudiTuples.h" 

#include "Event/HltDecReports.h"
#include "GaudiKernel/IAlgManager.h"

// local
#include "StrippingReport.h"
#include "Kernel/SelectionLine.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StrippingReport
//
// 2010-03-15 : Anton Poluektov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingReport );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StrippingReport::StrippingReport( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty("HDRLocation", m_hdrLocation = "Strip/Phys/DecReports");
  declareProperty("Selections", m_selections);
  declareProperty("OnlyPositive", m_onlyPositive = true);
  declareProperty("EveryEvent", m_everyEvent = false);
  declareProperty("ReportFrequency", m_reportFreq = 100);
  declareProperty("PrintNonResponding", m_printNonResponding = true);
  declareProperty("PrintHot", m_printHot = true);
  declareProperty("HotThreshold", m_hotThreshold = 0.5);
}

//=============================================================================
// Destructor
//=============================================================================
StrippingReport::~StrippingReport() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StrippingReport::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_algMgr = svc<IAlgManager>   ( "ApplicationMgr" );
  
  std::vector<std::string>::iterator i;

  for (i = m_selections.begin(); i != m_selections.end(); i++) {
    ReportStat stat;
    stat.name = *i;
    stat.candidates = 0;
    stat.decisions = 0;
    m_stat.push_back(stat);
  }
  
  m_event = 0;

  return StatusCode::SUCCESS;
}

//===============================================================================
// Function to show selection statistics 
//===============================================================================
void StrippingReport::report(bool onlyPositive) {

  std::vector<ReportStat>::iterator i;

  char str[80];

  sprintf(str,"%-51.51s: %8.8s %10.10s %7.7s", "Decision name", "Rate", "Accepted", "Mult.");

  info() << "-------------------------------------------------------------------------------------" << endmsg;
  info() << str << endmsg;

  for (i = m_stat.begin(); i != m_stat.end(); i++) {
    std::string strippedName = i->name;

    double rate = 0.;
    if (m_event > 0) rate = (double)i->decisions/(double)m_event;

    if (i->candidates >= 0) {
      double mult = 0;
      if (i->decisions > 0) mult = (double)i->candidates/(double)i->decisions;
      if (i->decisions > 0 || !onlyPositive) {
        sprintf(str,"-- %-48.48s: %8.6f %10.1d %7.3f", strippedName.data(), rate, i->decisions, mult);
        info() << str << endmsg;
      }
    } else if (i->decisions > 0 || !onlyPositive) {
      // Not a Selection::Line
      sprintf(str,"%-51.51s: %8.6f %10.1d", strippedName.data(), rate, i->decisions);
      info() << "-------------------------------------------------------------------------------------" << endmsg;
      info() << str << endmsg;
    }
      
  }
  info() << "-------------------------------------------------------------------------------------" << endmsg;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode StrippingReport::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  StatusCode result = StatusCode::SUCCESS;
  
  std::vector< ReportStat >::iterator i;

  char str[80];

  if (m_everyEvent) { 
    sprintf(str,"%-51.51s: %4s %5s", "Decision name", "Decn", "Cands");
 
    info() << "----------------------------------------------------------------" << endmsg;
    info() << str << endmsg;
  }

  for (i = m_stat.begin(); i != m_stat.end(); i++) {
    
    int passed = 0;
    int cand = 0;
    
    IAlgorithm* myIAlg(0);

    StatusCode result = m_algMgr->getAlgorithm( i->name, myIAlg );
    if ( result.isSuccess() ) {
        Algorithm* myAlg = dynamic_cast<Algorithm*>(myIAlg);
        if (myAlg) passed = myAlg->filterPassed();
        Selection::Line* strAlg = dynamic_cast<Selection::Line*>(myIAlg);
        if (strAlg) cand = strAlg->numberOfCandidates();
        else {
          // Not a Selection::Line
          cand = -1;
        }
    }
    
    i->candidates += cand;
    i->decisions += passed;

    if (m_everyEvent && (passed != 0 || cand < 0 || !m_onlyPositive)) { 
      if (cand >= 0) {
        sprintf(str,"-- %-48.48s: %4.1d %5.1d", i->name.data(), passed, cand);
        info() << str << endmsg;
      } else {  
        sprintf(str,"%-51.51s: %4.1d", i->name.data(), passed);
        info() << "----------------------------------------------------------------" << endmsg;
        info() << str << endmsg;
      }
    }

  }

  if (m_everyEvent) { 
    info() << "----------------------------------------------------------------" << endmsg;
  }
  
  m_event++;
  if (m_reportFreq > 0 && (m_event % m_reportFreq == 0) ) report(m_onlyPositive);
  
  return result;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode StrippingReport::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // Produce selection statistics report (always show all lines, even non-responding)
  report(false);

  std::vector<ReportStat>::iterator i;

  char str[80];

  // List non-responding lines
  if (m_printNonResponding) {

    sprintf(str,"%-51.51s", "Non-responding lines:");

    warning() << endmsg;
    warning() << "-----------------------------------------------------------------" << endmsg;
    warning() << str << endmsg;

    for (i = m_stat.begin() + 1; i != m_stat.end(); i++) {
      std::string strippedName = i->name;

      if (i->candidates >= 0) {
        if (i->decisions == 0) {
          sprintf(str,"-- %-48.48s", strippedName.data());
          warning() << str << endmsg;
        } 
      } else {
        // Not a Selection::Line
//        sprintf(str,"%-51.51s", strippedName.data());
//        warning() << "-----------------------------------------------------------------" << endmsg;
//        warning() << str << endmsg;
      }
    }
    warning() << "-----------------------------------------------------------------" << endmsg;
  }

  // List hot lines
  if (m_printHot) {

    sprintf(str,"%-51.51s", "Hot lines:");

    warning() << endmsg;
    warning() << "-----------------------------------------------------------------" << endmsg;
    warning() << str << endmsg;

    for (i = m_stat.begin() + 1; i != m_stat.end(); i++) {
      std::string strippedName = i->name;

      double rate = 0.;
      if (m_event > 0) rate = (double)i->decisions/(double)m_event;

      if (i->candidates >= 0) {
        if (rate > m_hotThreshold) {
          sprintf(str,"-- %-48.48s %8.6f", strippedName.data(), rate);
          warning() << str << endmsg;
        }
      } else {
        // Not a Selection::Line
//        sprintf(str,"%-51.51s", strippedName.data());
//        warning() << "-----------------------------------------------------------------" << endmsg;
//        warning() << str << endmsg;
      }
    }
    warning() << "-----------------------------------------------------------------" << endmsg;
  }


  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
