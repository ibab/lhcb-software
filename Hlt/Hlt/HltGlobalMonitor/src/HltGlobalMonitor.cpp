// $Id: HltGlobalMonitor.cpp,v 1.48 2010-01-11 13:43:25 graven Exp $
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
#include "boost/assign/list_of.hpp"
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"

// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"
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
  , m_startClock(0)
  , m_startEvent(0)
  , m_currentTime(0), m_gpstimesec(0), m_virtmem(0), m_events(0), m_lastL0TCK(0)
{
  declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
  declareProperty("L0DUReport",        m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  declareProperty("HltDecReports",     m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("Hlt1Decisions",     m_Hlt1Lines );
  declareProperty("ScanEvents",        m_scanevents = 1 );
  declareProperty("TotalMemory",       m_totalmem   = 3000 );
  declareProperty("TimeSize",          m_timeSize = 120 );   // number of minutes of history (half an hour)
  declareProperty("TimeInterval",      m_timeInterval = 1 ); // binwidth in minutes 
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

  m_gpstimesec=0;
  m_startClock = System::currentTime( System::microSec );
  

  m_L0Input         = book1D("L0 channel",-0.5,18.5,19);
  // this code may break when the enums are no longer directly exposing the hardware
  m_odin            = book1D("ODIN trigger type",  "ODIN trigger Type ",-0.5, 7.5, 8);
  std::vector<std::pair<unsigned,std::string> > odinLabels = boost::assign::list_of< std::pair<unsigned,std::string> >
                (ODIN::PhysicsTrigger,    "Physics")
                (ODIN::BeamGasTrigger,    "BeamGas")
                (ODIN::LumiTrigger,       "Lumi")
                (ODIN::TechnicalTrigger,  "Technical")
                (ODIN::AuxiliaryTrigger,  "Auxiliary")
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
      std::vector<std::string> strip = boost::assign::list_of<std::string>("Hlt1")("Decision"); //TODO: replace with regex with capture
      for (std::vector<std::string>::const_iterator j = strip.begin();j!=strip.end();++j) {
     	std::string::size_type k =  s.find(*j);
     	if (k != std::string::npos) s.erase(k,k+j->size());
      }
      labels.push_back(s);  
}

  m_hltNAcc         = book1D("# positive HltLines ", -0.5,m_Hlt1Lines.size()+0.5, m_Hlt1Lines.size()+1);
  m_hltInclusive    = book1D("HltLines Inclusive",   -0.5,m_Hlt1Lines.size()-0.5, m_Hlt1Lines.size());

  if (!setBinLabels( m_hltInclusive,  labels )) {
    error() << "failed to set binlables on inclusive hist" << endmsg;
  }
  
  m_hltCorrelations = book2D("HltLines Correlations",-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size(),-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size());

   if (!setBinLabels( m_hltCorrelations, labels, labels )) {
    error() << "failed to set binlables on correlation hist" << endmsg;
  }


  m_hltVirtTime  = bookProfile1D("Virtual memory",   0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
  setAxisLabels( m_hltVirtTime, "time since start of run [min]", "memory[MB]");

  m_hltEventsTime  = bookProfile1D("average time per event", 0,m_timeSize,int(m_timeSize/m_timeInterval+0.5));
  setAxisLabels( m_hltEventsTime, "time since start of run [min]", "average time/event [ms]");

  m_hltTime  = book1D("time per event ", -1, 4 );
  setAxisLabels( m_hltTime, "log10(time/event/ms)", "events");

  declareInfo("COUNTER_TO_RATE[virtmem]", m_virtmem, "Virtual memory");
  declareInfo("COUNTER_TO_RATE[elapsed time]", m_currentTime, "Elapsed time");
  

  declareInfo("COUNTER_TO_RATE[L0Accept]",counter("L0Accept"),"L0Accept");
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",m_gpstimesec,"Gps time of last event");

  for (unsigned i=0; i!=m_GroupLabels.size();++i) {
    m_alley.push_back(&counter(m_GroupLabels.at(i)));
    declareInfo("COUNTER_TO_RATE["+m_GroupLabels.at(i)+"]", *m_alley.back(),m_GroupLabels.at(i)+" Alley");
  }

  // register for incidents...
  IIncidentSvc* incidentSvc = svc<IIncidentSvc>( "IncidentSvc" );
  // insert at high priority, so we also time what happens during BeginEvent incidents...
  long priority = std::numeric_limits<long>::max();
  incidentSvc->addListener(this,IncidentType::BeginEvent, priority ) ;
  incidentSvc->addListener(this,IncidentType::BeginRun, 0 ) ;

  // start by kicking ourselves in action -- just in case we don't get one otherwise...
  this->handle(Incident(name(), IncidentType::BeginRun ));

  return StatusCode::SUCCESS;
};

void HltGlobalMonitor::handle ( const Incident& incident ) {
  m_startEvent = System::currentTime( System::microSec );
  if (m_startClock == 0 || incident.type() == IncidentType::BeginRun) m_startClock = m_startEvent;
  m_currentTime = double(m_startEvent-m_startClock)/1000000 ;  // seconds
}

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
  m_gpstimesec=int(gpstime/1000000-904262401); //@TODO: is this still OK with ODIN v6?
  counter("ODIN::Lumi")    += (odin->triggerType()==ODIN::LumiTrigger);
  counter("ODIN::NotLumi") += (odin->triggerType()!=ODIN::LumiTrigger);
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
  for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
      fill( m_L0Input, i->second->id(), l0du->channelDecision( i->second->id() ) );
  }

  unsigned int L0TCK = l0du->tck();
  if (L0TCK != m_lastL0TCK && m_L0Input!=0) {
      std::vector< std::pair<unsigned, std::string> > labels;
      for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
        labels.push_back(std::make_pair( i->second->id(), i->first ));
      }
      setBinLabels( m_L0Input, labels );
      m_lastL0TCK = L0TCK;
  }
};

