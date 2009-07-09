// $Id: HltGlobalMonitor.cpp,v 1.38 2009-07-09 09:02:00 kvervink Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// BOOST
// ============================================================================
#include "boost/regex.hpp"
#include "boost/assign/list_of.hpp"
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
// ============================================================================
// GaudiUtils
// ============================================================================
#include "GaudiUtils/HistoStats.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// Memory Usage
// ============================================================================
#include "GaudiKernel/Timing.h"
#include "GaudiKernel/Memory.h"
// ============================================================================
// HLT
// ============================================================================
#include "HltBase/HltHistogramUtilities.h"
using namespace Hlt::HistogramUtilities;
// ============================================================================
// local
// ============================================================================
#include "HltGlobalMonitor.h"
// ============================================================================

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitor );

// utilities


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
    , m_gpstimesec(0), m_time(0), m_virtmem(0), m_events(0)
{
  declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
  declareProperty("L0DUReport",        m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  declareProperty("HltDecReports",     m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("Hlt1Decisions",     m_Hlt1Lines );
  declareProperty("ScanEvents",        m_scanevents = 200 );
  declareProperty("TotalMemory",       m_totalmem   = 3000 );
  declareProperty("TimeSize",          m_timeSize = 100 );
  declareProperty("TimeInterval",      m_timeInterval = 1 );
  declareProperty("DecToGroup",        m_DecToGroup);
  declareProperty("GroupLabels",       m_GroupLabels);
}
//=============================================================================
// Destructor
//=============================================================================
HltGlobalMonitor::~HltGlobalMonitor() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGlobalMonitor::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_L0Input         = book1D("L0 channel",-0.5,18.5,19);
  m_odin            = book1D("ODIN type",  "ODIN Type ",-0.5, 7.5, 8);
  std::vector<std::pair<unsigned,std::string> > odinLabels = boost::assign::list_of< std::pair<unsigned,std::string> >
                (ODIN::Reserve,           "Reserve")
                (ODIN::PhysicsTrigger,    "Physics")
                (ODIN::AuxilliaryTrigger, "Auxilliary")
                (ODIN::RandomTrigger,     "Random")
                (ODIN::PeriodicTrigger,   "Periodic")
                (ODIN::NonZSupTrigger,    "NonZSup")
                (ODIN::TimingTrigger,     "Timing")
                (ODIN::CalibrationTrigger,"Calibration");
   if (!setBinLabels( m_odin, odinLabels )) {
    error() << "failed to set binlables on ODIN hist" << endmsg;
  }

  // create a histogram with one bin per Alley
  // the order and the names for the bins are
  // configured in HLTConf/Configuration.py  
  m_hlt1alley       = book1D("Hlt1 Alleys", "Hlt1 Alleys", -0.5, m_GroupLabels.size()-0.5 , m_GroupLabels.size() );
  if (!setBinLabels( m_hlt1alley, m_GroupLabels )) {
    error() << "failed to set binlables on Alley hist" << endmsg;
  }

  std::vector<std::string> labels;
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
      std::string s = *i;
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision");
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labels.push_back(s);  
  }

  m_hltNAcc         = book1D("# positive HltLines ", -0.5,m_Hlt1Lines.size()+0.5,
                             m_Hlt1Lines.size()+1);
  m_hltInclusive    = book1D("HltLines Inclusive",   -0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size());

  if (!setBinLabels( m_hltInclusive,  labels )) {
    error() << "failed to set binlables on inclusive hist" << endmsg;
  }
  
  m_hltCorrelations = book2D("HltLines Correlations",-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size(),-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size());

   if (!setBinLabels( m_hltCorrelations, labels, labels )) {
    error() << "failed to set binlables on correlation hist" << endmsg;
  }


  m_hltVirtinTime  = book1D("Virtual memory per event",   -m_timeSize*m_timeInterval, 0, m_timeSize );
  setAxisLabels( m_hltVirtinTime, "time", "memory[MB]");

  m_hltVirtMem    = book1D("Virtual Memory",   -0.5, (double)m_totalmem, m_totalmem);
  setAxisLabels( m_hltVirtMem, "memory[MB]", "");

  m_hltEventsTime  = book1D("time per event", -m_timeSize*m_timeInterval, 0, m_timeSize );
  setAxisLabels( m_hltEventsTime, "time", "time/event[min]");

  m_hltTime  = book1D("time per event dist", -1000.0, 1000.0, 2000 );
  setAxisLabels( m_hltTime, "time/event", "events");

  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     m_allAcc.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+" Acc]",  m_allAcc.back(),  "Hlt1 "+*i+" Line Accepts");
     m_allCall.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+" Call]", m_allCall.back(), "Hlt1 "+*i+" Line Calls");
  }

  declareInfo("COUNTER_TO_RATE[virtmem]", m_virtmem, "Virtual memory");
  declareInfo("COUNTER_TO_RATE[elapsed time]", (double)m_time, "Elapsed time");
  
  m_gpstimesec=0;

  declareInfo("L0Accept",        "",&counter("L0Accept"),        0,std::string("L0Accept"));
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",m_gpstimesec,"Gps time of last event");

  declareInfo("#eventsHLT","",&counter("#events"),0,std::string("Events hlt1 input"));
  declareInfo("#acceptHLT","",&counter("#accept"),0,std::string("Events hlt1 accepted"));

  //klo1
  for (uint i=0; i!=m_GroupLabels.size();++i) {
    m_allAlleyAcc.push_back(0);
    declareInfo("COUNTER_TO_RATE["+m_GroupLabels.at(i)+" Acc]", m_allAlleyAcc.back(), "Hlt1 "+m_GroupLabels.at(i)+" Alley Accepts");
    m_allAlleyCall.push_back(0);
    declareInfo("COUNTER_TO_RATE["+m_GroupLabels.at(i)+" Call]", m_allAlleyCall.back(), "Hlt1 "+m_GroupLabels.at(i)+" Alley Calls");
  }
  //klo2

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalMonitor::execute() {  

  LHCb::ODIN*         odin = fetch<LHCb::ODIN>( LHCb::ODINLocation::Default);
  LHCb::L0DUReport*   l0du = fetch<LHCb::L0DUReport>( m_L0DUReportLocation );
  LHCb::HltDecReports* hlt = fetch<LHCb::HltDecReports>( m_HltDecReportsLocation );

  monitorODIN(odin,l0du,hlt);
  monitorL0DU(odin,l0du,hlt);
  monitorHLT(odin,l0du,hlt);

 if(  (m_events)%m_scanevents ==0) monitorMemory();

  counter("#events")   += 1.0;
  m_events             += 1;

  return StatusCode::SUCCESS;
  
}

