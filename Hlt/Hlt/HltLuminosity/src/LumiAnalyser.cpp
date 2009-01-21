// $Id: LumiAnalyser.cpp,v 1.7 2009-01-21 12:02:58 graven Exp $
// Include files 
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "Event/ODIN.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"

#include "HltBase/ANNSvc.h"
#include "LumiAnalyser.h"

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

#include "boost/foreach.hpp"

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
  declareProperty("MaxBin",                 m_MaxBin = 500 );
  declareProperty("RawHistos",              m_rawHistos = true );
  declareProperty("TrendSize"             , m_trendSize = 100 );
  declareProperty("TrendInterval"         , m_trendInterval = 100 );
  declareProperty("BXTypes"               , m_BXTypes ) ;
  declareProperty("AddBXTypes"            , m_addBXTypes ) ;
  declareProperty("SubtractBXTypes"       , m_subtractBXTypes ) ;
  declareProperty("PublishToDIM"          , m_publishToDIM = true ) ;
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

  m_call_counter = 0;

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

  // to publish to DIM 
  m_size = m_Variables.size();
  m_means = new double[m_size];            // create a fixed location for DIM to look at
  m_thresholds = new double[m_size];       // create a fixed location for DIM to look at
  m_infoKeys = new unsigned int[m_size];   // corresponding key in the info 
  int k = 0;
  for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
      ivar!= m_Variables.end() ; ++ivar, ++k ){  
    std::string name  = *ivar;
    // announce the values
    if (m_publishToDIM ) {
      declareInfo("COUNTER_TO_RATE["+name+"_mean]", m_means[k], "mean of "+name);
      declareInfo("COUNTER_TO_RATE["+name+"_threshold]", m_thresholds[k], "fraction over threshold of "+name);
      declareInfo(name+"_mean", m_means[k], "mean of "+name);
      info() << "counter " << name << " declared at " << k << endmsg;
    }
  }

  
  debug() << "Initialised Lumi Analyser" << endmsg ;
  return sc;

}

//---------------------------------------------------------

//---------------------------------------------------------
StatusCode LumiAnalyser::execute() {

  counter("#Entries") += 1;
  counter("HistoInputs") += 1;
  m_call_counter++;

  // accumulate the counters with their normalization
  accumulate();

  // analyse at fixed time intervals
  if ( gpsTimeInterval() ) analyse();
  //if (  m_call_counter%100 == 0 ) analyse();

  setDecision(true);
  return StatusCode::SUCCESS;  
}

//---------------------------------------------------------
void LumiAnalyser::setDecision(bool ok) {
  setFilterPassed(ok);
}

//---------------------------------------------------------
StatusCode LumiAnalyser::finalize() {
  debug() << "LumiAnalyser::finalize()" << endmsg;
  // report avergaes of all counters
  for (std::vector<std::string>::iterator iVar = m_Averages.begin(); iVar != m_Averages.end(); ++iVar) {
    std::string var=*iVar;
    info() << "R/N for " << var;
    for (std::vector<std::string>::iterator iBx = m_BXTypes.begin(); iBx != m_BXTypes.end(); ++iBx) {
      std::string bx=*iBx;
      iiMap *theMap=m_theStore[bx];
      iiPair *thePair = (*theMap)[var];
      // take average
      double norm = thePair->first;
      double sums = thePair->second;
      double R = 0;
      if (norm) {R = sums/norm;}
      info() << "  " << bx << ": " << R << "/" << thePair->first;
    }
    info() << endmsg;
  }


  // release storage
  delete[] m_means;
  delete[] m_thresholds;
  delete[] m_infoKeys;

  return HltBaseAlg::finalize();
}

//---------------------------------------------------------
void LumiAnalyser::setupStore() {
  debug() << "LumiAnalyser::setupStore()0" << endmsg;

  // set up store of pairs for averaging and for raw R histos
  for (std::vector<std::string>::iterator iBx = m_BXTypes.begin(); iBx != m_BXTypes.end(); ++iBx) {
    std::string bx=*iBx;
    iiMap *theMap = new iiMap();
    iiMap *prevMap = new iiMap();
    m_theStore[bx] = theMap;
    m_prevStore[bx] = prevMap;
    histoMap* hMap=new histoMap();     // create storage for the existing histograms 
    m_histoStore[bx]=hMap;
    for (std::vector<std::string>::iterator iVar = m_Averages.begin(); iVar != m_Averages.end(); ++iVar) {
      std::string var=*iVar;
      iiPair *thePair = new iiPair();
      iiPair *prevPair = new iiPair();
      (*theMap)[var] = thePair;
      (*prevMap)[var] = prevPair;

      // the raw histos for R
      if ( m_rawHistos ) {
        std::string prefix="RawR_";
        // defaults for threshold histos
        double x_min = 0;
        double x_max = 1.;
        unsigned int nbin = 100;
        if ( var.find("_threshold") == std::string::npos ) {
          // direct R
          x_min = -0.5;
          x_max = m_MaxBin-x_min;
          nbin = m_MaxBin;
        }
        AIDA::IHistogram1D* theHisto = initializeHisto(prefix+var+'_'+bx, x_min, x_max, nbin);
        (*hMap)[var]=theHisto;  // store in map
      }
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
  if( !exist<LHCb::HltLumiSummary>(m_DataName) ){
    StatusCode sc = Warning("LumiSummary cannot be loaded",StatusCode::FAILURE);
    return sc;
  }
  LHCb::HltLumiSummary* HltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

  // get ODIN to determine BXType
  if( !exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    StatusCode sc = Error("ODIN cannot be loaded",StatusCode::FAILURE);
    return sc;
  }
  LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);

  // bunch crossing type
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
      int ivalue = HltLumiSummary->info(counter,-1);
      if (ivalue > -1) {
        ((*theMap)[var])->first += 1;
        ((*theMap)[var])->second +=(long) ivalue;
        ((*theMap)[var+"_threshold"])->first += 1;
        if ( ivalue > m_Thresholds[i] ) {
          ((*theMap)[var+"_threshold"])->second += 1;
	  /*
          warning() << "LumiAnalyser::accumulate()" << bxType << " " << var << " " 
                    << counter << " " << ivalue << endmsg;
	  */
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------
StatusCode LumiAnalyser::analyse() {
  debug() << "LumiAnalyser::analyse()" << endmsg;

  // take delta from all counters
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
      if (norm) {R = sums/norm;}
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
      // fill the rawR histos
      if ( m_rawHistos ) {
        (*(m_histoStore[bx]))[var]->fill(R);
	/*
	  double entries = (*(m_histoStore[bx]))[var]->entries();
	  warning() << "Raw Histos: " << bx << " " << var << " " << R << " entries "<< entries << endmsg;
	*/
      }
      
    }
    // R-calculation
    m_resultMap[var] = meanR;
    
    debug() << var << ": R from pairs: " << meanR << endmsg;
    // store trend
    storeTrend(var, meanR);
  }

  // update DIM locations
  int k = 0;
  for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
      ivar!= m_Variables.end() ; ++ivar, ++k ){  
    m_means[k] = m_resultMap[*ivar];
    m_thresholds[k] = m_resultMap[(*ivar)+"_threshold"];
    debug() << "DIM data: " << *ivar << " " << m_means[k] << " and " << m_thresholds[k] << endmsg;
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
  debug() << "trend " << varname << ": ";
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
    debug() << theHist->binHeight(i) << " ";
  }
  debug() << endmsg;
}
