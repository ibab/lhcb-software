// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltGlobalMonitoring.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltGlobalMonitoring
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltGlobalMonitoring );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitoring::HltGlobalMonitoring( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
    , m_L0DUReportLocation ( LHCb::L0DUReportLocation::Default  )
    , m_keymu ("MuonEntry")
    , m_keyhad ("L0EntryTriggerHadron")
{
  
  // se nao tiver declarado no options, ele usa este
  declareProperty("L0DUReportLocation", m_L0DUReportLocation );

  // declareProperty("HadronAlleySelections", m_hadronalleySelections);

  //  declareProperty( "ADCconvert", m_ADCconvert = true );  
}
//=============================================================================
// Destructor
//=============================================================================
HltGlobalMonitoring::~HltGlobalMonitoring() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltGlobalMonitoring::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  info() << " Doing HltGlobalMonitoring::initialize() " << endreq;
  starthistos();

  _counter1=0;            // "All events"
  _counter2=0;           // "L0 accepted evts"
  _efficiency=0;        // "Ratio counter2/counter1"

  info() << " Declaring infos to be published " << endreq;

  declareInfo("counter1",_counter1,"All events");
  declareInfo("counter2",_counter2,"L0 accepted evts");
  declareInfo("efficiency",_efficiency,"Ratio counter2/counter1");

  declareInfo("m_histoL0",m_histoL0,"Successful L0 bits");
  declareInfo("m_histoL0corr",m_histoL0corr,"Correlated L0 bits");
  declareInfo("m_histoalleycall",m_histoalleycall,"Called Alleys");
  declareInfo("m_histoalleyacc",m_histoalleyacc,"Accepted by Alley");
  declareInfo("m_hcorrallcall",m_hcorrallcall,"Calls Correlation");
  declareInfo("m_hcorrallacc",m_hcorrallacc,"Correlated Success");

   setKey.push_back(stringKey("Hlt1MuonSingle"));
   setKey.push_back(stringKey("Hlt1MuonSingleNoIP"));
   setKey.push_back(stringKey("Hlt1MuonDiMuonWithIP"));
   setKey.push_back(stringKey("Hlt1MuonDiMuonNoIP"));
   setKey.push_back(stringKey("Hlt1MuonDiMuon2L0WithIP"));
   setKey.push_back(stringKey("Hlt1MuonDiMuon2L0NoIP"));
   setKey.push_back(stringKey("Hlt1MuonDiMuonMuonSegWithIP"));
   setKey.push_back(stringKey("Hlt1MuonDiMuonMuonSegNoIP"));
   setKeyHad.push_back(stringKey("Hlt1HadronSingle"));
   setKeyHad.push_back(stringKey("Hlt1HadronDi"));
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltGlobalMonitoring::execute() {  


  m_l0 = get<L0DUReport>(m_L0DUReportLocation);
  if (!m_l0) error() << " No L0 in TES!" << endreq;
  
  _counter1++;  // count all evts
  monitorL0();

  // IN THE EFF you can use the dataSvc() the FULL summary!
   monitorHLT();

//  monitorAlleysinput();
//  monitoracceptAlleys(sum);


  return StatusCode::SUCCESS;
}


void HltGlobalMonitoring::monitorL0() {

  // Passed ?
  // bool ok = m_l0->decision();
  // if (!ok) return;

  if (!m_l0->decision()) return;
  
  _counter2++;  // count L0 accepts

  _efficiency= float(_counter2)/_counter1;
  bool first=true;

  for (int i = 0; i<14; i+=1){ 
    if (m_l0->channelDecision(i)) {
      fillHisto(*m_histoL0, 1.*i , 1.);
      if (i!=0){ 
        for (int j = i+1; j<14; j+=1){ 
	  if (m_l0->channelDecision(j)) {
            fill( m_histoL0corr, i , j, 1.);
            if (first){ 
	      fill( m_histoL0corr, 0., 0., 1.);
	      first=false;
            } // book only one entry for a correlated trigger
	  } //if there was more than one trigger in the event
        } // loop over channels to check if there is an overlap of triggers 
      } // just to avoid entering loop for bin 0
    } // if there is a bit different of 0
  }//loop over the channels report
  
  debug() << " accepted  l0 entry " << endreq;
};

void HltGlobalMonitoring::monitorHLT() {

   monitorAlleysinput();
  // for name in m_hadronAlleySelections:
  //   if (hltsum.selectionDecision(name)):
  //       histoFill(m_histoalleyhadron,1,1)
  monitorAlleySpecific();
//  monitoracceptAlleys();
};

void HltGlobalMonitoring::monitorAlleySpecific() {
  muonalleyacc=false;
  hadronalleyacc=false;
  monitorMuonAlley();
//  monitorMuonHadAlley(sum);
  monitorHadronAlley();
//  monitorCaloAlley(sum);
};

void HltGlobalMonitoring::monitorMuonAlley() {
//Filling the histo for L0 Entry and Confirmation of the object
//   stringKey keymu("MuonEntry");
   if (dataSvc().selection(m_keymu,this).decision()) fillHisto(*m_histomuonalley, binL0entry, 1.);

//Putting the different groups of the muon alley Trigger
   bool corr=false;
   int cont1=0;
   int cont2=0;
   for (it = setKey.begin(); it != setKey.end(); it++ ){
     cont1=cont1+1;
     cont2=cont1;
     if (dataSvc().selection((*it),this).decision()){ 
       // muonalleyacc=true;
       muonalleyacc=true;
       fillHisto(*m_histomuonalley, binTrigOr+cont1, 1.);
       fill(m_histoMuonTrigCorr, binTrigOr+cont1, binTrigOr+cont1, 1.);
     
       for (it2 = it+1; it2 != setKey.end(); it2++ ){
         cont2=cont2+1;
//         info() << "Alley 1" << *it << "Alley 2" << *it2 << endreq;
//         info() << "cont 1" << cont1 << "cont 2" << cont2 << endreq;
         if (dataSvc().selection(*it,this).decision() && dataSvc().selection(*it2,this).decision()){
           fill(m_histoMuonTrigCorr, binTrigOr+cont1, binTrigOr+cont2, 1.);
           corr=true;
         } 
       }  
     }
   }   
// Filling the or bin  
if (muonalleyacc) fillHisto(*m_histomuonalley, binTrigOr, 1.);

// Or of the correlation histo
if (corr) fill(m_histoMuonTrigCorr,binTrigOr,binTrigOr,1.);
};

void HltGlobalMonitoring::monitorHadronAlley() {
//Filling the histo for L0 Entry and Confirmation of the object
//stringKey keyhad("L0EntryTriggerHadron");
if (dataSvc().selection(m_keyhad,this).decision()) fillHisto(*m_histohadronalley, binL0entry, 1.);

//Putting the different groups of the hadron alley Trigger
   bool corr=false;
   int cont1=0;
   int cont2=0;
   for (it = setKeyHad.begin(); it != setKeyHad.end(); it++ ){
     cont1=cont1+1;
     cont2=cont1;
     if (dataSvc().selection(*it,this).decision()){ 
       hadronalleyacc=true;
       fillHisto(*m_histohadronalley, binTrigOr+cont1, 1.);
       fill(m_histoHadronTrigCorr, binTrigOr+cont1, binTrigOr+cont1, 1.);
     
       for (it2 = it+1; it2 != setKeyHad.end(); it2++ ){
         cont2=cont2+1;
         info() << "Alley 1" << *it << "Alley 2" << *it2 << endreq;
         info() << "cont 1" << cont1 << "cont 2" << cont2 << endreq;
         if (dataSvc().selection(*it,this).decision() && dataSvc().selection(*it2,this).decision()){
           fill(m_histoHadronTrigCorr, binTrigOr+cont1, binTrigOr+cont2, 1.);
           corr=true;
         } 
       }  
     }
   }   
// Filling the or bin  
if (hadronalleyacc) fillHisto(*m_histohadronalley, binTrigOr, 1.);

// Or of the correlation histo
if (corr) fill(m_histoHadronTrigCorr,binTrigOr,binTrigOr,1.);
};

void HltGlobalMonitoring::monitorAlleysinput() {
//Getting the ids of the entries algos  
//   stringKey keymu("MuonEntry");
   stringKey keymuhad("MuTrackEntry");
//   stringKey keyhad("L0EntryTriggerHadron");
   stringKey keyele("EleL0Entry");
//fill overall alley entry
   bool entries=false;
   if (dataSvc().selection(m_keymu,this).decision()){ 
     fillHisto(*m_histoalleycall, binMuonAlley, 1.);
     fill(m_hcorrallcall,binMuonAlley, binMuonAlley, 1.);
     entries=true;
}
   if (dataSvc().selection(keymuhad,this).decision()){ 
     fillHisto(*m_histoalleycall, binMuonHadAlley, 1.);
     fill(m_hcorrallcall,binMuonHadAlley, binMuonHadAlley, 1.);
     entries=true;
} 
   if (dataSvc().selection(m_keyhad,this).decision()){ 
     fillHisto(*m_histoalleycall, binHadronAlley, 1.);
     fill(m_hcorrallcall,binHadronAlley, binHadronAlley, 1.);
     entries=true;
}
   if (dataSvc().selection(keyele,this).decision()){ 
     fillHisto(*m_histoalleycall, binCaloAlley, 1.);
     fill(m_hcorrallcall,binCaloAlley, binCaloAlley, 1.);
     entries=true;
}
   if (entries) fillHisto(*m_histoalleycall, binAlleyOr,1.);

// Filling the correlation histo for Alley calls
bool corr=false;
if (dataSvc().selection(m_keymu,this).decision()&&
    dataSvc().selection(keymuhad,this).decision()){
      fill(m_hcorrallcall,binMuonAlley, binMuonHadAlley,1.);
      corr=true;
}
if (dataSvc().selection(m_keymu,this).decision()&&
    dataSvc().selection(m_keyhad,this).decision()){
      fill(m_hcorrallcall,binMuonAlley,binHadronAlley,1.);
      corr=true;
}
if (dataSvc().selection(m_keymu,this).decision()&&
    dataSvc().selection(keyele,this).decision()){
      fill(m_hcorrallcall,binMuonAlley,binCaloAlley,1.);
      corr=true;
}
if (dataSvc().selection(keymuhad,this).decision()&&
    dataSvc().selection(m_keyhad,this).decision()){ 
      fill(m_hcorrallcall,binMuonHadAlley, binHadronAlley,1.);
      corr=true;
}
if (dataSvc().selection(keymuhad,this).decision()&&
    dataSvc().selection(keyele,this).decision()){ 
      fill(m_hcorrallcall,binMuonHadAlley, binCaloAlley,1.);
      corr=true;
}
if (dataSvc().selection(m_keyhad,this).decision()&&
    dataSvc().selection(keyele,this).decision()){ 
      fill(m_hcorrallcall,binHadronAlley, binCaloAlley,1.);
      corr=true;
}
// And again the OR bin for the correlation
if (corr) fill(m_hcorrallcall,binAlleyOr,binAlleyOr,1.);
};




//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltGlobalMonitoring::finalize() {

  info() << " Doing HltGlobalMonitoring::finalize() " << endreq;

  return HltBaseAlg::finalize();  
  // must be called after all other actions

}

//=============================================================================
