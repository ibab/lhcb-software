// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// #include "Event/L0CaloCandidate.h"
// #include "Event/L0MuonCandidate.h"

// local
#include "HltGlobalMonitor.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
#include "Event/HltDecReports.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitor
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  // se nao tiver declarado no options, ele usa este
  declareProperty("ODIN",                   m_ODINLocation = LHCb::ODINLocation::Default);
  declareProperty("L0DUReport",       m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  declareProperty("HltDecReports", m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("Hlt1Decisions", m_Hlt1Lines );

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

  info() << " Doing HltGlobalMonitor::initialize() " << endreq;
  
//L0 output histograms
//  m_histoL0 = initializeHisto("L0 bits",0.,14.,14);
  m_histoL0corr = this->book2D("Correlated L0 bits",0.,14.,14,0.,14.,14);


  m_hlt1allcall=0;            // "All events"
  m_randallcall=0;
  m_physallcall=0;
  m_counter2=0;           // "L0 accepted evts"
  m_efficiency=0;        // "Ratio counter2/hlt1allcall"

  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     m_allAcc.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Acc]",  m_allAcc.back(), "Hlt1 "+*i+" Line Accepts");
     m_allCall.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Call]", m_allCall.back(), "Hlt1 "+*i+" Line Calls");
  }
  m_orallacc=0;
  gpstime=0;
  gpstimesec=0;

  info() << " Declaring infos to be published " << endreq;

  m_histoalleycall = book1D("histoalleycall", "Hlt1 Lines Called", -0.5, m_Hlt1Lines.size()+0.5,m_Hlt1Lines.size()+1);
  m_histoodintype  = book1D("histoodintype",  "ODIN Trigger Type Accept",0., 8., 8);
  m_histoodinentry = book1D("histoodinentry", "ODIN Trigger Type Entries", 0., 8.,8);
  m_histoL0        = book1D("histoL0",        "Successfull L0 bits",0.,14.,14);
  
//  declareInfo("hlt1allcall",_hlt1allcall,"All events");
  declareInfo("counter2",m_counter2,"L0 accepted evts");
  declareInfo("efficiency",m_efficiency,"Ratio counter2/hlt1allcall");
  declareInfo("COUNTER_TO_RATE[Hlt1AlleyOr]", m_orallacc, "Hlt1 Alleys Or Accepts");
  declareInfo("COUNTER_TO_RATE[Hlt1Calls]",m_hlt1allcall,"Hlt1 Calls");
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",gpstimesec,"Gps time of last event");
  declareInfo("m_histoL0corr",m_histoL0corr,"Correlated L0 bits");

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalMonitor::execute() {  

  m_hlt1allcall++;  // count all evts

  LHCb::ODIN* odin = 0;
  if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)) {
    odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    monitorODIN(*odin);
  } else {
    debug() << "ODIN missing, skipping ODIN monitoring!" << endreq;
  }

  LHCb::L0DUReport* l0du = 0;
  if (exist<L0DUReport>(m_L0DUReportLocation)){
    l0du = get<L0DUReport>(m_L0DUReportLocation);
    monitorL0DU(*l0du);
  } else {
    debug() << "L0DU missing, skipping L0 monitoring" << endreq;
  }

  LHCb::HltDecReports* decisions = 0;
  if (exist<LHCb::HltDecReports>( m_HltDecReportsLocation )){
    decisions = get<LHCb::HltDecReports>( m_HltDecReportsLocation );
    monitorHLT(*decisions);
  } else {
    debug() << "HltDecReports missing, skipping HLT monitoring" << endreq;
  }
  return StatusCode::SUCCESS;
}

void HltGlobalMonitor::monitorODIN(const LHCb::ODIN& odin) {
  gpstime=odin.gpsTime();
  debug() << "gps time" << gpstime << endreq;
  gpstimesec=int(gpstime/1000000-904262401);
  fill(m_histoodinentry, odin.triggerType(), 1.);
  if (odin.triggerType()==ODIN::RandomTrigger) ++m_randallcall;
  else                                         ++m_physallcall;

   //if (anyAccept) { 
      ++m_orallacc;
      fill(m_histoalleycall, 0, 1.);
      fill(m_histoodintype, odin.triggerType(), 1.);
   //}
   debug() << "ODIN trigger type" << odin.triggerType() << endreq;
}