void HltGlobalMonitor::monitorODIN(const LHCb::ODIN* odin,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {
  if (odin == 0 ) return;
  unsigned long long gpstime=odin->gpsTime();
  if (msgLevel(MSG::DEBUG)) debug() << "gps time" << gpstime << endreq;
  m_gpstimesec=int(gpstime/1000000-904262401);
  counter("ODIN::Random")    += (odin->triggerType()==ODIN::RandomTrigger);
  counter("ODIN::NotRandom") += (odin->triggerType()!=ODIN::RandomTrigger);
  fill(m_odin, odin->triggerType(), 1.);
  if ( hlt == 0 ) return;


}


void HltGlobalMonitor::monitorL0DU(const LHCb::ODIN*,
                                   const LHCb::L0DUReport* l0du,
                                   const LHCb::HltDecReports* ) {
  if (l0du == 0) return;

  counter("L0Accept") += l0du->decision();
  counter("L0Forced") += l0du->forceBit();

  if (!l0du->decision()) return;

  LHCb::L0DUChannel::Map channels = l0du->configuration()->channels();
  std::vector<std::pair<std::string, int> > repsL;
  
  for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
    bool acc = l0du->channelDecision( i->second->id() );
    
    repsL.push_back(std::make_pair( i->first, i->second->id()));
    fill( m_L0Input  , i->second->id(), acc );
  }
  
  //TODO: only do this _once_... if the L0TCK changes...
  if ( 0 != m_L0Input ) {
        std::vector< std::pair<unsigned, std::string> > labels;
        for ( unsigned ibin = 0; ibin < repsL.size() ; ++ibin ) {
          debug() << "ibin = " << ibin << " repsL[ibin].first = " << repsL[ibin].first <<
            " repsL[ibin].second = " << repsL[ibin].second << endreq;
          labels.push_back(std::make_pair( repsL[ibin].second , repsL[ibin].first));
        }
        setBinLabels( m_L0Input, labels );
   }
};

