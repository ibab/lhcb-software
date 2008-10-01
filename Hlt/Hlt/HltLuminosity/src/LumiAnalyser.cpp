// $Id: LumiAnalyser.cpp,v 1.1 2008-10-01 15:04:12 panmanj Exp $
// Include files 
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Event/ODIN.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"

#include "HltBase/ANNSvc.h"
#include "LumiAnalyser.h"
#include "HltBase/ESequences.h"

// to access the histos
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/Bootstrap.h" 
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/SmartDataPtr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiAnalyser
//
// 2008-10-01 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiAnalyser );

LumiAnalyser::LumiAnalyser( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : HltBaseAlg ( name , pSvcLocator )
{
  declareProperty("HistogramUpdatePeriod" , m_histogramUpdatePeriod = 1 );
  declareProperty("InputDataContainer",     m_DataName = LHCb::HltLumiSummaryLocation::Default );
  declareProperty("InputVariables",         m_Variables );
  declareProperty("Thresholds",             m_Thresholds );
  declareProperty("Threshold",              m_Threshold = 5 );
  declareProperty("TrendSize"             , m_trendSize = 100 );
  declareProperty("TrendInterval"         , m_trendInterval = 100 );
  declareProperty("BXTypes"               , m_BXTypes ) ;
  declareProperty("AddBXTypes"            , m_addBXTypes ) ;
  declareProperty("SubtractBXTypes"       , m_subtractBXTypes ) ;
}

LumiAnalyser::~LumiAnalyser()
{

} 


StatusCode LumiAnalyser::initialize() {

  StatusCode sc = HltBaseAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  // initialize lists
  if (m_BXTypes.empty()) {
      m_BXTypes.push_back("BeamCrossing");
      m_BXTypes.push_back("SingleBeamRight");
      m_BXTypes.push_back("SingleBeamLeft");
      m_BXTypes.push_back("NoBeam");
  }
  if (m_addBXTypes.empty()) {
      m_addBXTypes.push_back("BeamCrossing");
      m_addBXTypes.push_back("NoBeam");
  }
  if (m_subtractBXTypes.empty()) {
      m_subtractBXTypes.push_back("SingleBeamRight");
      m_subtractBXTypes.push_back("SingleBeamLeft");
  }

  if (m_Variables.empty()) {
      m_Variables.push_back("SPDMult");
      m_Variables.push_back("PUMult");
      m_Variables.push_back("CaloEt");
  }

  // ------------------------------------------
  m_annSvc = svc<IANNSvc>("LumiANNSvc");

  initCounters();

  if (m_Variables.size()!=m_Thresholds.size() && !m_Thresholds.empty()) {
    error() << "wrong numer of threshold values" << endmsg;
    return StatusCode::FAILURE;
  }

  // set the default threshold
  if (m_Thresholds.empty()) {
    for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
	ivar!= m_Variables.end() ; ++ivar ){  
      m_Thresholds.push_back(m_Threshold);
    }
  }

  // keep a double list of averages (w and w/o threshold)
  for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
      ivar!= m_Variables.end() ; ++ivar ){  
    m_Averages.push_back(*ivar);
  }
  for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
      ivar!= m_Variables.end() ; ++ivar ){  
    m_Averages.push_back(*ivar+"_threshold");
  }

  debug()<< "going to set up"<<endmsg;
  setupStore();
  
  debug() << "Initialised Lumi Analyser" << endmsg ;
  return sc;

}

//---------------------------------------------------------
void LumiAnalyser::initCounters() 
{
   // counters
  initializeCounter(m_counterEntries,    "Entries");
  initializeCounter(m_counterHistoInputs,     "HistoInputs");
  info()<< "Counters initialised!"<<endmsg;
  
}

//---------------------------------------------------------
StatusCode LumiAnalyser::execute() {

  StatusCode sc = StatusCode::SUCCESS;  
  increaseCounter(m_counterEntries,1);
  increaseCounter(m_counterHistoInputs,1);

  // accumulate the counters with their normalization
  accumulate();

  // analyse at fixed time intervals
  if ( gpsTimeInterval() ) analyse();

  setDecision(true);
  return sc;
}

//---------------------------------------------------------
void LumiAnalyser::setDecision(bool ok) {
  setFilterPassed(ok);
}

//---------------------------------------------------------
StatusCode LumiAnalyser::finalize() {
  debug() << "LumiAnalyser::finalize()" << endmsg;
  infoTotalEvents(m_counterEntries);
  infoSubsetEvents(m_counterEntries,m_counterHistoInputs, " events to Histos ");
  return HltBaseAlg::finalize();
}

//---------------------------------------------------------
void LumiAnalyser::setupStore() {
  debug() << "LumiAnalyser::setupStore()0" << endmsg;

  // set up store of pairs for averaging
  for (std::vector<std::string>::iterator iBx = m_BXTypes.begin(); iBx != m_BXTypes.end(); ++iBx) {
    std::string bx=*iBx;
    iiMap *theMap = new iiMap();
    iiMap *prevMap = new iiMap();
    m_theStore[bx] = theMap;
    m_prevStore[bx] = prevMap;
    for (std::vector<std::string>::iterator iVar = m_Averages.begin(); iVar != m_Averages.end(); ++iVar) {
      std::string var=*iVar;
      iiPair *thePair = new iiPair();
      iiPair *prevPair = new iiPair();
      (*theMap)[var] = thePair;
      (*prevMap)[var] = prevPair;
    }
  }
  
  // the trend histos
  std::string prefix="Trend_";
  for (std::vector<std::string>::iterator iVar = m_Averages.begin(); iVar != m_Averages.end(); ++iVar) {
    std::string var=*iVar;
    AIDA::IHistogram1D* trendHisto = initializeHisto(prefix+var, -m_trendSize*m_trendInterval, 0, m_trendSize);
    m_trendMap[var] = trendHisto;  // store in trend map
  }
}

