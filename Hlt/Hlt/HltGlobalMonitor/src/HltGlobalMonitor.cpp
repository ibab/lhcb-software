// $Id: HltGlobalMonitor.cpp,v 1.28 2009-03-06 16:39:37 kvervink Exp $
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
// Memory Usage
// ============================================================================
#include "GaudiKernel/Timing.h"
#include "GaudiKernel/Memory.h"
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

namespace {
    bool setBinLabels( TAxis* axis,  const std::vector<std::pair<unsigned,std::string> >& labels ) {
        if (axis==0) return false;
        int nbins = axis->GetNbins(); 
        for (std::vector<std::pair<unsigned,std::string> >::const_iterator i = labels.begin();i!=labels.end();++i ) {
            //TODO: check bin exists... 
            if (1+i->first <= 0 ||  1+i->first > nbins ) return false;
            // Argh... ROOT bins start counting at '1' instead of '0'
            axis -> SetBinLabel(1 + i->first  ,i->second.c_str() );
        }
        return true;
    }

    bool setBinLabels( AIDA::IHistogram1D* hist, const std::vector<std::pair<unsigned,std::string> >& labels ) {
        if (hist==0) return false;
        TH1D *h1d = Gaudi::Utils::Aida2ROOT::aida2root( hist );  
        if (h1d==0) return false;
        return setBinLabels(  h1d->GetXaxis(), labels );
    }

    bool setBinLabels( AIDA::IHistogram1D* hist, const std::vector<std::string>& labels ) {
        std::vector<std::pair<unsigned,std::string> > l;
        for (unsigned i = 0;i<labels.size();++i) {
            l.push_back(std::make_pair( i , labels[i] ) );
        }
        return setBinLabels(hist,l);
    }

    bool setBinLabels( AIDA::IHistogram2D* hist, const std::vector<std::string>& xlabels,
                                                 const std::vector<std::string>& ylabels) {
        if (hist==0) return false;
        std::vector<std::pair<unsigned,std::string> > lx;
        for (unsigned i = 0;i<xlabels.size();++i) {
            lx.push_back(std::make_pair( i , xlabels[i] ) );
        }
        std::vector<std::pair<unsigned,std::string> > ly;
        for (unsigned i = 0;i<ylabels.size();++i) {
            ly.push_back(std::make_pair( i , ylabels[i] ) );
        }
        TH2D *h2d = Gaudi::Utils::Aida2ROOT::aida2root( hist );  
        if (h2d==0) return false;
        return setBinLabels(  h2d->GetXaxis(), lx) && setBinLabels( h2d->GetYaxis(), ly );
    }