void HltGlobalMonitor::monitorHLT(const LHCb::ODIN*,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {


  if (hlt==0) return;

  ///////////////////////////////////////////////////////////////////////////////
  std::vector<std::pair<std::string,const LHCb::HltDecReport*> > reps;
  unsigned nAcc = 0;
  std::vector<unsigned> nAccAlley(m_GroupLabels.size(),unsigned(0));

  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();i++) {
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {  
       warning() << "report " << *i << " not found" << endreq;
    }
    reps.push_back( std::make_pair( *i, report ) );
    if (report && report->decision()){
      ++nAcc;
      ++nAccAlley[m_DecToGroup.find(*i)->second];
    }
  }

  for (unsigned i=0; i<m_GroupLabels.size();i++) {
    *m_alley[i] += ( nAccAlley[i] > 0 );
    fill(m_hlt1alley,i,(nAccAlley[i]>0));
  }

  fill( m_hltNAcc, nAcc, 1.0);  //by how many lines did the current event get accepted?
 
  for (size_t i = 0; i<reps.size();++i) {
    bool accept = reps[i].second->decision();
    fill( m_hltInclusive, i, accept);
    if (!accept) continue;
    if (nAcc==1) fill( m_hltExclusive, i, accept );
    for (size_t j = 0; j<reps.size(); ++j) fill(m_hltCorrelations,i,j,accept);
  }

}



void HltGlobalMonitor::monitorMemory() {

  double elapsedTime = double(System::currentTime( System::microSec ) - m_startEvent);
  double t = log10(elapsedTime)-3; // convert to log(time/ms)
  fill( m_hltTime, t ,1.0); 
  storeTrend(m_hltEventsTime, elapsedTime/1000 );

  //TODO: only need to do this once 'per bin'  interval...
  m_virtmem  = virtualMemory(System::MByte, System::Memory);
  storeTrend(m_hltVirtTime, double(m_virtmem));

 
}

//=============================================================================
void HltGlobalMonitor::storeTrend(AIDA::IProfile1D* h, double Value) 
{
#ifdef TODO_IMPLEMENT_SHIFT_USING_AIDA2ROOT_ON_A_PROFILE_HISTOGRAM
  double offset = m_currentTime - m_trendLHSEdge;
  if ( offset > ... ) {
      const AIDA::IAxis & axis = h->axis();
      long bins = axis.bins();
      for ( long i = 0; i < bins; ++i ) {
        double binValue = h->binHeight(i);
        double nextValue = ( i < bins - 1 ) ? h->binHeight(i+1)
                                            : Value;
        double x = 0.5*(axis.binUpperEdge(i)+axis.binLowerEdge(i));
        h->fill(x, nextValue - binValue);
      }
       update m_trendLHSEdge
       shift bins to left
  }
#endif
  h->fill(m_currentTime/60, Value); // go from seconds -> minutes
}
