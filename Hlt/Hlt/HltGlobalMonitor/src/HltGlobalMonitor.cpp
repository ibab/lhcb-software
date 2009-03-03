// $Id: HltGlobalMonitor.cpp,v 1.21 2009-03-03 11:04:45 graven Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
// ============================================================================
// GaudiUtils
// ============================================================================
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiUtils/HistoStats.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// ROOT
// ============================================================================
#include "TH1D.h"
#include "TH2D.h"
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
  , m_gpstimesec(0)
  , m_TriggerTisTosTool(0)

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

  m_L0Input         = book1D("L0 channel",-0.5,16.5,17);
  m_odin            = book1D("ODIN type",  "ODIN Type ",0., 4., 4);
  m_hlt1alley       = book1D("Hlt1 Alleys", "Hlt1 Alleys", -0.5, 15.5 , 16 );
  m_hltAcc          = book1D("Hlt1 lines Accept", "Hlt1 Lines Accept",
                             -0.5, m_Hlt1Lines.size()+0.5,m_Hlt1Lines.size()+1);
  m_hltNAcc         = book1D("# positive HltLines ", -0.5,m_Hlt1Lines.size()+0.5,
                             m_Hlt1Lines.size()+1);
  m_hltInclusive    = book1D("HltLines Inclusive",   -0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size());
  m_hltCorrelations = book2D("HltLines Correlations",-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size(),-0.5,m_Hlt1Lines.size()-0.5,
                             m_Hlt1Lines.size());



  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     m_allAcc.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Acc]",  m_allAcc.back(),  "Hlt1 "+*i+" Line Accepts");
     m_allCall.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Call]", m_allCall.back(), "Hlt1 "+*i+" Line Calls");
  }
  m_gpstimesec=0;

  m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos", this); 
  if ( !m_TriggerTisTosTool ) {   
    fatal() << "TriggerTisTosTool could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  
  declareInfo("L0Accept",        "",&counter("L0Accept"),        0,std::string("L0Accept"));
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",m_gpstimesec,"Gps time of last event");

  return StatusCode::SUCCESS;
};

//=============================================================================
// Finalize
//=============================================================================
StatusCode HltGlobalMonitor::finalize() {
  return HltBaseAlg::finalize();
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


  //labels
  if ( 0 != m_odin )
  {
    TH1D *ho = Gaudi::Utils::Aida2ROOT::aida2root( m_odin );  
    if ( 0 != ho) {
      TAxis* axiso = ho->GetXaxis() ;
      if( 0 != axiso){
        if(odin->triggerType()==ODIN::RandomTrigger){
          axiso-> SetBinLabel (odin ->triggerType()+1, "Random");
        }
        if(odin->triggerType()!=ODIN::RandomTrigger){
          axiso-> SetBinLabel (odin ->triggerType()+1, "NotRandom");
        }
      }
    }
  }
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
  
  //labels  
  if ( 0 != m_L0Input )
  {
    TH1D *ho = Gaudi::Utils::Aida2ROOT::aida2root( m_L0Input );
    if ( 0 != ho) {
      TAxis* axiso = ho->GetXaxis() ;
      if( 0 != axiso) 
      {
        for ( unsigned ibin = 0; ibin < repsL.size() ; ibin++ ) {
          debug() << "ibin = " << ibin << " repsL[ibin].first = " << repsL[ibin].first <<
            " repsL[ibin].second = " << repsL[ibin].second << endreq;
          const char * test = repsL[ibin].first.c_str();
          axiso-> SetBinLabel ((repsL[ibin].second)+1 , test );
        }
      } 
    } 
  }
};