  bool setAxisLabels( AIDA::IHistogram1D* hist, const std::string & xAxis,const std::string & yAxis  ) {
    if (hist==0) return false;
    TH1D *h1d = Gaudi::Utils::Aida2ROOT::aida2root( hist );  
    if (h1d==0) return false;
    h1d->SetXTitle(xAxis.c_str());
    h1d->SetYTitle(yAxis.c_str());
    return true;
  }
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltGlobalMonitor::HltGlobalMonitor( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
    , m_gpstimesec(0), m_time(0), m_scanevents(10),  m_totaltime(100000), m_totalmem(2000)
    , m_virtmem(0)



{
  // se nao tiver declarado no options, ele usa este
  declareProperty("ODIN",              m_ODINLocation = LHCb::ODINLocation::Default);
  declareProperty("L0DUReport",        m_L0DUReportLocation = LHCb::L0DUReportLocation::Default);
  declareProperty("HltDecReports",     m_HltDecReportsLocation = LHCb::HltDecReportsLocation::Default);
  declareProperty("Hlt1Decisions",     m_Hlt1Lines );
  //  declareProperty("ScanEvents",        m_scanevents = 2000 );
  //  declareProperty("TotalTime",         m_totaltime  = 2000 );
  //  declareProperty("TotalMemory",       m_totalmem   = 5000 );

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

  //  m_time_ref    = time(NULL);  //time or mtime or mtimeref... zie constructor
  
  m_L0Input         = book1D("L0 channel",-0.5,16.5,17);
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
  
  //TODO: grab alley names (and mapping) from job options, count instead of hardwiring 12..
  m_hlt1alley       = book1D("Hlt1 Alleys", "Hlt1 Alleys", -0.5, 11.5 , 12 );
  std::vector<std::string> alleyLabels = boost::assign::list_of<std::string> 
                ( "L0" )( "XPress" )( "Hadron" )
                ( "SingleMuon" )( "DiMuon" )( "MuonTrack" )
                ( "Lumi" )( "Velo" )
                ( "Electron" )( "Photon" )
                ( "IgnoreLumi" )( "Global" );
  if (!setBinLabels( m_hlt1alley, alleyLabels )) {
    error() << "failed to set binlables on Alley hist" << endmsg;
  }

  std::vector<std::string> labels;
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
      std::string s = *i;
      // remove "Hlt1" and "Decision"
      std::string::size_type i =  s.find("Hlt1");
      if (i != std::string::npos) s.erase(i,i+4);
      i =  s.find("Decision");
      if (i != std::string::npos) s.erase(i,i+8);
      labels.push_back(s);  
  }




  m_hltAcc          = book1D("Hlt1 lines Accept", "Hlt1 Lines Accept",
                             -0.5, m_Hlt1Lines.size()+0.5,m_Hlt1Lines.size()+1);
   if (!setBinLabels(m_hltAcc, labels)) {
    error() << "failed to set binlables on accept hist" << endmsg;
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


  m_hltVirtinTime  = book1D("Virtual memory per event",   -0.5, (double)m_totaltime, m_totaltime);

  setAxisLabels( m_hltVirtinTime, "time[min]", "memory[MB]");

  m_hltVirtMem    = book1D("Virtual Memory",   -0.5, (double)m_totalmem, m_totalmem);
  
  setAxisLabels( m_hltVirtMem, "memory[MB]", "");

  m_hltEventsTime  = book1D("time per event",   -0.5, (double)m_totaltime, m_totaltime);

  setAxisLabels( m_hltEventsTime, "time/event[min]", "10^2 event");


  
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     m_allAcc.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Acc]",  m_allAcc.back(),  "Hlt1 "+*i+" Line Accepts");
     m_allCall.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Call]", m_allCall.back(), "Hlt1 "+*i+" Line Calls");
  }

  declareInfo("COUNTER_TO_RATE", m_virtmem, "Virtual memory");
  declareInfo("COUNTER_TO_RATE", (double)m_time, "Ellapsed time");
  
  m_gpstimesec=0;

  declareInfo("L0Accept",        "",&counter("L0Accept"),        0,std::string("L0Accept"));
  declareInfo("COUNTER_TO_RATE[GpsTimeoflast]",m_gpstimesec,"Gps time of last event");

  declareInfo("#accept","",&counter("#accept"),0,std::string("Events accepted"));

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

  if(  (counter("#accept").nEntries())%m_scanevents ==0) monitorMemory();
  
  counter("#accept") += 1;
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
  
  //TODO: only do this once...
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
          axiso-> SetBinLabel ((repsL[ibin].second)+1 , repsL[ibin].first.c_str() );
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

    ++m_allCall[i];
    fill( m_hltInclusive, i, reps[i].second->decision());
    if (!reps[i].second->decision()) continue;
    ++m_allAcc[i];
   
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

  for(size_t j = 0; j < reps.size(); ++j) {
    for (size_t i=0;names[i]!=0;++i) {
        boost::regex exp( names[i] );
        boost::smatch what;
        if (boost::regex_match(reps[j].first,what,exp)) {
            fill(m_hlt1alley, i, reps[j].second->decision());
        }
    }
  }
}



void HltGlobalMonitor::monitorMemory() {

  //should this also come in the configuration.py?


  System::InfoType fetchT = System::Times;
  System::InfoType fetchM = System::Memory;
  System::TimeType typ    = System::Min; 
  System::MemoryUnit unit = System::MByte;
  long pid = -1;
  ////////////////////////////////////////////////////


  m_time   = ellapsedTime(typ, fetchT, pid);
  m_virtmem  = virtualMemory(unit, fetchM, pid);
  
  fill(m_hltVirtinTime, m_time, m_virtmem);
  if(counter("#accept").nEntries() >0){
    fill(m_hltEventsTime, (double)m_time/(double)(counter("#accept").nEntries()),
         (counter("#accept").nEntries())/100);
  }
  fill(m_hltVirtMem   , m_virtmem, 1);
}

