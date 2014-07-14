// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Particle.h"
//#include "GaudiAlg/GaudiTuples.h"

#include "Event/HltDecReports.h"
#include "Event/HltDecReport.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ChronoEntity.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Kernel/SelectionLine.h"

// local
#include "StrippingReport.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StrippingReport
//
// 2010-03-15 : Anton Poluektov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingReport )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StrippingReport::StrippingReport( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty("HDRLocation", m_hdrLocation = "");
  declareProperty("Selections", m_selections);
  declareProperty("OnlyPositive", m_onlyPositive = true);
  declareProperty("EveryEvent", m_everyEvent = false);
  declareProperty("ReportFrequency", m_reportFreq = 100);
  declareProperty("PrintNonResponding", m_printNonResponding = false);
  declareProperty("PrintHot", m_printHot = false);
  declareProperty("HotThreshold", m_hotThreshold = 0.0005);
  declareProperty("NormalizeByGoodEvents", m_normalizeByGoodEvents = true);
  declareProperty("Latex", m_latex = false);
  declareProperty("StrippingGlobalName", m_strippingGlobalName = "StrippingGlobal");
  declareProperty("Correlation", m_correlation = false);
  declareProperty("CorrelationGreenThreshold", m_correlationGreenThreshold=0.02);
  declareProperty("CorrelationYellowThreshold", m_correlationYellowThreshold=0.05);
  declareProperty("CorrelationRedThreshold", m_correlationRedThreshold=0.3);
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

  m_chronoSvc = svc<IChronoStatSvc> ( "ChronoStatSvc" );

  m_incSvc = svc<IIncidentSvc> ( "IncidentSvc" );

  if ( !m_incSvc ) {
    error() << "Could not retrieve 'IncidentSvc'" << endmsg;
    return StatusCode::FAILURE;
  }
  m_incSvc->addListener ( this , IncidentType::BeginEvent );

  std::vector<std::string>::iterator i;

  for (i = m_selections.begin(); i != m_selections.end(); i++) {
    ReportStat stat;
    stat.name = *i;
    stat.candidates = 0;
    stat.decisions = 0;
    stat.errors = 0;
    stat.incidents = 0;
    stat.slow_events = 0;
    if (m_correlation)    //Allocate memory for correlation studies
      stat.corr.assign(m_selections.size(), 0); //only if needed.
    m_stat.push_back(stat);
  }

  m_event = 0;
  m_goodEvent = 0;

  return StatusCode::SUCCESS;
}

//===============================================================================
// Function to show selection statistics
//===============================================================================
void StrippingReport::report(bool onlyPositive) {

  std::vector<ReportStat>::iterator i;

  char str[128];

  sprintf(str," |%61.61s|%8.8s|%10.10s|%7.7s|%8.8s|", "*Decision name*", "*Rate,%*", "*Accepted*", "*Mult*","*ms/evt*");

  info() << "Event " << m_event << ", Good event " << m_goodEvent << "\n" << str << "\n";

  for (i = m_stat.begin(); i != m_stat.end(); i++) {

    double rate = 0.;

    if (m_normalizeByGoodEvents) {
      if (m_goodEvent > 0) rate = 100.*(double)i->decisions/(double)m_goodEvent;
    }
    else {
      if (m_event > 0) rate = 100.*(double)i->decisions/(double)m_event;
    }

    if (i->candidates >= 0) {
      std::string outputName = "!" + i->name;
      double mult = 0;
      if (i->decisions > 0) mult = (double)i->candidates/(double)i->decisions;
      if (i->decisions > 0 || !onlyPositive) {
        if (i->avgtime > 0)
          sprintf(str," |%-61.61s|%8.4f|%10.1d|%7.3f|%8.3f|", outputName.data(),
                  rate, i->decisions, mult, i->avgtime);
        else
          sprintf(str," |%-61.61s|%8.4f|%10.1d|%7.3f|        |", outputName.data(),
                  rate, i->decisions, mult);
        info() << str << "\n";
      }
    } else if (i->decisions > 0 || !onlyPositive) {
      std::string outputName = "_" + i->name + "_";

      // Not a Selection::Line
      if (i->avgtime > 0)
        sprintf(str," |%-61.61s|%8.4f|%10.1d|       |%8.3f|", outputName.data(), rate, i->decisions, i->avgtime);
      else
        sprintf(str," |%-61.61s|%8.4f|%10.1d|       |        |", outputName.data(), rate, i->decisions);
      info() << str << "\n";
    }

  }

  info() << endmsg;

}