//---------------------------------------------------------
StatusCode LumiAnalyser::accumulate() {
  debug() << "LumiAnalyser::accumulate()" << endmsg;

  // get data container
  m_HltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

  // get ODIN to determine BXType
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    StatusCode sc = Error("ODIN cannot be loaded",StatusCode::FAILURE);
    return sc;
  }
  std::stringstream sbxType("");
  sbxType << (LHCb::ODIN::BXTypes) odin->bunchCrossingType();
  std::string bxType = sbxType.str();

  // use the storage belonging to the bxType
  iiMap *theMap = m_theStore[bxType];
  
  int i = 0;
  for (std::vector<std::string>::iterator iVar = m_Variables.begin(); iVar != m_Variables.end(); ++iVar,++i) {
    std::string var=*iVar;
    boost::optional<IANNSvc::minor_value_type> x = m_annSvc->value("LumiCounters", var);
    if (!x) {
      warning() << "LumiCounter not found with name: " << var <<  endmsg;
    } else {
      int counter = x->second;
      int ivalue = m_HltLumiSummary->info(counter,-1);
      if (ivalue > -1) {
        ((*theMap)[var])->first += 1;
        ((*theMap)[var])->second +=(long) ivalue;
        ((*theMap)[var+"_threshold"])->first += 1;
        if ( ivalue > m_Thresholds[i] ) {
          ((*theMap)[var+"_threshold"])->second += 1;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------
StatusCode LumiAnalyser::analyse() {
  debug() << "LumiAnalyser::analyse()" << endmsg;


  // take delta from all histos
  for (std::vector<std::string>::iterator iVar = m_Averages.begin(); iVar != m_Averages.end(); ++iVar) {
    std::string var=*iVar;
    double meanR = 0;  // for R calculation
    for (std::vector<std::string>::iterator iBx = m_BXTypes.begin(); iBx != m_BXTypes.end(); ++iBx) {
      std::string bx=*iBx;
      iiMap *theMap=m_theStore[bx];
      iiMap *prevMap=m_prevStore[bx];
      iiPair *thePair = (*theMap)[var];
      iiPair *prevPair = (*prevMap)[var];
      // subtract the previous results from the present ones and store in the previous
      iiPair deltaPair;
      deltaPair.first = thePair->first-prevPair->first;
      deltaPair.second = thePair->second-prevPair->second;
      debug() << " subtraction result: " << bx << "/" << var 
              << " entries: " << thePair->first << " - " << prevPair->first << " = " << deltaPair.first
              << " sums: " << thePair->second << " - " << prevPair->second << " = " << deltaPair.second
              << endmsg;
      // add and subtract according to crossing type
      double norm = deltaPair.first;
      double sums = deltaPair.second;
      double R = 0;
      if (norm) {
        R = sums/norm;
      }
      // add or subtract this crossing type
      for (std::vector<std::string>::iterator iABx = m_addBXTypes.begin(); iABx != m_addBXTypes.end(); ++iABx) {
        if ( (*iABx) == bx ) {
          meanR += R;
          break;
        }
      }
      for (std::vector<std::string>::iterator iSBx = m_subtractBXTypes.begin(); iSBx != m_subtractBXTypes.end(); ++iSBx) {
        if ( (*iSBx) == bx ) {
          meanR -= R;
          break;
        }
      }
      // save the data in store for the next time
      prevPair->first = thePair->first;
      prevPair->second = thePair->second;
    }
    // R-calculation
    info() << var << ": R from pairs: " << meanR << endmsg;
    // store trend
    storeTrend(var, meanR);
  }
  return StatusCode::SUCCESS;
  
}

//=============================================================================
ulonglong LumiAnalyser::gpsTime() {

  // get ODIN bank
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    StatusCode sc = Error("ODIN cannot be loaded",StatusCode::FAILURE);
    return 0;
  }
  return odin->gpsTime();  // in microseconds
}

//=============================================================================
bool LumiAnalyser::gpsTimeInterval() 
{
  // check if time interval has elapsed
  ulonglong now = gpsTime();
  verbose() << "gps: " << now << endmsg;
  
  if ( m_gpsPrevious ) {
    double delta = (now - m_gpsPrevious);
    double interval = (m_trendInterval);
    
    if ( delta > (interval*(1.0E6)) ) {
      m_gpsPrevious = now;
      return true;
    }
    return false;
  }
  m_gpsPrevious = now;
  return false;
}

//=============================================================================
void LumiAnalyser::storeTrend(std::string varname, double lumiValue) 
{
  // store trends for
  AIDA::IHistogram1D *theHist = m_trendMap[varname];
  const AIDA::IAxis& axis = theHist->axis();
  long bins = axis.bins();
  info() << "trend " << varname << ": ";
  for ( long i = 0; i < bins; ++i ) {
    double binValue = theHist->binHeight(i);
    double nextValue;
    if ( i < bins - 1 ) {
      nextValue = theHist->binHeight(i+1);
    }
    else {
      nextValue = lumiValue;
    }
    double x = 0.5*(axis.binUpperEdge(i)+axis.binLowerEdge(i));
    theHist->fill(x, nextValue - binValue);
    info() << theHist->binHeight(i) << " ";
  }
  info() << endmsg;
}
