// $Id: L0DUReportMonitor.cpp,v 1.2 2007-11-02 07:34:58 cattanem Exp $
// Include files 
#include <cmath>
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// local
#include "L0DUReportMonitor.h"
// local
//-----------------------------------------------------------------------------
// Implementation file for class : L0DUReportMonitor
//
// 2006-02-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DUReportMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUReportMonitor::L0DUReportMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_chanCntMap(), 
    m_condCntMap(),
    m_chanRateMap(),
    m_condRateMap(),
    m_evtCntMap(),
    m_decCntMap()  
{
  declareProperty( "ReportLocation"    , m_reportLocation =  LHCb::L0DUReportLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
L0DUReportMonitor::~L0DUReportMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DUReportMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_prevTCK   = -1;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DUReportMonitor::execute() {

  debug() << "==> Execute" << endmsg;
  

  LHCb::L0DUReport* report   = get<LHCb::L0DUReport>( m_reportLocation );
  LHCb::L0DUConfig* config   = report->configuration();
  unsigned int tck           = report->tck();
  

  // Initialisation
  bool init = false;
  if((int) tck != m_prevTCK ){
    m_prevTCK = (int) tck;
    // is this tck known ?
    std::map<unsigned int,double>::iterator it = m_evtCntMap.find(tck);
    if( it == m_evtCntMap.end() ){
      m_configs[tck]=config;
      m_evtCnt = 0.;
      m_decCnt = 0.;
      init = true;
    }
    else{  
      m_chanCnt     = m_chanCntMap[tck];
      m_condCnt     = m_condCntMap[tck];
      m_chanRate    = m_chanRateMap[tck];
      m_chanRelRate = m_chanRelRateMap[tck];
      m_condRate    = m_condRateMap[tck];
      m_evtCnt      = m_evtCntMap[tck];
      m_decCnt      = m_decCntMap[tck];
    }
  }


  LHCb::L0DUChannel::Map channels = config->channels();
  LHCb::L0DUElementaryCondition::Map conditions = config->conditions();
  

  
  m_evtCnt += 1.;  // event counter
  if(report->decision())m_decCnt += 1.; // decision counter
  std::stringstream chanName("[");
  std::stringstream condName("[");
  for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
    if( init )m_chanCnt[ (*it).first ]=0;//init    
    if(report->channelDecision( ((*it).second)->id() )) m_chanCnt[ (*it).first ] += 1.;
    m_chanRate[(*it).first ] = 0.;
    m_chanRelRate[(*it).first ] = 0.;
    chanName << ":" <<  (*it).first;
  }
  for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
    if( init ) m_condCnt[ (*it).first ]=0;//init    
    if( report->conditionValue( ((*it).second)->id() ) ) m_condCnt[ (*it).first ] += 1.;
    m_condRate[(*it).first ] = 0.;
    condName << ":" <<  (*it).first;
  }
  chanName << "]";
  condName << "]";

  // Ratios

  for(CounterMap::iterator it = m_chanCnt.begin(); m_chanCnt.end()!=it;it++){
    double rate = 0;
    double relRate = 0;
    if(m_evtCnt != 0) rate = 100. * (*it).second / m_evtCnt;
    if(m_decCnt != 0) relRate= 100. * (*it).second / m_decCnt;    
    m_chanRate[(*it).first] = rate ;
    m_chanRelRate[(*it).first] = relRate ;
  }
  for(CounterMap::iterator it = m_condCnt.begin(); m_condCnt.end()!=it;it++){
    double rate = 0;
    if(m_evtCnt !=0 ) rate = 100.* (*it).second / m_evtCnt;
    m_condRate[ (*it).first ]= rate ;
  }


  m_chanCntMap[tck]    = m_chanCnt;
  m_condCntMap[tck]     = m_condCnt;
  m_chanRateMap[tck]   = m_chanRate;
  m_chanRelRateMap[tck]= m_chanRelRate;
  m_condRateMap[tck]    = m_condRate;
  m_evtCntMap[tck]     = m_evtCnt;
  m_decCntMap[tck]     = m_decCnt;


  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DUReportMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;


  
  info() << "======================================================================== " <<endreq;
  info() << " L0DUReport Monitoring ran on " << m_evtCntMap.size() <<" different Configuration(s) " << endreq;
  info() << "======================================================================== "  << endreq;
  info() << " " << endreq;
  for(std::map<unsigned int,LHCb::L0DUConfig*>::iterator it = m_configs.begin(); it != m_configs.end(); it++){
    unsigned int tck = (*it).first;
    LHCb::L0DUConfig* config =(*it).second;

    m_chanCnt     = m_chanCntMap[tck];
    m_condCnt     = m_condCntMap[tck];
    m_chanRate    = m_chanRateMap[tck];
    m_chanRelRate = m_chanRelRateMap[tck];
    m_condRate    = m_condRateMap[tck];
    m_evtCnt      = m_evtCntMap[tck];
    m_decCnt      = m_decCntMap[tck];
    double rate =  100.* m_decCnt / m_evtCnt ;
    double eRate = 100.* sqrt(m_decCnt)/m_evtCnt;
    
    info() << "   **************************************************** " << endreq;    
    info() << "   ***  Trigger Configuration Key "  << tck << endreq;
    info() << "   **************************************************** " << endreq;    
    debug() << "       REMIND : the corresponding L0DU algorithm description is : " << endreq;
    debug() << config->description() << endreq;
    info() << " " << endreq;

    info() << "   ------------------------------------------------------------------- " <<endreq;
    info() << "   -- L0 Performance on " << m_evtCnt << " events" << endreq;  
    info() << "   ------------------------------------------------------------------- " <<endreq;
    info() << "              *  Accepted L0          : " 
           << format( " %8.0f events :  rate = ( %6.2f +- %6.2f) %% ", m_decCnt, rate, eRate ) 
           << endreq;
    info() << "   ------------------------ CHANNELS --------------------------------- " <<endreq;
    for(CounterMap::iterator ic =  m_chanRate.begin(); m_chanRate.end()!=ic ; ic++){
      std::string name = (*ic).first;
      LHCb::L0DUChannel::Map channels = config->channels();
      LHCb::L0DUChannel* channel = channels[name];
      std::string status="** DISABLED **";
      if(channel->enable() ){
        status ="   ENABLED    ";
        info() << "   * " << status << " channel : " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%  (rel. rate =  %6.2f %% ) ", 
                        name.c_str(), m_chanCnt[name], m_chanRate[name] , m_chanRelRate[name]) << endreq;
      }else{
        info() << "   * " << status << " channel : " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%   ", 
                        name.c_str(), m_chanCnt[name], m_chanRate[name]) << endreq;
      }
    }
    info() << "   ------------------------ CONDITIONS ------------------------------ " <<endreq;
    for(CounterMap::iterator ic =  m_condRate.begin(); m_condRate.end()!=ic ; ic++){
      std::string name = (*ic).first;
    info() << "   *  Elementary Condition  " 
             << format( "%20s :  %8.0f events : rate = %6.2f  %%   ", 
                        name.c_str(), m_condCnt[name], m_condRate[name]) << endreq;
    }
    info() << "======================================================================== " <<endreq;
  }

   return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================