void HltGlobalMonitor::monitorHLT(const LHCb::ODIN*,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {


  if (hlt==0) return;

 
  ///////////////////////////////////////////////////////////////////////////////
  std::vector<std::pair<std::string,const LHCb::HltDecReport*> > reps;
  unsigned nAcc = 0;
  
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {  
       warning() << "report " << *i << " not found" << endreq;
    }
    reps.push_back( std::make_pair( *i, report ) );
    if (report && report->decision()) ++nAcc;
  }
  
  fill( m_hltNAcc, nAcc, 1.0);  //by how many lines did 1 event get accepted?

  for (size_t i = 0; i<reps.size();++i) {
   
    fill( m_hltInclusive, i, reps[i].second->decision() );
    if (!reps[i].second->decision()) continue;
   
    if (nAcc==1) fill( m_hltExclusive, i, reps[i].second->decision() );

    for (size_t j = 0; j<reps.size(); ++j) {
      fill(m_hltCorrelations,i,j,reps[j].second->decision());
    }
  }


  std::vector<std::string> hlt1L0Triggers     
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1L0*Decision");
  std::vector<std::string> hlt1XPressTriggers 
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*XPress*Decision");
  std::vector<std::string> hlt1HadronTriggers 
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*Hadron*Decision");
  std::vector<std::string> hlt1SMuonTriggers  
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*SingleMuon*Decision");
  std::vector<std::string> hlt12MuonTriggers  
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*DiMuon*Decision");
  std::vector<std::string> hlt1TrackTriggers  
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*MuonTrack*Decision");
  std::vector<std::string> hlt1LumiTriggers   
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1LumiDecision");
  std::vector<std::string> hlt1VeloTriggers   
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*Velo*Decision");
  std::vector<std::string> hlt1ElecTriggers   
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*Electron*Decision");
  std::vector<std::string> hlt1PhoTriggers    
    = m_TriggerTisTosTool->triggerSelectionNames("Hlt1*Pho*Decision");
  std::vector<std::string> hlt1IgnTriggers    
    = m_TriggerTisTosTool->triggerSelectionNames("*Ignor*");
  std::vector<std::string> hlt1GlobTriggers   
    = m_TriggerTisTosTool->triggerSelectionNames("*Global*");
  
  
  
  for (size_t k = 0; k<hlt1L0Triggers.size(); ++k) {
    for(size_t j = 0; j < reps.size(); j++){
      if (hlt1L0Triggers[k] == reps[j].first ){
        fill(m_hlt1alley, 0, reps[j].second->decision());
      }
    }
  }
  
  for (size_t k = 0; k<hlt1XPressTriggers.size(); ++k) {
    for(size_t j = 0; j < reps.size(); j++){  
      if (hlt1XPressTriggers[k] == reps[j].first) {
        fill(m_hlt1alley, 1, reps[j].second->decision());
      }
    }
  }
  
  
  for (size_t k = 0; k<hlt1HadronTriggers.size(); ++k) {
    for(size_t j = 0; j < reps.size(); j++){  
      if (hlt1HadronTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 2, reps[j].second->decision());
      }
    }   
  }
  
  
  for (size_t k = 0; k<hlt1SMuonTriggers.size(); ++k) { 
    for(size_t j = 0; j < reps.size(); j++){
      if (hlt1SMuonTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 3, reps[j].second->decision());
      }
    } 
  }
  
  
  for (size_t k = 0; k<hlt12MuonTriggers.size(); ++k) {
    for(size_t j = 0; j < reps.size(); j++){
      if (hlt12MuonTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 4, reps[j].second->decision());
      }
    }   
  }
  
  
  for (size_t k = 0; k<hlt1TrackTriggers.size(); ++k) {
    for(size_t j = 0; j < reps.size(); j++){
      if (hlt1TrackTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 5, reps[j].second->decision());
      }
    }   
  }
  
  
  for (size_t k = 0; k<hlt1LumiTriggers.size(); ++k) {
    for(size_t j = 0; j < reps.size(); ++j){
      if (hlt1LumiTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 6, reps[j].second->decision());
      }
    }   
  }
  
  
  for (size_t k = 0; k<hlt1VeloTriggers.size(); ++k) { 
    for(size_t j = 0; j < reps.size(); j++){
      if (hlt1VeloTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 7, reps[j].second->decision());
      }
    }   
  }
  
  
  for (size_t k = 0; k<hlt1ElecTriggers.size(); ++k) { 
    for(size_t j = 0; j < reps.size(); j++){
      if (hlt1ElecTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 8, reps[j].second->decision());
      }
    }   
  }
  
  for (size_t k = 0; k<hlt1PhoTriggers.size(); ++k) { 
    for(size_t j = 0; j < reps.size(); j++){  
      if (hlt1PhoTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 9, reps[j].second->decision());
      }
    }   
  }
  
  
  for (size_t k = 0; k<hlt1IgnTriggers.size(); ++k) { 
    for(size_t j = 0; j < reps.size(); j++){  
      if (hlt1IgnTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 10, reps[j].second->decision());
      }
    }   
  }
  
  
  for (size_t k = 0; k<hlt1GlobTriggers.size(); ++k) {
    for(size_t j = 0; j < reps.size(); j++){
      if (hlt1GlobTriggers[k] == reps[j].first ){
        fill(m_hlt1alley, 11, reps[j].second->decision());
      }
    }   
  }


  //labels
  if ( 0 != m_hltInclusive && m_hltCorrelations && m_hlt1alley) 
  {
    TH1D *h = Gaudi::Utils::Aida2ROOT::aida2root( m_hltInclusive );
    TH1D *ha = Gaudi::Utils::Aida2ROOT::aida2root( m_hlt1alley );
    TH2D *hc = Gaudi::Utils::Aida2ROOT::aida2root( m_hltCorrelations );
    if ( 0 != h && 0 != hc) {
      TAxis* axis   = h->GetXaxis() ;
      TAxis* axisa  = ha->GetXaxis() ;
      TAxis* axiscx = hc->GetXaxis() ;
      TAxis* axiscy = hc->GetYaxis() ;

      const Int_t alleybins = 12;
      char *label[alleybins] = {"L0", "XPress", "Hadron", "SingleMuon", "DiMuon", "MuonTrack", 
                         "Lumi", "Velo", "Electron", "Photon", "IgnoreLumi", "Global"};
      
      if( 0 != axis && 0 != axiscx && 0 != axiscy && 0 != axisa) 
      {
        for ( int iabin = 1; iabin <= alleybins ; ++iabin ) {        
          axisa->SetBinLabel(iabin,label[iabin-1]);
        }
        for ( unsigned ibin = 1; ibin <= reps.size() ; ++ibin ) {
          if(ibin < reps.size()){
            // cut the last 8 (=decision) letters off
           reps[ibin-1].first.resize(reps[ibin-1].first.size()-8);
          }
          // cut the first 4 letters off (=Hlt1)
          const char * test = reps[ibin-1].first.c_str() + 4;
          axis-> SetBinLabel ( ibin, test );
          axiscx-> SetBinLabel ( ibin, test );
          axiscy-> SetBinLabel ( ibin, test );
        }
      } 
    } 
  }
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
