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

  info() << " Doing HltGlobalMonitor::initialize() " << endreq;
  
//L0 on HLT input and output
  m_L0Input      = book1D("L0 channel",-0.5,32.5,33);
  m_L0Hlt1Accept = book1D("L0 channel [ Hlt1 Accept ]",-0.5,32.5,33);
  m_L0Hlt2Accept = book1D("L0 channel [ Hlt2 Accept ]",-0.5,32.5,33);



  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
     m_allAcc.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Acc]",  m_allAcc.back(),  "Hlt1 "+*i+" Line Accepts");
     m_allCall.push_back(0);
     declareInfo("COUNTER_TO_RATE["+*i+"Call]", m_allCall.back(), "Hlt1 "+*i+" Line Calls");
  }
  m_gpstimesec=0;


  m_odin           = book1D("ODIN type",  "ODIN Type ",0., 8., 8);

  m_hltAcc          = book1D("Hlt1 lines Accept", "Hlt1 Lines Accept", -0.5, m_Hlt1Lines.size()+0.5,m_Hlt1Lines.size()+1);
  m_hltNAcc         = book1D("# positive HltLines ", -0.5,m_Hlt1Lines.size()+0.5, m_Hlt1Lines.size()+1);
  m_hltInclusive    = book1D("HltLines Inclusive",   -0.5,m_Hlt1Lines.size()-0.5, m_Hlt1Lines.size());
  m_hltExclusive    = book1D("HltLines Exclusive",   -0.5,m_Hlt1Lines.size()-0.5, m_Hlt1Lines.size());
  m_hltCorrelations = book2D("HltLines Correlations",-0.5,m_Hlt1Lines.size()-0.5, m_Hlt1Lines.size()
                                                    ,-0.5,m_Hlt1Lines.size()-0.5, m_Hlt1Lines.size());
  
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

}

void HltGlobalMonitor::monitorL0DU(const LHCb::ODIN*,
                                   const LHCb::L0DUReport* l0du,
                                   const LHCb::HltDecReports* hlt) {

  if (l0du == 0) return;

  counter("L0Accept") += l0du->decision();
  counter("L0Forced") += l0du->forceBit();

  if (!l0du->decision()) return;

  bool hlt1Accept = false;
  bool hlt2Accept = false;
  if (hlt!=0) {
        // find Hlt1Global
        // find Hlt2Global
  }

  LHCb::L0DUChannel::Map channels = l0du->configuration()->channels();
  for(LHCb::L0DUChannel::Map::iterator i = channels.begin();i!=channels.end();++i){
    bool acc = l0du->channelDecision( i->second->id() );
    fill( m_L0Input  , i->second->id(), acc );
    if (hlt1Accept) fill( m_L0Hlt1Accept, i->second->id(), acc );
    if (hlt2Accept) fill( m_L0Hlt2Accept, i->second->id(), acc );
  }
};

void HltGlobalMonitor::monitorHLT(const LHCb::ODIN*,
                                   const LHCb::L0DUReport*,
                                   const LHCb::HltDecReports* hlt) {

  if (hlt==0) return;

  std::vector<const LHCb::HltDecReport*> reps;
  unsigned nAcc = 0;
  for (std::vector<std::string>::const_iterator i = m_Hlt1Lines.begin(); i!=m_Hlt1Lines.end();++i) {
    const LHCb::HltDecReport*  report = hlt->decReport( *i );
    if (report == 0 ) {  
       warning() << "report " << *i << " not found" << endreq;
    }
    //TODO: skip Hlt1Global
    reps.push_back( report );
    if (report && report->decision()) ++nAcc;
  }
  fill( m_hltNAcc, nAcc, 1.0);

  for (size_t i = 0; i<reps.size();++i) {
    fill( m_hltInclusive, i, reps[i]->decision() );
    if (!reps[i]->decision()) continue;
    if (nAcc==1) fill( m_hltExclusive, i, reps[i]->decision() );
    for (size_t j = 0; j<reps.size(); ++j) {
       fill(m_hltCorrelations,i,j,reps[j]->decision());
    }
  }
};



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