void StrippingReport::reportLatex(bool onlyPositive) {

  std::vector<ReportStat>::iterator i;

  char str[128];

  sprintf(str," %61.61s&%8.8s&%10.10s&%7.7s&%8.8s\\\\ \\hline", "Decision name", "Rate, \\%", "Accepted", "Mult","ms/evt");

  info() << "Event " << m_event << ", Good event " << m_goodEvent << "\n" << str << "\n";

  for (i = m_stat.begin(); i != m_stat.end(); i++) {

    double rate = 0.;

    if (m_normalizeByGoodEvents) {
      if (m_goodEvent > 0) rate = 100.*(double)i->decisions/(double)m_goodEvent;
    }
    else {
      if (m_event > 0) rate = 100.*(double)i->decisions/(double)m_event;
    }

    if (i->candidates >= 0) {
      std::string outputName = i->name;
      double mult = 0;
      if (i->decisions > 0) mult = (double)i->candidates/(double)i->decisions;
      if (i->decisions > 0 || !onlyPositive) {
        if (i->avgtime > 0)
          sprintf(str," %-61.61s&%8.4f&%10.1d&%7.3f&%8.3f\\\\", outputName.data(),
                  rate, i->decisions, mult, i->avgtime);
        else
          sprintf(str," %-61.61s&%8.4f&%10.1d&%7.3f&        \\\\", outputName.data(),
                  rate, i->decisions, mult);
        info() << str << "\n";
      }
    } else if (i->decisions > 0 || !onlyPositive) {
      std::string outputName = i->name;

      // Not a Selection::Line
      if (i->avgtime > 0)
        sprintf(str," %-61.61s&%8.4f&%10.1d&       &%8.3f\\\\", outputName.data(), rate, i->decisions, i->avgtime);
      else
        sprintf(str," %-61.61s&%8.4f&%10.1d&       &       \\\\", outputName.data(), rate, i->decisions);
      info() << str << "\n";
    }

  }

  info() << endmsg;

}



//=============================================================================
// Main execution
//=============================================================================
StatusCode StrippingReport::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  StatusCode result = StatusCode::SUCCESS;

  std::vector< ReportStat >::iterator i,j;

  char str[128];

  if (m_everyEvent) {
    sprintf(str,"%-61.61s: %4s %5s", "Decision name", "Decn", "Cands");

    info() << "----------------------------------------------------------------" << endmsg;
    info() << str << endmsg;
  }

  std::vector<int> passedVector;
  if (m_correlation)
    passedVector.assign(m_stat.size(), 0);

  for (i = m_stat.begin(); i != m_stat.end(); i++) {

    int passed = 0;
    int cand = 0;
    int executed = 0;

    i->avgtime = -1;

    IAlgorithm* myIAlg(0);

    StatusCode result = m_algMgr->getAlgorithm( i->name, myIAlg );
    if ( result.isSuccess() ) {
      Algorithm* myAlg = dynamic_cast<Algorithm*>(myIAlg);
      if (myAlg) {
        executed = myAlg->isExecuted();
        passed = myAlg->filterPassed();
      }
      Selection::Line* strAlg = dynamic_cast<Selection::Line*>(myIAlg);
      if (strAlg) cand = strAlg->numberOfCandidates();
      else {
        // Not a Selection::Line
        cand = -1;
      }

      if (m_chronoSvc) {
        verbose() << "Chrono service found" << endmsg;
        const ChronoEntity* chrono = m_chronoSvc->chrono(i->name + ":Execute");
        if (chrono) {
          verbose() << "ChronoSvc returned execute for " << i->name << endmsg;
          i->avgtime = chrono->uMeanTime()/1.e3;
        }
      }
    }

    if (cand < 0) {
      // It is a sequencer
      i->candidates = -1;
      i->decisions  += passed;

      if (m_correlation)  //Saves in a vector the decisions for THIS event
        passedVector[i - m_stat.begin()] = passed;

    } else if ( executed ) {
      // Only if the line was executed, i.e. it was not bad event, add to counters
      i->candidates += cand;
      i->decisions += passed;

      if (m_correlation)  //Saves in a vector the decisions for THIS event
        passedVector[i - m_stat.begin()] = passed;
    }


    if (m_everyEvent && (passed != 0 || cand < 0 || !m_onlyPositive)) {
      if (cand >= 0) {
        sprintf(str,"-- %-58.58s: %4.1d %5.1d", i->name.data(), passed, cand);
        info() << str << endmsg;
      } else {
        sprintf(str,"%-61.61s: %4.1d", i->name.data(), passed);
        info() << "----------------------------------------------------------------" << endmsg;
        info() << str << endmsg;
      }
    }

  }

  if (m_everyEvent) {
    info() << "----------------------------------------------------------------" << endmsg;
  }

  m_goodEvent++;

  // Fill correlation info in m_stat
  // parsing passedVector
  if (m_correlation)
    for (i = m_stat.begin(); i != m_stat.end(); i++) 
    {
      bool thisEventWasKeptAnyhow = false;
      const size_t currLineId = i - m_stat.begin();
      if (passedVector[currLineId]) //line fired
        for (j = m_stat.begin(); j != m_stat.end(); j++) 
        {
          if (j->name == m_strippingGlobalName) continue;
          const size_t compLineId = j - m_stat.begin();
          if (compLineId == currLineId) continue; //avoid testing self-correlation
          if (passedVector[compLineId])
          { //uses space of self-correlation for OR correlation of all other lines
            if ((i->candidates >= 0) == (j->candidates >= 0)) //both lines||streams
              thisEventWasKeptAnyhow = true;

            m_stat[currLineId].corr[compLineId] += passedVector[compLineId];
          }
        }

      if (thisEventWasKeptAnyhow)
        m_stat[currLineId].corr[currLineId] += 1;
    }

  if (m_reportFreq > 0 && (m_goodEvent % m_reportFreq == 0) ) {
    if (m_latex) {
      reportLatex(m_onlyPositive);
    } else {
      report(m_onlyPositive);
    }
  }

  return result;
}