void HltGlobalMonitor::monitorL0DU(const LHCb::L0DUReport& l0) {

  // Passed ?
  // bool ok = l0.decision();
  // if (!ok) return;

  if (l0.decision()) debug() << "L0 is true" << endreq; 
  if (l0.forceBit()) debug() << "L0 is forced" << endreq; 

  if (!l0.decision()) return;
  
  m_counter2++;  // count L0 accepts

  m_efficiency= double(m_counter2)/m_hlt1allcall;
//  bool first=true;

  for (int i = 0; i<14; i+=1){ 
    if (l0.channelDecision(i)) {
      fill(m_histoL0, i , 1.);
//      if (i!=0){ 
//        for (int j = i+1; j<14; j+=1){ 
//	  if (l0.channelDecision(j)) {
//            fill( m_histoL0corr, i , j, 1.);
 //           if (first){ 
//	      fill( m_histoL0corr, 0., 0., 1.);
//	      first=false;
 //           } // book only one entry for a correlated trigger
//	  } //if there was more than one trigger in the event
 //       } // loop over channels to check if there is an overlap of triggers 
  //    } // just to avoid entering loop for bin 0
    } // if there is a bit different of 0
  }//loop over the channels report
  
};

void HltGlobalMonitor::monitorHLT(const HltDecReports& decisions) {


  int j=0;
  bool anyAccept=false;
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     const LHCb::HltDecReport*  decision = decisions.decReport( *i );
     if (decision == 0 ) {
       warning() << "decision " << *i << " not found" << endreq;
     } else if (decision->decision()) {
      anyAccept = true;
      fill(m_histoalleycall, j+1, 1.);
      ++m_allAcc[j];
     }
     j=j+1;
  }
#if 0
   // create list of all positive decisions
   std::vector<std::pair<unsigned,const LHCb::HltDecReport*> > reps;
   reps.reserve( m_Hlt1Lines.size() );

   for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     const LHCb::HltDecReport*  report = decisions->decReport( *i );
     if (report == 0 ) {  
        warning() << "report " << *i << " not found" << endreq;
        continue;
     }
     if (!decision->decision()) continue;
     size_t offset =  i - m_Hlt1Lines.begin();
     reps.push_back( make_pair( offset, report ) );
   }
   for (size_t i = 0; i < reps.size();++i) {
        bool exclusive = (reps.size()==1);
        plot1D( exclusive ? x : y , reps.first, 1 ) // plot the in/exclusive accepts
   }
#endif
};



//=============================================================================

#if 0
// taken from $L0DUROOT/src/L0DUReportMonitor.cpp 
// correlations
      if( report->channelDecision( id ) ){
        for(LHCb::L0DUChannel::Map::iterator jt = channels.begin() ;jt!=channels.end();jt++){

   }


        other = true
        plot2D(    
        
     }
     // plot plain rate
     plot1D( 
     // plot the inclusive rate for each line
     if (other)  { plot1D }
     // plot the exclusive rate for each line
     else { plot1D }
   }
};

//=============================================================================

// taken from $L0DUROOT/src/L0DUReportMonitor.cpp 
// correlations
      if( report->channelDecision( id ) ){
        for(LHCb::L0DUChannel::Map::iterator jt = channels.begin() ;jt!=channels.end();jt++){
          int jd = ((*jt).second)->id() ;
          if( report->channelDecision( jd ) ){
            // inclusive 2D counters
            plot2D( (double) id , (double) jd , base.str() + "/L0Channels/Counters2D/1"
                    , "L0DU Channels Decision 2D INclusive counters (TCK = " + ttck.str() + ")"
                    , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
            //exclusive 2D counters
            bool isX = true;
            for(LHCb::L0DUChannel::Map::iterator kt = channels.begin();kt!=channels.end();kt++){
              int kd = ((*kt).second)->id() ;
              if( kd == id || kd == jd)continue;
              if( report->channelDecision( kd ) ){
                isX = false;
                break;
              }
            }
            if(isX)plot2D( (double) id , (double) jd , base.str() + "/L0Channels/Counters2D/2"
                           , "L0DU Channels Decision 2D EXclusive counters (TCK = " + ttck.str() + ")"
                           , -1. ,(double) cBin, -1. ,(double) cBin , cBin+1 , cBin+1);
          }
        }
      }
#endif
