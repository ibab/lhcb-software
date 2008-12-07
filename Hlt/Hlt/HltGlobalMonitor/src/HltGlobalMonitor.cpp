// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// #include "Event/L0CaloCandidate.h"
// #include "Event/L0MuonCandidate.h"

// local
#include "HltGlobalMonitor.h"
#include "Event/ODIN.h"
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
  declareProperty("L0DUReportLocation", m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  declareProperty("Hlt1Decisions", m_Hlt1Lines );
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);

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
  
  starthistos();

  m_hlt1allcall=0;            // "All events"
  m_randallcall=0;
  m_physallcall=0;
  m_counter2=0;           // "L0 accepted evts"
  m_efficiency=0;        // "Ratio counter2/hlt1allcall"

  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     m_allAcc.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Acc]",  m_allAcc.back(), "Hlt1 "+*i+" Alley Accepts");
     m_allCall.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Call]", m_allCall.back(), "Hlt1 "+*i+" Alley Calls");
  }
  m_orallacc=0;
  gpstime=0;
  gpstimesec=0;

  info() << " Declaring infos to be published " << endreq;

  //TODO: replace by book1D, which does book+declareInfo
  m_histoalleycall = histoSvc()->book("m_histoalleycall","Lines Called",m_Hlt1Lines.size()+1, -0.5, m_Hlt1Lines.size()+0.5);
  declareInfo("m_histoalleycall", m_histoalleycall,"Hlt1 Lines Accept ");

  m_histoodintype = histoSvc()->book("m_histoodintype","Trigger Type",8, 0., 8.);
  declareInfo("m_histoodintype", m_histoodintype,"ODIN Trigger Type Accept");

  m_histoodinentry = histoSvc()->book("m_histoodinentry","Trigger Type Entry",8, 0., 8.);
  declareInfo("m_histoodinentry", m_histoodinentry,"ODIN Trigger Type Entries");

  m_histoL0 = histoSvc()->book("m_histoL0","L0 bits",14,0.,14.);
  declareInfo("m_histoL0",m_histoL0,"Successful L0 bits");
  
//  declareInfo("hlt1allcall",_hlt1allcall,"All events");
  declareInfo("counter2",m_counter2,"L0 accepted evts");
  declareInfo("efficiency",m_efficiency,"Ratio counter2/hlt1allcall");
  declareInfo("COUNTER_TO_RATE[Hlt1AlleyOr]", m_orallacc, "Hlt1 Alleys Or Accepts");
  declareInfo("COUNTER_TO_RATE[Hlt1Calls]",m_hlt1allcall,"Hlt1 Calls");
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",gpstimesec,"Gps time of last event");
   declareInfo("m_histoL0corr",m_histoL0corr,"Correlated L0 bits");
//   declareInfo("m_histoalleyacc",m_histoalleyacc,"Accepted by Alley");
//   declareInfo("m_hcorrallcall",m_hcorrallcall,"Calls Correlation");
//   declareInfo("m_hcorrallacc",m_hcorrallacc,"Correlated Success");

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalMonitor::execute() {  


//  m_l0 = get<L0DUReport>(m_L0DUReportLocation);
//  if (!m_l0) error() << " No L0 in TES!" << endreq;
  
  m_hlt1allcall++;  // count all evts
//  monitorL0();

 //  LHCb::HltSummary* sum = get<LHCb::HltSummary>(LHCb::
//        HltSummaryLocation::Default);
   monitorHLT();

//  monitorAlleysinput(sum);
//  monitoracceptAlleys(sum);


  return StatusCode::SUCCESS;
}


void HltGlobalMonitor::monitorL0() {

  // Passed ?
  // bool ok = m_l0->decision();
  // if (!ok) return;
  L0DUReport* m_l0 =0;
  if (exist<L0DUReport>(m_L0DUReportLocation)){
    m_l0 = get<L0DUReport>(m_L0DUReportLocation);
  }
  else {
   debug() << "L0DU missing, skipping L0 mon" << endreq;
   return;
  }

  if (!m_l0) error() << " No L0 in TES!" << endreq;
  if (m_l0->decision()) debug() << "L0 is true" << endreq; 
  if (m_l0->forceBit()) debug() << "L0 is forced" << endreq; 

  if (!m_l0->decision()) return;
  
  m_counter2++;  // count L0 accepts

  m_efficiency= double(m_counter2)/m_hlt1allcall;
//  bool first=true;

  for (int i = 0; i<14; i+=1){ 
    if (m_l0->channelDecision(i)) {
      fill(m_histoL0, i , 1.);
//      if (i!=0){ 
//        for (int j = i+1; j<14; j+=1){ 
//	  if (m_l0->channelDecision(j)) {
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
  
  debug() << " accepted  l0 entry " << endreq;
};

void HltGlobalMonitor::monitorHLT() {

//  monitorAlleySpecific(sum);
  monitorAlleysinput();
//  monitoracceptAlleys(sum);
};

void HltGlobalMonitor::monitorAlleysinput() {
// Filling bins for Alley call
//bool accep=false;
  LHCb::ODIN* odin = 0;
  if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)){
    odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    gpstime=odin->gpsTime();
    gpstimesec=int(gpstime/1000000-904262401);
  } else {
    this->setFilterPassed( false );
    info() << "ODIN missing, skipping event" << endreq;
    return;
  }
  fill(m_histoodinentry, odin->triggerType(), 1.);
  if (odin->triggerType()==ODIN::RandomTrigger){
    ++m_randallcall ;
  } else {
    ++m_physallcall ;
  }
  if (!exist<LHCb::HltDecReports>( m_location )){
    error() << "No HltDecReport" << endreq;
    return;
  }
  LHCb::HltDecReports* decisions = get<LHCb::HltDecReports>( m_location );
  int j=0;
  bool anyAccept=false;
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     const LHCb::HltDecReport*  decision = decisions->decReport( *i );
     if (decision == 0 ) {
       warning() << "decision " << *i << " not found" << endreq;
     } else if (decision->decision()) {
      anyAccept = true;
      fill(m_histoalleycall, j+1, 1.);
      ++m_allAcc[j];
     }
     j=j+1;
  }
   if (anyAccept) { 
      ++m_orallacc;
      fill(m_histoalleycall, 0, 1.);
      fill(m_histoodintype, odin->triggerType(), 1.);
   }
   debug() << "ODIN trigger type" << odin->triggerType() << endreq;
   debug() << "gps time" << odin->gpsTime() << endreq;

};


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltGlobalMonitor::finalize() {
  info() << " Doing HltGlobalMonitor::finalize() " << endreq;
  // must be called after all other actions
  return HltBaseAlg::finalize();  
}

//=============================================================================

#if 0
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