void StrippingReport::correlationReport(bool linesNotStreams)
{
  std::vector<ReportStat>::iterator i;
  char line[128], header[128];
  std::string outReport;
  info() << "\n | *Line* || *Events* | *Correlated* | *Overlap (%)* | \n";
  for (i = m_stat.begin(); i != m_stat.end(); i++)
  {
    if (i->decisions == 0) continue;

    sprintf(header, " | *!%-59.59s* || %10.1d | %10.1d | %8.1f | \n", 
                    i->name.data(),             // name of the line 
                    i->decisions,               // # event selected
                    i->corr[i-m_stat.begin()],  // # event useless
                    100.*float(i->corr[i-m_stat.begin()])/i->decisions
                    );

    const bool iIsLine = (i -> candidates >= 0);
    if (iIsLine != linesNotStreams) continue;
    bool atLeastOne = false;
    for (size_t jId = 0; jId < m_stat.size(); jId++)
      if ( i->corr[jId] && jId != size_t(i - m_stat.begin())) //it is correlated
      {                                                // not with itself
        const bool jIsLine = (m_stat[jId].candidates >= 0);
        if (jIsLine != linesNotStreams) continue;

        if (atLeastOne == false) 
          outReport += header;
        atLeastOne = true;

        const float ratio = float(i->corr[jId]) / i->decisions;
        std::string color;
        if (ratio > m_correlationRedThreshold)
          color = ":red:";
        else if (ratio > m_correlationYellowThreshold)
          color = ":yellow:";
        else if (ratio > m_correlationGreenThreshold)
          color = ":green:";
        else 
          continue;


        sprintf(line, " | %-10.10s | !%-49.49s | %10.1d | %-21.21s ||\n", 
                color.data(), m_stat[jId].name.data(), i->corr[jId], "");
        outReport += line;
      }
  }
  info() << outReport << endmsg;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode StrippingReport::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // Produce selection statistics report (always show all lines, even non-responding)
  if (m_latex) {
    reportLatex(false);
  } else {
    report(false);
  }

  std::vector<ReportStat>::iterator i;

  char str[128];

  // List non-responding lines
  if (m_printNonResponding) {

    sprintf(str,"%-61.61s", "Non-responding lines:");

    warning() << endmsg;
    warning() << "-----------------------------------------------------------------" << endmsg;
    warning() << str << endmsg;

    for (i = m_stat.begin() + 1; i != m_stat.end(); i++) {
      std::string strippedName = i->name;

      if (i->candidates >= 0) {
        if (i->decisions == 0) {
          sprintf(str,"-- %-58.58s", strippedName.data());
          warning() << str << endmsg;
        }
      }
    }
    warning() << "-----------------------------------------------------------------" << endmsg;
  }

  // List hot lines
  if (m_printHot) {

    sprintf(str,"%-61.61s", "Hot lines:");

    warning() << endmsg;
    warning() << "-----------------------------------------------------------------" << endmsg;
    warning() << str << endmsg;

    for (i = m_stat.begin() + 1; i != m_stat.end(); i++) {
      std::string strippedName = i->name;

      double rate = 0.;
      if (m_normalizeByGoodEvents) {
        if (m_goodEvent > 0) rate = (double)i->decisions/(double)m_goodEvent;
      }
      else {
        if (m_event > 0) rate = (double)i->decisions/(double)m_event;
      }

      if (i->candidates >= 0) {
        if (rate > m_hotThreshold) {
          sprintf(str,"-- %-58.58s %8.6f", strippedName.data(), rate);
          warning() << str << endmsg;
        }
      }
    }
    warning() << "-----------------------------------------------------------------" << endmsg;
  }

  if (m_correlation)
  {
    correlationReport(false); //For streams
    correlationReport(true); //For lines
  }

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=======================================================================
//  Handler for BeginEvent incident: increment event counter.
//=======================================================================

void StrippingReport::handle ( const Incident& i ) {
  if ( IncidentType::BeginEvent == i.type () ) {
    ++m_event;
  }
}

//=============================================================================
