// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// #include "Event/L0CaloCandidate.h"
// #include "Event/L0MuonCandidate.h"

// local
#include "HltGlobalMonitor.h"
//#include "InitHistos.h"
#include "Event/HltEnums.h"
//#include "Event/HltSummary.h"
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
    , m_L0DUReportLocation ( LHCb::L0DUReportLocation::Default  )
{
  
  // se nao tiver declarado no options, ele usa este
  declareProperty("L0DUReportLocation", m_L0DUReportLocation );
  declareProperty("Hlt1Lines", m_Hlt1Lines );
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);

  // declareProperty("HadronAlleySelections", m_hadronalleySelections);

  //  declareProperty( "ADCconvert", m_ADCconvert = true );  
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

  sc = service("HistogramDataSvc", m_histosvc, true );
  if( !sc.isSuccess() ){
    fatal() << "Unable to locate HistogramSvc" << endreq;
    return sc;
  }
  
  info() << " Doing HltGlobalMonitor::initialize() " << endreq;
  
  starthistos();

  hlt1allcall=0;            // "All events"
  _counter2=0;           // "L0 accepted evts"
  _efficiency=0;        // "Ratio counter2/hlt1allcall"
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     m_allAcc.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Acc]",  m_allAcc.back(), "Hlt1 "+*i+" Alley Accepts");
     m_allCall.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Call]", m_allCall.back(), "Hlt1 "+*i+" Alley Calls");
  }
  orallacc=0;
  gpstime=0;
  gpstimesec=0;

  info() << " Declaring infos to be published " << endreq;

  m_histoalleycall = m_histosvc->book("m_histoalleycall","Alleys Called",6, 0., 6.);
  m_histoodintype = m_histosvc->book("m_histoodintype","Trigger Type",8, 0., 8.);
  m_histoodinentry = m_histosvc->book("m_histoodinentry","Trigger Type Entry",8, 0., 8.);
  m_histoL0 = m_histosvc->book("m_histoL0","L0 bits",14,0.,14.);
  