void HltGlobalMonitor::monitorHLT(const LHCb::ODIN*,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {


  if (hlt==0) return;

  ///////////////////////////////////////////////////////////////////////////////
  std::vector<std::pair<std::string,const LHCb::HltDecReport*> > reps;
  unsigned nAcc = 0;
  nAccAlley = (unsigned *)calloc(m_GroupLabels.size(),sizeof(unsigned));//klo
  //for (uint i=0; i<m_GroupLabels.size();++i) {nAccAlley[i]=0;}//klo

  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();i++) {
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {  
       warning() << "report " << *i << " not found" << endreq;
    }
    reps.push_back( std::make_pair( *i, report ) );
    if (report && report->decision()){
      ++nAcc;
      ++nAccAlley[m_DecToGroup.find(*i)->second];//klo
    }
  }

  //klo1
  for (uint i=0; i<m_GroupLabels.size();i++) {
    m_allAlleyCall[i]++;
    m_allAlleyAcc[i] += ( nAccAlley[i] > 0 );
  }
  //klo2

  counter("#accept") += ( nAcc > 0 );

  fill( m_hltNAcc, nAcc, 1.0);  //by how many lines did 1 event get accepted?
 
  for (size_t i = 0; i<reps.size();++i) {

    ++m_allCall[i];
    fill( m_hltInclusive, i, reps[i].second->decision());
    if (!reps[i].second->decision()) continue;
    ++m_allAcc[i];
   
    if (nAcc==1) fill( m_hltExclusive, i, reps[i].second->decision() );

    for (size_t j = 0; j<reps.size(); ++j) {
      fill(m_hltCorrelations,i,j,reps[j].second->decision());
    }
  }

  // m_DecToGroup is a mapping of a Decision to the BinNumber
  // of the Group it should be accounted for
  // Together with the BinLabels this is configured
  // in HLTConf/Configuration.py
  
  /*
  COMMENTED OUT BY KLAUS
  m_DecToGroupType::iterator end   = m_DecToGroup.end();
  m_DecToGroupType::iterator found = m_DecToGroup.end();
  for(size_t j = 0; j < reps.size(); ++j) {
    found = m_DecToGroup.find(reps[j].first);
    if (found != end) {
      fill(m_hlt1alley, found->second, reps[j].second->decision());
    }
  }
  */
  //klo1
  for (uint i=0; i<m_GroupLabels.size();i++) {
    fill(m_hlt1alley,i,(nAccAlley[i]>0));
  }
  std::free(nAccAlley);
  //klo2
}



void HltGlobalMonitor::monitorMemory() {

  //should this also come in the configuration.py?

  m_time     = ellapsedTime(System::Min, System::Times);
  m_virtmem  = virtualMemory(System::MByte, System::Memory);
  
  storeTrend(m_hltVirtinTime,m_virtmem);

  fill(m_hltVirtMem, m_virtmem, 1);

  if(counter("#events").nEntries() >0){
    fill(m_hltTime, (double)m_time/(double)(counter("#events").nEntries()), 1);    
    storeTrend(m_hltEventsTime, (double)m_time/(double)(counter("#events").nEntries()));

  }
 
}

//=============================================================================
void HltGlobalMonitor::storeTrend(AIDA::IHistogram1D* theHist, double Value) 
{
  const AIDA::IAxis & axis = theHist->axis();
  long bins = axis.bins();
  for ( long i = 0; i < bins; ++i ) {
    double binValue = theHist->binHeight(i);
    double nextValue = ( i < bins - 1 ) ? theHist->binHeight(i+1)
                                        : Value;
    double x = 0.5*(axis.binUpperEdge(i)+axis.binLowerEdge(i));
    theHist->fill(x, nextValue - binValue);
  }
}
