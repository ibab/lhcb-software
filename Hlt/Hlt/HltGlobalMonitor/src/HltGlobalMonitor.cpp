// $Id: HltGlobalMonitor.cpp,v 1.22 2009-03-03 12:09:50 graven Exp $
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
        for ( unsigned ibin = 0; ibin < repsL.size() ; ++ibin ) {
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

  //TODO: move this into python configuration...
  //      make a map once such that we do not do regex matches / searches every event...
  const char *names[] = { "Hlt1L0.*Decision", "Hlt1.*XPress.*Decision", "Hlt1.*Hadron.*Decision",
                          "Hlt1.*SingleMuon.*Decision", "Hlt1.*DiMuon.*Decision", "Hlt1.*MuonTrack.*Decision",
                          "Hlt1.*Lumi.*Decision", "Hlt1.*Velo.*Decision",
                          "Hlt1.*Electron.*Decision", "Hlt1.*Pho.*Decision",
                          ".*IgnoreLumi.*", ".*Global.*",0 };
  const char *label[] = {"L0", "XPress", "Hadron", 
                         "SingleMuon", "DiMuon", "MuonTrack", 
                         "Lumi", "Velo",
                         "Electron", "Photon", 
                         "IgnoreLumi", "Global", 0};

  for(size_t j = 0; j < reps.size(); ++j) {
    for (size_t i=0;names[i]!=0;++i) {
        boost::regex exp( names[i] );
        boost::smatch what;
        if (boost::regex_match(reps[j].first,what,exp)) {
            fill(m_hlt1alley, i, reps[j].second->decision());
        }
    }
  }

  //labels //TODO: move to initialize!!!
  static bool m_first(true);
  if (m_first && 0 != m_hltInclusive && m_hltCorrelations && m_hlt1alley) 
  {
    m_first = false;
    TH1D *h = Gaudi::Utils::Aida2ROOT::aida2root( m_hltInclusive );
    TH1D *ha = Gaudi::Utils::Aida2ROOT::aida2root( m_hlt1alley );
    TH2D *hc = Gaudi::Utils::Aida2ROOT::aida2root( m_hltCorrelations );
    if ( 0 != h && 0 != hc) {
      TAxis* axis   = h->GetXaxis() ;
      TAxis* axisa  = ha->GetXaxis() ;
      TAxis* axiscx = hc->GetXaxis() ;
      TAxis* axiscy = hc->GetYaxis() ;

      if( 0 != axis && 0 != axiscx && 0 != axiscy && 0 != axisa) 
      {
        for ( unsigned iabin = 0; label[iabin]!=0 ; ++iabin ) {        
          axisa->SetBinLabel(1+iabin,label[iabin]);
        }
        for ( unsigned ibin = 1; ibin <= reps.size() ; ++ibin ) {
          if(ibin < reps.size()){
            // cut the last 8 (=decision) letters off
            std::string& s = reps[ibin-1].first;
            s.resize(s.size()-8);
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