//  declareInfo("hlt1allcall",_hlt1allcall,"All events");
  declareInfo("counter2",_counter2,"L0 accepted evts");
  declareInfo("efficiency",_efficiency,"Ratio counter2/hlt1allcall");
  declareInfo("COUNTER_TO_RATE[Hlt1AlleyOr]", orallacc, "Hlt1 Alleys Or Accepts");
  declareInfo("COUNTER_TO_RATE[Hlt1Calls]",hlt1allcall,"Hlt1 Calls");
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",gpstimesec,"Gps time of last event");
   declareInfo("m_histoL0",m_histoL0,"Successful L0 bits");
   declareInfo("m_histoL0corr",m_histoL0corr,"Correlated L0 bits");
   declareInfo("m_histoalleycall", m_histoalleycall,"Physics and Random Trigger");
   declareInfo("m_histoodintype", m_histoodintype,"ODIN Trigger Type Accept");
   declareInfo("m_histoodinentry", m_histoodinentry,"ODIN Trigger Type Entries");
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
  
  hlt1allcall++;  // count all evts
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
  
  _counter2++;  // count L0 accepts

  _efficiency= float(_counter2)/hlt1allcall;
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
/*
void HltGlobalMonitor::monitorAlleySpecific(LHCb::HltSummary* sum) {
  monitorMuonAlley(sum);
  monitorMuonHadAlley(sum);
  monitorHadronAlley(sum);
  monitorCaloAlley(sum);
};

void HltGlobalMonitor::monitorMuonAlley(LHCb::HltSummary* sum) {
//Filling the histo for L0 Entry and Confirmation of the object
if (CheckDecision(sum,HltEnums::MuonEntry)) fillHisto(*m_histomuonalley, binL0entry, 1.);
if (CheckDecision(sum,HltEnums::MuonL0Conf)) fillHisto(*m_histomuonalley, binL0conf, 1.);

//Putting the different groups of the muon alley Trigger
bool groupor=false;

if (CheckDecision(sum,HltEnums::MuonTriggerSingle)){
fillHisto(*m_histomuonalley, binMuonTrigS, 1.);
fill(m_histoMuonTrigCorr, binMuonTrigS, binMuonTrigS, 1.);
groupor=true;
}

if (CheckDecision(sum,HltEnums::MuonTriggerDiMuon)){
fillHisto(*m_histomuonalley, binMuonTrigDi, 1.);
fill(m_histoMuonTrigCorr, binMuonTrigDi, binMuonTrigDi, 1.);
groupor=true;
}

if (CheckDecision(sum,HltEnums::MuonTriggerIPDiMuon)){
fillHisto(*m_histomuonalley, binMuonTrigIPDi, 1.);
fill(m_histoMuonTrigCorr, binMuonTrigIPDi, binMuonTrigIPDi, 1.);
groupor=true;
}
// Filling the or bin  
if (groupor) fillHisto(*m_histomuonalley, binTrigOr, 1.);

//Trying to put the correlation histo between Triggers
bool corr=false;
if (CheckDecision(sum,HltEnums::MuonTriggerSingle)&&
CheckDecision(sum,HltEnums::MuonTriggerDiMuon)){
fill(m_histoMuonTrigCorr,binMuonTrigS,binMuonTrigDi,1.);
corr=true;
}

if (CheckDecision(sum,HltEnums::MuonTriggerSingle)&&
CheckDecision(sum,HltEnums::MuonTriggerIPDiMuon)){
fill(m_histoMuonTrigCorr,binMuonTrigS,binMuonTrigIPDi,1.);
corr=true;
}

if (CheckDecision(sum,HltEnums::MuonTriggerIPDiMuon)&&
CheckDecision(sum,HltEnums::MuonTriggerDiMuon)){
fill(m_histoMuonTrigCorr,binMuonTrigDi,binMuonTrigIPDi,1.);
corr=true;
}
// Or of the correlation histo
if (corr) fill(m_histoMuonTrigCorr,binTrigOr,binTrigOr,1.);
};

void HltGlobalMonitor::monitorCaloAlley(LHCb::HltSummary* sum) {
//Filling the histo for Alley Entry and Confirmation of L0 object
if (CheckDecision(sum,HltEnums::CalEntry)) fillHisto(*m_histocaloalley, binL0entry, 1.);
if (CheckDecision(sum,HltEnums::CalL0Conf)) fillHisto(*m_histocaloalley, binL0conf, 1.);

//Filling Trigger of Calo Alley
if (CheckDecision(sum,HltEnums::CalTrigger)){
fillHisto(*m_histocaloalley, binTrigOr, 1.);
}

};
void HltGlobalMonitor::monitorMuonHadAlley(LHCb::HltSummary* sum) {
//Filling the histo for Alley Entry and Confirmation of L0 object
if (CheckDecision(sum,HltEnums::MuonHadEntry)) fillHisto(*m_histomuonhadalley, binL0entry, 1.);
if (CheckDecision(sum,HltEnums::MuonHadL0Conf)) fillHisto(*m_histomuonhadalley, binL0conf, 1.);

if (CheckDecision(sum,HltEnums::MuonHadTrigger)){
fillHisto(*m_histomuonhadalley, binTrigOr, 1.);
}
};

void HltGlobalMonitor::monitorHadronAlley(LHCb::HltSummary* sum) {
//Filling the histo for L0 Entry and Confirmation of the object
if (CheckDecision(sum,HltEnums::HadEntry)) fillHisto(*m_histohadronalley, binL0entry, 1.);
if (CheckDecision(sum,HltEnums::HadL0Conf)) fillHisto(*m_histohadronalley, binL0conf, 1.);

// Checking if the Hadron Alley accepted the event 
if (CheckDecision(sum,HltEnums::HadTrigger))
fillHisto(*m_histohadronalley, binTrigOr, 1.);
};
*/
void HltGlobalMonitor::monitorAlleysinput() {
// Filling bins for Alley call
//bool accep=false;
//   stringKey keymu("HltMuonAlley");
//   stringKey keymuhad("HltMuHadAlley");
//   stringKey keyhad("HltHadAlley");
 LHCb::ODIN* odin = 0;
 if (exist<LHCb::ODIN> ( LHCb::ODINLocation::Default)){
// try {
   odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
   gpstime=odin->gpsTime();
   gpstimesec=int(gpstime/1000000-904262401);
 }
 else {
// catch( const GaudiException& Exception ) {
  // m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
   this->setFilterPassed( false );
//   return Error( "ODIN missing, skipping event", StatusCode::SUCCESS );
   info() << "ODIN missing, skipping event" << endreq;
   return;
 }
   fill(m_histoodinentry, odin->triggerType(), 1.);
   if (odin->triggerType()==3){
     randallcall=randallcall+1;
   }
   else {
     physallcall=physallcall+1;
   }
  if (!exist<LHCb::HltDecReports>( m_location )){
//  if (!exist<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default )){
    warning() << "No HltDecReport" << endreq;
    return;
  }
  LHCb::HltDecReports* decisions = get<LHCb::HltDecReports>( m_location );
//  LHCb::HltDecReports* decisions = get<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
  int j=0;
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     const LHCb::HltDecReport*  decision = decisions->decReport( *i );
     if (decision == 0 ) {
       error() << "Name of teh decision is incorrect" << endreq;
       return;
     }
     if (decision->decision()) {
      fill(m_histoalleycall, j+1, 1.);
      fill(m_histoodintype, odin->triggerType(), 1.);
      m_allAcc[j]=m_allAcc[j]+1;
      orallacc=orallacc+1;
      j=j+1;
     }
  }
   debug() << "ODIN trigger type" << odin->triggerType() << endreq;
   debug() << "gps time" << odin->gpsTime() << endreq;

};
/*
void HltGlobalMonitor::monitoracceptAlleys(LHCb::HltSummary* sum) {
// trying to get the Hlt Summaries
bool accep=false;
if (CheckDecision(sum,HltEnums::MuonTriggerSingle)||
CheckDecision(sum,HltEnums::MuonTriggerDiMuon)||
CheckDecision(sum,HltEnums::MuonTriggerIPDiMuon)){ 
fillHisto(*m_histoalleyacc, binMuonAlley, 1.);
fill(m_hcorrallacc, binMuonAlley, binMuonAlley, 1.);
accep=true;
}

if (CheckDecision(sum,HltEnums::MuonHadTrigger)){
fillHisto(*m_histoalleyacc, binMuonHadAlley, 1.);
fill(m_hcorrallacc, binMuonHadAlley, binMuonHadAlley, 1.);
accep=true;
}

if (CheckDecision(sum,HltEnums::HadTrigger)){
fillHisto(*m_histoalleyacc, binHadronAlley, 1.);
fill(m_hcorrallacc, binHadronAlley, binHadronAlley, 1.);
accep=true;
}

if (CheckDecision(sum,HltEnums::CalTrigger)){
fillHisto(*m_histoalleyacc, binCaloAlley, 1.);
fill(m_hcorrallacc, binCaloAlley, binCaloAlley, 1.);
accep=true;
}

//     Filling 0 bin for the histos if any entry was made in the histo
if (accep) fillHisto(*m_histoalleyacc, binAlleyOr, 1.);

//Trying to put the correlation histo
bool corr=false;
if ((CheckDecision(sum,HltEnums::MuonTriggerSingle)||
CheckDecision(sum,HltEnums::MuonTriggerDiMuon)||
CheckDecision(sum,HltEnums::MuonTriggerIPDiMuon))&&
(CheckDecision(sum,HltEnums::MuonHadTrigger))) {
fill(m_hcorrallacc,binMuonAlley,binMuonHadAlley,1.);
corr=true;
}

if ((CheckDecision(sum,HltEnums::MuonTriggerSingle)||
CheckDecision(sum,HltEnums::MuonTriggerDiMuon)||
CheckDecision(sum,HltEnums::MuonTriggerIPDiMuon))&&
(CheckDecision(sum,HltEnums::HadTrigger))) {
fill(m_hcorrallacc,binMuonAlley,binHadronAlley,1.);
corr=true;
}

if ((CheckDecision(sum,HltEnums::MuonTriggerSingle)||
CheckDecision(sum,HltEnums::MuonTriggerDiMuon)||
CheckDecision(sum,HltEnums::MuonTriggerIPDiMuon))&&
(CheckDecision(sum,HltEnums::CalTrigger))) {
fill(m_hcorrallacc,binMuonAlley,binCaloAlley,1.);
corr=true;
}

if (CheckDecision(sum,HltEnums::MuonHadTrigger)&&
CheckDecision(sum,HltEnums::HadTrigger)) {
fill(m_hcorrallacc,binMuonHadAlley,binHadronAlley,1.);
corr=true;
}

if (CheckDecision(sum,HltEnums::MuonHadTrigger)&&
CheckDecision(sum,HltEnums::CalTrigger)) {
fill(m_hcorrallacc,binMuonHadAlley,binCaloAlley,1.);
corr=true;
}

if (CheckDecision(sum,HltEnums::HadTrigger)&&
CheckDecision(sum,HltEnums::CalTrigger)) {
fill(m_hcorrallacc,binHadronAlley,binCaloAlley,1.);
corr=true;
}

if (corr) fill(m_hcorrallacc,binAlleyOr, binAlleyOr, 1.);
};


bool HltGlobalMonitor::CheckDecision(LHCb::HltSummary* sum, int sumid){
//if (!sum->hasSelectionSummary(sumid)) return false; 
//  LHCb::HltSelectionSummary& thissummary = sum->
//      selectionSummary(sumid);
//if (thissummary.decision()) return true;
// return false;
};
*/


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltGlobalMonitor::finalize() {

  info() << " Doing HltGlobalMonitor::finalize() " << endreq;

  return HltBaseAlg::finalize();  
  // must be called after all other actions

}

//=============================================================================
